///***************************************************************************
//* evaldetection.cpp
//*
//* Author: Christian Wolf
//*         christian.wolf@insa-lyon.fr
//*
//* Changelog:
//* 31.08.2006 cw: introduced -z feature (ignore zero rectangles).
//* 27.04.2004 cw: first version
//*
//* 1 tab = 4 spaces
//***************************************************************************/
//
//#define VERSION		"version 2.00, 02.03.2009."
//
//// C++
//#include <iostream>
//#include <new>
//#include <map>
//#include <set>
//#include "getopt.h"
//// libxml2
//
//#include <libxml/xmlmemory.h>
//#include <libxml/parser.h>
//
//// From the CIL module
//#include "CIL.h"
//
//// From the BOXES module
//#include "RectEvalList.h"
//#include "BoxEvaluator.h"
//using namespace std;
//
//bool IgnoreZeroRects;
//
//// The comparison function needed for the map object
////排列函数，从小到大排列
//struct lessthanstring
//{
//	bool operator()(const char* s1, const char* s2) const
//	{
//		return strcmp(s1, s2) < 0;
//	}
//};
//
//static void usage(char *com)
//{
//	std::cerr << "usage:	\n"
//		<< "   " << com << " [options]  detection-xml groundtruth-xml\n\n"
//		<< "   -p <options>   Set the evaluation parameters: \n"
//		<< "                  -p <a>,<b>,<c>,<d>,<e>,<f>,<g>,<h>,<i>,<j>\n"
//		<< "                  Default values: " << EvalParameterDefaults << "\n"
//		<< "   -d <count>     How many levels in the image name path do we ignore\n"
//		<< "                  (detection-xml)\n"
//		<< "   -g <count>     How many levels in the image name path do we ignore\n"
//		<< "                  (groundtruth-xml)\n"
//		<< "   -z             Ignore detected rectangles having zero coordinates\n"
//		<< "   -m             Allow mixing specified and unspecified object types\n"
//		<< "   -o             Specify a comme separated list of object types which shall\n"
//		<< "                  shall be loaded from the groundtruth, all other objects \n"
//		<< "                  will be ignored in the groundtruth (but not in the detection\n"
//		<< "                  list!). Default: all objects will be loaded.\n"
//		<< "   -v             Print version string\n"
//		<< endl;
//}
//
///**********************************************************
//* Global new handler for memory management
//**********************************************************/
//
//void globalNewHandler()
//{
//	std::cerr << "Out Of	Memory.	Sorry.\n";
//	std::exit(1);
//}
//
///**********************************************************
//* The error function
//**********************************************************/
//
//void Error(char *text)
//{
//	std::cerr << text;
//}
//
//// ********************************************************************
//// A help function
//// ********************************************************************
//
//static char * getIdStr(xmlNodePtr node, char *id)
//{
//	char *idstr = (char *)xmlGetProp(node, (const xmlChar *)id);
//	if (!idstr)
//	{
//		std::cerr << "*** Syntax error: did not found property '" << id << "'\n";
//		std::exit(1);
//	}
//	return idstr;
//}
///*
//static char * getIdStrNULLIfNotFound (xmlNodePtr node, char *id)
//{
//char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
//if(!idstr)
//return NULL;
//else
//return idstr;
//}*/
//
//static int getIdIntMinus1IfNotFound(xmlNodePtr node, char *id)
//{
//	char *idstr = (char *)xmlGetProp(node, (const xmlChar *)id);
//	if (!idstr)
//		return -1;
//	else
//		return atol(idstr);
//}
//
//
//// ********************************************************************
//// Help function
//// ********************************************************************
//
//inline void EatBlanks(xmlNodePtr &x)	{
//	while (xmlIsBlankNode(x))
//		x = x->next;
//}
//
//// ********************************************************************
//// Check the name of a node
//// ********************************************************************
//
//static bool nameIsOk(xmlNodePtr node, const char *s, bool warning)
//{
//	if (node == NULL)
//	{
//		if (warning)
//			cerr << "*** Syntax error: node is null searching for " << s << endl;
//		return false;
//	}
//	if (!node->name)
//	{
//		if (warning)
//			cerr << "*** Syntax error: node w/o name searching for " << s << endl;
//		return false;
//	}
//	if (xmlStrcmp(node->name, (const xmlChar *)s))
//	{
//		if (warning)
//			cerr << "*** Syntax error: found " << node->name
//			<< " searching for " << s << endl;
//		return false;
//	}
//	// cerr << "[" << node->name << "]";
//	return true;
//}
//
//
//// *******************************************************************************
//// Read a single Image Tag
//// *******************************************************************************
//
//static RectEvalList * readICDARXMLImageTag(xmlDocPtr doc, xmlNodePtr &imagenode,
//	char *imagename, int noIgnoreDirLevels, bool allowTypeMixing,
//	set<unsigned int> *allowedObjectTypesInGT)
//{
//	xmlNodePtr snode, ssnode;
//	RectEvalList *rl;
//
//	snode = imagenode->children;
//	EatBlanks(snode);
//
//	rl = new RectEvalList;
//
//	// find the sub nodes in each image node
//	imagename = NULL;
//	while (snode != NULL) {
//
//		if (nameIsOk(snode, "imageName", false))
//		{
//			char *cp = (char *)xmlNodeListGetString(doc, snode->xmlChildrenNode, 1);
//
//			// Ignore the first noIgnoreDirLevels levels of the path in the image name
//			for (int i = 0; i<noIgnoreDirLevels; ++i)
//			{
//				while (*cp != '\0' && *cp != '\\' && *cp != '/')
//					++cp;
//				if (*cp == '\0')
//				{
//					std::cerr << "Cannot ignore directory level as told per option!\n";
//					std::cerr << "The following image name is too shallow: "
//						<< imagename << endl;
//					std::exit(1);
//				}
//				++cp;
//			}
//			imagename = cp;
//		}
//
//		if (nameIsOk(snode, "taggedRectangles", false))
//		{
//			ssnode = snode->children;
//			EatBlanks(ssnode);
//
//			while (ssnode != NULL)
//			{
//
//				if (nameIsOk(ssnode, "taggedRectangle", true))
//				{
//					bool doAdd = true;
//					RectEval *r = new RectEval;
//					r->left = atol(getIdStr(ssnode, (char *) "x"));
//					r->top = atol(getIdStr(ssnode, (char *) "y"));
//					r->right = atol(getIdStr(ssnode, (char *) "width")) + r->left - 1;
//					r->bottom = atol(getIdStr(ssnode, (char *) "height")) + r->top - 1;
//
//					r->objectType = getIdIntMinus1IfNotFound(ssnode, (char *) "modelType");
//					if (r->objectType == -1)
//						r->objectType = getIdIntMinus1IfNotFound(ssnode, (char *) "typeModel");
//
//					if ((r->objectType != -1) && (allowedObjectTypesInGT != NULL))
//						if (allowedObjectTypesInGT->find(r->objectType) == allowedObjectTypesInGT->end())
//						{
//							doAdd = false;
//							std::cerr << "Ignoring o-type " << r->objectType << " in ground truth\n";
//						}
//
//					if (doAdd && IgnoreZeroRects)
//					{
//						doAdd = false;
//						if (r->left>0 || r->top>0 || r->right>0 || r->bottom>0)
//							doAdd = true;
//					}
//
//					if (doAdd)
//					{
//						rl->add(r);
//						if (r->objectType == -1)
//							++rl->noObjectsWithoutType;
//						else
//							++rl->noObjectsWithType;
//					}
//					else
//						delete r;
//				}
//
//				ssnode = ssnode->next;
//				EatBlanks(ssnode);
//			}
//		}
//
//		snode = snode->next;
//		EatBlanks(snode);
//	}
//
//	if (imagename == NULL) {
//		cerr << "Can't find image name!!\n";
//		exit(1);
//	}
//
//	if ((rl->noObjectsWithoutType>0) && (rl->noObjectsWithType>0))
//	{
//		if (allowTypeMixing)
//		{
//			cerr << "Mix of specified and unspecified object types, but this has been allowed explicitly by user action\n";
//		}
//		else
//		{
//			cerr << "The XML files contain a mix of rectangles with specified "
//				"object types and unspecified object types. Normally this does not make any sens "
//				"since you might want to detect specific objects, e.g. specified by a model, or "
//				"generic objects (text), but not both.\n\nIf you want to proceed anyway, relaunch "
//				"the application with the -m option (meaning '_M_ix object types') and pray.";
//			exit(1);
//		}
//	}
//
//	return rl;
//}
//
//// ***********************************************************
//// Read in the XML file and store it in a map
//// **********************************************************
//
//static void readICDARXML(map<const char*, RectEvalList*, lessthanstring> &m,
//	char * filename, int noIgnoreDirLevels, bool allowTypeMixing,
//	unsigned int &noObjectsWithType, unsigned int &noObjectsWithoutType,
//	set<unsigned int> *allowedObjectTypesInGT)
//{
//	xmlNodePtr imagenode, root;
//	xmlDocPtr doc;
//	RectEvalList *rl;
//	char *imagename;
//
//	// Load and parse the document
//	doc = xmlParseFile(filename);
//	if (doc == NULL) {
//		cerr << "Couldn't parse document " << filename << endl;
//		exit(1);
//	}
//
//	// Check the document type
//	root = xmlDocGetRootElement(doc);
//	if (!nameIsOk(root, "tagset", true)) exit(1);
//	imagenode = root->children;
//	EatBlanks(imagenode);
//
//	// Travers the image nodes
//	noObjectsWithType = noObjectsWithoutType = 0;
//	while (imagenode != NULL)
//	{
//
//		if (!nameIsOk(imagenode, "image", true))
//			exit(1);
//
//		rl = readICDARXMLImageTag(doc, imagenode, imagename, noIgnoreDirLevels, allowTypeMixing,
//			allowedObjectTypesInGT);
//
//		if (rl != NULL)
//		{
//			m[imagename] = rl;
//			noObjectsWithType += rl->noObjectsWithType;
//			noObjectsWithoutType += rl->noObjectsWithoutType;
//		}
//		imagenode = imagenode->next;
//		EatBlanks(imagenode);
//	}
//
//	// clean up
//	xmlFreeDoc(doc);
//}
//
//// ***********************************************************
//// The main	function
//// **********************************************************
//
//int	main(int argc, char **argv)
//{
//	char *filename_det, *filename_gt;
//	char *evalParameterString = EvalParameterDefaults;;
//	double evalParameters[10];
//	int c;
//	
//	unsigned int gwitht, gwithoutt, dwitht, dwithoutt;
//	char *objectTypeList = NULL;
//	set<unsigned int> allowedObjectTypesInGT;
//	bool allowTypeMixing = false;
//
//	try
//	{
//		// Maps which store the two sets
//		map<const char*, RectEvalList*, lessthanstring> map_det;
//		map<const char*, RectEvalList*, lessthanstring> map_gt;
//		map<const char*, RectEvalList*, lessthanstring>::iterator iter_det, iter_gt;
//		while ((c = getopt1(argc, argv, "p:d:g:zvmo:")) != EOF) {
//
//			switch (c) {
//
//			case 'd':
//				ignoreDetDirLevels = atol(optarg1);
//				break;
//
//			case 'm':
//				allowTypeMixing = true;
//				break;
//
//			case 'g':
//				ignoreGTDirLevels = atol(optarg1);
//				break;
//
//			case 'p':
//				evalParameterString = optarg1;
//				break;
//
//			case 'o':
//				objectTypeList = optarg1;
//				break;
//
//			case 'z':
//				IgnoreZeroRects = true;
//				break;
//
//			case 'v':
//				cerr << *argv << " " << VERSION << endl;
//				return 0;
//
//			case '?':
//				usage(*argv);
//				cerr << "\nProblem parsing the options!\n\n";
//				exit(1);
//			}
//		}
//		if (argc - optind != 2) {
//			usage(*argv);
//			exit(1);
//		}
//		filename_det = argv[optind];
//		filename_gt = argv[optind + 1];
//
//		// Parse the parameter string into a array of doubles
//		parseParameterString(evalParameterString, evalParameters, EV_PARAM_COUNT, (char *) "-p");
//
//		// If requested, parse the list of object types
//		// admitted in the ground truth file
//		if (objectTypeList != NULL)
//		{
//			double typearr[2000];
//			int len = parseParameterString(objectTypeList, typearr, -1, (char *) "-o");
//			if (len<0)
//				exit(1);
//			for (int i = 0; i<len; ++i)
//				allowedObjectTypesInGT.insert((int)rint(typearr[i]));
//
//			cerr << "List of allowed object types: ";
//			for (set<unsigned int>::iterator i = allowedObjectTypesInGT.begin();
//				i != allowedObjectTypesInGT.end();
//				++i)
//				cerr << *i << ",";
//			cerr << endl;
//		}
//
//		// Read the XML files
//		readICDARXML(map_det, filename_det, ignoreDetDirLevels, allowTypeMixing, dwitht, dwithoutt, NULL);
//		readICDARXML(map_gt, filename_gt, ignoreGTDirLevels, allowTypeMixing, gwitht, gwithoutt,
//			(allowedObjectTypesInGT.size()>0 ? &allowedObjectTypesInGT : NULL));
//
//		cerr << "==== DETECTION SET\n"
//			<< map_det.size() << " image tags in the detection set ("
//			<< filename_det << ").\n"
//			<< dwitht << " rectangles with object type\n"
//			<< dwithoutt << " rectangles without object type\n"
//			<< "==== GROUND TRUTH SET\n"
//			<< map_gt.size() << " image tags in the ground truth set ("
//			<< filename_gt << ").\n"
//			<< gwitht << " rectangles with object type\n"
//			<< gwithoutt << " rectangles without object type\n"
//			;
//		// 		if (map_det.size() != map_gt.size())
//		// 		{
//		// 			cerr << "*** ERROR: the image tag counts do not match!!!\n";
//		// 			exit (1);
//		// 		}
//
//		// Add empty detection results for all images which are in the ground
//		// truth but not in the detection file
//		for (iter_gt = map_gt.begin(); iter_gt != map_gt.end(); ++iter_gt)
//		{
//			iter_det = map_det.find(iter_gt->first);
//			if (iter_det == map_det.end())
//			{
//				/*	cerr << "Add. empty det result for image '" << iter_gt->first
//				<< "' found in the ground truth file.\n";
//				*/
//				map_det[iter_gt->first] = new RectEvalList;
//			}
//		}
//
//		// Write the XML header
//		cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//		cout << "<evaluationSet>\n";
//
//		// Travers the detection list
//		for (iter_det = map_det.begin(); iter_det != map_det.end(); ++iter_det)
//		{
//			iter_gt = map_gt.find(iter_det->first);
//			if (iter_gt == map_gt.end())
//			{
//				cerr << "*** Could not find image name " << iter_det->first
//					<< " in the ground truth file.\n";
//				exit(1);
//			}
//
//			else
//			{
//				cout << "  <evaluation noImages=\"1\" imageName=\"" << iter_det->first << "\">\n";
//				BoxEvaluator boxEv(iter_gt->second, iter_det->second, evalParameters);
//				boxEv.matchScore(&cout, iter_gt->second->size());
//				cout << "  </evaluation>\n";
//
//				// Remove the entry in the ground truth structure
//				if (map_gt.erase(iter_gt->first) != 1)
//				{
//					cerr << "Internal error: could not erase ground truth key!" << endl;
//					exit(1);
//				}
//			}
//		}
//		cout << "</evaluationSet>\n";
//
//		if (map_gt.size() != 0) {
//			cerr << "*** ERROR: " << map_gt.size()
//				<< " unmatched elements in the ground truth file!\n";
//			exit(1);
//		}
//
//	} // try
//
//	catch (EError &e)
//	{
//		cerr << e.message << endl;
//	}
//
//#ifndef VISUALBUG
//	// Install new handler for memory management
//	set_new_handler(globalNewHandler);
//#endif
//
//	return 0;
//}
