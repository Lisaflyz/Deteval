/***************************************************************************
 * RectEvalList.cpp
 *
 * Author: Christian Wolf, christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 ***************************************************************************/

// C++
#include <set>

// From this module
#include "RectEvalList.h"

using namespace std;

// ********************************************************************
// Constructor
// ********************************************************************

RectEvalList::RectEvalList (const RectVectorAccessor *other) 
{
	unsigned int s=other->size();

	cerr << "RectEvalList::RectEvalList (const RectVectorAccessor *)\n";

	// Copy	the	rectangles from	the	other list,	not	just the pointers
	for	(unsigned int i=0; i<s; ++i)
		add	(new RectEval (*(*other)[i]));
		
	noObjectsWithType = 0;
    noObjectsWithoutType = 0;
}

// ********************************************************************
// Assignment operator
// ********************************************************************

RectEvalList & RectEvalList::operator =	(const RectEvalList	& other) 
{
	if (this==&other)
		return *this;

	// clear this list and delete all elements
	for	(iterator iter=begin();	iter !=	end(); ++iter)
		delete (*iter);
	clist.clear();

	add	(other);

	return *this;
}

// ********************************************************************
// Add another list
// ********************************************************************

void RectEvalList::add (const RectEvalList & other)	
{
	// Copy	the	rectangles from	the	other list,	not	just the pointers
	for	(const_iterator	iter=other.begin();	iter !=	other.end(); ++iter)
		add	(new RectEval (**iter));
}

