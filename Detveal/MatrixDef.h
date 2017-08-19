/**********************************************************
 * MatrixDef.h
 * The definition of the matrix, w/o the methods
 *
 * Christian Wolf
 * Beginn: 22.9.2000
 **********************************************************/

#ifndef	_WOLF_MATRIXDEF_H_
#define	_WOLF_MATRIXDEF_H_

// From the main module
#include "CIL.h"

using namespace std;

template <class T>
class Vector;

template <class T>
class Matrix {

	public:

		// Constructors
		Matrix () 											{ data=NULL; noRows=noColumns=0; }
		Matrix (Matrix &o);
		Matrix (unsigned int rows, unsigned int columns);
		Matrix (unsigned int rows, unsigned int columns, T initVal);
		
		// Destructor
		inline virtual ~Matrix ();
		
		void resize(unsigned int rows, unsigned int columns);
		void resizeNonDestructiveZero(unsigned int rs, unsigned int cs);
		void        crop(unsigned int left, unsigned int right, 
				         unsigned int top, unsigned int bottom);
		Matrix<T> * cropR(unsigned int left, unsigned int right, 
				         unsigned int top, unsigned int bottom);
		
		// Assignment operator
		Matrix<T> & operator = (const Matrix<T> & other);

		// Acessors
		unsigned int rows() const							{ return noRows; }
		unsigned int columns() const						{ return noColumns; }
		
		// ATTENTION ATTENTION ATTENTION!!!!!!!!!!!!!!!!!!!!!!!
		// The operator uses y,x notation (MATHEMATICS)
		// whereas set/get uses x,y notation (IMAGE PROCESSING)
		// ATTENTION ATTENTION ATTENTION!!!!!!!!!!!!!!!!!!!!!!!		
		T&    operator() (unsigned int r, unsigned int c);
		T& 	  get        (unsigned int x, unsigned int y) const;
		void  set        (unsigned int x, unsigned int y, T val);				
		T*    getPointerXY (unsigned int x, unsigned int y) const;
		T*    getPointerYX (unsigned int x, unsigned int y) const;
		
		// Clear the matrix
		void setZero ();
		void setToValue (T val);
		
		T sum();
		T max();
		T min();
		
		// Algebra
		Vector<T> operator *  (Vector<T> &o);					// Matrix-Vector multiplication
		template <class TO> void operator += (Matrix<TO> &o);
		template <class TO> void operator *= (TO scalar);		// Matrix-Scalar multiplication
		template <class TO> void operator /= (TO scalar);		// Matrix-Scalar division		
		
		// --------------------------------------------------
		// Linear algebra
		// --------------------------------------------------
		
		void invert();
		Matrix<T> * pseudoInverse();
		
		// --------------------------------------------------
		// Image processing
		// --------------------------------------------------
		
		void smearOverZerosDirectionLeft();
		
		// --------------------------------------------------
		// IO
		// --------------------------------------------------
		
		void write (int filedescriptor);
		void read (int filedescriptor);		
		void print (FILE *fp);
		void matlabOut (ostream & os);
		void LaTeXOut  (ostream & os, int width=-1, int precision=-1);
		
	private:

		void _AllocPlane(int rows, int columns);
		inline void free();
		
	private:
		
		T **data;
		unsigned int noRows, noColumns;
};

// Global functions
template <class T>
ostream & operator << (ostream &os, Matrix<T> &m);


#endif

