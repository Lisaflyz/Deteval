/*********************************************************************
 * BoxOverlap.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/
 
#ifndef _WOLF_BOXOVERLAP_H_
#define _WOLF_BOXOVERLAP_H_

#include <iostream>

#include  "BoxMatrix.h"

class RectVectorAccessor;

class BoxOverlap {

	public:

		// COnstructors
		BoxOverlap (RectVectorAccessor *lG, RectVectorAccessor *lD);

		// Destructor
		~BoxOverlap () { if (m!=NULL) delete m; }

    public:		// DATA
			
		BoxMatrix *m;
		RectVectorAccessor *pG, *pD;
};


#endif
