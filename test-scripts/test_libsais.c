#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "parse_fasta.h"
#include "libsais64.h"


// A function that terminates when enter key is pressed 
void fun() 
{ 
    printf("fun() starts \n"); 
    printf("Press enter to stop fun \n"); 
    while(1) 
    { 
        if (getchar()) 
            break; 
    } 
    printf("fun() ends \n"); 
} 
 
// The main program calls fun() and measures time taken by fun() 
int main(int argc, char *argv[]) 
{ 
    // Calculate the time taken by fun() 
    clock_t t;
    char *string = NULL; 
    ssize_t len = parse_fasta_upper(argv[1], &string);
    if (len <= 0)
	return 1;
    printf("Beginning testing with genome of length %ld\n", len);
    for (int i=0; i < 1; i++) {
        t = clock(); 
        int64_t *array = malloc(len * sizeof(int64_t));
        libsais64((unsigned char *)string, array, len, 0, NULL); 
        t = clock() - t; 
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("Seconds to execute: %f\n", time_taken); 
	free(array);
    }

    free(string);
    
    return 0; 
}

