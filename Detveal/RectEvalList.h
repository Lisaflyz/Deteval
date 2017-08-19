/***************************************************************************
 * RectEvalList.h
 *
 * Author: Christian Wolf, christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 ***************************************************************************/

#ifndef	_WOLF_RECTEVALLIST_H_
#define	_WOLF_RECTEVALLIST_H_

// C++ STL Headers
#include <algorithm>
#include <list>
#include <vector>

// From this module
#include "RectEval.h"
#include "RectVectorAccessor.h"

class RectEvalList : public RectVectorAccessor 
{
   public:

   		// Constructors
     	RectEvalList () 
     	{
     		noObjectsWithType = 0;
     		noObjectsWithoutType = 0;
     	}
   		RectEvalList (const RectVectorAccessor *other);
   		
		// Destructor
		virtual ~RectEvalList () 
		{
			 for (iterator iter=begin(); iter!=end(); ++iter)
				delete (*iter);
		}

		// Assignment operator
		RectEvalList & operator	= (const RectEvalList &	other);

		// The iterators
		typedef	vector<RectEval	*>::iterator iterator;
		typedef	vector<RectEval	*>::const_iterator const_iterator;
		iterator begin() { return clist.begin(); }
		iterator end()	{ return clist.end(); }
		const_iterator begin() const { return clist.begin(); }
		const_iterator end()   const { return clist.end(); }

		// Access functions
		void add (RectEval *im)	{ clist.push_back (im);	}
		void add (const	RectEvalList &list);
		void pop_back() { clist.pop_back(); }
			
		virtual int	size () const { return clist.size();	}
		virtual RectEval * operator [] (int index) const 
		{
			return clist[index];
		}
		
	public:
	
		unsigned noObjectsWithType, noObjectsWithoutType;
		
	private:

		vector<RectEval	*> clist;
};

#endif

