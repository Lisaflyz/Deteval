///***************************************************************************
// * readeval.cpp
// *
// * author: christian wolf
// *         christian.wolf@insa-lyon.fr
// *
// * changelog:
// * 24.04.2009 cw: -bugfix: no break after many files not containing text
// * 19.04.2006 cw: -added the comparison between two xml files
// * 27.04.2004 cw: -first version 
// *
// * 1 tab = 4 spaces
// ***************************************************************************/
// 
//#define version		"version 2.01, 24.04.2009" 
//
//// c
//#include <math.h>
//#include <io.h>
//#include <process.h>
//#include "getopt.h"
//// c++
//#include <iostream>
//#include <iomanip>
//#include <fstream>
//#include <sstream>
//#include <set>
//#include <map>
//
//// libxml2
//#include <libxml/xmlmemory.h>
//#include <libxml/parser.h>
//#pragma comment(lib,"libxml2.lib")
//using namespace std;
//
//// which type of xml file do we process?
//enum filetype 
//{
//	ft_no_type=0,
//	ft_set,
//	ft_series
//};
//
//// which of the result measures do we print?
//enum printmeasure 
//{
//	pm_no_print=0,
//	pm_print_r,
//	pm_print_p,
//	pm_print_h,
//	pm_print_mode_msd,
//	pm_print_mode_sh,
//	pm_print_mode_sdw
//};
//
//
//// ********************************************************************
//// the class for an evaluation result (possibly for a set of images)
//// ********************************************************************
//
//class evresults 
//{
//	public:
//
//		evresults() { clear(); }
//		void clear() 
//		{
//			crispr=crispp=crispnogt=crispnod=0;
//			icdarr=icdarp=icdarsurfgt=icdarsurfd=0;
//			surfr=surfp=surfgt=surfd=0;
//			modessda=modessdb=0;
//			modesheight=modesdiffwidth=0;
//			modenopixels=modenont=modenoignored=0;
//			id=noimages=0;
//		}
//		
//		bool operator== (const evresults &o) const
//		{
//			return (crispr==o.crispr) && (crispp==o.crispp) && 
//			       (crispnogt==o.crispnogt) && (crispnod==o.crispnod) &&
//			       (noimages==o.noimages);			      
//		}
//		
//		
//		bool isgreaterthan (const evresults &o) const
//		{
//			return (crispr>o.crispr) || (crispp>o.crispp);
//		}
//		
//		bool islessthan (const evresults &o) const
//		{
//			return (crispr<o.crispr) || (crispp<o.crispp);
//		}
//
//		void printoneresult (ostream &st, printmeasure pm) const;
//        void printresultstag(ostream &st, const char *parameterstring,
//        	bool domodeestev, bool dolatex) const;
//
//        void add (const evresults &e);
//
//	public:		// data
//
//		// detection evaluation
//		double crispr,crispp,crispnogt,crispnod;
//		double icdarr,icdarp,icdarsurfgt,icdarsurfd;
//		double surfr,surfp,surfgt,surfd;
//
//		// mode estimation evaluation
//		double modessda, modessdb;
//		int modenopixels, modenoignored;
//		double modesheight, modesdiffwidth;
//		int modenont;
//
//		bool haveparameter;
//		double parameter;
//
//		unsigned int id;
//		unsigned int noimages;
//		
//		string imagename;
//};
//
//// ********************************************************************
//// different functionals to sort the results
//// ********************************************************************
//
//class sortid: public binary_function<evresults,evresults,bool> 
//{
//	public:
//		bool operator()(const evresults &x, const evresults &y) const 
//		{
//			return x.id < y.id;
//		}
//};
//
//class sortgt: public binary_function<evresults,evresults,bool> 
//{
//	public:
//		bool operator()(const evresults &x, const evresults &y) const 
//		{
//			if (x.crispnogt!=y.crispnogt)
//				return x.crispnogt < y.crispnogt;
//			else
//				return x.id < y.id;
//		}
//};
//
//class sortname: public binary_function<evresults,evresults,bool> 
//{
//	public:
//		bool operator()(const evresults &x, const evresults &y) const 
//		{
//			return x.imagename < y.imagename;
//		}
//};
//
//// ********************************************************************
//// add the results of one to the sum of another
//// ********************************************************************
//
//void evresults::add (const evresults &e) 
//{
//	crispr += (e.crispr * e.crispnogt);	
//    crispp += (e.crispp * e.crispnod);
//    crispnogt += e.crispnogt;    
//    crispnod  += e.crispnod;
////     cerr << "aaa step: " << e.crispnod << "-->" << crispnod
////     	 << "   [ " << e.crispp << " " << e.crispr << " " << e.crispnod << " " << e.crispnogt << "]\n";
//    icdarr += (e.icdarr * e.icdarsurfgt);
//    icdarp += (e.icdarp * e.icdarsurfd);
//    icdarsurfgt += e.icdarsurfgt;
//    icdarsurfd  += e.icdarsurfd;
//    surfr += (e.surfr * e.surfgt);
//    surfp += (e.surfp * e.surfd);
//    surfgt += e.surfgt;
//    surfd  += e.surfd;
//    noimages += e.noimages;
//    modessda += e.modessda;
//    modessdb += e.modessdb;
//    modenopixels += e.modenopixels;
//    modenoignored += e.modenoignored;
//    modesheight += e.modesheight;
//    modesdiffwidth += e.modesdiffwidth;
//    modenont += e.modenont;
//}
//
//// ********************************************************************
//// print one of the results in plain ascii text
//// ********************************************************************
//
//void evresults::printoneresult (ostream &st, printmeasure pm) const 
//{
//	double crisph = (crispp+crispr==0 ? 0 : 2.0*crispp*crispr/(crispp+crispr));
//
//   	if (haveparameter)
//   		st << parameter << " ";
//
//   	switch (pm) {
// 		case pm_print_r:
//   			st << crispr << "\n";
//      		break;
//
// 		case pm_print_p:
//   			st << crispp << "\n";
//      		break;
//
// 		case pm_print_h:
//   			st << crisph << "\n";
//      		break;
//
//        case pm_print_mode_msd:
//        	st << (modessda+modessdb)/(float)modenopixels << "\n";
//         	break;
//
//        case pm_print_mode_sh:
//        	st << modesheight/(float)modenont << "\n";
//         	break;
//
//        case pm_print_mode_sdw:
//        	st << modesdiffwidth/(float)modenont << "\n";
//         	break;
//          
//
//        default:
//        	std::cerr << "internal error (2).\n";
//         	std::exit (1);
//  	}
//}
//
//// ********************************************************************
//// print the results
//// ********************************************************************
//
//void evresults::printresultstag(ostream &st, const char *parameterstring,
//	bool domodeestev, bool dolatex) const 
//{
//
//	double divcrispr,divcrispp,divcrisph,
// 		   divicdarr, divicdarp, divicdarh,
//		   divsurfr,divsurfp,divsurfh;
//		   
//// 	cerr << "aaa dividing: " << crispp << "/" << crispnod << "=" << crispp/crispnod << endl;
//
//  	// score
//  	divcrispr = (crispnogt==0 ? 0 : crispr/crispnogt);
//   	divcrispp = (crispnod==0  ? 0 : crispp/crispnod);
//    divcrisph = (divcrispp+divcrispr==0 ? 0 : 2.0*divcrispp*divcrispr/(divcrispp+divcrispr));
//
//    // icdar 2003
//    divicdarr = (icdarsurfgt==0 ? 0 : icdarr/icdarsurfgt);
//    divicdarp = (icdarsurfd==0  ? 0 : icdarp/icdarsurfd);
//    divicdarh = (divicdarp+divicdarr==0 ? 0 : 2.0*divicdarp*divicdarr/(divicdarp+divicdarr));
//
//    // score with surface
//    divsurfr = (surfgt==0 ? 0 : surfr/surfgt);
//   	divsurfp = (surfd==0  ? 0 : surfp/surfd);
//    divsurfh = (divsurfp+divsurfr==0 ? 0 : 2.0*divsurfp*divsurfr/(divsurfp+divsurfr));
//
//    // latex format
//    if (dolatex) 
//    {
//		st << setw(4) << setprecision(3);
//		if (domodeestev) 
//		{
//			st << "mode " << modessda+modessdb			
//			   << " & " << (modessda+modessdb)/(float)modenopixels
//			   << " & " << modesheight
//			   << " & " << modesdiffwidth
//			   << " \\\\\n";
//		}
//		else 
//		{
//// 			st << "eval-method & recall & precision & harmonic mean \\\\ \n"
//// 			   << "\\hline\\\\ \n";
//    		st << "icdar " << 100.0*divicdarr << " & " << 100.0*divicdarp << " & " << 100.0*divicdarh << " \\\\\n";
//    		st << "crisp " << 100.0*divcrispr << " & " << 100.0*divcrispp << " & " << 100.0*divcrisph << " \\\\\n";
//#ifdef detailed_eval_output    		
//    		st << "area "  << 100.0*divsurfr << " & " << 100.0*divsurfp << " & " << 100.0*divsurfh << " \\\\\n";
//#endif    		
//  		}
//    }
//
//    // xml format
//    else 
//    {
//
//       	st	<< "  <evaluation noimages=\"" << noimages << "\"";
//
//        if (parameterstring!=null)
//	        st << " p=\"" << parameterstring << "\"";
//	   	
//	   	if (imagename.size()>0)
//	   		st << " imagename=\"" << imagename << "\"";
//
//        st << ">\n";
//
//        if (domodeestev) 
//        {
//        	st	<< "    <modeestimation "
//            			<< "ssda=\"" << modessda << "\" "
//                      	<< "ssdb=\"" << modessdb << "\" "
//                        << "ssdtot=\"" << modessda+modessdb << "\" "
//                        << "msdtot=\"" << (modessda+modessdb)/(float)modenopixels << "\" "
//                    	<< "nopixels=\"" << modenopixels << "\" "
//                		<< "noignored=\"" << modenoignored << "\" "                		
//                  	    << "sheight=\"" << modesheight << "\" "
//                        << "sdiffwidth=\"" << modesdiffwidth << "\" "
//                        << "nont=\"" << modenont << "\" "
//                        << "/>\n";
//        }
//        
//        else 
//        {
//        	st  << "    <icdar2003 r=\"" << divicdarr << "\""
//    			 		<< " p=\"" << divicdarp << "\""
//    			   		<< " hmean=\"" << divicdarh << "\""
//    			     	<< " nogt=\"" << icdarsurfgt   << "\""
//    			   		<< " nod=\"" << icdarsurfd   << "\""
//    			     	<< "/>\n"
//     			<< "    <score r=\"" << divcrispr << "\""
//    					<< " p=\"" << divcrispp << "\""
//    					<< " hmean=\"" << divcrisph << "\""
//    					<< " nogt=\"" << crispnogt << "\""
//    					<< " nod=\"" << crispnod << "\""
//    					<< "/>\n"
//#ifdef detailed_eval_output
//    			<< "    <scoresurf r=\"" << divsurfr << "\""
//    			 		<< " p=\"" << divsurfp << "\""
//    			   		<< " hmean=\"" << divsurfh << "\""
//    			     	<< " surfgt=\"" << surfgt   << "\""
//    			   		<< " surfd=\"" << surfd   << "\""
//    			 		<< "/>\n"
//#endif
//    			;
//       	}
//        
//    	st		<< "  </evaluation>\n";
//	}
//}
//
//
//// ********************************************************************
//// a help function
//// ********************************************************************
//
//inline char * getidstr (xmlnodeptr node, char *id) 
//{
//	char *idstr = (char *) xmlgetprop(node, (const xmlchar *) id);
//	if(!idstr) 
//	{
//		std::cerr << "*** syntax error: did not find property '" << id << "'\n";
//		std::exit (1);
//    }
//    return idstr;
//}
//
//// ********************************************************************
//// the usage help output
//// ********************************************************************
//
//void usage (char *com) 
//{
//	std::cerr << "usage: " << com << " [ options ] input-xmlfile [ input-xmlfile2]\n"
//		 << "\n"
//		 << "  the xml file(s) must contain one of the following root-tags:\n"
//		 << "    <evaluationset>    evaluations for different images: calculate the\n"
//		 << "                       performance measures for the total set.\n"
//		 << "    <evaluationseries> evaluations for different parameters. create a\n"
//		 << "                       plain text file for input to gnuplot.\n"
//		 << "\n"
//		 << "  options:\n"
//		 << "    [ -g ]             create a series with falling generality\n"
//		 << "                       (only tresholded prec or rec / generality)\n"
//		 << "    [ -m ]             treat mode estimation evaluation results\n"
//		 << "    [ -n <count> ]     restrict number of evaluations to <count>\n"
//		 << "    [ -p <par-value> ] print the given value into the \"p\" field\n"
//		 << "                       when writing an output <evaluation> tag.\n"
//		 << "    [ -s <prefix> ]    the prefix for the 3 files containing the\n"
//		 << "                       the detailed output (recall,precision,hmean)\n"
//		 << "    [ -c <cmptype> ]   compares two xml files (the second file must\n"
//		 << "                       be specified!) cmptype:\n"
//		 << "                       a ... all differences\n"
//		 << "                       g ... results where rec or prec is greater in \n"
//		 << "                             the first file\n"
//		 << "                       l ... results where rec or prec is less in \n"
//		 << "                             the first file\n"
//		 << "    [ -l ]             print the output in latex tabular format.\n"
//		 << "    [ -v ]             print version information.\n"
//		 ;
//}
//
//// ********************************************************************
//// help function
//// ********************************************************************
//
//inline void eatblanks(xmlnodeptr &x)	
//{
//	while (xmlisblanknode(x))
//		x=x->next;
//}
//
//// ********************************************************************
//// check the name of a node
//// ********************************************************************
//
//bool nameisok (xmlnodeptr node, const char *s, bool warning) 
//{
//	if (node==null) 
//	{
//		if (warning)
//			cerr << "*** syntax error: node is null searching for " << s << endl;
//		return false;
//	}
//	if(!node->name) 
//	{
//		if (warning)
//			cerr << "*** syntax error: node w/o name searching for " << s << endl;
//		return false;
//	}
//	if (xmlstrcmp(node->name,(const xmlchar *) s)) 
//	{
//       if (warning)
//			cerr << "*** syntax error: found " << node->name
//            	 << " searching for " << s << endl;
//       return false;
//    }
//    // cerr << "[" << node->name << "]";
//    return true;
//}
//
//// ********************************************************************
//// read the evaluations from the xml file and store them into a structure,
//// which is sorted acording to a template-specified criteria
////
//// the caller may chose whether the structure is a multiset or a mapaipyramid
////
//// snode .......... the first xml node
//// maxcounter ..... restrict the number of images to process to this 
//// domodeestev .... perform modeestimation (internal for text 
////                  the wolf text detection algorithm)
//// ********************************************************************
//
//template<class t>
//void readevaluations(xmlnodeptr snode, int maxcounter, bool domodeestev, 
//	bool domultiset, 
//	multiset<evresults,t> *&rv_ms,  map<string, evresults> *&rv_map)
//{
//	xmlnodeptr ssnode;
//	evresults e;
//	int counter;
//	int allscoresfound;
//	ostringstream name;
//	ofstream st;
//
//	// create the returned structure
//	if (domultiset)
//		rv_ms = new multiset<evresults,t>;
//	else
//		rv_map = new map<string, evresults>;
//
//    // travers the nodes and search for the evaluation of an image
//    counter=0;
//    while (snode != null) 
//    {
//
//    	if (nameisok(snode, "evaluation", false)) 
//    	{
//     		char *pstr;
//
//       		++counter;
//         	e.clear();
//          	e.id = counter;
//
//       		ssnode = snode->children;
//         	eatblanks(ssnode);
//
//          	// travers the nodes and search for the score node
//            allscoresfound=0;
//	    	while (ssnode != null) 
//	    	{
//
//	     		// node "score"
//	     		if (nameisok(ssnode, "score", false)) 
//	     		{
//	       			allscoresfound |= 1;
//		       		e.crispr = atof(getidstr(ssnode, (char *) "r"));
//		         	e.crispp = atof(getidstr(ssnode, (char *)"p"));
//		           	e.crispnogt = atof(getidstr(ssnode, (char *)"nogt"));
//		            e.crispnod = atof(getidstr(ssnode, (char *)"nod"));
//		       	}
//
//		        // node "icdar2003"
//	     		if (nameisok(ssnode, "icdar2003", false)) 
//	     		{
//	       			allscoresfound |= 2;
//		       		e.icdarr = atof(getidstr(ssnode, (char *)"r"));
//		         	e.icdarp = atof(getidstr(ssnode, (char *)"p"));
//		            e.icdarsurfgt = atof(getidstr(ssnode, (char *)"nogt"));
//		            e.icdarsurfd = atof(getidstr(ssnode,(char *)"nod"));
//		       	}
//
//		        // node "scoresurf"
//	     		if (nameisok(ssnode, "scoresurf", false)) 
//	     		{
//	       			allscoresfound |= 4;
//		       		e.surfr = atof(getidstr(ssnode, (char *)"r"));
//		         	e.surfp = atof(getidstr(ssnode, (char *)"p"));
//		           	e.surfgt = atof(getidstr(ssnode, (char *)"surfgt"));
//		            e.surfd = atof(getidstr(ssnode, (char *)"surfd"));
//		       	}
//
//		        // node "modeestimation"
//	     		if (nameisok(ssnode, "modeestimation", false)) 
//	     		{
//	       			allscoresfound |= 8;
//		       		e.modessda = atof(getidstr(ssnode, (char *)"ssda"));
//		         	e.modessdb = atof(getidstr(ssnode, (char *)"ssdb"));
//		          	e.modenopixels = atol(getidstr(ssnode, (char *)"nopixels"));
//		           	e.modenoignored = atol(getidstr(ssnode, (char *)"noignored"));
//		            e.modesheight = atof(getidstr(ssnode, (char *)"sheight"));
//		            e.modesdiffwidth = atof(getidstr(ssnode, (char *)"sdiffwidth"));
//		            e.modenont = atol(getidstr(ssnode, (char *)"nont"));
//		       	}
//
//		        ssnode = ssnode->next;
//		       	eatblanks(ssnode);
//	     	}
//
//          	if (domodeestev) 
//          	{
//           		if ((allscoresfound&8)!=8) 
//           		{
//    	       		cerr << "found an <evaluation> node without <modeestimation> node\n"
//              			 << "which is needed for the -m option!\n"
//    	         		 << "error code = " << allscoresfound << ".\n";
//    	            exit (1);
//    	       	}
//           	}
//			/*
//			else {	      
//    	      	if ((allscoresfound&7)!=7) {
//    	       		cerr << "found an <evaluation> node with one of the scores missing!\n"
//    	         		 << "error code = " << allscoresfound << ".\n";
//    	            exit (1);
//              	}
//	       	}
//			*/
//
//       		// let's check whether we have information about the parameters
//       		pstr= (char *) xmlgetprop(snode, (const xmlchar *) "p");
//       		if (pstr!=null) 
//       		{
//         		e.haveparameter=true;
//         		e.parameter = atof (pstr);
//         	}
//           	else
//           		e.haveparameter = false;
//
//            // let's check whether we have information about the number of images
//       		pstr= (char *) xmlgetprop(snode, (const xmlchar *) "noimages");
//       		if (pstr!=null)
//         		e.noimages = atol (pstr);
//           	else
//           		e.noimages = 1;
//           		
//           	// and on the image name
//       		pstr= (char *) xmlgetprop(snode, (const xmlchar *) "imagename");
//       		if (pstr!=null)
//         		e.imagename = pstr;
//           	else
//           		e.imagename = "";
//
//            // add the evaluation node to the result set
//            if (domultiset)
//         		rv_ms->insert (e);
//         	else
//         	{
//         		if (pstr==null)
//         		{
//         			cerr << "found an <evaluation> node without imagename property!\n"
//              			 << "this property is needed for the comparison of different\n"
//    	         		 << "evaluation results!\n";
//    	            exit (1);
//         		}
//         		else
//         			(*rv_map)[e.imagename] = e;
//         	}
//     	}
//
//      	// the user restricted the number of images to process
//      	if (maxcounter>0 && counter>=maxcounter) 
//      	{
//       		cerr << "stopping accumulation after " << maxcounter << "results\n"
//         		 << "due to user request.\n";
//            cerr.flush();
//            break;
//       	}
//
//        snode = snode->next;
//       	eatblanks(snode);
//	}
//}
//
//
//// ********************************************************************
//// travers the evaluations once
//// ********************************************************************
//
//template <class t>
//evresults processseries(multiset<evresults,t> &evs, printmeasure pm, char *filename) 
//{
//	evresults rv;
//	ostringstream name;
//	ofstream st;
//
//	// open the output files
//	if (filename!=null && pm!=pm_no_print) 
//	{
//
//     	// create the filename
//      	name << filename;
//       	switch (pm) {
//        	case pm_print_r: 		name << "recall"; 					break;
//         	case pm_print_p: 		name << "precision"; 				break;
//         	case pm_print_h: 		name << "harmonic mean"; 			break;
//          	case pm_print_mode_msd: name << "mode: mean squared error"; break;
//            case pm_print_mode_sh: 	name << "mode: sum height"; 		break;
//            case pm_print_mode_sdw: name << "mode: sum diff. width"; 	break;
//          	default: case pm_no_print: break;
//        }
//
// 		st.open (name.str().c_str(),  ios::out);
//     	if (!st.good()) 
//     	{
//     		cerr << "cannot open file '" << filename << "' for writing!\n";
//     		exit (1);
//     	}
//   	}
//
//    // travers the nodes and search for the evaluation of an image
//    rv.clear();
//    for (typename multiset<evresults,t>::iterator e=evs.begin(); e!=evs.end(); ++e) 
//    {
//
//		rv.add (*e);
//
//      	if (filename!=null && pm!=pm_no_print)
//       		e->printoneresult (st, pm);
//	}
//
//	if (filename!=null && pm!=pm_no_print)
//		st.close();
//
//	return rv;
//}
//
//// ********************************************************************
//// travers the evaluations once
//// ********************************************************************
//
//evresults processset (ostream &st, bool doprintgeneralityseries, bool domodeestev,
//	multiset<evresults,sortgt> &evs) 
//	{
//
//	bool wereachedzeros=false;
//	evresults sum;
//	int no_images=0,
//		no_images_void=0;
//
//	if (doprintgeneralityseries) 
//	{
//		st << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
//		   << "<evaluationseries>\n";
//	}
//
//    // travers the nodes and search for the evaluation of an image
//    sum.clear();
//    multiset<evresults,sortgt>::iterator e=evs.end();
//    while (e!=evs.begin()) 
//    {
//
//    	--e;
//
//     	// we reached the first image without text in the groundtruth
//      	// -> write the performance before adding this image
//		if (!domodeestev && !wereachedzeros && e->crispnogt==0) 
//		{
//			wereachedzeros=true;
//			if (doprintgeneralityseries) 
//			{
//				ostringstream gstr;
//				gstr << (double) no_images / sum.crispnogt;
//				sum.printresultstag (st, gstr.str().c_str(), domodeestev, false);
//			}
//      	}
//
//     	// add the current image
//      	sum.add (*e);
//      	++no_images;
//       	if (e->crispnogt==0)
//        	++no_images_void;
//
//     	if (doprintgeneralityseries && wereachedzeros) 
//     	{
//      		ostringstream gstr;
//      		gstr << (double) no_images / sum.crispnogt;
//			sum.printresultstag (st, gstr.str().c_str(), domodeestev, false);
//		}
//
//		// we have reached the maximum level of
//        // non-zero images
//        if (doprintgeneralityseries && !domodeestev && 
//            wereachedzeros && (no_images_void >= (no_images-no_images_void)))
//        	break;
//		
//	}
//
//	if (doprintgeneralityseries)
//		st << "</evaluationseries>" << endl;
//
//	if (no_images<1)
//		cerr << "no images processed!\n";
//	else
//    	cerr << "included " << no_images-no_images_void << " images with non-zero groundtruth\n"
//    		 << "included " << no_images_void << " images with zero groundtruth\n"
//    		 << "skipped " << evs.size()-no_images << " images with zero groundtruth.\n"
//			 << "total-number-of-processed-images: " << no_images << endl 			 
//    		 << (100.0*(double)(no_images-no_images_void))/(double)no_images
//    		 << "% of the images contain objects.\n"
//    		 << "generality: " << sum.crispnogt / (double) no_images << endl
//    		 << "inverse-generality: " << (double) no_images / sum.crispnogt << endl;
//    		 ;
//
//	return sum;
//}	
//
//// ********************************************************************
//// compare two evaluation sets
//// ********************************************************************
//
//void comparesets (map<string, evresults> &set1, map<string, evresults> &set2, char comparetype)
//{
//	map<string, evresults>::iterator iter1, iter2;
//		
//	// travers the first set
//	for (iter1=set1.begin(); iter1!=set1.end(); ++iter1) 
//	{		
//		// search the entry in the second set
//		iter2 = set2.find(iter1->first);
//		
//		// the entry exists in the first set but not in the second
//		if (iter2 == set2.end()) 
//		{			
//			cout << "====== <<< (insert):\n";
//			iter1->second.printresultstag(cout, null, false, false);
//		}
//		
//		// the entry exists in both sets
//		else 
//		{	
//			if (!(iter1->second == iter2->second))
//			{
//				if (comparetype=='a' || 
//				    (comparetype=='g' && iter1->second.isgreaterthan(iter2->second)) ||
//				    (comparetype=='l' && iter1->second.islessthan(iter2->second)))
//				{
//					cout << "====== <<< (diff):\n";
//					iter1->second.printresultstag(cout, null, false, false);
//					cout << "====== >>> (diff):\n";
//					iter2->second.printresultstag(cout, null, false, false);
//				}
//			}	
//			
//			// remove the entry in the second set
//			if (set2.erase (iter2->first)!=1) 
//			{
//				cerr << "internal error: could not erase evaluation entry!" << endl;	 
//				exit (1);
//			}
//		}		
//	}	
//	
//	// the entries which are in the second set now have not been in the 
//	// first set
//	for (iter2=set2.begin(); iter2!=set2.end(); ++iter2) 
//	{		
//		cout << "====== >>> (delete):\n";
//		iter2->second.printresultstag(cout, null, false, false);
//	}
//}
//
//// ********************************************************************
//// read and check the xml document
//// ********************************************************************
//
//static filetype readandcheckdocument (char *inputfilename, xmldocptr &doc, xmlnodeptr &root)
//{	
//	filetype filetype;	
//	
//	// load and parse the document
//	doc = xmlparsefile(inputfilename);
//	if (doc == null) 
//	{
//		cerr << "couldn't parse document " << inputfilename << endl;
//		exit (1);
//	}
//
//	// check the document type
//	root = xmldocgetrootelement(doc);
//	filetype=ft_no_type;
//	if (nameisok(root, "evaluationset", false))
//		filetype = ft_set;
//	if (nameisok(root, "evaluationseries", false))
//		filetype = ft_series;
//		
//	if (filetype==ft_no_type) 
//	{
//		cerr << "invalid root node in document '" << inputfilename << "'!\n";
//		exit (1);
//	}
//	
//	return filetype;
//}
//
//// ********************************************************************
//// the main function
//// ********************************************************************
//
//int main_2 (int argc, char **argv) 
//{
//	xmldocptr doc,doc2;
//	xmlnodeptr snode, snode2, root, root2;	
//	char *optparameterstring=null,
//		 *opttextoutputprefix=(char *)"";
//	bool optdogeneralityseries=false,
//		 optdolatexoutput=false,
//		 optmodeevaluation=false,
//		 optdocompare=false;
//	char *inputfilename, *inputfilename2;
//	char comparetype='a';
//	int c, maxcounter;
//	evresults total;
//	filetype filetype, filetype2;
//	multiset<evresults,sortid> *evsi;
//	multiset<evresults,sortgt> *evsg, *dummys;
//	map<string, evresults> *evm1, *evm2, *dummym;
//
//    //	***************************************************
//	//	arguments
//	//	***************************************************
//
//	maxcounter=-1;
//  
//	// check the arguments
//	if ((!optdocompare && (argc-optind != 1) ) || (optdocompare && (argc-optind != 2)))
//	{
//	 	usage (*argv);
//	 	exit(1);
//	}
//	inputfilename=argv[optind];
//	inputfilename2=argv[optind+1];
//	
//	filetype = readandcheckdocument(inputfilename, doc, root);
//	if (optdocompare)
//		filetype2 = readandcheckdocument(inputfilename2, doc2, root2);
//	
//	if (filetype==ft_series && optdogeneralityseries) 
//	{
//		usage (*argv);
//		cerr << "\ncannot create a generality series out of a <evaluationseries> xml file!\n";
//		exit (1);
//	}
//				
//	snode=root->children;
//    eatblanks(snode);
//    
//    // ---------------------------------------------------------------------------
//    // we compare two xml files
//    // ---------------------------------------------------------------------------
//    
//    if (optdocompare)
//    {
//    	snode2=root2->children;
//    	eatblanks(snode2);
//    
//    	readevaluations(snode,  0, false, false, dummys, evm1);
//    	readevaluations(snode2, 0, false, false, dummys, evm2);
//    	
//    	comparesets(*evm1, *evm2, comparetype);
//    	delete evm1;
//    	delete evm2;
//	}
//	
//	// ---------------------------------------------------------------------------
//    // we process only a single xml file
//    // ---------------------------------------------------------------------------
//    
//    else
//	{
//		switch (filetype) 
//		{
//	
//			case ft_set:
//	
//				// we need to create a series with falling generality
//				if (optdogeneralityseries) 
//				{
//					readevaluations<sortgt>(snode, maxcounter, optmodeevaluation, true, evsg, dummym);
//					processset (cout, true, optmodeevaluation, *evsg);
//					delete evsg;
//				}
//	
//				// we read them all in
//				else 
//				{
//					readevaluations<sortgt>(snode, maxcounter, optmodeevaluation, true, evsg, dummym);
//					total = processset (cout, false, optmodeevaluation, *evsg);
//					total.printresultstag(cout, optparameterstring,
//						optmodeevaluation, optdolatexoutput);
//					delete evsg;
//				}
//				break;
//	
//			case ft_series:
//				readevaluations<sortid>(snode, maxcounter, optmodeevaluation, true, evsi, dummym);
//				if (optmodeevaluation) 
//				{
//					processseries (*evsi, pm_print_mode_msd, opttextoutputprefix);
//					processseries (*evsi, pm_print_mode_sh, opttextoutputprefix);
//					processseries (*evsi, pm_print_mode_sdw, opttextoutputprefix);
//				}
//				else 
//				{
//					processseries (*evsi, pm_print_r, opttextoutputprefix);
//					processseries (*evsi, pm_print_p, opttextoutputprefix);
//					processseries (*evsi, pm_print_h, opttextoutputprefix);
//				}
//				delete evsi;
//				break;
//	
//			default:
//				cerr << "internal error (1).\n";
//				exit (1);
//		}
//	}
//	
//	// clean up
//	xmlfreedoc(doc);
//
// 	return 0;
//}
//
