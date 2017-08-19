/***********************************************************************
 * OptionContainer.cpp
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

// From this module
#include "OptionContainer.h"
#include <string.h>
#include <stdlib.h>


/*****************************************************************************
 * Set a string option from the string given at the command line
 * The option must have been defined before
 *****************************************************************************/

void OptionContainer::set (string str)
{
	unsigned int i;
	string name;
	char *endptr;
	const char *c_str;

	for (i=0; i<str.length(); ++i)
	{
		if (str[i]=='=')
			break;
	}

	if (str[i]!='=')
		ERR_THROW ("option '" << str << "': missing '='!");

	name=str.substr(0,i);

	// A string option
	if (so.find(name)!=so.end())
	{
		if (str.length()==i+1)
			so[name]="";
		else
			so[name] = str.substr(i+1);
		return;
	}

	// If the option is not a string, then it can't be empty
	if (str.length()==i+1)
		ERR_THROW ("option '" << name << "' can't be empty!");

	// A boolean option
	if (bo.find(name)!=bo.end())
	{
		string v = str.substr(i+1);
		bool ok=true;
		char c;

		if (v.length()!=1)
			ok=false;
		else
		{
			c=tolower(v[0]);
			if (c!='y' && c!='n')
				ok=false;
		}

		if (ok)
			bo[name] = (c=='y');
		else
			ERR_THROW ("A boolean option must be 'y' or 'n'!");
		return;
	}

	c_str=str.substr(i+1).c_str();

	// An integer option
	if (io.find(name)!=io.end())
	{
		int i=strtol(c_str,&endptr,10);
		if (*endptr!=0)
			ERR_THROW ("option '" << name << "' must be an integer!");
		io[name]=i;
		return;
	}

	// An integer option
	if (fo.find(name)!=fo.end())
	{
		float f=strtof(c_str,&endptr);
		if (*endptr!=0)
			ERR_THROW ("option '" << name << "' must be a real number!");
		fo[name]=f;
		return;
	}

	ERR_THROW ("Unknown option '" << str << "'!");
}

/*****************************************************************************
 * run the container on an argument set
 * We only treat options beginning with double dashes ("--"),
 * exception: "-h" and "-v"
 *****************************************************************************/

int OptionContainer::run (int xargc, char **xargv)
{
	int rv=0;
	argc = xargc;
	argv = xargv;
	nonOptPointer=1;
	for (int i=0; i<argc; ++i)
	{
		if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0))
			rv|=OCOPT_USAGE;
		if ((strcmp(argv[i],"-v")==0) || (strcmp(argv[i],"--version")==0))
			rv|=OCOPT_VERSION;

		if ((argv[i][0]=='-') && (argv[i][1]=='-'))
			set (string(argv[i]+2));
	}
	return rv;
}

/*****************************************************************************
 * Access the non-options: their number
 *****************************************************************************/

int OptionContainer::noNonOpts()
{
	if (argv==NULL)
		ERR_THROW ("OptionContainer: you need to call run() before noNonOpts!");

	int cnt=0;
	for (int i=1; i<argc; ++i)
	{
		if (argv[i][0]!='-')
			++cnt;
	}
	return cnt;
}

/*****************************************************************************
 * Access the non-options
 *****************************************************************************/

char * OptionContainer::getNextNonOpt()
{
	if (argv==NULL)
		ERR_THROW ("OptionContainer: you need to call run() before getNextNonOpt!");

	while (nonOptPointer<argc)
	{
		if (argv[nonOptPointer][0]!='-')
		{
			++nonOptPointer;
			return argv[nonOptPointer-1];
		}
		++nonOptPointer;
	}
	ERR_THROW ("OptionContainer: no more option available!");
}
