#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "gsaca-double-sort.hpp"
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
    printf("Beginning testing with genome of length %ld\n", len);
    char *string_start = string;
    for (int j=0; j < count; j++) {
        t = clock(); 
	//We have to modify the string to add a 0 byte at the start
	//There is extra space at the end due to not including headers
	//But we still go through moving the string one byte to 
	//introduce a 0 byte at the start and give some penalty.
	for (size_t i = len; i > 0; i--) {
	    string_start[i] = string_start[i-1];
	}
	string_start[0] = 0;
	//printf("\"%s\"\n", string_start+1);
	//string[0] = string[len-1] = 0;
        uint64_t *array = (uint64_t*) malloc((len+2) * sizeof(uint64_t));
	gsaca_dsh((unsigned char *) string_start, array, len+2);
	//gsaca_dsh((unsigned char *) string, array, len);
	//Now, we have to simulate modifying the output
	//We know that the first two results in the SA are the first & last 0 bytes.
	//The real array starts at 2, and We decrement all other entries by 1 to 
	//account for the 0th entry being the whole string starting with \0
	for (size_t i = 2; i < len + 2; i++) {
	    array[i]--;
	}
	t = clock() - t;
        string_start++;	
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("Seconds to execute: %f\n", time_taken); 
	//for (size_t i = 2; i < len+2; ++i)
        //    printf("%lu ", array[i]);
        //printf("\n");
	free(array);
    }

    free(string);
    
    return 0; 
}

