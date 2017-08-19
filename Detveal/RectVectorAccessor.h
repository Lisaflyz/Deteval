/*********************************************************************
 * RectVectorAccessor.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * This class is an abstract class defining the protocoll
 * to access a vector of rectangles
 *
 * 1 tab = 4 spaces
 *********************************************************************/

#ifndef _WOLF_RECTVECTORACCESSOR_H_
#define _WOLF_RECTVECTORACCESSOR_H_

// From this module
#include "RectEval.h"

class RectVectorAccessor {

	public:
	
		virtual int size() const = 0;
        virtual RectEval * operator [] (int index) const = 0;
		
		virtual ~RectVectorAccessor () {};
};

#endif

