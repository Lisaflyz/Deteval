/*********************************************************************
 * BoxEval.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/

#ifndef	_WOLF_RECTEVAL_H_
#define	_WOLF_RECTEVAL_H_

// C
#include <stdlib.h>
#include <stdio.h>

// C++
//#include <iostream>
#include <string>

//using namespace std;
#include "visualbug.h"

// From the IMAGE library
#include "Rect.h"

typedef	unsigned char byte;

// All possible	flags of results and groundtruth
// for each	pixel and for each rectangle

enum TypeTruthType {
	GTCOLOR_BLANK  = 0,
	GTCOLOR_FOUND  = 1,
	GTCOLOR_SCENE  = 2,
	GTCOLOR_ARTIF  = 4
};

enum TypeMatchType {
	MTYPE_MISSED,
	MTYPE_OO_PRE,
	MTYPE_OO_O,
	MTYPE_OM_O,
	MTYPE_OM_M,
	MTYPE_MM_M,
};

class RectEval : public	Rect {

	public:

		// Constructors
		RectEval (): Rect()	{
			truthType =	GTCOLOR_ARTIF;
			matchType =	MTYPE_MISSED;
			objectType = -1;
		}
		RectEval (const	Rect &b)
			:Rect (b) {
			truthType =	GTCOLOR_BLANK;
			matchType =	MTYPE_MISSED;
			objectType = -1;
		}
		RectEval (const	Rect &b, TypeTruthType tt)
			:Rect(b) {
			matchType =	MTYPE_MISSED;
			truthType =	tt;
			objectType = -1;
		}
		RectEval (int t, int b,	int	l, int r, TypeTruthType	tt = GTCOLOR_ARTIF)
			:Rect (t,b,l,r)	{
			truthType =	tt;
			matchType =	MTYPE_MISSED;
			objectType = -1;
		}
		RectEval (FILE *fp);
		
		bool truthTypeHasProperty (TypeTruthType property)
		{
			return ((truthType & property)==property);
		}

		// Debug output
		friend std::ostream & operator << (std::ostream & os, RectEval &r);

		// Output in format of icdar 2003
		void writeICDAR2003 (std::ostream & os);
		
		bool textHorizComplete (const	RectEval &other, bool memberIsGT) const;
		/*
		bool errorAnglesAreUniform (const RectEval &other, float alpha) const; 
		bool errorAnglesAreUniformOneDirection (const RectEval &other, float alpha) const;		
		*/
		void readableTruthType (char *buf) const;

	public:		// DATA

		// Is it real text or text which we	do not need	to
		// detect necessarily but which	does not result	into a
		// false positive either?
		TypeTruthType truthType;

		// This	rectangle matches with another rectangle
		// with	which type?
		TypeMatchType matchType;
		
		// What type of object (for non text objects)
		int objectType;
};

ostream & operator << (ostream & os, RectEval &r);


#endif

