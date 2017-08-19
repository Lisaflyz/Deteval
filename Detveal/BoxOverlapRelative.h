/*********************************************************************
 * BoxOverlapRelative.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/
 
#ifndef	_WOLF_BoxOverlapRelative_H_
#define	_WOLF_BoxOverlapRelative_H_

// From this module
#include "RectEval.h"
#include "BoxMatrix.h"
#include "RectVectorAccessor.h"

class BoxOverlapRelative	
{

	public:

		// COnstructors
		inline BoxOverlapRelative (RectVectorAccessor *lG, RectVectorAccessor *lD);

		// Destructor
		inline ~BoxOverlapRelative () 
		{
			if (mG!=NULL) 
			{
				delete mG;
				delete mD;
			}
		}
		
	protected:

		inline void calcElements (int g, int d);

		BoxMatrix *mG, *mD;
		RectVectorAccessor *pG, *pD;
};

// *******************************************************************
// Constructor - build using two lists of rectangles
// *******************************************************************

inline BoxOverlapRelative::BoxOverlapRelative (RectVectorAccessor *lG, RectVectorAccessor *lD) {
	pG = lG;
	pD = lD;

	if (lG->size()>0 && lD->size()>0) 
	{
		mG = new BoxMatrix (lG->size(), lD->size());
		mD = new BoxMatrix (lG->size(), lD->size());

		for (int countG=0; countG<pG->size(); ++countG)
			for (int countD=0; countD<pD->size(); ++countD)
				calcElements (countG,countD);
	
		mG->fillArr();
		mD->fillArr();
	}

	else
		mG=mD=NULL;
}

// *******************************************************************
// Calculate and update	the	matrices for indices g and d
// *******************************************************************

inline void BoxOverlapRelative::calcElements (int g, int d)	
{
	int	oa;

	oa = (*pG)[g]->overlapArea(*((*pD)[d]));
	mG->set	(g,	d, (double)	oa / (double) (*pG)[g]->area());
	mD->set	(g,	d, (double)	oa / (double) (*pD)[d]->area());
}

#endif

