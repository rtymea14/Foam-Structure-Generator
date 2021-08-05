# Voro++ makefile
#
# Author : Chris H. Rycroft (LBL / UC Berkeley)
# Email  : chr@alum.mit.edu
# Date   : August 30th 2011

# Load the common configuration file
include /home/raihan/Desktop/voro++-0.4.6/config.mk

# List of executables
EXECUTABLES=import #import2 random_points 

# Makefile rules
all: $(EXECUTABLES)

import: import.cc
	$(CXX) $(CFLAGS) -no-pie $(E_INC) $(E_LIB) -o import import.cc -lvoro++

clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean
