High throughtput rsa encryption/decryption using halide scheduling.

current construction of the project:
      |-Makefile src-|
              (src) |-rsa crypto include main.cpp-|
                      (rsa) |-rsa.cpp-| (crypto) |-all crypto cpp-| (include) |-rsa crypto-|
                                              (rsa) |-rsa.h-| (crypto) |-all crypto headers-|

This project uses a upgraded gcc compiler present on ghc clusters.
This allow to replace LLVM+clang required to use halide binaries.

ENvironement setup:

- Add into the ~/.bashrc:
 export PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/bin/:$PATH
 export LD_LIBRARY_PATH=/afs/cs.cmu.edu/academic/class/15418-f18/public/gcc-install/lib64/:${LD_LIBRARY_PATH}

- Source using:
 source ~/.bashrc
