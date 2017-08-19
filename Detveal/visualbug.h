#ifdef VISUALBUG

/************************************************************************************************
 FOR THE MICROSOFT VISUAL C++ COMPILER
 *************************************************************************************************/

/* workaround for bug in the treatment of local variables 
 * declared in for loops.
 */
#define for if (0) {} else for

/* MS does not know basic maths? */
#define M_PI		3.1415926535
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define rint(x) (int)((x)+0.5)
#define isnan		_isnan

//#define ostream ::ostream

/* Stupid windows does not know getopt() :( */
#ifdef __cplusplus
extern "C" {
#endif
int getopt(int nargc, char** nargv, char *ostr);
extern int opterr;		
extern int optind;		
extern int optopt;		
extern char	*optarg;	
#ifdef __cplusplus
}
#endif

#else

/************************************************************************************************
 FOR NON - WINDOWS COMPILERS
 *************************************************************************************************/

#define O_BINARY	0

#endif
