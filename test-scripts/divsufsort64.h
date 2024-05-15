//Minimal divsufsort header from the GitHub API example
//And stuff that would be done by CMake
#include <stdint.h>
#include <stdlib.h>

/*These would normally be parsed together by CMake for 32 or 64 bit version*/
/* Data types, hardcoded as 64 bit */
#ifndef SAUCHAR_T
#define SAUCHAR_T
typedef uint8_t sauchar_t;
#endif /* SAUCHAR_T */
#ifndef SAINT_T
#define SAINT_T
typedef int32_t saint_t;
#endif /* SAINT_T */
#ifndef SAIDX64_T
#define SAIDX64_T
typedef int64_t saidx64_t;
#endif /* SAIDX@W64BIT@_T */

/* Output format specifiers*/
#ifndef PRIdSAINT_T
#define PRIdSAINT_T "d"
#endif /* PRIdSAINT_T */
#ifndef PRIdSAIDX64_T
#define PRIdSAIDX64_T "ld"
#endif /* PRIdSAIDX@W64BIT@_T */

/*Misc macros*/
#ifndef INLINE
#define INLINE inline
#endif
#ifndef PROJECT_VERSION_FULL
#define PROJECT_VERSION_FULL "2.0.2-1"
#endif

//Among other stuff, this defines divsufsort as divsufsort64 which we need to call divsufsort
//From the test script after including this. the private.h is guarded against being included
//multiple times, so this is fine.
#include "divsufsort_private.h"

/*
 * Constructs the suffix array of a given string.
 * @param T[0..n-1] The input string.
 * @param SA[0..n-1] The output array or suffixes.
 * @param n The length of the given string.
 * @return 0 if no error occurred, -1 or -2 otherwise.
 */
saint_t
divsufsort64(const sauchar_t *T, saidx64_t *SA, saidx64_t n);

/*
 * Constructs the burrows-wheeler transformed string of a given string.
 * @param T[0..n-1] The input string.
 * @param U[0..n-1] The output string. (can be T)
 * @param A[0..n-1] The temporary array. (can be NULL)
 * @param n The length of the given string.
 * @return The primary index if no error occurred, -1 or -2 otherwise.
 */
saidx64_t
divbwt64(const sauchar_t *T, sauchar_t *U, saidx64_t *A, saidx64_t n);
