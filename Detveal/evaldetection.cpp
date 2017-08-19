/***************************************************************************
* evaldetection.cpp
*
* Author: Christian Wolf
*         christian.wolf@insa-lyon.fr
*
* Changelog:
* 31.08.2006 cw: introduced -z feature (ignore zero rectangles).
* 27.04.2004 cw: first version
*
* 1 tab = 4 spaces
***************************************************************************/

#define VERSION		"version 2.00, 02.03.2009."

// C++
#include <iostream>
#include<fstream>
#include <new>
#include <map>
#include <set>
#include<vector>
#include "getopt.h"
// libxml2

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// From the CIL module
#include "CIL.h"

// From the BOXES module
#include "RectEvalList.h"
#include "BoxEvaluator.h"
using namespace std;

bool IgnoreZeroRects;

// The comparison function needed for the map object
//排列函数，从小到大排列
struct lessthanstring
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

static void usage(char *com)
{
	std::cerr << "usage:	\n"
		<< "   " << com << " [options]  detection-xml groundtruth-xml\n\n"
		<< "   -p <options>   Set the evaluation parameters: \n"
		<< "                  -p <a>,<b>,<c>,<d>,<e>,<f>,<g>,<h>,<i>,<j>\n"
		<< "                  Default values: " << EvalParameterDefaults << "\n"
		<< "   -d <count>     How many levels in the image name path do we ignore\n"
		<< "                  (detection-xml)\n"
		<< "   -g <count>     How many levels in the image name path do we ignore\n"
		<< "                  (groundtruth-xml)\n"
		<< "   -z             Ignore detected rectangles having zero coordinates\n"
		<< "   -m             Allow mixing specified and unspecified object types\n"
		<< "   -o             Specify a comme separated list of object types which shall\n"
		<< "                  shall be loaded from the groundtruth, all other objects \n"
		<< "                  will be ignored in the groundtruth (but not in the detection\n"
		<< "                  list!). Default: all objects will be loaded.\n"
		<< "   -v             Print version string\n"
		<< endl;
}

/**********************************************************
* Global new handler for memory management
**********************************************************/

void globalNewHandler()
{
	std::cerr << "Out Of	Memory.	Sorry.\n";
	std::exit(1);
}

/**********************************************************
* The error function
**********************************************************/

void Error(char *text)
{
	std::cerr << text;
}

// ********************************************************************
// A help function
// ********************************************************************

static char * getIdStr(xmlNodePtr node, char *id)
{
	char *idstr = (char *)xmlGetProp(node, (const xmlChar *)id);
	if (!idstr)
	{
		std::cerr << "*** Syntax error: did not found property '" << id << "'\n";
		std::exit(1);
	}
	return idstr;
}
/*
static char * getIdStrNULLIfNotFound (xmlNodePtr node, char *id)
{
char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
if(!idstr)
return NULL;
else
return idstr;
}*/

static int getIdIntMinus1IfNotFound(xmlNodePtr node, char *id)
{
	char *idstr = (char *)xmlGetProp(node, (const xmlChar *)id);
	if (!idstr)
		return -1;
	else
		return atol(idstr);
}


// ********************************************************************
// Help function
// ********************************************************************

inline void EatBlanks(xmlNodePtr &x)	{
	while (xmlIsBlankNode(x))
		x = x->next;
}

// ********************************************************************
// Check the name of a node
// ********************************************************************

static bool nameIsOk(xmlNodePtr node, const char *s, bool warning)
{
	if (node == NULL)
	{
		if (warning)
			cerr << "*** Syntax error: node is null searching for " << s << endl;
		return false;
	}
	if (!node->name)
	{
		if (warning)
			cerr << "*** Syntax error: node w/o name searching for " << s << endl;
		return false;
	}
	if (xmlStrcmp(node->name, (const xmlChar *)s))
	{
		if (warning)
			cerr << "*** Syntax error: found " << node->name
			<< " searching for " << s << endl;
		return false;
	}
	// cerr << "[" << node->name << "]";
	return true;
}


// *******************************************************************************
// Read a single Image Tag
// *******************************************************************************

static RectEvalList * readICDARXMLImageTag(xmlDocPtr doc, xmlNodePtr &imagenode,
	char *imagename, int noIgnoreDirLevels, bool allowTypeMixing,
	set<unsigned int> *allowedObjectTypesInGT)
{
	xmlNodePtr snode, ssnode;
	RectEvalList *rl;

	snode = imagenode->children;
	EatBlanks(snode);

	rl = new RectEvalList;

	// find the sub nodes in each image node
	imagename = NULL;
	while (snode != NULL) {

		if (nameIsOk(snode, "imageName", false))
		{
			char *cp = (char *)xmlNodeListGetString(doc, snode->xmlChildrenNode, 1);

			// Ignore the first noIgnoreDirLevels levels of the path in the image name
			for (int i = 0; i<noIgnoreDirLevels; ++i)
			{
				while (*cp != '\0' && *cp != '\\' && *cp != '/')
					++cp;
				if (*cp == '\0')
				{
					std::cerr << "Cannot ignore directory level as told per option!\n";
					std::cerr << "The following image name is too shallow: "
						<< imagename << endl;
					std::exit(1);
				}
				++cp;
			}
			imagename = cp;
		}

		if (nameIsOk(snode, "taggedRectangles", false))
		{
			ssnode = snode->children;
			EatBlanks(ssnode);

			while (ssnode != NULL)
			{

				if (nameIsOk(ssnode, "taggedRectangle", true))
				{
					bool doAdd = true;
					RectEval *r = new RectEval;
					r->left = atol(getIdStr(ssnode, (char *) "x"));
					r->top = atol(getIdStr(ssnode, (char *) "y"));
					r->right = atol(getIdStr(ssnode, (char *) "width")) + r->left - 1;
					r->bottom = atol(getIdStr(ssnode, (char *) "height")) + r->top - 1;

					r->objectType = getIdIntMinus1IfNotFound(ssnode, (char *) "modelType");
					if (r->objectType == -1)
						r->objectType = getIdIntMinus1IfNotFound(ssnode, (char *) "typeModel");

					if ((r->objectType != -1) && (allowedObjectTypesInGT != NULL))
						if (allowedObjectTypesInGT->find(r->objectType) == allowedObjectTypesInGT->end())
						{
							doAdd = false;
							std::cerr << "Ignoring o-type " << r->objectType << " in ground truth\n";
						}

					if (doAdd && IgnoreZeroRects)
					{
						doAdd = false;
						if (r->left>0 || r->top>0 || r->right>0 || r->bottom>0)
							doAdd = true;
					}

					if (doAdd)
					{
						rl->add(r);
						if (r->objectType == -1)
							++rl->noObjectsWithoutType;
						else
							++rl->noObjectsWithType;
					}
					else
						delete r;
				}

				ssnode = ssnode->next;
				EatBlanks(ssnode);
			}
		}

		snode = snode->next;
		EatBlanks(snode);
	}

	if (imagename == NULL) {
		cerr << "Can't find image name!!\n";
		exit(1);
	}

	if ((rl->noObjectsWithoutType>0) && (rl->noObjectsWithType>0))
	{
		if (allowTypeMixing)
		{
			cerr << "Mix of specified and unspecified object types, but this has been allowed explicitly by user action\n";
		}
		else
		{
			cerr << "The XML files contain a mix of rectangles with specified "
				"object types and unspecified object types. Normally this does not make any sens "
				"since you might want to detect specific objects, e.g. specified by a model, or "
				"generic objects (text), but not both.\n\nIf you want to proceed anyway, relaunch "
				"the application with the -m option (meaning '_M_ix object types') and pray.";
			exit(1);
		}
	}

	return rl;
}

vector<string> split(string str, string pattern){
	str += pattern;
	vector<string> result;
	int size = str.size();

	int pos = 0;
	for (int i = 0; i < size; i++){
		pos = str.find(pattern, i);
		if (pos < size){
			string s = str.substr(i, pos - i);
			i = pos + pattern.size() - 1;
			result.push_back(s);
		}

	}

	return result;

}

//返回一张图片的矩形
static RectEvalList *readImage(string file){
	RectEvalList *r1;
	r1 = new RectEvalList;
	fstream imageFile;//定义文本操作符
	imageFile.open(file.c_str(), ios::in);//用操作符打开文件并输入
	char buffer[256];
	stringstream ss;
	while (!imageFile.eof()){//未到文件末尾，则逐行读入矩形框并加入
		imageFile.getline(buffer, 256, '\n');//表示该行字符已达到256或遇到换行
		//	cout << "buffer = " << buffer << endl;
		vector<string> poss = split(buffer, ",");
		if (poss.size() < 4){
			break;
		}
		//	cout << "poss.size() = " << poss.size() << endl;
		vector<int> posi;//读入文件中矩形框的坐标
		RectEval *r = new RectEval;
		for (int i = 0; i < 4; i++){

			//利用stringstream将string转换为int
			ss << poss[i];
			int temp;
			ss >> temp;
			ss.clear();
			posi.push_back(temp);

		}

		r->left = posi[0];
		r->top = posi[1];
		r->right = posi[2];
		r->bottom = posi[3];

		r1->add(r);


	}

	imageFile.close();
	return r1;

}

// ***********************************************************
// Read in the XML file and store it in a map
// **********************************************************

static void readICDAR(map<const string, RectEvalList*> &m,
	char * filename)
{
	//string fileDir = "E:/project/data/TxtFile/result_focus_hsv1/";//文件所在path
	stringstream ss;
	for (int i = 1; i <= 255; i++){
		//cout << "开始读入第" << i << "张图片" << endl;
		stringstream ss;
		ss << (i+100);
		const string num = ss.str();
		ss.clear();
		stringstream idss;
		idss << i;
		string id = idss.str();
		idss.clear();
		string fileDir = filename;
		string file = fileDir + "res_img_" + id + ".txt";
	//	cout << "num = " << num << " id = " << id << endl;
		m.insert(pair<const string, RectEvalList*>(num, readImage(file)));//编号从101开始，文件从1开始
		//m[num.c_str()] = readImage(file);
		//cout << "已读入第" << i << "张图片" << endl;
	}
	cout << "m.size() = " << m.size() << endl;

}

// ***********************************************************
// The main	function
// **********************************************************

int	main123(int argc, char **argv)
{
	//文件和参数
	char *filename_det, *filename_gt;
	char *evalParameterString = EvalParameterDefaults;;
	double evalParameters[10];
	int optind = 1;
	try
	{
		// Maps which store the two sets
		//读入矩形，key为文件名，value为矩形框
		map<const string, RectEvalList*> map_det;
		map<const string, RectEvalList*> map_gt;
		map<const string, RectEvalList*>::iterator iter_det, iter_gt;

		if (argc - optind != 2) {
			usage(*argv);
			exit(1);
		}
		filename_det = argv[optind];//配置的第一个参数为检测文本所在的path
		filename_gt = argv[optind + 1];//配置的第二个文本为真实框path

		// Parse the parameter string into a array of doubles
		parseParameterString(evalParameterString, evalParameters, EV_PARAM_COUNT, (char *) "-p");


		//关键步骤，读入文件，存入数据
		readICDAR(map_det, filename_det);
		readICDAR(map_gt, filename_gt);

		cout << "==== DETECTION SET\n"
			<< map_det.size() << " image tags in the detection set ("
			<< filename_det << ").\n"

			<< "==== GROUND TRUTH SET\n"
			<< map_gt.size() << " image tags in the ground truth set ("
			<< filename_gt << ").\n"
			;

		ofstream out("out.txt");
		ofstream totalFile("totalFile.txt");

		// Travers the detection list
		for (iter_det = map_det.begin(); iter_det != map_det.end(); ++iter_det)
		{
			iter_gt = map_gt.find(iter_det->first);//找到和检测框同名的真实框
			if (iter_gt == map_gt.end())
			{
				out<< "*** Could not find image name " << iter_det->first
					<< " in the ground truth file.\n";
				exit(1);
			}

			else
			{
				//矩形框的数目
				//cout << "noGT = " << (iter_gt->second->size()) << " noD = " << (iter_det->second->size()) << endl;
				//cout << endl;
				out << "  <evaluation  imageName=\"" << iter_det->first << "\">\n";
				BoxEvaluator boxEv(iter_gt->second, iter_det->second, evalParameters);
				boxEv.matchScore(&out,&totalFile, iter_gt->second->size());
				out << "  </evaluation>\n";

				// Remove the entry in the ground truth structure
				if (map_gt.erase(iter_gt->first) != 1)
				{
					cerr << "Internal error: could not erase ground truth key!" << endl;
					exit(1);
				}
			}
		}
		out << "</evaluationSet>\n";

		if (map_gt.size() != 0) {
			cerr << "*** ERROR: " << map_gt.size()
				<< " unmatched elements in the ground truth file!\n";
			exit(1);
		}
		out.close();
		totalFile.close();

	} // try

	catch (EError &e)
	{
		cerr << e.message << endl;
	}

#ifndef VISUALBUG
	// Install new handler for memory management
	set_new_handler(globalNewHandler);
#endif

	return 0;
}
