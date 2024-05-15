#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include "Suffix_Array.hpp"
#include "parse_fasta.h"

//taken from the CaPS-SA implementation
#include <chrono>
typedef std::chrono::high_resolution_clock::time_point time_point_t;
constexpr static auto now = std::chrono::high_resolution_clock::now;
constexpr static auto duration = [](const std::chrono::nanoseconds& d) { return std::chrono::duration_cast<std::chrono::duration<double>>(d).count(); };

 
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
    const char *threads = argc < 4 ? "1" : argv[3];
    setenv("PARLAY_NUM_THREADS",threads,1);
    printf("Beginning testing with genome of length %ld\n", len);
    for (int i=0; i < count; i++) {
        t = clock(); 
	const auto start_time = now();
	CaPS_SA::Suffix_Array<uint64_t> suf_arr(string, len, 0, 0);
	suf_arr.construct();
	t = clock() - t; 
        const auto end_time = now();
	double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("Clock seconds to execute: %f\n", duration(end_time-start_time));
	printf("CPU-seconds to execute:   %f\n", time_taken);
    }

    free(string);
    
    return 0; 
}

