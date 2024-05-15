#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "Suffix_Array.hpp"
#include "parse_fasta.h"
//#include <fstream>

 
int main(int argc, char *argv[]) 
{ 
    // Calculate the time taken by fun() 
    clock_t t;
    char *string = NULL; 
    //Turns OK this is OK to compile in C++ as well
    ssize_t len = parse_fasta_upper(argv[1], &string);
    if (len <= 0)
	return 1;
    int count = argc < 3 ? 1 : atoi(argv[2]);
    //const std::string op_path(argv[3]);
    //std::ofstream output(op_path);
    fprintf(stderr, "Beginning testing with genome of length %ld\n", len);
    for (int i=0; i < count; i++) {
        t = clock(); 
	//Need to convert the string to be 1-index integer based
	//Output will also go here
	uint64_t *array = (uint64_t*) malloc((len+1) * sizeof(uint64_t));
	for (uint64_t j = 0; j < len; j++) {
	    switch (string[j]) {
		case 'A': array[j] = 1; break;
		case 'C': array[j] = 2; break;
		case 'G': array[j] = 3; break;
        #ifdef ACGTONLY
        case 'T': array[j] = 4; break;
        #else
        case 'N': array[j] = 4; break;
        case 'T': array[j] = 5; break;
        #endif
		default: array[j] = 0; break;
	    }
	}
	array[len] = 0;
	//for (size_t j = 0; j < len+1; ++j) 
        //    printf("%lu ", array[j]);
        //printf("\n");
	fprintf(stderr, "Conversion complete\n");
    #ifdef ACGTONLY
	Goto_SA::Suffix_Array<uint64_t> suf_arr(array, len+1, 5 );//, true);
    #else
    Goto_SA::Suffix_Array<uint64_t> suf_arr(array, len+1, 6);//, true);
    #endif
    suf_arr.construct();
    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
    fprintf(stderr, "Seconds to execute: %f\n", time_taken); 
	//suf_arr.dump(output);
	const uint64_t *SA = suf_arr.SA();
	for (size_t j = 1; j < len+1; ++j) //Skip first element
	    printf("%lu ", SA[j]);
	printf("\n");
	free(array);
    }
    free(string);
    
    return 0; 
}

