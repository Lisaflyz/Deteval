/***********************************************************************
 * OptionContainer.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 * 
 * Changelog:
 * 17.07.2005 cw: First version (for bib2html)
 * 09.05.2006 cw: generalisation 
 *
 * 1 tab = 4 spaces
 ***********************************************************************/
 
#ifndef _WOLF_OPTION_CONTAINER_H_ 
#define _WOLF_OPTION_CONTAINER_H_

// C++
#include <string>

// STL
#include <map>

// From this module
#include "CIL.h"

using namespace std;

// Return types for the run method
// MUST BE POWERS OF 2!!!
#define OCOPT_USAGE 		1
#define OCOPT_VERSION		2
 
/*****************************************************************************
 * A class for storage of the program options
 *****************************************************************************/
 
class OptionContainer
{
	public:
		OptionContainer ()					{ argv=NULL; }
	
		// Get an option 
		string getS(string s)				{ return so[s]; }
		bool   getB(string s)				{ return bo[s]; }
		int    getI(string s)				{ return io[s]; }
		float  getF(string s)				{ return fo[s]; }
		
		// Define an option
		void defS (string s, string v)		{ so[s] = v; }
		void defB (string s, bool b)		{ bo[s] = b; }
		void defI (string s, int i)			{ io[s] = i; }
		void defF (string s, float f)		{ fo[s] = f; }
		
		// Set an already defined option
		void set (string s);  
		
		// run the container on an argument set
		int run (int argc, char **argv);
		
		// Access the non-option arguments
		int noNonOpts();
		char *getNextNonOpt();
		
	private:
	
		// The string and boolean options
		map<string,string> so;		
		map<string,bool>   bo;				
		map<string,int>    io;		
		map<string,float>  fo;
						
		int argc;
		char **argv;
		int nonOptPointer;
}; 

#endif
