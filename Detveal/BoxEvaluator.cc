/*********************************************************************
 * BoxEvaluator.cc
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * 1 tab = 4 spaces
 *********************************************************************/

// C++
#include <vector>
#include<fstream>
// From this module
#include "BoxMatrix.h"
#include "BoxEvaluator.h"
#include<string>

// Fixed parameters: the ksc function which determines how 
// fragmentation is penalised
#define MTYPE_WEIGHT_MISSED				0.0
#define MTYPE_WEIGHT_OO_PRE				1.0
#define MTYPE_WEIGHT_OO_O				1.0
#define MTYPE_WEIGHT_OM_O				0.8
#define MTYPE_WEIGHT_OM_M				0.8
#define MTYPE_WEIGHT_MM_M				0.3

double BoxEvaluator::MatchTypeArr[] = 
{
	MTYPE_WEIGHT_MISSED,
	MTYPE_WEIGHT_OO_PRE,
	MTYPE_WEIGHT_OO_O,		
	MTYPE_WEIGHT_OM_O,		
	MTYPE_WEIGHT_OM_M,		
	MTYPE_WEIGHT_MM_M			
};

// The index in the parameter array which contains 
// the configurable parameters

char *EvalParameterDefaults = 
	"0.8,"	// tr: EV_PARAM_IND_ONE_G
	"0.4,"	// tp: EV_PARAM_IND_ONE_D
	"0.8,"	// tr: EV_PARAM_IND_OM_SUM_O_G
	"0.4,"	// tp: EV_PARAM_IND_OM_M_D
	"0.4,"	// tp: EV_PARAM_IND_OM_SUM_O_D
	"0.8,"	// tr: EV_PARAM_IND_OM_M_G 
	"0,"	//     EV_PARAM_IND_BOOL_HORIZ_TEST
	"1"		//     EV_PARAM_IND_CENTER_DIFF_THR
	;

// *******************************************************************
// Helper function: determine whether two object types are compatible,
// if they are set (i.e. != -1)
// *******************************************************************


bool objTypesCompatible (RectVectorAccessor *l1, RectVectorAccessor *l2, 
	unsigned int i1, unsigned int i2)
{	
	if (((*l1)[i1]->objectType != (*l2)[i2]->objectType) && 
		((*l1)[i1]->objectType != -1) && 
		((*l2)[i2]->objectType != -1))
	{
// 		cerr << "False match type: " << (int) (*l1)[i1]->objectType 
// 			 << "!=" << (int) (*l2)[i2]->objectType << "!\n";
		return false;
	}
	else
		return true;
}

// *******************************************************************
// Search for one to one matches
// *******************************************************************

void BoxEvaluator::searchOO	(std::ostream *ost) 
{
	double rInc, pInc;
	double normDist;
	bool chk_overlap,
	     chk_horiz,
		 chk_center_diff;

#ifdef DEBUG_BOXEVALUATOR	
	cerr << "searchOO()"<<endl;
#endif	   
	
//#warning POTENTIAL BUG HERE: mG mG or mG mD????	
	for	(unsigned int y=0; y<mG->ysize(); ++y)	
	{
		for	(unsigned int x=0; x<mG->xsize(); ++x)	
		{
			// Check for correct object type
			if (!objTypesCompatible(pG,pD,x,y))
				continue;			

			// Found a one to one match
			if (mG->isSingleOverlap(x,y) &&	mD->isSingleOverlap(x,y))  
			{			
				// Check if the overlap is big enough
				chk_overlap = ((mG->get(x,y) >= params[EV_PARAM_IND_ONE_G]) &&	
				               (mD->get(x,y) >= params[EV_PARAM_IND_ONE_D]));
							   
				// If we are supposed to, check if the overlap information is equally distributed
				// across the rectangle. If not, than even a small error might be not so good.
				// There are two methods available: a heuristic (textHorizComplete())
				// and a KolmogorovSmirnovTest which checks the distribution of the angles against
				// a uniform distribution (errorAnglesAreUniform()).							
				
				if (params[EV_PARAM_IND_BOOL_HORIZ_TEST]>0) 
					chk_horiz = (*pG)[x]->textHorizComplete(*((*pD)[y]),true);
				else
					chk_horiz = true;
																				
				// Check the normalized distance between the two rectangle centers
				normDist = (*pG)[x]->centerDistance(*((*pD)[y])); 			
				normDist /= ((*pG)[x]->diag() + (*pD)[y]->diag());
				normDist *=2.0;
				
				chk_center_diff = (normDist<params[EV_PARAM_IND_CENTER_DIFF_THR]);			
															
				// If all checks pass, then we can count the match. 
				// If the ks test doesn't match but the 
				if (chk_overlap && chk_center_diff)
				{
#ifdef DEBUG_BOXEVALUATOR	
			cerr << "   acc ";			
#endif

					(*pG)[x]->matchType = MTYPE_OO_PRE;
					(*pD)[y]->matchType	= MTYPE_OO_PRE;

					// See research notebook 3.7.2003, pp95-96
					rInc=MatchTypeArr[MTYPE_OO_O];
					pInc=MatchTypeArr[MTYPE_OO_O];
					recallAccum+=rInc;
					precisionAccum+=pInc;
    				areaOverlap+= (mG->get(x,y) * (*pG)[x]->area());

					if (ost!=NULL) // && ((mG->get(x,y)>0)||(mD->get(x,y)>0))) 
					{
#ifdef DETAILED_EVAL_OUTPUT					
    					*ost << "      <one2one surfR=\"" << mG->get(x,y) << "\""
    					     << " surfP=\"" << mD->get(x,y) << "\""
    					     << " areaGT=\"" << (*pG)[x]->area() << "\""
    					     << " areaD=\"" << (*pD)[y]->area() << "\""
              				 << " rInc=\"" << rInc << "\""
                   			 << " pInc=\"" << pInc << "\""
                       		 << " no=\"1\""
              				 << "/>\n";
#endif              				 
    				}
				}
#ifdef DEBUG_BOXEVALUATOR	
				else
				{
					cerr << "   acc ";			
				}
				
				fprintf (stderr, "	%6.4f:	(%6.4f)	",
					mG->get(x,y), mD->get(x,y));
				(*pG)[x]->print (stderr);
				fprintf (stderr, "                            ");   
				(*pD)[y]->print (stderr);
#endif	   
			}
		}
	}
}

// *******************************************************************
// Search for one-to-many and many-to-one matches
// *******************************************************************

void BoxEvaluator::searchOM (bool oneIsGT, float thOneSum, float thMany, std::ostream *ost) 
{
	int	countOne, countMany;
	BoxMatrix *mOne, *mMany;
	RectVectorAccessor *lOne, *lMany;
	vector<int>	manyList;
	float oneSum, manySingle;
 	float allManySum;
	int no_allMany, allManyArea;
	double rInc, pInc;
	
#ifdef DEBUG_BOXEVALUATOR
	fprintf (stderr,"searchOM(%s,thOneSum=%.4f,thMany=%.4f)\n",
		(oneIsGT?"OneIsGT":"OneIsDet"),thOneSum,thMany);
#endif	

	// Many	of the structures depend on	whether	the	ground truth
	// or the found	rectangles are the "one" or	the	"many" side.
	if (oneIsGT) 
	{
		countOne = pG->size();
		countMany =	pD->size();
		mOne = mG; lOne	= pG;
		mMany =	mD;	lMany =	pD;
	}
	else 
	{
		countOne = pD->size();
		countMany =	pG->size();
		mOne = mD; lOne	= pD;
		mMany =	mG;	lMany =	pG;
	}

	// ----------------------------------------------------------------
	// Travers the different possible "one"	rectangles
	// ----------------------------------------------------------------

	for	(int co=0; co<countOne;	++co) 
	{

		// We already matched the rectangle
		if ((*lOne)[co]->matchType != MTYPE_MISSED && (*lOne)[co]->matchType != MTYPE_OO_PRE)
			continue;

		// The "one" rectangle doesn't have	more than one overlap -> skip it
		if (mOne->getCountConditional(oneIsGT, co) <= 1)
			continue;

#ifdef DEBUG_BOXEVALUATOR
		fprintf (stderr,	"[ONE: %d overlaps]", mOne->getCountConditional(oneIsGT, co));
		   (*lOne)[co]->print (stderr);
		   fprintf (stderr,	"[MANYs:]\n");
#endif

		// ----------------------------------------------------------------
		// Search the possible "many" partners for this	"one" rectangle
		// ----------------------------------------------------------------

		manyList.clear();
		oneSum = 0;
  		allManySum = 0;
    	no_allMany = 0;
     	allManyArea = 0;

		for	(int cm=0; cm<countMany; ++cm) 
		{
			// We already matched the rectangle
			if ((*lMany)[cm]->matchType	!= MTYPE_MISSED)
				continue;

    		manySingle = mMany->getConditional(oneIsGT, co, cm);
    		
			// Only	if the rectangle is	covered	by almost all it's area
			// it can be in	a many relationship
			if	(manySingle >= thMany) 
			{
				// Check for correct object type				
				if (objTypesCompatible(lMany,lOne,cm,co))
				{											
					manyList.push_back(cm);
					oneSum += (mOne->getConditional	(oneIsGT, co, cm));
#ifdef DEBUG_BOXEVALUATOR
					cerr << "   acc ";				
#endif
				}
			}
#ifdef DEBUG_BOXEVALUATOR	
			else
			{
				cerr << "** rej ";
			}
			
			fprintf (stderr,	"	%6.4f:	(%6.4f)	",
				mMany->getConditional (oneIsGT,	co,	cm),
				mOne->getConditional (oneIsGT, co, cm));
			(*lMany)[cm]->print (stderr);
#endif	    		
   			allManySum += manySingle;
    		allManyArea += (*lMany)[cm]->area();
    		++no_allMany;
		}

  		// The surface of all the manies is the sum of all of them.
    	// since manySingle gave only the part of each of them, we need
     	// to divide by the number of manies.
  		allManySum /= no_allMany;
#ifdef DEBUG_BOXEVALUATOR
		fprintf (stderr,	"	  oneSum: %.4f (thr=%.4f)\n", oneSum, thOneSum);
#endif		

		// ----------------------------------------------------------------
		// Only	one	of the many-rectangles qualified ->
		// This	match degraded to a	one-to-one match
		// ----------------------------------------------------------------

		if (manyList.size()	== 1) 
		{
			int	cmFirst	= (*(manyList.begin()));
   			double tg, td;   			
#ifdef DEBUG_BOXEVALUATOR			
			fprintf (stderr,	"	  --> only one of the overlaps qualifies -> match is one2one.\n");
#endif	
			// We still	need to	check the thresholds
   			tg = mG->getConditional(oneIsGT,co,cmFirst);
      		td = mD->getConditional(oneIsGT,co,cmFirst);
			if ((tg	>= params[EV_PARAM_IND_ONE_G]) && (td >= params[EV_PARAM_IND_ONE_D])) 
			{

			    (*lOne)[co]->matchType = MTYPE_OO_O;
			   	(*lMany)[cmFirst]->matchType	= MTYPE_OO_O;
				// 	fprintf (stderr,	"	  --> degrades to:\n");

				// See research notebook 3.7.2003, pp95-96
				rInc=MatchTypeArr[MTYPE_OO_O];
				pInc=MatchTypeArr[MTYPE_OO_O];
				recallAccum+=rInc;
				precisionAccum+=pInc;
    			areaOverlap += (tg * (*lOne)[co]->area());

          		if (ost!=NULL) 
				{
#ifdef DETAILED_EVAL_OUTPUT
    				*ost << "      <one2one surfR=\"" << tg << "\""
    				     << " surfP=\"" << td << "\""
    				     << " areaGT=\"" << (*lOne)[co]->area() << "\""
    				     << " areaD=\"" << (*lMany)[cmFirst]->area() << "\""
             			 << " rInc=\"" << rInc  << "\""
                         << " pInc=\"" << pInc << "\""
                         << " no=\"1\""
                         << "/>\n";
#endif
    			}
			} 
#ifdef DEBUG_BOXEVALUATOR			
			else {				 
				fprintf (stderr,	"	  --> did not qualify as one-to-one match\n");
			}
#endif			

			continue;
		}

		// ----------------------------------------------------------------
		// The area	of the "one" rectangle,	covered	by all the "many"
		// rectangles, is bigger than the threshold
		// --> we found	a "one-to-many"	relationship
		// ----------------------------------------------------------------

		if (oneSum < thOneSum)
		{
#ifdef DEBUG_BOXEVALUATOR		
			cerr << "rectangle did not satisfy oneSum constraint:\n"
				 << "    oneSum=" << oneSum << ", thOneSum=" << thOneSum << endl;
#endif				 
		}
		
		else 
		{
			// Mark	the	"one" rectangle
			(*lOne)[co]->matchType = MTYPE_OM_O;

			// Mark	all	the	"many" rectangles
			for	(vector<int>::iterator iter=manyList.begin(); iter!=manyList.end();	++iter)
				(*lMany)[(*iter)]->matchType = MTYPE_OM_M;

			// One ground truth	rectangle has been split up	into many
			// found rectangles. The more rectangles the more we
			// punish to match.
			if (oneIsGT) 
			{
				rInc=0;
				
				// See research notebook 3.7.2003, pp95-96
				// the expression is a simplified version of a more complicated one
				rInc=MatchTypeArr[MTYPE_OM_O];
				pInc=manyList.size()*MatchTypeArr[MTYPE_OM_O];
				recallAccum+=rInc;
				precisionAccum+=pInc;
           		areaOverlap += (oneSum * (*lOne)[co]->area());

             	if (ost!=NULL) 
				{
#ifdef DETAILED_EVAL_OUTPUT				
                	*ost << "      <many2onegt surfR=\"" << oneSum << "\""
    				     << " surfP=\"" << allManySum << "\""
    				     << " areaGT=\"" << (*lOne)[co]->area() << "\""
    				     << " areaD=\"" << allManyArea << "\""
    				     << " rInc=\"" << rInc << "\""
                         << " pInc=\"" << pInc << "\""
                         << " no=\"" << manyList.size() << "\""
                         << "/>\n";
#endif
    			}
			}

			// More	than one ground	truth rectangle	has	been merged	to
			// a single	found rectangle.
			// -> For each real	rectangle increase the score
			else 
			{
				// See research notebook 3.7.2003, pp95-96
				// the expression is a simplified version of a more complicated one
				rInc=manyList.size()*MatchTypeArr[MTYPE_OM_O];
				pInc=MatchTypeArr[MTYPE_OM_O];
				recallAccum+=rInc;
				precisionAccum+=pInc;
               	areaOverlap += (allManySum * allManyArea);

                if (ost!=NULL) 
				{
#ifdef DETAILED_EVAL_OUTPUT
               		*ost << "      <manygt2one surfR=\"" << allManySum << "\""
    				     << " surfP=\"" << oneSum << "\""
    				     << " areaGT=\"" << allManyArea << "\""
    				     << " areaD=\"" << (*lOne)[co]->area() << "\""
    				     << " rInc=\"" << rInc  << "\""
                         << " pInc=\"" << pInc << "\""
                         << " no=\"" << manyList.size() << "\""
                         << "/>\n";
#endif
             	}
			}
		}
	}
}

// *******************************************************************
// Compute Precision and recall according to the method developped
// for the text locating competition of the ICDAR 2003 conference
// *******************************************************************

void BoxEvaluator::prICDAR2003 (double &out_p, double &out_r) 
{
	double sumMatchScoresR=0,
	       sumMatchScoresP=0;
    int noG=pG->size(),
    	noD=pD->size();
    int foundIndex;

    // RECALL: Travers the targets and search the closest matchs
    for (int g=0; g<noG; ++g) 
	{
    	double ov, matchscore, maxmatchscore;

    	foundIndex=-1;
    	maxmatchscore=-1;

    	for (int d=0; d<noD; ++d) 
		{
     		ov =  (mG->get(g,d) * (*pG)[g]->area());
     		matchscore = (2* ov / ((*pG)[g]->area()+(*pD)[d]->area()));

     		if (matchscore>maxmatchscore)
       			maxmatchscore=matchscore;
     	}

      	// We found an overlap for this rectangle
       	if (maxmatchscore>0)
          	sumMatchScoresR += maxmatchscore;
    }

    // PRECISION: Travers the detected rectangles and search the closest matchs
    for (int d=0; d<noD; ++d) 
	{
    	double ov, matchscore, maxmatchscore;

    	foundIndex=-1;
    	maxmatchscore=-1;

    	for (int g=0; g<noG; ++g) 
		{
            ov =  (mG->get(g,d) * (*pG)[g]->area());
     		matchscore = (2* ov / ((*pG)[g]->area()+(*pD)[d]->area()));

       		if (matchscore>maxmatchscore)
       			maxmatchscore=matchscore;
     	}

      	// We found an overlap for this rectangle
       	if (maxmatchscore>0)
          	sumMatchScoresP += maxmatchscore;
    }


    if (noG==0) 
	{
    	if (noD==0)
     		out_r = 1.0;
       	else
        	out_r = 0.0;

    }
    else
    	out_r = sumMatchScoresR/noG;

    if (noD==0) 
	{
    	if (noG==0)
     		out_p = 1.0;
       	else
        	out_p = 0.0;

    }
    else
    	out_p = sumMatchScoresP/noD;
}


// *******************************************************************
// Search the matches between the rectangles
// calculate the distance between the ground truth and the found
// rectangles
// *******************************************************************

double BoxEvaluator::matchScore	(std::ostream *ost, std::ostream *o2, int numberGTRectangles) 
{
	int gs=pG->size();
	int ds=pD->size();
	double recall, precision, hmean;
	double totAreaGT=0, totAreaD=0;

	/*
	cerr << "Evaluation has parameters: ";
	for (unsigned int i=0; i<EV_PARAM_COUNT; ++i)
		cerr << params[i] << " ";
	cerr << endl;
	*/

	precisionAccum=recallAccum=0;
	areaOverlap=0;

	for (int i=0; i<gs; ++i)
		totAreaGT += (*pG)[i]->area();
	for (int i=0; i<ds; ++i)
		totAreaD += (*pD)[i]->area();
	ofstream totalFile("totalFile.txt");
	// One of the lists is empty
	if (gs==0 || ds==0) 
	{
		*o2 << gs << " " << ds << " " << recallAccum << " " << precisionAccum << "\n";
		double res = (ds==gs?1:0);

		if (ost!=NULL) {

            *ost << "    <icdar2003 r=\"" << res << "\""
			 			<< " p=\"" << res  << "\""
			   			<< " hmean=\"" << res<< "\""
			      		<< " noGT=\"" << gs << "\""
			   			<< " noD=\"" << ds << "\""
			     		<< "/>\n"
			 	 << "    <score r=\"" << res << "\""
			 			<< " p=\"" << res   << "\""
			   			<< " hmean=\"" << res << "\""
			    		<< " noGT=\"" << gs << "\""
			   			<< " noD=\"" << ds << "\""
			     		<< "/>\n"
#ifdef DETAILED_EVAL_OUTPUT
			     << "    <scoreSurf r=\"" << res << "\""
			 			<< " p=\"" << res   << "\""
			   			<< " hmean=\"" << res << "\""
			    		<< " surfGT=\"" << totAreaGT << "\""
			      		<< " surfD=\"" << totAreaD  << "\""
			     		<< "/>\n"
#endif
			     ;
  		}

    	return res;
	}

	// normal operation: both lists have elements
	else {
#ifdef DETAILED_EVAL_OUTPUT	
		if (ost!=NULL)
			*ost << "    <thresholdedPairs>\n";
#endif			
	}

	// ---------------------------------------------------------------
	// Search the corresponding rectangles
	// ---------------------------------------------------------------


	// Search for one to one matches. Do not add a score yet.
	searchOO(ost);

	// Search for one-to-many and many-to-one matches
	// cerr << "ONE IS GROUND TRUTH\n";
	searchOM (true,	params[EV_PARAM_IND_OM_SUM_O_G], params[EV_PARAM_IND_OM_M_D], ost);
	// cerr << "MANY IS GROUND TRUTH\n";
	searchOM (false, params[EV_PARAM_IND_OM_SUM_O_D], params[EV_PARAM_IND_OM_M_G], ost);

	// ---------------------------------------------------------------
	// Output
	// ---------------------------------------------------------------

	recall    = recallAccum / (double) gs;
	precision = precisionAccum / (double) ds;
	hmean = (precision+recall==0 ? 0 : 2.0*precision*recall/(precision+recall));
	string temp = "";
	cout << gs << " " << ds << " " << recallAccum << " " << precisionAccum << endl;
	//*o2 << gs << " ";
	*o2 << gs << " " << ds  << " " << recallAccum << " " << precisionAccum <<"\n";
    if (ost!=NULL) 
	{
    	double icdarP, icdarR, icdarH;
#ifdef DETAILED_EVAL_OUTPUT     	    	
    	double surfR = (totAreaGT==0 ? 0 : areaOverlap/totAreaGT);
     	double surfP = (totAreaD==0 ? 0 : areaOverlap/totAreaD);
      	double surfH = (surfP+surfR==0 ? 0 : 2.0*surfP*surfR/(surfP+surfR));
#endif      	

       	// calculate the performance according the ICDAR 2003 criteria
       	prICDAR2003 (icdarP, icdarR);
        icdarH = (icdarP+icdarR==0 ? 0 : 2.0*icdarP*icdarR/(icdarP+icdarR));
        
#ifdef DETAILED_EVAL_OUTPUT     	
		*ost << "    </thresholdedPairs>\n";
#endif				
		*ost << "    <icdar2003 r=\"" << icdarR  << "\""
			 		<< " p=\"" << icdarP << "\""
			   		<< " hmean=\"" << icdarH << "\""
			     	<< " noGT=\"" << gs   << "\""
			   		<< " noD=\"" << ds   << "\""
			     	<< "/>\n"
			 << "    <score r=\"" << recall << "\""
			 		<< " p=\"" << precision << "\""
			   		<< " hmean=\"" << hmean << "\""
			     	<< " noGT=\"" << gs   << "\""
			   		<< " noD=\"" << ds   << "\""
			 		<< "/>\n";

//#ifdef DETAILED_MATCH_RESULTS
		for (int t=0; t<ds; ++t){
			*ost << "    <taggedRectangle x=\"" << (*pD)[t]->left << "\""
						<< " y=\"" << (*pD)[t]->top << "\""
						<< " width=\"" << (*pD)[t]->width() << "\""
						<< " height=\"" << (*pD)[t]->height()   << "\""
						<< " match=\"" << (*pD)[t]->matchType << "\""
						<< " isGT=\"false\""
						<< "/>\n";
		}
		for (int k=0; k<gs; ++k){
			*ost << "    <taggedRectangle x=\"" << (*pG)[k]->left << "\""
			<< " y=\"" << (*pG)[k]->top << "\""
			<< " width=\"" << (*pG)[k]->width() << "\""
			<< " height=\"" << (*pG)[k]->height()   << "\""
			<< " match=\"" << (*pG)[k]->matchType << "\""
			<< " isGT=\"true\""
			<< "/>\n";
		}
//#endif


#ifdef DETAILED_EVAL_OUTPUT
            *ost << "    <scoreSurf r=\"" << surfR << "\""
				 		<< " p=\"" << surfP << "\""
				 		<< " hmean=\"" << surfH  << "\""
				 		<< " surfGT=\"" << totAreaGT << "\""
				 		<< " surfD=\"" << totAreaD  << "\""
				 		<< "/>\n";
#endif
	}

	return hmean;
}

