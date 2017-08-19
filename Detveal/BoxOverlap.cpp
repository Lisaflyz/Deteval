/*********************************************************************
 * BoxOverlap.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/

// From this module
#include "BoxOverlap.h"
#include "BoxMatrix.h"
#include "RectVectorAccessor.h"

// *******************************************************************
// Constructor - build using two lists of rectangles
// *******************************************************************

BoxOverlap::BoxOverlap (RectVectorAccessor *lG, RectVectorAccessor *lD) {
	pG = lG;
	pD = lD;

	if (lG->size() > 0 && lD->size()>0) {

    	m = new BoxMatrix (lG->size(), lD->size());

    	for (int countG=0; countG<pG->size(); ++countG)
    		for (int countD=0; countD<pD->size(); ++countD)
    			m->set(countG,countD,(*pG)[countG]->overlapArea(*((*pD)[countD])));

    	m->fillArr();
 	}
  	else
   		m=NULL;
}

