/***************************************************************************
 * ParameterParsing.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * Changelog:
 * 06.02.2008 cw: begin
 *
 * 1 tab = 4 spaces
 ***************************************************************************/

// C++
#include <iostream>
#include <stdlib.h>

// From the main module
#include "CIL.h"
using namespace std;

// *******************************************************************************
// Parse the argument string with the parameters
// if no_params > 0 then the number of parameters is fixed
// returns the number of parameters read, or -1 on error
//*******************************************************************************

int parseParameterString (char *s, double *params, int no_params, char *optionName)
{
	char *cp=s, *endptr;
	unsigned int index=0;
	float f;

	while (*cp!='\0' && *cp!='\n')
	{

		switch (*cp)
		{

			case ',':
				++index;
				++cp;
				if ((no_params>0) && (index>=no_params))
				{
					cerr << "Too many parameters for option "
						 << optionName << "!\n";
					return -1;
				}
				break;

            // This means we are at the end of the evaluation parameter part
			case '\n':
			case ';':
				break;

			default:
	            f=strtod(cp,&endptr);
	            if (endptr==cp)
	            {

	            	cerr << "Parse error parsing parameters (option "
	            		 << optionName << "-p)!\n" << "at: " << cp << endl;
	                exit(1);
	            }
	            cp=endptr;
	            params[index]=f;
				break;
		}
	}

	if ((no_params>0) && (index!=no_params-1))
	{
		cerr << "We need " << no_params << " parameters, you specified only "
			 << index+1 << "!\n";
		exit(1);
	}

	return index+1;
}

