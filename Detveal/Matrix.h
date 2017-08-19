/**********************************************************
 * Matrix.h
 * A template of a general matrix
 *
 * Christian Wolf
 * Beginn: 22.9.2000
 **********************************************************/

#ifndef	_WOLF_MATRIX_H_
#define	_WOLF_MATRIX_H_

// C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
// C++
#include <iostream>
#include <iomanip>

// From the main module
#include "CIL.h"

// From the compatibility module
#include "visualbug.h"

// From this module
#include "Vector.h"
#include "MatrixDef.h"
#include "Matrix_Invert.h"

using namespace std;

#define MAGIC_NUMBER 240819731

// Global functions
template <class T>
ostream & operator << (ostream &os, Matrix<T> &m);

// *************************************************************
// Constructor
// *************************************************************

template <class T>
Matrix<T>::Matrix (unsigned int rs, unsigned int cs)
{
	_AllocPlane (rs,cs);
}

// *************************************************************
// Constructeur:
// initialises to a constant value
// *************************************************************

template <class T>
Matrix<T>::Matrix	(unsigned int rs, unsigned int cs, T initVal)
{
	_AllocPlane (rs,cs);
	setToValue (initVal);
}

// *************************************************************
// Copy constructeur:
// *************************************************************

template <class T>
Matrix<T>::Matrix	(Matrix &o)
{
	_AllocPlane (o.rows(),o.columns());
	memcpy (*data, *o.data, rows()*columns()*sizeof(T));
}

// *************************************************************
// Destructor
// *************************************************************

template <class T>
void Matrix<T>::free()
{
	if (data != NULL)
	{
		T *p=*data;
		delete [] p;
		delete [] data;
	}
}

template <class T>
Matrix<T>::~Matrix ()
{
	free();
}

// *************************************************************
// Class method:
// allocate a plane
// *************************************************************

template <class T>
void Matrix<T>::_AllocPlane (int rs, int cs)
{
	T *big;
	typedef T *pT;

	noRows=rs;
	noColumns=cs;
	data = new pT [noColumns];
	big	= new T [noColumns*noRows];

	for	(unsigned int i=0; i<noColumns ; i++)
		data[i] = big + i*noRows;
}

// **************************************************************
// Access methods
// ATTENTION ATTENTION ATTENTION!!!!!!!!!!!!!!!!!!!!!!!
// The operator uses y,x notation (MATHEMATICS)
// whereas set/get uses x,y notation (IMAGE PROCESSING)
// ATTENTION ATTENTION ATTENTION!!!!!!!!!!!!!!!!!!!!!!!
// **************************************************************

template <class T>
T& Matrix<T>::operator() (unsigned int r, unsigned int c)
{
#ifdef PEDANTIC_CHECK_CODE
	if (r<0 || r>=noRows || c<0 || c>=noColumns)
	{
		CORE_DUMP_ON_EXCEPTION;
		throw EBoundsError (c,r,noColumns,noRows);
	}
#endif
/*
#ifdef PEDANTIC_MATRIX_FINITE_CHECK
#ifdef PEDANTIC_MATRIX_FINITE_CHECK_ISFLOAT
	float check; memcpy (&check, &(data[c][r]), sizeof(float));
#else
	double check; memcpy (&check, &(data[c][r]), sizeof(double));
#endif
	if (!finite(check))
	{
		CORE_DUMP_ON_EXCEPTION;
		ERR_THROW ("Internal error in Matrix<T>::(): not finite!");
	}
#endif
*/
	return data[c][r];
}

template <class T>
T & Matrix<T>::get (unsigned int x, unsigned int y) const
{
#ifdef PEDANTIC_CHECK_CODE
	if (y<0 || y>=noRows || x<0 || x>=noColumns)
	{
		CORE_DUMP_ON_EXCEPTION;
		throw EBoundsError (x,y,noColumns,noRows);
	}
#endif
/*
#ifdef PEDANTIC_MATRIX_FINITE_CHECK
#ifdef PEDANTIC_MATRIX_FINITE_CHECK_ISFLOAT
	float check; memcpy (&check, &(data[x][y]), sizeof(float));
#else
	double check; memcpy (&check, &(data[x][y]), sizeof(double));
#endif
	if (!finite(check))

	{
		CORE_DUMP_ON_EXCEPTION;
		ERR_THROW ("Internal error in Matrix<T>::get(): not finite!");
	}
#endif
*/
	return data[x][y];
}

template <class T>
void Matrix<T>::set (unsigned int x, unsigned int y, T val)
{
#ifdef PEDANTIC_CHECK_CODE
	if (x<0 || x>=noColumns || y<0 || y>=noRows)
	{
		CORE_DUMP_ON_EXCEPTION;
		throw EBoundsError (x,y,noColumns,noRows);
	}
#endif
/*
#ifdef PEDANTIC_MATRIX_FINITE_CHECK
#ifdef PEDANTIC_MATRIX_FINITE_CHECK_ISFLOAT
	float check; memcpy (&check, &val, sizeof(float));
#else
	double check; memcpy (&check, &val, sizeof(double));
#endif
	if (!finite(check))
	{
		CORE_DUMP_ON_EXCEPTION;
		ERR_THROW ("Internal error in Matrix<T>::set(): not finite!");
	}
#endif
*/
	data[x][y] = val;
}

template <class T>
T * Matrix<T>::getPointerXY (unsigned int x, unsigned int y) const
{
#ifdef PEDANTIC_CHECK_CODE
	if (y<0 || y>=noRows || x<0 || x>=noColumns)
	{
		CORE_DUMP_ON_EXCEPTION;
		throw EBoundsError (x,y,noColumns,noRows);
	}
#endif
	return &(data[x][y]);
}

template <class T>
T * Matrix<T>::getPointerYX (unsigned int y, unsigned int x) const
{
#ifdef PEDANTIC_CHECK_CODE
	if (y<0 || y>=noRows || x<0 || x>=noColumns)
	{
		CORE_DUMP_ON_EXCEPTION;
		throw EBoundsError (x,y,noColumns,noRows);
	}
#endif
	return &(data[x][y]);
}

// ******************************************************
// Clear the matrix
// ******************************************************

template <class T>
void Matrix<T>::setZero ()
{
	if (data==NULL)
		return;
	memset (*data, 0, noRows*noColumns*sizeof(T));
}

// ******************************************************
// Set the whole matrix to a value
// ******************************************************

template <class T>
void Matrix<T>::setToValue (T val)
{
	T *p = *data;
	if (data==NULL)
		return;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		p[i] = val;
}

// ******************************************************
// Resize the matrix.
// The data gets lost!!!!!
// ******************************************************

template <class T>
void Matrix<T>::resize(unsigned int rs, unsigned int cs)
{
	if ((noRows==rs)&&(noColumns==cs))
		return;
    free();
	_AllocPlane(rs,cs);
}

// ******************************************************
// Resize the matrix.
// Existing data is kept, new elements are set to zero
// ******************************************************

template <class T>
void Matrix<T>::resizeNonDestructiveZero(unsigned int rs, unsigned int cs)
{
	if ((noRows==rs)&&(noColumns==cs))
		return;

	Matrix<T> copy (*this);
    free();
	_AllocPlane(rs,cs);

	// Copy the existing values
	for (unsigned int y=0; y<rows(); ++y)
	for (unsigned int x=0; x<columns(); ++x)
	{
		if (y<copy.rows() && x<copy.columns())
			(*this)(y,x)=copy(y,x);
		else
			(*this)(y,x)=0;
	}
}

// ******************************************************
// Like a resize with keeping the data.
// new elements are not initialized.
// ******************************************************

template <class T>
void Matrix<T>::crop(unsigned int left, unsigned int right,
	unsigned int top, unsigned int bottom)
{
	unsigned int nxs=right-left+1,
	             nys=bottom-top+1;

	if ((noRows==nys)&&(noColumns==nxs))
		return;

	(*this) = *cropR(left,right,top,bottom);
}

// ******************************************************
// Like a resize with keeping the data.
// new elements are not initialized.
// ******************************************************

template <class T>
Matrix<T> * Matrix<T>::cropR(unsigned int left, unsigned int right,
	unsigned int top, unsigned int bottom)
{
	unsigned int nxs=right-left+1,
	             nys=bottom-top+1;

	Matrix<T> *rv= new Matrix<T> (nys, nxs);

	// Copy the existing values
	for (unsigned int y=top; y<=bottom; ++y)
	for (unsigned int x=left; x<=right; ++x)
		(*rv)(y-top,x-left)=(*this)(y,x);
	return rv;
}

// ******************************************************
// Assignment operator
// ******************************************************

template <class T>
inline Matrix<T> & Matrix<T>::operator = (const Matrix<T> & other)
{
	if (this==&other)
		return *this;

	free();
	_AllocPlane (other.noRows,other.noColumns);
	memcpy (*data, *other.data, noRows*noColumns*sizeof(T));

	return *this;
}

// ******************************************************
// Sum of the elements
// ******************************************************

template <class T>
T Matrix<T>::sum ()
{
	T *tp = *data;
	T rv=0;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		rv += tp[i];
	return rv;
}

// ******************************************************
// Maximum and minimum of the elements
// ******************************************************

template <class T>
T Matrix<T>::max ()
{
	T *tp = *data;
	T rv=tp[0];
	for (unsigned int i=1; i<noRows*noColumns; ++i)
		if (tp[i]>rv)
			rv=tp[i];
	return rv;
}

template <class T>
T Matrix<T>::min ()
{
	T *tp = *data;
	T rv=tp[0];
	for (unsigned int i=1; i<noRows*noColumns; ++i)
		if (tp[i]<rv)
			rv=tp[i];
	return rv;
}

// ******************************************************
// Add two matrices
// ******************************************************

template <class T> template <class TO>
void Matrix<T>::operator += (Matrix<TO> &o)
{
	T *tp = *data,
	  *op = *(o.data);
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		tp[i] += op[i];
}

// ******************************************************
// Multiply with a scalar
// ******************************************************

template <class T>  template <class TO>
void Matrix<T>::operator *= (TO scalar)
{
	T *p = *data;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		p[i] *= scalar;
}


template <> template <>
inline void Matrix<unsigned int>::operator *= (float scalar)
{
	unsigned int *p = *data;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		p[i] = (unsigned int) rint (p[i] * scalar);
}

// ******************************************************
// Divide by a scalar
// ******************************************************

template <class T>  template <class TO>
void Matrix<T>::operator /= (TO scalar)
{
	T *p = *data;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		p[i] /= scalar;
}


template <> template <>
inline void Matrix<unsigned int>::operator /= (float scalar)
{
	unsigned int *p = *data;
	for (unsigned int i=0; i<noRows*noColumns; ++i)
		p[i] = (unsigned int) rint (p[i] / scalar);
}

// ******************************************************
// Matrix-Vector multiplication
// v' = M*v
// ******************************************************

template <class T>
Vector<T> Matrix<T>::operator* (Vector<T> &o)
{
	unsigned int rs=rows();
	unsigned int cs=columns();
	Vector<T> rv (rs);
	T v;

	for (unsigned int y=0; y<rs; ++y)
	{
		v=0;
		for (unsigned int x=0; x<cs; ++x)
			v += (*this)(y,x) * o[x];
		rv[y] = v;
	}
	return rv;
}

// ******************************************************
// The (Moore-Penrose) Pseudo inverse:
// M+ = (M^T M)^-1 M^T
// ******************************************************

template <class T>
Matrix<T> * Matrix<T>::pseudoInverse ()
{
	unsigned int rs=rows();
	unsigned int cs=columns();
	Matrix<T> *psinv;
	T v;

	// Create M^TM
	Matrix<T> mtm (cs,cs);
	for (unsigned int y=0; y<cs; ++y)
	for (unsigned int x=0; x<cs; ++x)
	{
		v=0;
		for (unsigned int i=0; i<rs; ++i)
			v += (*this)(i,y) * (*this)(i,x);
		mtm(y,x) = v;
	}

	// cerr << "M'M: " << mtm;

	// 	invert it
	mtm.invert();

	// cerr << "inv(M'M): " << mtm;

	// and multiply it with M^T
	psinv = new Matrix<T> (cs, rs);
	for (unsigned int y=0; y<cs; ++y)
	for (unsigned int x=0; x<rs; ++x)
	{
		v=0;
		for (unsigned int i=0; i<cs; ++i)
			v += mtm(y,i) * (*this)(x,i);
		(*psinv)(y,x) = v;
	}

	// cerr << "psinv " << *psinv;

	return psinv;
}

// ******************************************************
// Fill the zeros of the matrix with the first non-zero
// value to the _right_
// ******************************************************

template <class T>
inline void Matrix<T>::smearOverZerosDirectionLeft ()
{
	T val, lastval;

	for (int y=0; y<(int)noRows; ++y)
	{

		lastval = 0;
		for (int x=noColumns-1; x>=0; --x)
		{

			val = (*this)(y,x);

			// Zero -> set it
			if (val==0)
				set (x,y,lastval);

			// Not zero -> save the value for later use
			else
				lastval = val;
		}
	}
}

// *********************************************************************
// Read	and write to/from file
// *********************************************************************

#define CHECKWR		if (i<0) { cerr << "Write error: " << strerror(errno) << endl; \
					exit (1); }
#define CHECKRD		if (i<0) { cerr << "Read error: " << strerror(errno) << endl; \
					exit (1); }

template <class T>
void Matrix<T>::write (int fd)
{
	unsigned int magic = MAGIC_NUMBER;
	int i;
	i=write (fd, &magic, sizeof(unsigned int));			CHECKRD;
	i=write (fd, &noColumns, sizeof(unsigned int));			CHECKRD;
	i=write (fd, &noRows, sizeof(unsigned int));			CHECKRD;
	i=write (fd, *data, noColumns*noRows*sizeof(T));			CHECKRD;
}

template <class T>
void Matrix<T>::read (int fd)
{
	unsigned int magic, nx, ny;
	int i;
	i=read (fd, &magic, sizeof(unsigned int));			CHECKWR;
	if (magic!=MAGIC_NUMBER)
		throw EError ("Error in Matrix<T>::read(): Wrong magic number!\n");

	i=read (fd, &nx, sizeof(unsigned int));			CHECKWR;
	i=read (fd, &ny, sizeof(unsigned int));			CHECKWR;

	// Size changed, re-allocate
	if ((nx!=noColumns)||(ny!=noRows))
	{
		free();
		_AllocPlane(nx,ny);
	}

	// Read in the data
	i=read (fd, *data, noColumns*noRows*sizeof(T));			CHECKWR;
}

// *********************************************************************
// Debug output
// *********************************************************************

template <class T>
void Matrix<T>::print (FILE *fp)
{
	fprintf (fp, "Matrix has dimensions: noRows=%d, noColumns=%d.\n", noColumns, noRows);
	for (unsigned int y=0; y<noRows; ++y) {
		for (unsigned int x=0; x<noColumns; ++x)
			fprintf (fp, "%.5f ", (double) *this(y,x));
		fprintf (fp, "\n");
	}
}

// *********************************************************************
// OUTPUT - for matlab
// *********************************************************************

template <class T>
void Matrix<T>::matlabOut (ostream & os)
{
	os << "[\n";
	for (unsigned int y=0; y<rows(); ++y)
	{
		for (unsigned int x=0; x<columns(); ++x)
		{
			if (x>0) os << " ";
			os << (*this)(y,x);
		}
		os << " ; \n";
	}
	os << "]\n";
}

// *********************************************************************
// OUTPUT - for LaTeX
// width and precision controle the formatting.
// if width<0 then there is no formatting.
// *********************************************************************

template <class T>
void Matrix<T>::LaTeXOut (ostream & os, int width, int precision)
{
	if (width>=0)
	{
		os.setf(ios::fixed);
		os << setw(width) << setprecision(precision);
	}

	os << "\\begin{tabular}{";
	for (unsigned int x=0; x<columns(); ++x)
	{
		if (x>0) os << "|";
		os << "c";
	}
	os << "}" << endl;

	for (unsigned int y=0; y<rows(); ++y)
	{
		if (y>0)
			os << "\\hline" << endl;
		for (unsigned int x=0; x<columns(); ++x)
		{
			if (x>0) os << " & ";
			os << (*this)(y,x);
		}
		os << " \\\\ " << endl;
	}
	os << "\\end{tabular}" << endl;
}

// *********************************************************************
// Debug output
// *********************************************************************

template <class T>
inline ostream & operator << (ostream &os, Matrix<T> &m)
{
	os << "Matrix (" << m.rows() << "," << m.columns() << ")\n";
	for (unsigned int y=0; y<m.rows(); ++y)
	{
		for (unsigned int x=0; x<m.columns(); ++x)
		{
			if (x>0) os << " ";
			os << m(y,x);
		}
		os << endl;
	}
	return os;
}

inline ostream & operator << (ostream &os, Matrix<float> &m)
{
	os << "Matrix (" << m.rows() << "," << m.columns() << ")\n";
	for (unsigned int y=0; y<m.rows(); ++y)
	{
		for (unsigned int x=0; x<m.columns(); ++x)
		{
			if (x>0) os << " ";
			os << m(y,x);
		}
		os << endl;
	}
	return os;
}

#endif

