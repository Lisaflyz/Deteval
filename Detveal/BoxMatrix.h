/*********************************************************************
 * BoxMatrix.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/

#ifndef	_WOLF_BOXMATRIX_H_
#define	_WOLF_BOXMATRIX_H_

// From the MATHEMATICS library
#include "Matrix.h"

class BoxMatrix	  
{
	public:

		// Constructor
		BoxMatrix(int xs, int ys);

		// Destructor
		~BoxMatrix();

		// Access functions
		inline float & get (unsigned int x, unsigned int y) const 
			{ return m.get(x,y); }
		inline void  set (unsigned int x, unsigned int y, float val)
			{ m.set (x,y,val); }
		inline unsigned int ysize()	{ return m.rows(); }
		inline unsigned int xsize()	{ return m.columns(); }	
			
		inline bool isSingleOverlap(int x,	int	y);
		inline int	getCountX(int x) { return countX[x];}
		inline int	getCountY(int y) { return countY[y];}

		inline float getConditional (bool oneIsGT,	int	x, int y);
		inline int	getCountConditional	(bool oneIsGT, int xy);

		inline void clearLineAndColumn	(int index);

		void fillArr ();

		// void print (FILE *fp);

	private:

		Matrix<float> m;
		int	*countX, *countY;
};



inline bool BoxMatrix::isSingleOverlap(int x, int y) 
{
	return ((countX[x]==1)&&(countY[y])==1);
}

inline float BoxMatrix::getConditional (bool oneIsGT, int x, int y) 
{
	if (oneIsGT)
		return m.get(x,y);
	else
		return m.get(y,x);
}

inline int BoxMatrix::getCountConditional	(bool oneIsGT, int xy) 
{
	if (oneIsGT)
		return countX[xy];
	else
		return countY[xy];
}

inline void BoxMatrix::clearLineAndColumn	(int index)	
{
	for	(unsigned int i=0; i<m.columns(); ++i)	m.set (i, index, 0);
	for	(unsigned int i=0; i<m.rows(); ++i)	m.set (index, i, 0);
}

#endif

