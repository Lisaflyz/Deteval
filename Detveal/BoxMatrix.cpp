/*********************************************************************
 * BoxMatrix.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/

// From the CIL module
#include "CIL.h"

// From this module
#include "BoxMatrix.h"

// **********************************************************************
// Constructor
// **********************************************************************

BoxMatrix::BoxMatrix(int xs, int ys)
	// invert coords (Matrix::Matrix uses mathematical dimension order (row, column)
	: m(ys, xs) 
{
	if (xs==0 || ys==0)
		ERR_THROW("Internal error in BoxMatrix::BoxMatrix(): one of the coords is zero!");

	countX = new int [xs];
	countY = new int [ys];
};

// **********************************************************************
// Destructor
// **********************************************************************

BoxMatrix::~BoxMatrix()	
{
	if (countX!=NULL) 
	{
		delete [] countX;
		delete [] countY;
	}
}

// **********************************************************************
// Fill	the	statistic arrays
// **********************************************************************

void BoxMatrix::fillArr	() 
{
	// The horizontal counts
	for	(unsigned int y=0; y<ysize(); ++y)	
	{
		countY[y] =	0;
		for	(unsigned int x=0; x<xsize(); ++x)
			if (m.get(x,y)>0)
				++countY[y];
	}

	// The vertical	counts
	for	(unsigned int x=0; x<xsize(); ++x)	
	{
		countX[x] =	0;
		for	(unsigned int y=0; y<ysize(); ++y)
			if (m.get(x,y)>0)
				++countX[x];
	}
}

// *********************************************************************
// Print the values	to the file, readable!
// *********************************************************************
/*
void BoxMatrix::print (FILE	*fp) 
{
	// Call	the	parent method
	FloatMatrix::print (fp);

	fprintf	(fp, "	countX:\n  ");
	for	(int x=0; x<xsize; ++x)
		fprintf	(fp,"%d	",countX[x]);
	fprintf	(fp, "\n");

	fprintf	(fp, "	countY:\n  ");
	for	(int y=0; y<ysize; ++y)
		fprintf	(fp,"%d	",countY[y]);
	fprintf	(fp, "\n");
}
*/


