#include <stdio.h>
//#include <stdlib.h>

#include <algorithm>
#include <string>
#include <sstream>

#define NDEBUG
#include <algobase.h>

//Using only these subsets because the full files are not needed and raise other compile errors
//That I don;t want to deal with 
#include <bsearch.h>
//#include <minimum_storage_algos.h>
#include <swap.h>
//#include <linear_merge_algos.h>

#include <rotation_algos.h>
#include <hwang_lin.h>
#include <TAMC-2008/tamc_2008_algo.h>



template <class dtype> void merge_inplace(dtype *start, size_t mid, size_t len) {
    //if (mid > len - mid) {
    //    __rotate_swapping(start, start+mid, start+len);
    //    mid = len - mid;
    //}
    if (mid == 0 || len - mid == 0) return;
    TAMC2008_merge<dtype*,&__rotate_swapping>(start, start+mid, start+len);
}

//template <class dtype> void merge_inplace(dtype *start, size_t l1, size_t l2) {
//    if (l1 > l2) {
//        __rotate_swapping(start, start+l1, start+l1+l2);
//        l1 = l1 + l2; l2 = l1 - l2; l1 = l1 - l2;
//    }
//    TAMC2008_merge<dtype*,&__rotate_swapping>(start, start+l1, start+l1+l2);
//}
