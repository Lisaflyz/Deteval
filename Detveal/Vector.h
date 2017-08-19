/**********************************************************
 * Vector.h
 * A template of a general Vector
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 * 
 * Changelog:
 * 21.08.2002 cw: first version
 *
 * 1 tab = 4 spaces
 *
 **********************************************************/

#ifndef	_WOLF_VECTOR_H_
#define	_WOLF_VECTOR_H_

// C
#include <math.h>
#include <stdlib.h>

// C++
#include <iostream>

// From this module
#include "VectorDef.h"
#include "Matrix.h"

using namespace std;

// From the compatibility module
#include "visualbug.h"


// Global functions 
template <class T> ostream & operator << (ostream &os, Vector<T> &v);

// ******************************************************
// Constructor: empty vector from size
// ******************************************************

template <class T>
Vector<T>::Vector (unsigned int s) 
{
	_size = s;
	data =	new T [s];
}

// ******************************************************
// Constructor: a constant vector
// ******************************************************

template <class T>
Vector<T>::Vector (unsigned int s, T val) 
{
	_size = s;
	data =	new T [s];
	for (unsigned int i=0; i<_size; ++i)
		data[i] = val;
}

// ******************************************************
// Constructor: a 3-dimensional vector
// ******************************************************

template <class T>
Vector<T>::Vector (T val1, T val2, T val3)
{
	_size = 3;
	data =	new T [3];
	data[0]=val1;
	data[1]=val2;
	data[2]=val3;
}

// ******************************************************
// Copy constructor
// ******************************************************

template <class T>
Vector<T>::Vector (const Vector<T> &other)
{
	_size = other._size;
	data = new T [_size];
	memcpy (data, other.data, _size*sizeof(T));	
}

// ******************************************************
// FACTORY METHOD:
// Create a zero vector
// ******************************************************

template <class T>
inline Vector<T> Vector<T>::createZero(unsigned int dim)
{
	Vector<T> rv (dim);
	for (unsigned int d=0; d<dim; ++d)
		rv[d]=0;
	return rv;
}

// ******************************************************
// Destructor
// ******************************************************

template <class T>
Vector<T>::~Vector () 
{
	if (data!=NULL)
		delete [] data;
} 

// ******************************************************
// Assignment operator
// ******************************************************

template <class T>
Vector<T> & Vector<T>::operator = (const Vector<T> & other) 
{
	if (this==&other)
		return *this;
	
	if (_size!=other._size)
	{
		_size = other._size;
		delete [] data;
		data = new T [_size];
	}
	memcpy (data, other.data, _size*sizeof(T));	
	
	return *this;
}

// ******************************************************
// Resize the vector
// THE DATA GETS LOST!!!!!
// ******************************************************

template <class T>
void Vector<T>::resize (unsigned int s)
{
	if (data!=NULL)
		delete [] data;
	_size = s;
	data = new T [_size];
}

// ******************************************************
// Crop: a sort of resize where the data is kept.
// Is slower than resize, of course
// ******************************************************

template <class T>
Vector<T> Vector<T>::cropR (unsigned int beg, unsigned int end)
{
	Vector<T> rv(end-beg+1);	
	for (unsigned int i=beg; i<=end; ++i)
		rv[i-beg]=data[i];
	return rv;
}

// ******************************************************
// Clear the Vector
// ******************************************************

template <class T>
void Vector<T>::setZero () 
{
#ifdef PEDANTIC_CHECK_CODE
	if (data==NULL) 
		return;		
#endif	
	memset (data, 0, _size*sizeof(T));
}

// ******************************************************
// Add another vector
// ******************************************************

template <class T> template <class TO>
void Vector<T>::operator += (Vector<TO> &o) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] += (T) o[i];
}

template <class T> template <class TO>
void Vector<T>::operator += (TO *o) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] += (T) o[i];
}
// ******************************************************
// subtract another vector
// ******************************************************

template <class T> template <class TO>
void Vector<T>::operator -= (Vector<TO> &o) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] -= (T) o[i];
}

template <class T> template <class TO>
Vector<T> Vector<T>::operator - (Vector<TO> &o) 
{
	Vector<T> rv(_size);
	for (unsigned int i=0; i<_size; ++i)
		rv[i] = data[i] - (T) o[i];
	return rv;
}

template <class T> template <class TO>
void Vector<T>::operator -= (TO *o) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] -= (*o)[i];
}

// ******************************************************
// Compare with a vector
// ******************************************************

template <class T> template <class TO>
bool Vector<T>::operator == (Vector<TO> &o) 
{
	for (unsigned int i=0; i<_size; ++i)
		if (data[i] != o[i])
			return false;
	return true;
}

// ******************************************************
// Divide by a scalar
// ******************************************************

template <class T> template <class TO>
void Vector<T>::operator /= (TO scalar) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] /= scalar;
}

// ******************************************************
// Multiply with a scalar
// ******************************************************

template <class T> template <class TO>
void Vector<T>::operator *= (TO scalar) 
{
	for (unsigned int i=0; i<_size; ++i)
		data[i] *= scalar;
}

// ******************************************************
// Compare with a scalar
// ******************************************************

template <class T>
bool Vector<T>::operator == (T scalar) 
{
	for (unsigned int i=0; i<_size; ++i)
		if (data[i] != scalar)
			return false;
	return true;
}

// ******************************************************
// The dot product
// ******************************************************

template <class T>
T Vector<T>::dotProduct (Vector<T> &o)
{
	T sum=0;
	for (unsigned int i=0; i<_size; ++i)
		sum += get(i)*o.get(i);
	return sum;
}

// ******************************************************
// The norm
// ******************************************************

template <class T>
T Vector<T>::norm()
{
	T x,sum=0;
	for (unsigned int i=0; i<_size; ++i)
	{
		x = get(i);
		sum += x*x;
	}
	return sqrt(sum);
}

// ******************************************************
// ELEMENTWISE product
// ******************************************************

template <class T>
Vector<T> Vector<T>::elProduct (Vector<T> &o)
{
	Vector<T> rv(o._size);
	for (unsigned int i=0; i<_size; ++i)
		rv.set(i,get(i)*o.get(i));
	return rv;
}

// ******************************************************
// ELEMENTWISE inversion
// ******************************************************

template <class T>
void Vector<T>::elInverse ()
{
	for (unsigned int i=0; i<_size; ++i)
		set(i, 1/get(i));
}

// ******************************************************
// Sum of the elements
// ******************************************************

template <class T>
T Vector<T>::sum ()
{
	T sum=0;
	for (unsigned int i=0; i<_size; ++i)
		sum += get(i);
	return sum;
}

// ******************************************************
// Vector - Matrix product:
// v = v^T * M
// ******************************************************

template <class T>
Vector<T> Vector<T>::transposeMultiply (const Matrix<T> &m)
{
	unsigned int rs=m.rows();
	unsigned int cs=m.columns();
	Vector<T> rv (cs);
	T v;
	
	for (unsigned int x=0; x<cs; ++x)	
	{
		v=0;
		for (unsigned int y=0; y<rs; ++y)	
			v += (*this)[x] * m.get(x,y);
		rv[x] = v;
	}
	return rv;
}

// ******************************************************
// The maximum of the elements
// ******************************************************

template <class T>
T Vector<T>::max ()
{
	T v,rv=get(0);
	for (unsigned int i=1; i<_size; ++i)
	{
		v=get(i);
		if (v>rv)
			rv=v;
	}
	return rv;
}

// ******************************************************
// The Eucledian Distance and its square 
// (for speed reasons this is an extra method)
// ******************************************************

template <class T> template <class TO>
float Vector<T>::euclideanDistance (Vector<TO> &o) 
{	
	return sqrt(euclideanDistanceSquared(o));
}

template <class T> template <class TO>
float Vector<T>::euclideanDistanceSquared (Vector<TO> &o) 
{
	float dist=0,a,b;
	for (unsigned int i=0; i<_size; ++i)
	{
		a = get(i);
		b = o.get(i);
 		dist += (T) ((a-b)*(a-b));
	}
	return dist;
}

// *********************************************************************
// Output Visualization
// *********************************************************************

template <class T>
ostream & operator << (ostream &os, Vector<T> &m) 
{
	os << "Vector(" << m.size() << ")=[";	
	for (unsigned int i=0; i<m.size(); ++i)
		os << " " << m[i];
	os << " ]" << endl;
	return os;
}

// *********************************************************************
// OUTPUT - for matlab 
// *********************************************************************

template <class T>
void Vector<T>::matlabOut (ostream & os) 
{
	os << "[";	
	for (unsigned int i=0; i<size(); ++i)
		os << " " << (*this)[i];
	os << " ]" << endl;
}


#endif

