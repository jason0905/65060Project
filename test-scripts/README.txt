This directory contains screipts used to benchmark the SA slgorithms.
Once compiled scripts, take a FASTA file name as the first argument and the number of trials as the second
Parallel test scripts take a number of threads as the third argument.

Special cases: the Goto test script prints output. The libsais test script only performs one trial (no 2nd argument)

Compilation:
Libsais:
gcc -g -Ilibsais-master/include/ -Wall -O3 -o test_libsais test-scripts/test_libsais.c test-scripts/parse_fasta.c libsais-master/src/libsais64.c libsais-master/src/libsais.c

Parallel Libsais:
gcc -Ilibsais-master/include/ -DLIBSAIS_OPENMP -fopenmp -Wall -O3 -o test_libsais_par test-scripts/test_libsais_par.c test-scripts/parse_fasta.c libsais-master/src/libsais64.c libsais-master/src/libsais.c

LFGSACA:
g++ test-scripts/test_lfgsaca.cpp test-scripts/parse_fasta.c -o test_lfgsaca -std=c++20 -Ilfgsaca-main/ -O3

CaPS-SA:
g++ test-scripts/test_capssa.cpp test-scripts/parse_fasta.c CaPS-SA-main/src/Suffix_Array.cpp -o test_capssa -std=c++20 -ICaPS-SA-main/include/ -I../parlaylib/include/ -O3 -march=znver2
Remark: We downloaded parlay lib from its GitHub repo. It's required to specify the arcitecture so g++ can include avx2 operations.

Parallel CaPS-SA:
g++ test-scripts/test_capssa_par.cpp test-scripts/parse_fasta.c CaPS-SA-main/src/Suffix_Array.cpp -o test_capssa_par -std=c++20 -ICaPS-SA-main/include/ -I../parlaylib/include/ -O3 -march=znver2

GSACA-DSH:
g++ -g test-scripts/test_gsacads.cpp test-scripts/parse_fasta.c -o test_gsacads -std=c++17 -march=native -funroll-loops -Igsaca-double-sort-main/ -O3
Remark: compilation flags from documentation. 

(Parallel): GSACA-DS1: as above, but with test_gsacads1(_par).cpp

DivSufSort:
gcc -Wall -O3 -DBUILD_DIVSUFSORT64 -Itest-scripts/ -Ilibdivsufsort-master/include/ -o test_divsufsort test-scripts/test_divsufsort.c test-scripts/parse_fasta.c libdivsufsort-master/lib/*.c
Remark: wrote a header file to include with compilation. Flag taken from undocumented make file option.

Parallel DivSufSort:
g++ -static-libstdc++ -std=c++11 -Wall -Wextra  -DNDEBUG -O2 -ffast-math -funroll-loops -fopenmp -Iparallel-divsufsort-master/include/ -Iparallel-divsufsort-master/lib/ -I../parallel-range-lite/include/ -o test_pardss test_pardss.cpp parse_fasta.c parallel-divsufsort-master/lib/*.cpp ../parallel-range-lite/lib/*.cpp
Remarks: We downloaded parallel-range-lite from GitHub. We used O2 and these flags based on the GitHub examples. 

Goto:
g++ test-scripts/test_goto.cpp test-scripts/parse_fasta.c goto-SA-ACGT-only/src/Suffix_Array.cpp -o test_goto_inplace -std=c++20 -Igoto-SA-ACGT-only/include/ -Igoto-SA-ACGT-only/merge/ -O3 -D MERGEIP
Remarks: flag MERGEIP specifies to use in-place merge as opposed to linear-space naive merge. Flag ACGTONLY compiles for a no-N alphaget (used for human genome) 





