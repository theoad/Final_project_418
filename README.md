# Final_project_418
High throughput rsa encryption/decryption using Halide scheduling. 

High throughtput rsa encryption/decryption using halide scheduling.

current construction of the project: 
  |__|__src                                                      bin lib Makefile__|
     |__main.cpp       |__rsa            |__crypto                  |__include  __|
                       |__ rsa.cpp__|    |__ all crypto cpp__|      |__rsa      |__crypto  __|
                                                                    |__rsa.h    |__all crypto headers__|
                                                                    
This project uses a upgraded gcc compiler present on ghc clusters. This allow to replace LLVM+clang required to use halide binaries.

ENvironement setup:

  - Add into the ~/.bashrc: 
      export PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/bin/:$PATH 
      export LD_LIBRARY_PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/lib64/:${LD_LIBRARY_PATH}

  - Source using: 
      source ~/.bashrc
