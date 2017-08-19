/*********************************************************************
 * BoxEvaluator.h
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * This class performs the evaluation of the text detection
 * algorithm. It gets two lists of rectangles: The rectangles
 * detected by the algorithm and the list we got from the
 * ground truth data. Both are compared and the number of
 * detected rectangles is determined.
 *
 * 1 tab = 4 spaces
 *********************************************************************/
 
#ifndef	_WOLF_BOXEVALUATER_H_
#define	_WOLF_BOXEVALUATER_H_

// From the TEXT DETECTION library
#include "BoxOverlapRelative.h"

// The number of parameters to set
#define EV_PARAM_COUNT				8

enum EvalParameterIndex {
	EV_PARAM_IND_ONE_G=0,
	EV_PARAM_IND_ONE_D,
	EV_PARAM_IND_OM_SUM_O_G,	
	EV_PARAM_IND_OM_M_D,
	EV_PARAM_IND_OM_SUM_O_D,
	EV_PARAM_IND_OM_M_G,
	EV_PARAM_IND_BOOL_HORIZ_TEST,
	EV_PARAM_IND_CENTER_DIFF_THR,
};

extern char *EvalParameterDefaults;

class BoxEvaluator : public	BoxOverlapRelative 
{

	public:

		// Constructors
		BoxEvaluator (RectVectorAccessor *lG, RectVectorAccessor *lD, double *xParameters)
			: BoxOverlapRelative	(lG, lD)
		{
			params = xParameters;
			/*
			cerr << "BoxEvaluator" << endl
				 << "mG=" << mG->m << endl
				 << "mD=" << mD->m << endl;
			*/
		};

		double matchScore (std::ostream *ost, std::ostream *o2, int numberGTRectangles);		

	private:

		void searchOO (std::ostream *ost);
		void searchOM	(bool oneIsGT, float thOneSum, float thMany, std::ostream *ost);

		void prICDAR2003 (double &out_p, double &out_r);

		
	private: 	// DATA

		double precisionAccum, recallAccum;
		double areaOverlap;

		double *params;
		
	private: 	// CLASS VARIABLES
	
		static double MatchTypeArr[];		
};

#endif

