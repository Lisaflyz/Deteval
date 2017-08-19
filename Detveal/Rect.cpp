// C
#include "stdlib.h"
#include "stdio.h"
#define _USE_MATH_DEFINES
#include<cmath>

// From the IMAGE library
#include "Rect.h"

using namespace std;

/************************************************************************
 * A absolute function for double values
 ************************************************************************/

inline double d_abs (double a)	
{
	return a < 0 ? -1*a	: a;
}

// ************************************************************************
// Do the two rectangles overlap?
// ************************************************************************

bool Rect::doesOverlap (const Rect &other) const 
{

	if ((other.top>=top)&&(other.top<=bottom)) 
	{

		// Upper left corner inside
		if ((other.left>=left)&&(other.left<=right))
			return true;

		// Upper right corner inside
		if ((other.right>=left)&&(other.right<=right))
			return true;
	}

	if ((other.bottom>=top)&&(other.bottom<=bottom)) 
	{

		// Lower left corner inside
		if ((other.left>=left)&&(other.left<=right))
			return true;

		// Lower right corner inside
		if ((other.right>=left)&&(other.right<=right))
			return true;
	}

	return false;
}

// ************************************************************************
// Calculat	the	common area	of the two rectangles
// ************************************************************************

inline int Mathmax (int a, int b) { if (a>b) return a; else return b; }
inline int Mathmin (int a, int b) { if (a<b) return a; else return b; }

int	Rect::overlapArea (const Rect &other) const	
{
	int	oHeight, oWidth;

		/* THE ICDAR 2003 CODE
     	int xMin = Mathmax(left, other.left);
        int xMax = Mathmin(right, other.right);
        int yMin = Mathmax(top, other.top);
        int yMax = Mathmin(bottom, other.bottom);
        
        if ((yMin > yMax) || (xMin > xMax))
            return 0.0;

        return  (xMax-xMin)*(yMax-yMin);
		*/

	// Definitely no overlap ->	quick exit
	if ((other.top>bottom)||(other.bottom<top))
		return 0;
	if ((other.left>right)||(other.right<left))
		return 0;

	// Calculate the height	of the overlapping section
	if (other.top>=top)	{
		if (other.bottom>=bottom)
			oHeight	= bottom-other.top+1;
		else
			oHeight	= other.height();
	}
	else {
		if (other.bottom>=bottom)
			oHeight	= height();
		else
			oHeight	= other.bottom-top+1;
	}

	// Calculate the width of the overlapping section
	if (other.left>=left) {
		if (other.right>=right)
			oWidth = right-other.left+1;
		else
			oWidth = other.width();
	}
	else {
		if (other.right>=right)
			oWidth = width();
		else
			oWidth = other.right-left+1;
	}

	return oHeight*oWidth;
}

// ************************************************************************
// Calculate	the overlapping rectangle
// ************************************************************************

bool Rect::overlapRect (const Rect &other, Rect &r) const	
{
	
	// Definitely no overlap ->	quick exit
	if ((other.top>bottom)||(other.bottom<top)||(other.left>right)||(other.right<left)) {
		return false;	
	}

	r.top = other.top>=top ? other.top : top;
	r.left = other.left>left ? other.left : left;
	r.bottom = other.bottom>=bottom ? bottom : other.bottom;	
	r.right = other.right>right ? right : other.right;

	return true;
}

// ************************************************************************
// Returns if a	given rectangle	is included	in our rectangle or	the	other
// way around
// ************************************************************************

bool Rect::includesEachOther (const	Rect & other) const	
{
	int	arOverlap;

	arOverlap =	overlapArea(other);

	if ((arOverlap==area())||(arOverlap==other.area()))
		return true;
	else
		return false;
}

// ********************************************************************
// Return the distance between the centers of the two rectangles
// ********************************************************************

int Rect::centerDistance (const Rect & other) const 
{	
	int t_center_x, t_center_y, o_center_x, o_center_y;

    // Calculate the centers of the rectangles
	center (t_center_x, t_center_y);
	other.center (o_center_x, o_center_y);
			
	/* cerr << "EV: (" << t_center_x << "," << t_center_y << ") - (" 
	                << o_center_x << "," << o_center_y << ")" << endl; */
	
	return (int) Rect::pointDistance (t_center_x, t_center_y, o_center_x, o_center_y);
}

// ********************************************************************
// An essai for a distance from the users point of view
// (Used for tracking the rectangles in the video stream)
// ********************************************************************

double Rect::perceptiveDistance (const Rect & other) const 
{
    int t_center_x, t_center_y, o_center_x, o_center_y;
    double min, dist[5];

    // Calculate the centers of the rectangles
	center (t_center_x, t_center_y);
	other.center (o_center_x, o_center_y);
	
	// Calculate the distances of the respective reference points
	// (corners + centers);
	dist[0] = Rect::pointDistance (left, top, other.left, other.top);
	dist[1] = Rect::pointDistance (right, top, other.right, other.top);	
	dist[2] = Rect::pointDistance (right, bottom, other.right, other.bottom);	
	dist[3] = Rect::pointDistance (left, bottom, other.left, other.bottom);		
	dist[4] = Rect::pointDistance (t_center_x, t_center_y, o_center_x, o_center_y);

	// Search the minimum
	min = dist[0];
	for (int i=1; i<=4; ++i)
		if (dist[i] < min) min = dist[i];

	return min + 0.3 * d_abs(diag()-other.diag());	
}

// ********************************************************************
// The size of a segment of the rectangle which is given by the 
// angle of its middle ray and its angular size.
// 
// ATTENTION: 
// The angles are in radians and begin compass like, i.e. north=0
// 
// See research notebook 7.12.2004
// ********************************************************************

float Rect::segmentSize (float ang, float ang_size) const
{
	float ang_diag;
	float ang_size_2 = ang_size/2;
	float h=height();
	float w=width();
	float rv;
	
	// Get the angle of the diagonal since
	// it separations the different cases of calculation
	ang_diag = fabsf(atan(w/h));	
	// cerr << "EVALUATION: ang_diag=" << ang_diag*180./M_PI << endl;
		
	cerr << "EVALUATION-C: " << ang*180./M_PI << "\t";
	
	// Which quadrant
	if (ang<=M_PI_2)
		;
	else
		if (ang<=M_PI)
			ang=M_PI-ang;
		else
			if (ang<=3.*M_PI_2)
				ang-=M_PI;
			else
				ang=2.*M_PI-ang;
	
	// Now all angles are between 0 and M_PI_2. 
	// But what kind of case do we treat?
	
	// Case: north
	if (ang<ang_size_2)
	{	
		rv=0.25*h*(h/2.*tan(ang+ang_size_2)+
			       h/2.*tan(fabsf(ang-ang_size_2)));	
		cerr << ang*180./M_PI << "\tN" << "\t" << rv << endl;
	}
	
	// Case: north-north-east
	else if (ang<ang_diag-ang_size_2)		
	{	
		rv=0.25*h*(h/2.*tan(ang+ang_size_2)-
			       h/2.*tan(ang-ang_size_2));
		cerr << ang*180./M_PI << "\tNNE" << "\t" << rv << endl;
	}
	
	// Case: north-east
	else if (ang<ang_diag+ang_size_2)		
	{	
		rv=0.25*h*(w/2.-h/2.*tan(ang-ang_size_2)) + 
		   0.25*w*(h/2.-w/2.*tan(M_PI_2-ang-ang_size_2));			
		cerr << ang*180./M_PI << "\tNE" << "\t" << rv << endl;
	}			
	
	// Case: east-north-east		
	else if (ang<M_PI_2-ang_size_2)		
	{	
		rv=0.25*w*(w/2.*tan(M_PI_2-ang+ang_size_2)-
			       w/2.*tan(M_PI_2-ang-ang_size_2));
		cerr << ang*180./M_PI << "\tENE" << "\t" << rv << endl;
	}
		
	// Case: east		
	else
	{ 	
		rv=0.25*w*(w/2.*tan(M_PI_2-ang+ang_size_2)+
			       w/2.*tan(ang+ang_size_2-M_PI_2));
		cerr << ang*180./M_PI << "\tE" << "\t" << rv << endl;
	}	
	
	return rv;		
}

// ********************************************************************
// Get a rectangle which is	grown a	little bit
// ********************************************************************

void Rect::grow	(int growx,	int	growy) 
{
	left   =(left-growx)<0?0:left-growx;
	right  = right+growx;
	top	= (top-growy)<0?0:top-growy;
	bottom = bottom+growy;
}

// ********************************************************************
// Get a rectangle which is	grown a	little bit -
// and clip	it against the window boundaries
// ********************************************************************

void Rect::growAndClip (int	growx, int growy, int xsize, int ysize)	
{
	left   = (left-growx)<0		? 0		:	(left-growx);
	right  = (right+growx)>=xsize  ? xsize-1  :	(right+growx);
	top	= (top-growy)<0		 ? 0		:	(top-growy);
	bottom = (bottom+growy)>=ysize ? ysize-1  :	(bottom+growy);
}

// ********************************************************************
// Debug output
// ********************************************************************

void Rect::print (FILE *fp)	const 
{
	fprintf	(fp, "[%d,%d-%d,%d = %dx%d = %d]\n",
		left, top, right, bottom, width(), height(), area());
}

ostream & operator << (ostream & os, Rect &r) 
{
	int cx,cy;
	r.center(cx,cy);
	os << "[" << r.left << "," << r.top << "-" << r.right << "," << r.bottom
	    << " = " << r.width() << "x" << r.height() << " = " <<  r.area() << "px; "
		<< "center=" << cx << "," << cy << "]";
	return os;
}

// ********************************************************************
// CLASS FUNCTION: Distance between two points
// ********************************************************************

double Rect::pointDistance (int x1, int y1, int x2, int y2) 
{
	float distx = abs(x1 - x2);
	float disty = abs(y1 - y2);
	return sqrt (distx*distx + disty*disty);
}
