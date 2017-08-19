/**********************************************************
 * Vector.h
 * A template of a general Vector
 * - the definition only, w/o methods
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

#ifndef	_WOLF_VECTORDEF_H_
#define	_WOLF_VECTORDEF_H_

// C++
#include <iostream>

using namespace std;

template <class T>
class Matrix;

template <class T>
class Vector {

	public:
	
		typedef T ScalarType;

		// Constructors. 
		// See also the factory methods in the class methods section		
		Vector ()											{ _size=0; data=NULL; }
		Vector (unsigned int s);
		Vector (unsigned int s, T val);
		Vector (const Vector<T> &);
		Vector (T val1, T val2, T val3);
		
		// Destructor
		virtual	~Vector ();
		
		// Assignment operator
		Vector<T> & operator = (const Vector<T> & other);
		
		void resize (unsigned int s);
		Vector<T> cropR (unsigned int beg, unsigned int end);

		// Clear the Vector
		virtual	void setZero ();

		// Access functions
		T&	 get (int x) const 			{return data[x];}
		void set (int x, T val)	const 	{data[x]	= val;}
		T& operator [] (int x) 			{ return data[x]; }
		T& operator () (int x) 			{ return data[x]; }
		unsigned int size() 			{ return _size; }
				
		// Arithmetic
		template <class TO> void operator += (Vector<TO> &o);
		template <class TO> void operator += (TO *o);
		template <class TO> void operator -= (Vector<TO> &o);
		template <class TO> void operator -= (TO *o);
		template <class TO> void operator *= (TO scalar);
		template <class TO> void operator /= (TO scalar);		
		template <class TO> Vector<T> operator - (Vector<TO> &o);
		
		// Algebra
		T dotProduct (Vector<T> &o);
		T norm();				
		Vector<T> elProduct (Vector<T> &o);
		void      elInverse();
		Vector<T> transposeMultiply (const Matrix<T> &m);
		
		T sum();
		T max();
		
		// Comparison
		template <class TO> bool operator == (Vector<TO> &o);
		bool operator == (T scalar);
		
		// Distances		
		template <class TO>
		float euclideanDistance        (Vector<TO> &o);		
		template <class TO>
		float euclideanDistanceSquared (Vector<TO> &o);
		
		void matlabOut (ostream & os);
		
	public: // CLASS METHODS (FACTORY METHODS):
	
		// Create a zero vector
		static Vector<T> createZero(unsigned int dim);	
	
	protected:
	
		unsigned int _size;
		T *data;
};

// Global functions 
template <class T> ostream & operator << (ostream &os, Vector<T> &v);
#endif

