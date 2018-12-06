OBJDIR=objs
SRCDIR=src
RSADIR=rsa
CRYPTODIR=crypto

.PHONY: all clean dependancies

CXX=g++

# current construction of the project:
#	|-Makefile src-|
#		(src) |-rsa crypto include main.cpp-|
#			(rsa) |-rsa.cpp-| (crypto) |-crypto cpp-| (include) |-rsa crypto-|
#						(rsa) |-rsa.h-| (crypto) |-all crypto headers-|

# Add into the .bashrc and source
# export PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/bin/:$PATH
# export LD_LIBRARY_PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/lib64/:${LD_LIBRARY_PATH}

# This flag only works for linux machine, not Max OS
CXXFLAGS+= -g -Wall -std=c++11

# Add here the path of the directories containing the headers found with the "make dependancies" command
# This allow to have include "file_name.h" and not "directory1/directory2/file_name.h
# Alternatively, you can also mention a "master include" directory here and have the relative path directly FROM the master
# in your cpp file. For example, I include here the directory src/include. In my rsa.cpp I include rsa/rsa.h
# which has a relative path of src/include/rsa/rsa.h. This allow to know where are the headers with a quick look at the cpp.
INCLUDES = -I$(SRCDIR)/include \
		   -I$(HOME)/halide/include \
		   -I$(HOME)/halide/tools \
		   -I$(SRCDIR)/include/crypto

# define library paths in addition to /usr/lib
LDFLAGS = -L$(HOME)/halide/lib -lHalide -ldl

# define any libraries to link into executable:
LIBS = -lpthread -ldl

# The relevant cpp files can be found using the "make dependancies" command
# Add .cpp files appearing in the dependancies output here
# Add .h files path as describe earlier
CCFILES=$(SRCDIR)/$(RSADIR)/rsa.cpp $(SRCDIR)/main.cpp 

OBJS=$(OBJDIR)/*.o

# Compile & link direcly in one command (faster compile for small projects). 
# Consider partitioning the compilation as the project grows larger
rsa:
	$(CXX) $(CXXFLAGS) $(CCFILES) $(INCLUDES) $(LDFLAGS) $(LIBS) -o $@

# To know what are the dependencies of the cpp files to compile run make dependancies in the command
dependancies:
	$(CXX) -MM $(CCFILES) $(INCLUDES) $(LDFLAGS) $(LIBS)
dirs:
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) rsa
