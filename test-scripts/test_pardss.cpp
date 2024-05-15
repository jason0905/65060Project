#include <omp.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "parse_fasta.h"
#include "divsufsort.h"


 
// The main program calls fun() and measures time taken by fun() 
int main(int argc, char *argv[]) 
{ 
    clock_t t;
    char *string = NULL; 
    ssize_t len = parse_fasta_upper(argv[1], &string);
    if (len <= 0)
	return 1;
    int count = argc < 3 ? 1 : atoi(argv[2]);
    printf("Beginning testing with genome of length %ld\n", len);
    for (int i=0; i < count; i++) {
        t = clock(); 
	double start_time = omp_get_wtime();
	int64_t *array = (int64_t*) malloc(len * sizeof(int64_t));
        divsufsort((unsigned char *)string, array, len); 
        t = clock() - t; 
        double end_time = omp_get_wtime();
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("Clock seconds to execute: %f\n", end_time-start_time);
        printf("CPU-seconds to execute:   %f\n", time_taken);
	free(array);
    }

    free(string);
    
    return 0; 
}

