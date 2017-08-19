# *************************************************************************
# Makefile
#
# Author: Christian Wolf
#         christian.wolf@insa-lyon.fr
#
# Changelog:
# 14.04.2006 cw: First version
#
# 1 tab = 4 spaces
# *************************************************************************

CFLAGS  	=-Wall -g -I. -I/usr/include/libxml2 -DHAVE_LIBXML2
CC			=gcc
CPPLIBS		=-lm -lstdc++ -lxml2	
OBJECTS		= BoxEvaluator.o BoxMatrix.o BoxOverlap.o CIL.o OptionContainer.o ParameterParsing.o Rect.o RectEval.o RectEvalList.o 
BINARIES 	= evaldetection readdeteval
BUILD       = ../Bin

.SUFFIXES:
.SUFFIXES:	.o .c .cpp .cc

all: 		$(BINARIES)

.cc.o:		$< $*.h
		$(CC) $(CFLAGS) -c $*.cc

.cpp.o:		$< $*.h
		$(CC) $(CFLAGS) -c $*.cpp		
		
.c.o:		$< $*.h
		$(CC) $(CFLAGS) -c $*.c

evaldetection:	$(OBJECTS) evaldetection.o
			$(CC) -o $(BUILD)/evaldetection evaldetection.o $(OBJECTS) $(CPPLIBS)
		
readdeteval:	$(OBJECTS) readdeteval.o
			$(CC) -o $(BUILD)/readdeteval readdeteval.o $(OBJECTS) $(CPPLIBS)
			
clean:	
		rm -f *.o 
		rm -f $(addprefix $(BUILD)/, $(BINARIES))
