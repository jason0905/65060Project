# Implementation of the FGSACA Algorithm

This repository contains an implementation of the FGSACA Suffix Array, BBWT Construction Algorithm and eBWT Construction Algorithm from  
``
Jannik Olbrich, Enno Ohlebusch, Thomas Büchler *On the Optimisation of the GSACA Suffix Array Construction Algorithm*, 2022
``
# Computing the Suffix Array and BBWT
This has only been tested on GNU/Linux with GCC-12. **Other operating systems and compilers may or may not work.**
The suffix array can be computed using the following syntax.


For the interface see `fgsaca.hpp`. Some examples are given below.


The suffix array can be computed as follows:
```c++
#include "fgsaca.hpp"
using T = uint32_t; // use uint64_t if n is not smaller than std::numeric_limits<int32_t>::max()
uint8_t* text = ...;
T* SA = ...;
...
fgsaca<T>(text, SA, n, 256); // 256 is the size of the alphabet
```


The BBWT of a text can be constructed as follows:
```c++
uint8_t* text = ...;
uint8_t* BBWT = new uint8_t[n];
fbbwt<uint8_t>(text, BBWT, n, 256); // 256 is the size of the alphabet
```


There are varous ways to pass the string collection for the construction of the eBWT (see `main.cpp`)
```c++
using T = uint32_t; // use uint64_t if n is not smaller than std::numeric_limits<int32_t>::max()
uint8_t* texts = ...; // concatenation of the strings of which the eBWT should be constructed
T* lengths = ...; // lengths of the strings of which the eBWT should be constructed, in the same order as in texts
uint8_t* eBWT = new uint8_t[n]; // n is the sum of lengths
T* res_idx = new T[k]; // k is the number of strings of which the eBWT should be constructed
febwt<T>(texts, lengths, eBWT, res_idx, k, 256); // 256 is the size of the alphabet
```


Concrete examples can be found in `main.cpp`.
Compile with

```
g++ main.cpp -o main -std=c++20
```
