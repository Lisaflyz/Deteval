/***********************************************************************
 * The declarations for the exeception handling
 *
 * Author: Christian Wolf
 * Begin: 4.3.2005
 ***********************************************************************/
 
#ifndef _WOLF_ERROR_HANDLING_H_
#define _WOLF_ERROR_HANDLING_H_

// C++
#include <string>
#include <sstream>

using namespace std;

/*****************************************************************************
 * MACROS
 *****************************************************************************/

#define ERR_THROW(x) {ostringstream s; s << x; throw EError(s.str());}

/*****************************************************************************
 * The base class: a generic error
 *****************************************************************************/
class EError 
{
	public:

		EError () 			{}	
		EError (string s)	{ message=s; }
		EError (char *s) 	{ message=string(s); }
		
		string message;
};

/*****************************************************************************
 * An out of bounds error occured 
 * (in an image or a matrix)
 *****************************************************************************/
 
class EBoundsError : public EError
{	
	public:

		EBoundsError (int xx, int xy, int xxsize, int xysize)
		{
			ostringstream s;
			x=xx; y=xy; xsize=xxsize; ysize=xysize;
			s << "Coordinates out of bounds: size=[" << xsize << "," << ysize << "]";
			s << " access at (" << x << "," << y << ")" << endl;
			message = s.str();
		}
		
		// the accessed coordinates
		int x,y;
		
		// The image/matrix size
		int xsize, ysize;		
};

/*****************************************************************************
 * An out of bounds error occured 
 * (in a muli band image)
 *****************************************************************************/
 
class EBounds3DError : public EError
{	
	public:

		EBounds3DError (int xp, int xx, int xy, int xnoplanes, int xxsize, int xysize)
		{
			ostringstream s;
			p=xp; x=xx; y=xy; psize=xnoplanes; xsize=xxsize; ysize=xysize;
			s << "Coordinates out of bounds: noPlanes=" << psize 
			  << " size=[" << xsize << "," << ysize << "]"
			  << " access at plane=" << p << " pixel=(" << x << "," << y << ")" << endl;
			message = s.str();
		}
		
		// the accessed coordinates
		int p,x,y;
		
		// The image/matrix size
		int psize, xsize, ysize;		
};


/*****************************************************************************
 * An error occured in an Image Processing Task (Class IPTask)
 *****************************************************************************/
 
class ETaskError : public EError
{
	
	public:

		ETaskError (const string s) : EError (s) 
		{
			OutputImagesAvailable=false;
		}
		
		ETaskError (char *s=NULL) : EError (s) 
		{
			OutputImagesAvailable=false;
		}
				
		// Tells the interface whether the task was able to create
		// the output images
		bool OutputImagesAvailable;
};

#endif
