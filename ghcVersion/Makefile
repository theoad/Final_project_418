OBJDIR=objs
OUTPUTDIR=out
SRCDIR=src
RSADIR=rsa
CRYPTODIR=crypto
EXEC=rsa

.PHONY: all clean dependancies

CXX=g++

# current construction of the project:
#	|-Makefile src-|
#		(src) |-rsa include main.cpp-|
#			(rsa) |-rsa.cpp-| (include) |-rsa-|
#						(rsa) |-rsa.h-|

# Add into the .bashrc and source (need llvm+clang)
# export PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/bin/:$PATH
# export LD_LIBRARY_PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/lib64/:${LD_LIBRARY_PATH}
# export LD_LIBRARY_PATH=~/halide/bin/:${LD_LIBRARY_PATH}
# export LLVM_CONFIG=/usr/local/depot/llvm-7.0/bin/llvm-config
# export CLANG=/usr/local/depot/llvm-7.0/bin/clang

# Add into your home directory:
# halide directory containing the halides binaries and tools
# crypto directory containing the cryptopp files (as well as the compiled cryptolib.a -see crypto README.txt)

# This flag only works for linux machine, not Max OS
CXXFLAGS+= -g -Wall -std=c++11 -fexceptions

# Add here the path of the directories containing the headers found with the "make dependancies" command
# This allow to have include "file_name.h" and not "directory1/directory2/file_name.h
# Alternatively, you can also mention a "master include" directory here and have the relative path directly FROM the master
# in your cpp file. For example, I include here the directory src/include. In my rsa.cpp I include rsa/rsa.h
# which has a relative path of src/include/rsa/rsa.h. This allow to know where are the headers with a quick look at the cpp.
INCLUDES = -I$(SRCDIR)/include \
		   -I$(HOME)/halide/include \
		   -I$(HOME)/halide/tools \
		   -I$(HOME)/crypto

# define library paths in addition to /usr/lib
LDFLAGS = -L$(HOME)/halide/bin \
			-L$(HOME)/crypto

# define any libraries to link into executable:
LIBS = -lHalide -lpthread -ldl `libpng-config --cflags --ldflags` -ljpeg -lcryptopp

LLVMFLAGS = -rdynamic

# The relevant cpp files can be found using the "make dependancies" command
# Add .cpp files appearing in the dependancies output here
# Add .h files path as describe earlier
CCFILES=$(SRCDIR)/$(RSADIR)/rsa.cpp $(SRCDIR)/main.cpp

OBJS=$(OBJDIR)/*.o

# Compile & link direcly in one command (faster compile for small projects).
# Consider partitioning the compilation as the project grows larger
$(EXEC):
	$(CXX) $(CXXFLAGS) $(CCFILES) $(INCLUDES) $(LDFLAGS) $(LIBS) $(LLVMFLAGS) -o $@

# To know what are the dependencies of the cpp files to compile run make dependancies in the command
dependancies:
	$(CXX) -MM $(CCFILES) $(INCLUDES) $(LDFLAGS) $(LIBS)

install:
	mkdir $(OUTPUTDIR)
clean:
	rm -rf $(OBJDIR) $(OUTPUTDIR) $(EXEC)
