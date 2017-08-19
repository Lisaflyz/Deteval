/***********************************************************************
 * The global declarations necessary for all modules
 *
 * Author: Christian Wolf
 * Begin: 4.3.2005
 ***********************************************************************/
 

#ifndef _WOLF_CIL_H_
#define _WOLF_CIL_H_

// C
#include <time.h>

// Exception handling
#include "ErrorHandling.h"
#include "ParameterParsing.h"

/***********************************************************************
 * A couple of commonly used macros
 ***********************************************************************/

// Uncomment this if you prefer core dumps to exceptions (for debugging)
// Must be supported explictely by the routine throwing the exception!!!
#define CORE_DUMP_ON_EXCEPTION	CORE_DUMP
// #define CORE_DUMP_ON_EXCEPTION	;

#define DEBUG_PIXEL		false
#define DEBUG_X			376
#define DEBUG_Y			338
#define DEBUGIFPX(x,y) 	if (DEBUG_PIXEL && x==DEBUG_X && y==DEBUG_Y)
#define DEBUGSTRPX		"PIXEL(" << DEBUG_X << "," << DEBUG_Y << "): "

#define DBSAVE(im,x,y,z) { char buf[100]; sprintf (buf, "%s%s%s",x,y,z); \
						(im).write(buf); }

#define	DB				if (TextDetectDoDebug) {
#define	DE				}
#define	DI				if (TextDetectDoDebug)

#define TRIMGRAY(x)		{ if((x)>255)(x)=255; if ((x)<0)(x)=0; }
#define TRIMMEDGRAY(x)	((x)>255?255:((x)<0?0:(x)))

#define	ITERATE			for	(iterator iter=begin();	iter!=end(); ++iter)
#define	CONST_ITERATE	for	(const_iterator iter=begin(); iter!=end(); ++iter)
#define	OTHER_ITERATE	for	(iterator iter=other.begin(); iter!=other.end(); ++iter)

#define DO_COLORS				for (int col=1; col<=3; ++col)
#define DO_AVAILABLE_COLORS		for (int col=1; col<=(type==3?3:1); ++col)
#define DO_COLORS_IF(x)			for (int col=1; col<=((x)>1?3:1); ++col)

#define CORE_DUMP		{ cerr << "------> PROVOKING CORE DUMP !!!!!!!!!!! <-------\n"; \
						  int *crash_pointer=0; *crash_pointer = 1; exit (1); \
						}
						
/***********************************************************************
 * profiling stuff
 ***********************************************************************/
 
#ifdef ENABLE_PROFILING

extern clock_t LastProfilingClock;
#define PROFILING_OUTPUT(x)	{ 	clock_t newClock=clock(); \
								cerr << "PROFILING: " << (x) << ": " \
									 << (float) (newClock-LastProfilingClock) / CLOCKS_PER_SEC << endl; \
								LastProfilingClock = newClock; \
							}
#else
#define PROFILING_OUTPUT(x) // dummy
#endif
/***********************************************************************/
 
#ifdef PEDANTIC_CHECK_CODE
	#define HRULE			{ for (int i=0; i<60; ++i) cerr << "="; \
					          cerr << " PEDANTIC ENABLED!!" <<endl; }				
#else					
	
	#ifdef ENABLE_PROFILING
		#define HRULE			{ for (int i=0; i<40; ++i) cerr << "="; } PROFILING_OUTPUT("");		
	#else
		#define HRULE			{ for (int i=0; i<60; ++i) cerr << "="; cerr << endl; }				
	#endif
#endif

#endif
