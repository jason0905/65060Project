#ifndef GOTO_SUFFIX_ARRAY_HPP
#define GOTO_SUFFIX_ARRAY_HPP



#include <cstdint>
#include <cstddef>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include <immintrin.h>
#include <iostream>
// =============================================================================

namespace Goto_SA
{

// The Suffix Array (SA) constructor class for some given sequence.
template <typename T_idx_>
class Suffix_Array
{
private:

    typedef T_idx_ idx_t;   // Integer-type for indexing the input text.

    const idx_t* const T_;   // The input text.
    const idx_t n_; // Length of the input text.
    idx_t* const SA_;   // The suffix array.
    const idx_t alpha_size_; // The alphabet size.

    // Fields for profiling time.
    typedef std::chrono::high_resolution_clock::time_point time_point_t;
    constexpr static auto now = std::chrono::high_resolution_clock::now;
    constexpr static auto duration = [](const std::chrono::nanoseconds& d) { return std::chrono::duration_cast<std::chrono::duration<double>>(d).count(); };


    // Returns pointer to a memory-allocation for `size` elements of type `T_`.
    template <typename T_>
    static T_* allocate(idx_t size) { return static_cast<T_*>(std::malloc(size * sizeof(T_))); }

    // Step one of Goto's three step process. Returns the number of LMS substrings.
    T_idx_ step_one(const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size);

    // Processing a cell for step 1b part 1 of Goto's algorithm, returns the necessary offset for marking whether or not the cell is LML.
    T_idx_ step_oneb1_process_cell(idx_t target, const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size, idx_t arr_bound);

    // Processing a cell for step 1c part 1 of Goto's algorithm, returns the value to store in the cell based on whether or not it is LMS.
    T_idx_ step_onec1_process_cell(idx_t target, const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size);

    // Pre-recursive portion of second step of Goto's three step process; returns alphabet size for recursive case or zero if no recursion is needed.
   T_idx_ step_two_a(const idx_t* const T, idx_t* const SA, idx_t n, idx_t LMS_count);
    
    // Post-recursive portion of second step of Goto's three step process.
    void step_two_b(const idx_t* const T, idx_t* const SA, idx_t n, idx_t LMS_count);

    // Step three of Goto's three step process
    void step_three(const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size);

    // Inserting the preceding L suffix (if any) via the LE array
    void insert_L_via_LE(idx_t target, idx_t LE_offset, const idx_t* const T, idx_t* const SA, idx_t n, idx_t arr_bound);

    // Inserting the preceding R suffix (if any) via the RE array
    void insert_R_via_RE(idx_t target, idx_t RE_offset, bool is_S_when_equal, const idx_t* const T, idx_t* const SA, idx_t n);

    // Inserting using the LE array for Goto's algorithm
    void insert_via_LE(idx_t target, idx_t LE_offset, const idx_t* const T, idx_t* const SA, idx_t n, idx_t arr_bound);

    // Inserting using the RE array for Goto's algorithm
    void insert_via_RE(idx_t target, idx_t RE_offset, const idx_t* const T, idx_t* const SA, idx_t n);

public:

    // Constructs a suffix array object for the input text `T` of size `n`.
    Suffix_Array(const idx_t* T, idx_t n, idx_t alpha_size);

    // Constructs a suffix array for input text `T` of size `n` 
    // given a pointer to the start of the memory for the SA.
    Suffix_Array(const idx_t* T, idx_t n, idx_t alpha_size, idx_t* const SA);

    // Copy constructs the suffix array object from `other`.
    Suffix_Array(const Suffix_Array& other);

    ~Suffix_Array();

    const Suffix_Array& operator=(const Suffix_Array& rhs) = delete;

    // Returns the text.
    const idx_t* T() const { return T_; }

    // Returns the length of the text.
    idx_t n() const { return n_; }

    // Returns the suffix array.
    const idx_t* SA() const { return SA_; }

    // Constructs the suffix array.
    void construct();

    // Dumps the suffix array into the stream `output`.
    void dump(std::ofstream& output);
};

}

#endif
