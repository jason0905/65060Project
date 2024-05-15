#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "Suffix_Array.hpp"
#include "parse_fasta.h"

 
int main(int argc, char *argv[]) 
{ 
    // Calculate the time taken by fun() 
    clock_t t;
    char *string = NULL; 
    //Turns out this is OK to compile in C++ as well
    ssize_t len = parse_fasta_upper(argv[1], &string);
    if (len <= 0)
	return 1;
    int count = argc < 3 ? 1 : atoi(argv[2]);
    setenv("PARLAY_NUM_THREADS","1",1);
    printf("Beginning testing with genome of length %ld\n", len);
    for (int i=0; i < count; i++) {
        t = clock(); 
	//Probably bad to still do this in C++
        //uint64_t *array = (uint64_t*) malloc(len * sizeof(uint64_t));
	CaPS_SA::Suffix_Array<uint64_t> suf_arr(string, len, 0, 0);
	suf_arr.construct();
        t = clock() - t; 
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("Seconds to execute: %f\n", time_taken); 
    }

    free(string);
    
    return 0; 
}

