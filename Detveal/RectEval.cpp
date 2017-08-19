/*********************************************************************
 * RectEval.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/
 
// C++
#include <string.h>

// From this module
#include "RectEval.h"

// ********************************************************************
// Check the geometry of the rectangles	- if it	is possible	that the
// two rectangles match. Do	not	use	the	overlap	area.
// memberIsGT is true if the member	rectangle is the ground	truth and
// the other is	the	detected rectangle
// Result:
// true .... verification passes (constraints satisfied)
// false ... verification does not pass (constraints violated)
// ********************************************************************

bool RectEval::textHorizComplete (const	RectEval &other, bool memberIsGT) const	
{
	int	lr,	rr;
	double threshold;

	// Calculate the difference	of the left	and	right border
	// -> Is the complete word covered
	lr = left -	other.left;
	rr = other.right - right;
	if (!memberIsGT) 
	{
		lr *= -1;
		rr *= -1;
	}

	// Calculate the minimum horizontal	coverage for these rectangles
	threshold =	memberIsGT ? width() : other.width();
	threshold =	-threshold / 20.0;
	threshold =	threshold >	-8 ? threshold : -8;

	// Check the minimum coverage
	if ((lr<threshold)||(rr<threshold))	
	{
		/*		
		DB fprintf (stderr,	"	  Rectangle not accepted (text	not	complete: lr %d	rr %d threshold	%.2f:\n",lr,rr,threshold);
		   print (stderr);
		   other.print (stderr);
		DE
        */
		return false;
	}
	else
		return true;
}

// ********************************************************************
// Checks the same thing as RectText::textHorizComplete(), but
// uses a statistical test on the distribution of the angles of 
// error pixels
// Result:
// true .... verification passes (constraints satisfied)
// false ... verification does not pass (constraints violated)
// ********************************************************************

/*
bool RectEval::errorAnglesAreUniform (const RectEval &other, float alpha) const 
{
	return (// this->errorAnglesAreUniformOneDirection (other, alpha) &&
	        other.errorAnglesAreUniformOneDirection (*this, alpha));
}
*/

// ********************************************************************
// Help functions for the statistical test using the distribution of
// angles
// ********************************************************************

/*
float angle (int x1, int y1, int x2, int y2)
{
	float delta_y = y2-y1;
	float delta_x = x2-x1;
	float angle;
		
	// Special case in order to avoid division by zero
	if (delta_x==0)
	{
		if(delta_y>=0)
			angle=M_PI_2;
		else
			angle=-1.0*M_PI_2;
	}
		
	// Normal case
	else
		angle=atanf(delta_y/fabs(delta_x));
		
	// Perform some operations which convert the angle to 
	// a compass like interval
	angle=M_PI_2-angle;	
	if (delta_x<0)
		angle = 2*M_PI-angle;
	
	// cerr << "ANGLE " << x1 << "," << y1 << " " << delta_x << "," << delta_y << " = " << angle << " = " << angle*180/M_PI << endl; 
	
	return angle; 
}

#define MIN(x,y)	((x)<=(y)?(x):(y))
#define MAX(x,y)	((x)>(y)?(x):(y))

// ********************************************************************
// Checks the same thing as RectEval::textHorizComplete(), but
// uses a statistical test on the distribution of the angles of 
// error pixels
//
// This is the function which does it for one direction
// (GT->detected) or (detected->GT). 
// It is called twice by the method errorAnglesAreUniform().
//
// It only treats the error pixels which are part of 
// *this but _not_ part of other.
//
// Return value: result of the Kolmogorov-Smirnov test
// ********************************************************************


bool RectEval::errorAnglesAreUniformOneDirection (const RectEval &other, float alpha) const 
{
	Histogram<double> error (50, 0., 2.*M_PI, false);
	Histogram<double> unif  (50, 0., 2.*M_PI, false);
	int ocx, ocy;
	int N;
	double ks;
	
	// Get the center point
	other.center(ocx, ocy);
	
	cerr << "EVALUATION: KS-ANGELS: this : " << left << "," << top << "-" << right << "," << bottom << endl;
	cerr << "EVALUATION: KS-ANGELS: other: " << other.left << "," << other.top << "-" << other.right << "," << other.bottom << endl;
	cerr << "EVALUATION: KS-ANGELS: center=(" << ocx << "," << ocy << ")\n"; 

	// Error at the top
	if (top<other.top)
	{
		for (int y=top; y<other.top; ++y)
		for (int x=left; x<=right; ++x)
			error.add(angle(x,y,ocx,ocy));					
	}
	
	// Error at the bottom
	if (bottom>other.bottom)
	{
		for (int y=bottom; y>other.bottom; --y)
		for (int x=left; x<=right; ++x)
			error.add(angle(x,y,ocx,ocy));					
	}
	
	// Error at the right side: do not include the parts which
	// have already been treated by one of the two other parts (top and bottom)
	if (left<other.left)
	{
		int ttop=MAX(top,other.top);		
		int tbot=MIN(bottom, other.bottom);
		for (int x=left; x<other.left; ++x)
		for (int y=ttop; y<tbot; ++y)
			error.add(angle(x,y,ocx,ocy));	
	}
		
	// Error at the left side: do not include the parts which
	// have already been treated by one of the two other parts (top and bottom)
	if (right>other.right)
	{
		int ttop=MAX(top,other.top);		
		int tbot=MIN(bottom, other.bottom);		
		for (int x=right; x>other.right; --x)
		for (int y=ttop; y<tbot; ++y)
			error.add(angle(x,y,ocx,ocy));					
	}
	
	N = (int) rint(error.sum());
	
	// Create the uniform histogram
	unif.createUniformDistributionFromValue(1.);
	unif.normalize();
	unif.toCumulative();
			
	// Error histogram: normalize in an angular way, 
	// i.e. divide each bin by size of the segment in the other rectangle
	
	for (int i=0; i<error.getSize(); ++i)
	{
		error.bins[i] = error.bins[i] / other.segmentSize (
			error.getBinCenter(i), error.getStepSize());		
	}

	error.normalize();		
				
	cerr << "EVALUATION: ERROR-HIST" << endl;
	cerr << error;
	
			
	error.toCumulative();	
	
	// Perform the test
	ks = error.kolmogorovSmirnovTest(unif);
			
	cerr << "EVALUATION: ks=" << ks << endl;
	cerr << "EVALUATION: ks_limit=" << getLimitKolmogorovSmirnov(alpha, N) << endl;
	
	// Check the result against the test table
	return (ks <= getLimitKolmogorovSmirnov(alpha, N));
}
*/
