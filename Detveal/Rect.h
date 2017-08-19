#ifndef	_WOLF_RECT_H_
#define	_WOLF_RECT_H_

// C
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

// C++
#include <iostream>

using namespace std;

class Image;
class TextBox;

class Rect {

	friend ostream & operator << (ostream & os, Rect & r);

	public:

		// Constructors
		Rect ()	{ top =	bottom = left =	right; }
		Rect (int t, int b,	int	l, int r) {
			top	= t;
			bottom = b;
			left = l;
			right =	r;
		}

		// Geometry
		int	width () const { return	abs(right-left+1); }
		int	height() const { return	abs(bottom-top+1); }
		int	area  () const { return	height()*width(); }
		double diag  () const { int w=width(); int h=height(); return ::sqrt (h*h+w*w); }
		double shape ()	const {	return (double)width()/(double)height();}
		
		// returns the center of the rectangle
		void center ( int &out_x, int &out_y) const;

		// Modify the rectangle
		void grow		(int growx, int growy);
		void growAndClip (int growx, int growy,	int	xsize, int ysize);
		void shiftX (int offset);
		void shiftY (int offset);
		
		// Do the two rectangles overlap?
		bool doesOverlap (const	Rect &other) const;
		bool overlapRect (const Rect &other, Rect &out) const;
		int	 overlapArea (const Rect &other) const;		
		
		// Calculate the size of an angular segment
		float segmentSize (float ang, float ang_size) const;

		// Is the point	inside the rectangle?
		bool includes (int x, int y) {
			return ((x>=left)&&(x<=right)&&(y>=top)&&(y<=bottom));
		}

		// Is the rectangle	inside the rectangle ->	more than one criteria
		bool includes (const Rect &	other) const {
			return overlapArea(other) == other.area();
		}
		bool includesEachOther (const Rect & other)	const;
		inline bool includesFuzzy (const Rect & other,
				float threshold6, float threshold7, float threshold8,
				float th_nonal1, float th_nonal2, float th_nonal2_ovlap) const;
		
		// Distances between two rectangles
		int    centerDistance (const Rect & other) const;
		double perceptiveDistance (const Rect & other) const;

		void print (FILE *fp) const;
		
		
	public: // CLASS FUNCTIONS
	
		static double pointDistance (int xul, int yul, int xlr, int ylr);
				
	public: // DATA

		int	top, bottom, left, right;
		
};

// ****************************************************************
// Prototypes
// ****************************************************************

ostream & operator << (ostream & os, Rect &r);

// ****************************************************************
// returns the center of the rectangle
// ****************************************************************

inline void Rect::center ( int &out_x, int &out_y) const {
	out_x = left+(right-left+1)/2;
	out_y = top+(bottom-top+1)/2;			
}

// ****************************************************************
// Shift the rectangle
// ****************************************************************

inline void Rect::shiftX (int offset) {
	left += offset;
	right += offset;
}

inline void Rect::shiftY (int offset) {
	top += offset;
	bottom += offset;
}

// ************************************************************************
// Returns if a	given rectangle	is included	in our rectangle, but in a
// kind	of fuzzy way which doesn't care	if not the whole rectangle is
// included	in certain cases.
// ************************************************************************

inline bool Rect::includesFuzzy (const	Rect & other,
	float threshold6, float threshold7, float threshold8,
	float th_nonal1, float th_nonal2, float th_nonal2_ovlap
	) const	{

	int	arBig, arSmall,	arOverlap, arOutside;

	arOverlap =	overlapArea(other);

	// NO overlap -> no	inclusion possible
	if (arOverlap<=0)
		return false;

	arBig =	area();
	arSmall	= other.area();
	if (arBig <	arSmall)
		return false;

	arOutside =	arSmall	- arOverlap;

	// Complete	inclusion
	if (arOverlap == arSmall)
		return true;

	// Small size of the portion not included
	if (((double) arOutside	/ (double) arSmall)	< threshold6)
		return true;

	// For very	small regions, compared	to the bigger one, we are
	// less	strict
	if (((double) arSmall /	(double) arBig)	< threshold7)
		if (((double) arOutside	/ (double) arSmall)	< threshold8)
			return true;


	/*

	// If they touch each other with whatever non-zero overlap
	// But are horizontally aligned, then join.
	{	int h1 = height();
		int h2 = other.height();
		int maxheight = h1 > h2 ? h1 : h2;
        int nonalignment = abs(top-other.top) + abs(bottom-other.bottom);

        // The rectangles are very well aligned
		if ((((double) nonalignment)/ (double) maxheight) < th_nonal1)
			return true;

		// The rectangles are less well aligned, but they overlap very much
		// The rectangles are very well aligned
		if ((((double) nonalignment)/ (double) maxheight) < th_nonal2)
			if (((double) arOutside	/ (double) arSmall)	< th_nonal2_ovlap)
				return true;
		
	}

	*/

	return false;
}

#endif

