
#include "Suffix_Array.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cassert>

#ifdef MERGEIP
#include "merge.h"
#endif

namespace Goto_SA
{
#ifdef MERGEIP
/* What's this mess for?
 * The in-place merge assumes that it is sorting the provided values (here, indices)
 * But, we actually need to compare/sort the indices based on the values the reference
 * So, we create a fake struct with comparison operators that do that, and then
 * we cast the indices to this struct before invoking the in-place merge.
 * Since C stores structs with just their values, and the only value is the same
 * size as index, we can do this with 0 cost. 
 * This way, no modification of the general in-place merge code is required.
 * Unfortunately, to avoid needing a copy of the pointer to the T (text) array
 * in every element, that variable has to be global or static. This is not the
 * nicest thing and costs 8 bytes, but it is what it is. It seems the least bad option.
 * as I don't know a way to locally define a struct that references a local variable
 * in function definitions
 */
template <typename idx_t>
struct value_wrap {
    idx_t index;
    static const idx_t* values;

    bool operator<(const value_wrap &other) const {
        return values[index] < values[other.index];
    }
    bool operator<=(const value_wrap &other) const {
        return values[index] <= values[other.index];
    }
    bool operator>(const value_wrap &other) const {
        return values[index] > values[other.index];
    }
    bool operator>=(const value_wrap &other) const {
        return values[index] >= values[other.index];
    }
    bool operator==(const value_wrap &other) const {
        return values[index] == values[other.index];
    } 
    bool operator!=(const value_wrap &other) const {
        return values[index] != values[other.index];
    }
};
template <typename idx_t>
const idx_t *value_wrap<idx_t>::values = nullptr;
#endif


template <typename T_idx_>
Suffix_Array<T_idx_>::Suffix_Array(const idx_t* const T, const idx_t n, const idx_t alpha_size, const bool is_debug):
    T_(T),
    n_(n),
    SA_(allocate<idx_t>(n_)),
    alpha_size_(alpha_size),
    is_debug_(is_debug)
{
}


template <typename T_idx_>
Suffix_Array<T_idx_>::Suffix_Array(const idx_t* const T, const idx_t n, const idx_t alpha_size, idx_t* const SA, const bool is_debug):
    T_(T),
    n_(n),
    SA_(SA),
    alpha_size_(alpha_size),
    is_debug_(is_debug)
{
}


template <typename T_idx_>
Suffix_Array<T_idx_>::Suffix_Array(const Suffix_Array& other): Suffix_Array(other.T_, other.n_, other.alpha_size_, other.is_debug_)
{
    std::memcpy(SA_, other.SA_, n_ * sizeof(idx_t));
}


template <typename T_idx_>
Suffix_Array<T_idx_>::~Suffix_Array()
{
    std::free(SA_);
}


/*
    Assumptions: First n characters of T contains the string we want to sort
*/
template <typename T_idx_>
T_idx_ Suffix_Array<T_idx_>::step_one(const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size)
{
    //Step a: need to somehow move a bunch of indices into SA depending on the values in RE without overwriting RE.
    // Based off of ideas elsewhere, should probably overwrite RE, where I'm being lazy and not storing the first element of RE
    idx_t i, j, k, idx, tmp; // indices into SA
    idx_t curr; // letter of alphabet for comparison purposes
    if(is_debug_) {
        std::cerr << "Starting T and SA:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << T[i];
        }
        std::cerr << ' ' << alpha_size << '\n';
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i];
        }
        std::cerr << '\n';
    }

    //Step a0: store prefix sum of COUNT-1 into last alpha_size-1 elements of SA, not bothering with the number of 0s
    for(i = n - alpha_size + 1; i < n ;++i) {
        SA[i] = -1;
    }
    for(i = 0; i < n - 1; ++i) {
        SA[T[i] + n - alpha_size]++;
    }
    for(i = n - alpha_size + 2; i < n ;++i) {
        SA[i] += SA[i - 1];
    }

    if(is_debug_) {
        std::cerr << "Step A0 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }


    //Step a1: store the LMS-substring start locations into the appropriate approximately RE locations,
    //replacing the location storing the approximate RE with the item if it's the absolute last item possible with that
    //starting character/digit/whatever
    // We add a flag of adding by n + 2 when we replace an RE value to make it clear the replacement happened

    // Note that the last thing is always an LMS string and also the only LMS string starting from 0,
    // which we store in the first location to prevent everything from being one-indexed
    SA[n - alpha_size] = n + 1;
    SA[0] = n - 1;
    bool next_is_small = false;
    for(i = n - 3; i < n; --i) {
        if(T[i] < T[i + 1]) {
            next_is_small = true;
        }
        else if (T[i] > T[i + 1]) {
            if(next_is_small) {
                insert_via_RE(i + 1, n - alpha_size, T, SA, n);
            }
            next_is_small = false;
        }
    }

    // Cleanup step: go through and verify that nobody stole space from earlier intervals. Corresponds to looping through the RE values and verifying emptiness.
    // Flags don't exist yet in main array, so no cleanup needed
    for(i = n - alpha_size + 1, curr = 1; i < n; ++i, ++curr) {
        if(SA[i] < n && SA[SA[i]] != n + 1 && T[SA[SA[i]]] > curr) {
            idx = T[SA[SA[i]]] + n - alpha_size;
            SA[idx] = SA[SA[i]] + n + 2;
            SA[SA[i]] = n + 1;
        }
    }

    // Adding some flags for things
    for(i = 0; i < n - alpha_size + 1; ++i) {
        if(SA[i] < n) {
            SA[i] += 2 * n + 4;
        }
    }
    for(i = n - alpha_size + 1; i < n; ++i) { 
        if(SA[i] > n + 1) {
            SA[i] += 2 * n + 4;
        }
    }


    if(is_debug_) {
        std::cerr << "Step A1 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Step b0: Need to turn the RE values into LE values.
    // Easiest way might be to start from each RE value and go left until you hit a stored thing or the location of the next RE to the left
    // In particular the replaced RE values don't need to turn into LE values
    // TODO: Fix this so that itwon't run into issues when n = alpha_size? Or just ignore those cases for now? could just fix things by using signed ints

    for(i = n - 1; i > n - alpha_size; --i) {
        if(SA[i] < n) {
            idx = i--;
            break;
        }
    }
    for(; i > n - alpha_size; --i) {
        if(SA[i] < n) {
            // Replace the RE value in curr with an LE value
            for(j = SA[idx]; j > SA[i] && SA[j] == n + 1; j--) {}
            // Either j is the RE of the next interval on the left (which must also be fully unfilled)
            // or j has a thing in it. Either way, the appropriate LE must be j + 1
            SA[idx] = j + 1;
            idx = i;
        }
    }
    if(idx != n) {
        for(j = SA[idx]; j > 0 && SA[j] == n + 1; j--) {}
        SA[idx] = j + 1;
    }

    if(is_debug_) {
        std::cerr << "Step B0 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Step b1: Go through SA in increasing order, and for each suffix we find and add the immediately preceding L-suffix into the SA as well,
    // removing the thing that we just preceded. Will have to maintain invariant that thing in spare slot is always largest thing in array starting with that value.
    // In order to make sure that we can compute the RE values we need to do these deletions afterwards though: we'll just mark LML values by adding flag.
    // This is effectively true already given that all the LMS suffixes are equivalent so far as far as we're considered
    // Rough idea: Keep track of the next known LE number.
    // In general, if current spare slot is full then the interval should be filled too so we can just progress based on the known info that
    // we see in the main SA stuff. If main part is empty then the next smallest number must be the value in the spare slot.
    // Once the spare slot has an LE number in it then we can simply just traverse main array until we hit the next known LE number: the only possible empties
    // in the main array would be the current interval, which should be empty once we see an empty in the main array. As such, we'd be allowed to traverse main
    // array until we either hit the next known LE or hit a nonempty value (which would also correspond to a nonempty spare), and in the former case we can safely
    // progress through the entire spare array until we hit the next known LE number.
    // This is still linear time: one counter jumps forwards from one known LE to the next, and other counters go linearly, but no counters backtrack.

    // In other words: while main array nonempty, progress in main array. If main array is empty, update the next-LE-spot counter: we're happy to keep progressing
    // until we hit there. Once main array reaches that spot, make sure spare array catches up. Once we catch up, next-LE-spot counter needs to push forwards.

    // Current spare slot
    curr = 1;
    // Current spare slot index
    idx = n - alpha_size + 1;
    // Current main array location
    i = 1;
    // next LE spot in spare
    j = n - alpha_size + 1;
    for(; j < n && SA[j] > n; ++j) {}

    // process the boring first element, which is LMS and therefore not LML
    // TODO: Figure out the bug that if a main slot is replaced after already being processed then bad things happen.
    // However, this is impossible if the next one has been processed already so this is just a bunch of careful checks about where the replaced
    // thing is.
    step_oneb1_process_cell(0, T, SA, n, alpha_size, n - alpha_size + 1);
    while(idx < n) {
        k = SA[i];
        while(k > n + 1) {
            k -= n + 2;
        }
        // Note that we don't need to check if idx reached target in the while loop, because last spare cell is processed after last main cell
        if(i > n - alpha_size) {
            // main slots finished, just finish off the spare cells
            if(SA[idx] > n + 2) {
                // Spare cell has nonzero element of array
                if(step_oneb1_process_cell(idx, T, SA, n, alpha_size, n - alpha_size + 1)) {
                    --i;
                }
            }
            ++curr;
            ++idx;
        } else if(k == n + 1) {
            for(; j < n && SA[j] > n; ++j) {}
            if(j < n && SA[j] <= i) {
                // We don't know if we can process this empty cell, move in spare array until we catch up
                while(idx < j) {
                    if(SA[idx] > n + 2) {
                        // Spare cell has nonzero element of array
                        if(step_oneb1_process_cell(idx, T, SA, n, alpha_size, n - alpha_size + 1)) {
                            --i;
                            ++curr;
                            ++idx;
                            break;
                        }
                    }
                    ++curr;
                    ++idx;
                }
                j = idx + 1;
            } else {
                // We can process this cell, which is empty, so just skip
                ++i;
            }
        } else if(T[k] > curr) {
            // Use spare cell
            if(SA[idx] > n + 2) {
                // Spare cell has nonzero element of array
                if(step_oneb1_process_cell(idx, T, SA, n, alpha_size, n - alpha_size + 1)) {
                    --i;
                }
            }
            ++curr;
            ++idx;
        } else {
            if(T[k] == curr && SA[i] > 2 * n + 3 && SA[idx] > n + 2 && SA[idx] < 2 * n + 4) {
                // Main cell contains LMS, spare cell is LML, so swap main cell and spare cell 
                tmp = SA[i];
                SA[i--] = SA[idx] - n - 2;
                SA[idx] = tmp + n + 2;
            } else if(SA[i] > 0) {
                // Use main cell
                tmp = SA[i];
                step_oneb1_process_cell(i, T, SA, n, alpha_size, n - alpha_size + 1);
                if(SA[i] != tmp && SA[i] != tmp + n + 2) {
                    // Possible to have really annoying situations where your current cell actually belongs in spares array
                    // In this case we actually increment curr and idx and not i, since we may need to reprocess current cell.
                    // In particular, the item in spares might need changing by k.
                    ++curr;
                    ++idx;
                    --i;
                }
            }
            ++i;
        }
    }
    // First Cleanup step: anything excessively large gets decreased by 2n + 4
    for(i = 0; i < n - alpha_size + 1; ++i) {
        if(SA[i] > 2 * n + 3) {
            SA[i] -= 2 * n + 4;
        }
    }
    for(i = n - alpha_size + 1; i < n; ++i) {
        if(SA[i] > 3 * n + 5) {
            SA[i] -= 2 * n + 4;
        }
    }


    // Cleanup step: go through and verify that nobody stole space from later intervals. Corresponds to looping through the LE values and verifying emptiness.
    // Note that flags can mess with cleanup
    for(i = n - alpha_size + 1, curr = 1; i < n; ++i, ++curr) {
        if(SA[i] < n - alpha_size + 1 && SA[SA[i]] != n + 1) {
            tmp = SA[SA[i]];
            if(tmp > n) {
                tmp -= n + 2;
            }
            if(T[tmp] < curr) {
                idx = T[tmp] + n - alpha_size;
                SA[idx] = SA[SA[i]] + n + 2;
                SA[SA[i]] = n + 1;
            }
        }
    }

    if(is_debug_) {
        std::cerr << "Step B1 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }


    // Step c0: Change the LE values back into RE values. Note that unmarked things will need to be dropped, so any LE values currently used as spare cells
    // will need to be carefully considered. If they store a LML (technically any L) then we don't have to find the corresponding RE value, but otherwise
    // we should find the LE value.
    // Plan: We ignore RE[0], doesn't matter. Start from 1 and keep going until we hit either something starting with a value bigger than 1 or the next
    // LE value. If we encounter former first, RE[1] is just one less than the thing starting with digit bigger than 1. If we encounter latter first
    // (or at the same time), next RE is just one less than the next LE value. Furthermore everything until that coordinate of the LE array can all use that RE
    // Repeat until we finish RE[alpha_size - 1] (which should always be n - 1)

    // Ideally I would delete the flags but I think I need them later so I'll leave in the LML flags.

    // We won't bother deleting SA[0] because that would be useful later anyways.

    // Current LE/RE slot
    curr = 1;
    idx = n - alpha_size + 1;
    // Current main slot location
    i = 1;
    // Current index checked in LE/RE for emptiness
    j = n - alpha_size + 2;

    while(idx < n) {
        if(SA[idx] < 2 * n + 3) {
            if(idx >= j) {
                j = idx + 1;
            }
            for(; j < n && SA[j] > n; ++j) {}
            tmp = n - alpha_size + 1;
            if(j < n) {
                tmp = SA[j];
            }
            for(; i < tmp; ++i) {
                if(SA[i] > n + 1 && T[SA[i] - n - 2] > curr) {
                    break;
                } else if(SA[i] < n && T[SA[i]] > curr) {
                    break;
                }
                if(SA[i] < n) {
                    // Isn't LML, clear
                    SA[i] = n + 1;
                }
            }
            SA[idx] = i - 1;
        }
        curr++;
        idx++;
    }
    for(; i <= n - alpha_size; ++i) {
        if(SA[i] < n) {
            SA[i] = n + 1;
        }
    }

    if(is_debug_) {
        std::cerr << "Step C0 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Step C1: basically step b1 but going right to left and adding S suffixes instead of L. Also now we won't need to recompute LE or SE ever so we can just
    // delete things if they are processed.
    // We can theoretically even skip the cleanup step, I think, but then checking intermediate progress gets even more annoying so I'll delete it anyways.
    // Note that we have the annoying situation that the largest thing in each interval, if any, is the one that starts off in the large spot.
    // As a thought: maybe leave the LMLs marked, but the S's unmarked. Then main and spare elements with the same starting value would satisfy the property
    // that the thing in the spare is either the largest LML or smallest S, so the main takes precedent if it is S but the spare takes precedent if main isn't S.
    // This is quite bothersome, but probably is manageable...
    // In particular this means that when we extract everything, the smallest S will be in the spare slots (if those are filled).

    // Current spare slot
    curr = alpha_size - 1;
    // Current spare slot index
    idx = n - 1;
    // Current main array location
    i = n - alpha_size;
    // next RE spot in spare
    j = n - 1;
    for(; j > n - alpha_size && SA[j] > n; --j) {}


    // Note that SA[0], which we left in from before, is in fact an LMS suffix. Also we don't actually need to process that one
    // Bug: The spare array already contains an LML, but there are LMS things in that interval that are bigger, which needs to be added somehow.
    // Idea: When scanning LTR to find LMLs, whenever I see an LMS I can swap it into the spare slot if spare slot is full
    // In particular, LMS needs to be flagged, say with 2n + 4 extra, so that we can do these swaps.
    // Should be resolved.
    while(idx > n - alpha_size) {
        // Note that we still only need to check until idx hits minimum: anything earlier in the array than the smallest S/largest LML starting with 1
        // is going to be an L starting with 1 which can only be preceded by S if there's an S starting with 0. In fact, there will be no LML's starting with 1,
        // so there really shouldn't be anything to process by that point.
        if(i == 0) {
            // main slots finished, just finish off the spare cells
            if(SA[idx] > n + 2) {
                // Spare cell has nonzero element of array
                if(step_onec1_process_cell(idx, T, SA, n, alpha_size, true)) {
                    ++i;
                }
            }
            --curr;
            --idx;
        } else if(SA[i] == n + 1) {
            for(; j > n - alpha_size && SA[j] > n; --j) {}
            if(j > n - alpha_size && SA[j] >= i) {
                // We don't know if we can process this empty cell, move in spare array until we catch up
                while(idx > j) {
                    if(SA[idx] > n) {
                        // Spare cell has element of array, don't need to check for zero because we don't insert zeroes in step c.
                        if(step_onec1_process_cell(idx, T, SA, n, alpha_size, true)) {
                            ++i;
                            --curr;
                            --idx;
                            break;
                        }
                    }
                    --curr;
                    --idx;
                }
                j = idx - 1;
            } else {
                // We can process this cell, which is empty, so just skip
                --i;
            }
        } else {
            tmp = SA[i];
            if(tmp > n) {
                tmp -= n + 2;
            }
            if(T[tmp] < curr) {
                // Use spare cell
                if(SA[idx] > n) {
                    // Spare cell has element of array, don't need to check for zero because we don't insert zeroes in step c.
                    if(step_onec1_process_cell(idx, T, SA, n, alpha_size, true)) {
                        ++i;
                    }
                }
                --curr;
                --idx;
            } else if(SA[i] > n && (T[tmp] <= curr)) {
                // SA contains an L item, so we use spare cell if nonempty and appropriate
                if(SA[idx] > n) {
                    // Spare cell has element of array, don't need to check for zero because we don't insert zeroes in step c.
                    if(step_onec1_process_cell(idx, T, SA, n, alpha_size, true)) {
                        ++i;
                    }
                }
                --curr;
                --idx;
            } else {
                // Use main cell
                // Possible to have really annoying situations where your current cell actually belongs in spares array
                // In this case we actually decrement curr and idx and not i, since we may need to reprocess current cell.
                // In particular, the item in spares might need changing by k.
                tmp = SA[i];
                step_onec1_process_cell(i, T, SA, n, alpha_size, false);
                if(SA[i] == tmp || SA[i] == n + 2 + tmp) {
                    --i;
                } else {
                    --idx;
                    --curr;
                }
            }
        }
    }

    // Cleanup step: go through and verify that nobody stole space from earlier intervals. Corresponds to looping through the RE values and verifying emptiness.
    // No flags at this point I believe, so this is fine.
    for(i = n - alpha_size + 1, curr = 1; i < n; ++i, ++curr) {
        if(SA[i] < n - alpha_size + 1) {
            tmp = SA[SA[i]];
            while(tmp > n + 1) {
                tmp -= n + 2;
            }
            if(tmp != n + 1 && T[tmp] > curr) {
                idx = T[tmp] + n - alpha_size;
                SA[idx] = SA[SA[i]] + n + 2;
                SA[SA[i]] = n + 1;
            }
        }
        if(SA[i] < n + 2 || SA[i] > 2 * n + 3) {
            SA[i] = n + 1;
        } else {
            SA[i] -= n + 2;
        }
    }

    for(i = 1; i <= n - alpha_size; ++i) {
        if(SA[i] > n + 1) {
            SA[i] = n + 1;
        }
    }

    if(is_debug_) {
        std::cerr << "Step C1 done:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Step c2: Reorder the stuff in step c1 to be in increasing order and without blank spaces, all on the very left.
    // We do this in two steps: first, we move everything to the far right, removing blank spaces. We keep track of where n - alpha_size + 1 goes,
    // since that's important
    // Second, we merge the two regions in parallel. Note that objects that started in the spare parts of the array are smaller than LMS values
    // in the main part of the array of the same starting digit.

    // Note that a key reason why this won't run into issues is because at most half the things are LMS, so we can fit two copies side by side if need be.
    j = n;
    for(i = n - 1; i > n - alpha_size; --i) {
        if(SA[i] != n + 1) {
            SA[--j] = SA[i];
            if(j != i) {
                SA[i] = n + 1;
            }
        }
    }
    idx = j;
    tmp = j;
    for(; i > 0; --i) {
        if(SA[i] != n + 1) {
            SA[--j] = SA[i];
            if(j != i) {
                SA[i] = n + 1;
            }
        }
    }
    //gonna be lazy and not deal with SA[0] because we'd refill it with the same value immediately.

    // j contains the start of the nonempty parts, while idx contains the start of the things that used to be spare.
    // We duplicate idx in tmp as well so that one can move while the other stays as a marker.
    // i is the current location to insert things in the left, starting at 1.

    while(idx < n || j < tmp) {
        if(j == tmp) {
            SA[++i] = SA[idx];
            SA[idx++] = n + 1;
        } else if(idx == n) {
            SA[++i] = SA[j];
            SA[j++] = n + 1;
        } else if(T[SA[j]] < T[SA[idx]]) {
            SA[++i] = SA[j];
            SA[j++] = n + 1;
        } else {
            SA[++i] = SA[idx];
            SA[idx++] = n + 1;
        }
    }

    if(is_debug_) {
        std::cerr << "Step C2 done:\n";
        for(tmp = 0; tmp < n; ++tmp) {
            std::cerr << SA[tmp] << " ";
        }
        std::cerr << '\n';
    }

    return i + 1; // Number of LMS strings in the SA. We'll need this number later, no need to recompute.
}

// Returns the flag thing for in case the cell is LML
template <typename T_idx_>
bool Suffix_Array<T_idx_>::step_oneb1_process_cell(idx_t target_idx, const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size, idx_t arr_bound)
{
    idx_t target = SA[target_idx];
    while(target > n) {
        target -= n + 2;
    }
    if(T[target - 1] >= T[target]) {
        idx_t tmp = insert_via_LE(target - 1, n - alpha_size, T, SA, n, arr_bound);
        while(tmp > n + 1) {
            tmp -= n + 2;
        }
        return (tmp != n + 1 && T[tmp] <= T[target]);
    }
    // Turns out we were LML
    SA[target_idx] += n + 2;
    return false;
}


// Returns empty if thing is not LMS and value if it is.
template <typename T_idx_>
bool Suffix_Array<T_idx_>::step_onec1_process_cell(idx_t target_idx, const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size, bool add_flag)
{
    // Bug: Because we keep erasing things, we might not realize that we've booted something out of main array because the thing that we booted out
    // has already been erased.
    // Solution: Instead of erasing things, just flag them a bunch more times.
    idx_t target = SA[target_idx];
    while(target > n) {
        target -= n + 2;
    }
    if(T[target - 1] <= T[target]) {
        if(target > 1) {
            // Just to make sure we don't insert 0
            idx_t tmp = insert_via_RE(target - 1, n - alpha_size, T, SA, n);
            while(tmp > n + 1) {
                tmp -= n + 2;
            }
            if(tmp != target) {
                // If thing that you booted out is not literally the same thing that you're in right now then just flag current location for emptying
                SA[target_idx] = target + n + 2 + (add_flag ? n + 2 : 0);
            } else {
                // If thing you booted out is yourself then you need to find the appropriate location to flag.
                SA[T[target] + n - alpha_size] = target + 2 * n + 4;
            }
            return (tmp != n + 1 && T[tmp] >= T[target]);
        }
        SA[target_idx] = target + n + 2 + (add_flag ? n + 2 : 0);
        return false;
    }
    // Turns out we were LMS
    SA[target_idx] = target + (add_flag ? n + 2 : 0);
    return false;
}



template <typename T_idx_>
T_idx_ Suffix_Array<T_idx_>::step_two_a(const idx_t* const T, idx_t* const SA, idx_t n, idx_t LMS_count)
{
    // The whole idea of step 2 is that we take the sorted LMS strings (which don't actually have the LMS suffixes sorted properly,
    // just the differences between consecutive LMS suffixes) and use them to sort LMS suffixes.
    // We replace each entire LMS string with a single character representing its rank among LMS strings, then find the SA of the modified T.
    // The modified SA can be stored in the first half of the target SA storage space, and the modified T can be stored in the back half (size and stuff will
    // just be retrieved via linear scans because I just want to get this done...).

    // Then we recursively construct the SA of the modified array, except we can't actually use recursion. In practice we fiddle with the pointers used for T
    // and SA, although unfortunately this does mean T and SA need to use the same datatype. Manageable, but does make some optimizations I was hoping for
    // rather impossible. This is done by the two loops in construct.
    
    // Then we turn the modified SA back, which is step 2b. 

    // Anyways, to do step 2a: Essentially we need to compute the ranks (stored in the second half of SA) of the LMS blocks. Once these ranks are computed,
    // we simply need to store the values in the second half into the locations of the first half, then move everything to the back.
    // In particular, if these ranks are trivial then we just end the outside loop.

    // Also we'll just return the new alphabet size if there is one.

    idx_t i, j = n - LMS_count, k, l, curr_loc, curr_rank, tmp_loc, tmp_rank, prev_finish, curr_finish, curr_streak_start, ans;
    bool found_decrease, agrees;
    SA[j++] = 0;
    prev_finish = n - 1; // just to make sure nobody agrees with the one starting at n-1
    for(i = 1; i < LMS_count; ++i, ++j) {
        // Check if i - 1 and i are the same thing
        // Idea: follow SA[i] until you encounter (a) a decreasing value and (b) an increasing value after that.
        // Then the last thing before the most recent streak of equalities just before the increasing is the end of your LMS string.
        // Once we know where prev and curr finish we can do stuff.
        found_decrease = false;
        curr_streak_start = SA[i];
        for(k = SA[i] + 1; k < n - 1; ++k) {
            if(T[k] > T[k + 1]) {// Don't need to worry about zero here because all LMS are bigger than 0
                found_decrease = true;
                curr_streak_start = k + 1;
            } else if(T[k] < T[k + 1]) {
                if(found_decrease) {
                    // We passed the next LMS now, and curr_streak_start is the previous LMS value
                    curr_finish = curr_streak_start;
                    break;
                }
                curr_streak_start = k + 1;
            }
        }
        if(k == n) {
            // We kept going until we hit the last value, so we must have the chain that ends right at the last element.
            curr_finish = n - 1;
        }
        if(prev_finish - SA[i - 1] == curr_finish - SA[i]) {
            agrees = true;
            for(k = SA[i - 1], l = SA[i]; k <= prev_finish; ++k, ++l) {
                if(T[k] != T[l]) {
                    agrees = false;
                    break;
                }
            }
        } else {
            agrees = false;
        }
        SA[j] = SA[j - 1] + (agrees ? 0 : 1);
        prev_finish = curr_finish;
    }

    if(SA[n - 1] == LMS_count - 1) {
        for(i = LMS_count; i < n; ++i) {
            SA[i] = n + 1;
        }
        return 0;
    }


    ans = SA[n - 1] + 1;
    // Compute the thing we need to recurse on
    for(i = 0; i < LMS_count; ++i) {
        curr_loc = SA[i];
        curr_rank = SA[i + n - LMS_count];
        if(curr_loc < n) {
            SA[i] = n + 1;
            SA[i + n - LMS_count] = n + 1;
        }
        while(curr_loc < n) {
            if(SA[curr_loc] != n + 1) {
                // Since curr_loc has no duplicates, things not being empty can only happen if we are trying to insert into the first/last chunk of the array.
                if(curr_loc < LMS_count) {
                    // first part of array
                    tmp_loc = SA[curr_loc];
                    tmp_rank = SA[curr_loc + n - LMS_count];
                    SA[curr_loc + n - LMS_count] = n + 1;
                } else {
                    // last part of array
                    tmp_loc = SA[curr_loc + LMS_count - n];
                    tmp_rank = SA[curr_loc];
                    SA[curr_loc + LMS_count - n] = n + 1;
                }
                SA[curr_loc] = curr_rank + n + 2;
                curr_loc = tmp_loc;
                curr_rank = tmp_rank;
            } else {
                SA[curr_loc] = curr_rank + n + 2;
                curr_loc = n + 1;
            }
        }
    }


    // n > LMS_count so no uint issues
    for(i = n - 1, j = n - 1; j >= n - LMS_count; --i) {
        if(SA[i] > n + 1) {
            tmp_rank = SA[i] - n - 2;
            SA[i] = n + 1;
            SA[j--] = tmp_rank;
        }
    }
    for(i = 0; i < LMS_count; ++i) {
        SA[i] = LMS_count + 1;
    }

    if(is_debug_) {
        std::cerr << "Step 2a done and needed to recurse:\n";
        for(j = 0; j < n; ++j) {
            std::cerr << SA[j] << " ";
        }
        std::cerr << '\n';
    }

    return ans;
}

template <typename T_idx_>
void Suffix_Array<T_idx_>::step_two_b(const idx_t* const T, idx_t* const SA, idx_t n, idx_t LMS_count)
{
    // We can turn the modified SA back as follows: First, replace each element in the modified T by the actual index of the corresponding LMS suffix
    // in the original T. This can be done by a R to L scan on T. Next, we scan through the modified SA to replace each supposed suffix index with the
    // value in the appropriate slot of the modified T. Now the modified SA actually contains all the LMS suffixes in sorted order. Finally, we empty
    // the rest of the array by setting values equal to n+1 because that's what step three expects.
    idx_t *SA_new = SA, *T_new = SA + n - LMS_count;
    idx_t i, j = LMS_count - 1;

    if(is_debug_) {
    std::cerr << "Step 2b input\n";
        for(i = 0; i < n; ++i) {
            std::cerr << T[i] << " ";
        }
        std::cerr << '\n';

        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // First part: replacing the modified T
    bool is_small = true;
    T_new[LMS_count - 1] = n - 1;
    for(i = n - 2; i < n; --i) {
        if(T[i] < T[i + 1]) {
            is_small = true;
        } else if(T[i] > T[i + 1]) {
            if(is_small) {
                // We found an LMS
                T_new[j--] = i + 1;
            }
            is_small = false;
        }
    }

    if(is_debug_) {
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
        for(i = 0; i < LMS_count; ++i) {
            std::cerr << T_new[i] << " ";
        }
        std::cerr << '\n';
        for(i = 0; i < LMS_count; ++i) {
            std::cerr << SA_new[i] << " ";
        }
        std::cerr << '\n';
    }

    // Now replacing the modified SA
    for(i = 0; i < LMS_count; ++i) {
        SA_new[i] = T_new[SA_new[i]];
    }

    // Clear the rest of the array
    for(i = LMS_count; i < n; ++i) {
        SA[i] = n + 1;
    }

    if(is_debug_) {
        std::cerr << "step 2b output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }
}


template <typename T_idx_>
void Suffix_Array<T_idx_>::step_three(const idx_t* const T, idx_t* const SA, idx_t n, idx_t alpha_size)
{
    if(n == 1) {
        // SA should already have 0 in first element so we're good
        return;
    }
    idx_t i, j, k, l, idx, tmp; // indices into SA
    idx_t len_z1 = 0, len_z2 = 0, len_z3, len_x1 = 0, len_x2 = 0, len_x3;
    idx_t curr, val1, val2, val3, val4; // letter of alphabet for comparison purposes

    if(is_debug_) {
        std::cerr << "Starting Step 3: T and SA:\n";
        for(i = 0; i < n; ++i) {
            std::cerr << T[i] << " ";
        }
        std::cerr << ' ' << alpha_size << '\n';
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition one: shift SA_LMS to the end

    for(i = 0; SA[i] != 0; ++i) {
        if(SA[i] == n + 1) {
            break;
        }
        if(i == 0 || T[SA[i]] != T[SA[i - 1]]) {
            len_z1++;
        } else {
            len_z2++;
        }
    }
    for(i = 0, j = n - len_z1 - len_z2; j < n; ++i, ++j) {
        SA[j] = SA[i];
        SA[i] = n + 1;
    }

    if(is_debug_) {
        std::cerr << "transition one output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition two: Move the smallest LMS of each interval to the front of the SA_LMS, splitting the LMS-es into
    // arrays Z1 and Z2

    // basically since n > LMS_count this is fine
    // Move the smallest LMS of each interval to the front of the SA
    for(i = n - 1, j = len_z1 - 1; i > n - len_z1 - len_z2; --i) {
        if(T[SA[i]] != T[SA[i - 1]]) {
            SA[j--] = SA[i];
            SA[i] = n + 1;
        }
    }
    SA[0] = SA[n - len_z1 - len_z2];
    // Move the larger LMSes to be contiguous
    for(i = n - 1, j = n - 1; i > n - len_z1 - len_z2; --i) {
        if(SA[i] != n + 1) {
            SA[j--] = SA[i];
        }
    }
    // Move the smaller LMSes to be right in front of the larger ones
    for(i = n - len_z2 - 1, j = len_z1 - 1; i >= n - len_z2 - len_z1; --i, --j) {
        SA[i] = SA[j];
        SA[j] = n + 1;
    }

    if(is_debug_) {
        std::cerr << "transition two output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition three: Move the small LMSes from Z1 into locations in the front to form a "spares" array.
    // This array will eventually contain some other things too later but for now contains the smallest LMS suffix
    // for each character that has LMS suffixes starting with it, and empties everywhere else.
    // basically since n > LMS_count this is fine
    for(i = n - len_z2 - 1; i >= n - len_z2 - len_z1; --i) {
        idx = T[SA[i]];
        while(idx != i) {
            tmp = SA[i];
            SA[i] = SA[idx];
            SA[idx] = tmp;
            if(SA[i] == n + 1) {
                break;
            }
            idx = T[SA[i]];
        }
    }
    if(is_debug_) {
        std::cerr << "transition three output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition four (a): Compute a ``type'' array storing whether or not there is an L-suffix starting with the given character.
    // We can store this as flags in the thing at the front.
    bool next_is_small = true;
    len_z3 = len_z2;
    for(i = n - 2; i < n; --i) {
        if(T[i] > T[i + 1] || (T[i] == T[i + 1] && !next_is_small)) {
            next_is_small = false;
            idx = T[i];
            if(SA[idx] < n + 2) {
                if(SA[idx] != n + 1) {
                    len_z3++;
                }
                SA[idx] += n + 2;
                len_x2++;
            } else {
                len_x1++;
            }
        } else {
            next_is_small = true;
        }
    }
    
    // Transition four (b): Move the suffixes with flagged type to the spot and reverse the flag definition
    // That is, things without L's have n + 2 added to them, because we usually add n + 2 to things that have indices stored in them.
    // Note that things with L's are filled with zeros now so that we can do the prefix sums later.
    for(j = n - len_z3 - 1, i = alpha_size - 1, idx = n - len_z2 - 1; j >= n - len_z3 - alpha_size && j < n; --i, --j) {
        if(SA[i] > n + 1) {
            if(SA[i] != 2 * n + 3) {
                SA[idx] = SA[i] - n - 2;
                idx--;
            }
            SA[j] = 0;
        } else {
            SA[j] = SA[i] + n + 2;
        }
        if(i != j) {
            SA[i] = n + 1;
        }
    }
    if(is_debug_) {
        std::cerr << "transition four output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition five is a stable merge sort that is technically not needed if we just iterate through more things in parallel. So we skip it.
    // Transition six: We fill the blanks in the spares array with LE values for just the L suffixes (minus spare slots).
    // First we fill the appropriate spots (those with any L at all) with the number of L things starting there.
    next_is_small = true;
    for(i = n - 2; i < n; --i) {
        if(T[i] > T[i + 1] || (T[i] == T[i + 1] && !next_is_small)) {
            next_is_small = false;
            idx = T[i] + n - alpha_size - len_z3;
            SA[idx]++;
        } else {
            next_is_small = true;
        }
    }
    // Then we compute the appropriate prefix sum
    k = 0;
    for(i = n - len_z3 - alpha_size; i < n - len_z3; ++i) {
        if(SA[i] < n) {
            tmp = SA[i] - 1;
            SA[i] = k;
            k += tmp;
        }
    }

    if(is_debug_) {
        std::cerr << "transition six output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition seven: Use the same idea as step 1b to store all the L-suffixes in sorted order,
    // with the largest L-suffix of each interval ending up in the spares array.
    // Note that for items of the same starting digit, the thing in X1 is smallest, then the thing in Y, then the front part of Z3, then the Z2-part of Z3.
    // Also everything in X1 will be filled by the time you actually need it and everything tagged in Y will no longer be a value of LE by the time you reach it,
    // and the Z3 portions are always filled. Thus if you see something empty in X1 then you just wait and process the other arrays first.
    i = 0;
    val1 = n + 1;
    j = n - len_z3 - alpha_size;
    val2 = 0;
    k = n - len_z3;
    if(len_z3 > len_z2) {
        val3 = T[SA[k]];
    } else {
        val3 = n + 1;
    }
    l = n - len_z2;
    if(len_z2 > 0) {
        val4 = T[SA[l]];
    } else {
        val4 = n + 1;
    }

    while(j < n - len_z3) {
        // Last thing we need to process will be the last element of Y: there are no LMS or S strings starting with alpha_size - 1.
        if(i >= n - len_z3 - alpha_size || SA[i] == n + 1) {
            val1 = n + 1;
        } else {
            val1 = T[SA[i]];
        }
        curr = (val1 < val2) ? val1 : val2;
        curr = (curr < val3) ? curr : val3;
        curr = (curr < val4) ? curr : val4;
        if(val1 == curr) {
            // The cell in the main array might actually be really cursed and get replaced when we do the insertion if this cell actually belongs in the spare array.
            // Luckily, if it belongs in the spare array it must belong in the corresponding spot for this exact thing.
            // However, we might not notice that this guy is cursed for a while... But we won't move past the cursed thing.
            tmp = SA[i];
            insert_L_via_LE(SA[i], n - alpha_size - len_z3, T, SA, n, len_x1);
            if(tmp == SA[i]) {
                ++i;
            } else {
                j++;
                val2++;
            }
        } else if(val2 == curr) {
            tmp = SA[j++];
            if(tmp > n && tmp != 2 * n + 3) {
                if(insert_L_via_LE(tmp - n - 2, n - alpha_size - len_z3, T, SA, n, len_x1)) {
                    --i;
                }
            }
            val2++;
        } else if(val3 == curr) {
            if(insert_L_via_LE(SA[k++], n - alpha_size - len_z3, T, SA, n, len_x1)) {
                --i;
            }
            if(k >= n - len_z2) {
                val3 = n + 1;
            } else {
                val3 = T[SA[k]];
            }
        } else {
            if(insert_L_via_LE(SA[l++], n - alpha_size - len_z3, T, SA, n, len_x1)) {
                --i;
            }
            if(l >= n) {
                val4 = n + 1;
            } else {
                val4 = T[SA[l]];
            }
        }
    }

    if(is_debug_) {
        std::cerr << "transition seven output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition eight: Leave X1 alone, move all the L suffixes in Y into X2, then delete everything outside of X.
    // We do this in three steps: first we re-flag the L suffixes in Y by decreasing them by n + 2, then we do the move, then the delete.
    // Part 1
    next_is_small = true;
    for(i = n - 2; i < n; --i) {
        if(T[i] > T[i + 1] || (T[i] == T[i + 1] && !next_is_small)) {
            next_is_small = false;
            idx = T[i] + n - alpha_size - len_z3;
            if(SA[idx] > n + 1) {
                SA[idx] -= n + 2;
            }
        } else {
            next_is_small = true;
        }
    }
    // Part 2
    for(i = len_x1, j = n - alpha_size - len_z3; j < n - len_z3; ++j) {
        if(SA[j] < n) {
            SA[i++] = SA[j];
        }
    }
    for(i = len_x1 + len_x2; i < n; ++i) {
        SA[i] = n + 1;
    }

    if(is_debug_) {
        std::cerr << "transition eight output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Now onto sorting the S suffixes in:
    // Transition 1&2 of the new thing are unneeded, since we already have things split properly as of the end of transition 8.
    // Thus we start with the new transition 3, which I'll call transition 12=9+3
    // So, transition 12: move the things in X2 into the last alpha_size locations.
    for(i = len_x1 + len_x2 - 1; i >= len_x1 && i < n; --i) {
        idx = n - alpha_size + T[SA[i]];
        SA[idx] = SA[i];
        if(i != idx) {
            SA[i] = n + 1;
        }
    }

    if(is_debug_) {
        std::cerr << "transition twelve output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition 13 (a): Compute a ``type'' array storing whether or not there is an S-suffix starting with the given character.
    // We can store this as flags in the thing at the back.
    next_is_small = true;
    len_x3 = len_x1;
    for(i = n - 2; i < n; --i) {
        if(T[i] < T[i + 1] || (T[i] == T[i + 1] && next_is_small)) {
            next_is_small = true;
            idx = n - alpha_size + T[i];
            if(SA[idx] < n + 2) {
                if(SA[idx] != n + 1) {
                    len_x3++;
                }
                SA[idx] += n + 2;
            }
        } else {
            next_is_small = false;
        }
    }
    SA[n - alpha_size] += n + 2;

    // Transition 13 (b): Move the suffixes with flagged type to the spot in x3 and reverse the flag definition
    // That is, things without S's have n + 2 added to them, because we usually add n + 2 to things that have indices stored in them.
    // Note that we fill 0s into places with S's
    for(j = n - alpha_size, i = len_x3, idx = len_x1; j < n; ++i, ++j) {
        if(SA[j] > n + 1) {
            if(SA[j] != 2 * n + 3) {
                SA[idx] = SA[j] - n - 2;
                idx++;
            }
            SA[i] = 0;
        } else {
            SA[i] = SA[j] + n + 2;
        }
        if(i != j) {
            SA[j] = n + 1;
        }
    }
    if(is_debug_) {
        std::cerr << "transition 13 output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition 14: Sort the X3 array. We ignore this because we can just use two things to iterate at once
    // Transition 15: Replace the blanks in the spare array with the appropriate RE values.
    next_is_small = true;
    for(i = n - 2; i < n; --i) {
        if(T[i] < T[i + 1] || (T[i] == T[i + 1] && next_is_small)) {
            next_is_small = true;
            idx = T[i] + len_x3;
            SA[idx]++;
        } else {
            next_is_small = false;
        }
    }
    // Then we compute the appropriate prefix sum
    k = -1;
    for(i = len_x3 + 1; i < len_x3 + alpha_size; ++i) {
        if(SA[i] < n) {
            k += SA[i] - 1;
            SA[i] = k + len_x3 + alpha_size;
        }
    }

    // Because the last value is a weird case, we rotate things so that SA[len_x3 + alpha_size - 1] is n - 1
    // and the rest of Y is actually shifted one to the left (dropping the zero term)

    for(i = len_x3; i < len_x3 + alpha_size; ++i) {
        SA[i] = SA[i + 1];
    }
    SA[len_x3 + alpha_size - 1] = n - 1;

    if(is_debug_) {
        std::cerr << "transition 15 output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Transition 16: Similar to step 1c: go right to left to add a bunch of S suffixes into the array.
    i = n - 1;
    val1 = 0;
    j = len_x3 + alpha_size - 2;
    val2 = alpha_size - 1;
    k = len_x3 - 1;
    if(len_x3 > len_x1) {
        val3 = T[SA[k]];
    } else {
        val3 = 0;
    }
    l = len_x1 - 1;
    if(len_x1 > 0) {
        val4 = T[SA[l]];
    } else {
        val4 = 0;
    }

    // This loop breaks if string is just 0?
    while(i >= len_x3 + alpha_size - 1) {
        // Last thing we need to process will be the first element of Z-whatever.
        if(SA[i] == n + 1) {
            val1 = 0;
        } else {
            val1 = T[SA[i]];
        }
        curr = (val1 > val2) ? val1 : val2;
        curr = (curr > val3) ? curr : val3;
        curr = (curr > val4) ? curr : val4;
        if(val1 == curr) {
            // The cell in the main array might actually be really cursed and get replaced when we do the insertion if this cell actually belongs in the spare array.
            // Luckily, if it belongs in the spare array it must belong in the corresponding spot for this exact thing.
            tmp = SA[i];
            insert_R_via_RE(SA[i], len_x3 - 1, true, T, SA, n);
            if(tmp == SA[i]) {
                --i;
            } else {
                --j;
                --val2;
            }
        } else if(val2 == curr) {
            // Note that bad things can still happen here: SA[j] might still be blank, in which case what we actually need to do is swap SA[i + 1] into SA[j],
            // increment i, and decrement j.
            if(SA[j] < n + 2) {
                SA[j--] = SA[++i] + n + 2;
                SA[i] = n + 1;
                val2--;
            } else {
                tmp = SA[j--] - n - 2;
                if(insert_R_via_RE(tmp, len_x3 - 1, true, T, SA, n)) {
                    ++i;
                }
                // Note that the is_S_when_equal parameter doesn't matter for the L things in the Y array
                // because the L things here are the biggest ones, and adding the same digit in front would make it even bigger
                // within the same interval.
                val2--;
            }
        } else if(val3 == curr) {
            if(insert_R_via_RE(SA[k--], len_x3 - 1, false, T, SA, n)) {
                ++i;
            }
            if(k < len_x1) {
                val3 = 0;
            } else {
                val3 = T[SA[k]];
            }
        } else {
            if(insert_R_via_RE(SA[l--], len_x3 - 1, false, T, SA, n)) {
                ++i;
            }
            // Note that l will hit zero and then cycle because we're using unsigned ints
            if(l > n) {
                val4 = 0;
            } else {
                val4 = T[SA[l]];
            }
        }
    }

    // Since everything is now part of the suffix array, we can now just remove all the flags
    for(i = len_x3; i < len_x3 + alpha_size - 1; ++i) {
        if(SA[i] > n) {
            SA[i] -= n + 2;
        }
    }

    if(is_debug_) {
        std::cerr << "transition 16 output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }

    // Remaining steps: Merge the suffix arrays two at a time.
    // Note that the suffix arrays on the left always has smaller values than the suffix arrays on the right.
    // We do this by just calling the merge function a few times.

    #ifdef MERGEIP
    //Stick the current T into the static field of the struct
    //Very gross, I know
    //But I don't know of a way to locally define a struct using a local variable
    value_wrap<idx_t>::values = T;
    //Casting the indices to the struct that overrides comparisons
    value_wrap<idx_t> *SA_cast = (value_wrap<idx_t>*) SA;
    //std::cout << "\n\n\nMerging\n";
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << SA[i] << ' ';
    //}
    //std::cout << std::endl;
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << T[i] << ' ';
    //}
    //std::cout << std::endl;
    //std::cout << len_x1 << " " << len_x3 << " " << len_x3+alpha_size-1 << " " << n << std::endl;
    merge_inplace(SA_cast, len_x1, len_x3);
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << SA[i] << ' ';
    //}
    //std::cout << std::endl;
    merge_inplace(SA_cast+len_x3, alpha_size-1, n-len_x3);
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << SA[i] << ' ';
    //}
    //std::cout << std::endl;
    merge_inplace(SA_cast, len_x3, n);
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << SA[i] << ' ';
    //}
    //std::cout << std::endl;

    
    #else
    i = 0;
    val1 = len_x1 > 0 ? T[SA[0]] : n + 1;
    j = len_x1;
    val2 = len_x3 > len_x1 ? T[SA[j]] : n + 1;
    k = len_x3;
    val3 = T[SA[k]];
    l = len_x3 + alpha_size - 1;
    val4 = n > l ? T[SA[l]] : n + 1;

    idx_t* L = allocate<idx_t>(n);

    for(idx = 0; idx < n; ++idx) {
        curr = (val1 < val2) ? val1 : val2;
        curr = (curr < val3) ? curr : val3;
        curr = (curr < val4) ? curr : val4;
        if(curr == val1) {
            L[idx] = SA[i++];
            val1 = len_x1 > i ? T[SA[i]] : n + 1;
        } else if(curr == val2) {
            L[idx] = SA[j++];
            val2 = len_x3 > j ? T[SA[j]] : n + 1;
        } else if(curr == val3) {
            L[idx] = SA[k++];
            val3 = len_x3 + alpha_size - 1 > k ? T[SA[k]] : n + 1;
        } else {
            L[idx] = SA[l++];
            val4 = n > l ? T[SA[l]] : n + 1;
        }
    }
    //for (size_t i = 0; i < n; i++) {
    //    std::cout << L[i] << ' ';
    //}
    //std::cout << std::endl;
    for(idx = 0; idx < n; ++idx) {
        SA[idx] = L[idx];
    }
    #endif

    if(is_debug_) {
        std::cerr << "Step 3 output\n";
        for(i = 0; i < n; ++i) {
            std::cerr << SA[i] << " ";
        }
        std::cerr << '\n';
    }
}


template <typename T_idx_>
bool Suffix_Array<T_idx_>::insert_L_via_LE(idx_t target, idx_t LE_offset, const idx_t* const T, idx_t* const SA, idx_t n, idx_t arr_bound)
{
    if(target > 0 && T[target - 1] >= T[target]) {
        idx_t tmp = insert_via_LE(target - 1, LE_offset, T, SA, n, arr_bound);
        return (tmp != n + 1 && T[tmp] <= T[target]);
    }
    return false;
}


template <typename T_idx_>
bool Suffix_Array<T_idx_>::insert_R_via_RE(idx_t target, idx_t RE_offset, bool is_S_when_equal, const idx_t* const T, idx_t* const SA, idx_t n)
{
    if(target > 0 && (T[target - 1] < T[target] || (is_S_when_equal && T[target - 1] == T[target]))) {
        idx_t tmp = insert_via_RE(target - 1, RE_offset, T, SA, n);
        return (tmp != n + 1 && T[tmp] >= T[target]);
    }
    return false;
}


// Need to check if you exited array on right.
template <typename T_idx_>
T_idx_ Suffix_Array<T_idx_>::insert_via_LE(idx_t target, idx_t LE_offset, const idx_t* const T, idx_t* const SA, idx_t n, idx_t arr_bound)
{
    idx_t idx = T[target] + LE_offset;
    idx_t tmp = SA[SA[idx]];
    while(tmp > n + 1) {
        tmp -= n + 2;
    }
    if(SA[SA[idx]] == n + 1 && SA[idx] < arr_bound) {
        // Space is free
        SA[SA[idx]] = target;
        SA[idx]++;
    }
    else if (tmp < n && T[tmp] < T[target] && SA[idx] < arr_bound) {
        // overwritten by something that should have been in an earlier interval,
        // so move that item to the appropriate location near the end of SA
        // then place this item in
        idx_t idx2 = T[tmp] + LE_offset;
        SA[idx2] = SA[SA[idx]] + n + 2;
        SA[SA[idx]] = target;
        SA[idx]++;
        return SA[idx2] - n - 2;
    }
    else {
        // attempting to overwrite something that is in a later interval,
        // so this item actually replaces the approximate value of LE near the end of SA
        SA[idx] = target + n + 2;
    }
    return n + 1;
}


// Doesn't need to check if you've exited the array on the left because the 0 item is there to block
template <typename T_idx_>
T_idx_ Suffix_Array<T_idx_>::insert_via_RE(idx_t i, idx_t RE_offset, const idx_t* const T, idx_t* const SA, idx_t n)
{
    idx_t idx = T[i] + RE_offset;
    idx_t tmp = SA[SA[idx]];
    while(tmp > n + 1) {
        tmp -= n + 2;
    }
    if(SA[SA[idx]] == n + 1) {
        // Space is free
        SA[SA[idx]] = i;
        SA[idx]--;
    }
    else if (tmp < n && T[tmp] > T[i]) {
        // overwritten by something that should have been in a later interval,
        // so move that item to the appropriate location near the end of SA
        // then place this item in
        idx_t idx2 = T[tmp] + RE_offset;
        SA[idx2] = SA[SA[idx]] + n + 2;
        SA[SA[idx]] = i;
        SA[idx]--;
        return SA[idx2] - n - 2;
    }
    else {
        // attempting to overwrite something that is in an earlier interval,
        // so this item actually replaces the approximate value of RE near the end of SA
        SA[idx] = i + n + 2;
    }
    return n + 1;
}



/*TODO: Currently will have bugs if n = alpha_size or n=2^32 or n=2^64 for uint_32t and uint_64t*/
template <typename T_idx_>
void Suffix_Array<T_idx_>::construct()
{
    const auto t_start = now();
    
    // Emptying the array
    idx_t i;
    for(i = 0; i < n_; ++i) {
        SA_[i] = n_ + 1;
    }
    const idx_t *T = T_;
    idx_t *SA = SA_; // note that SA actually never moves
    idx_t LMS_count, recursive_alpha_size;
    idx_t n = n_, alpha_size = alpha_size_;
    idx_t recursion_count = 0;
    while(true) {
        LMS_count = step_one(T, SA, n, alpha_size);
        recursive_alpha_size = step_two_a(T, SA, n, LMS_count);
        if(recursive_alpha_size == 0) {
            step_three(T, SA, n, alpha_size);
            recursion_count--;
            break;
        }
        recursion_count++;
        T = SA + n - LMS_count;
        n = LMS_count;
        alpha_size = recursive_alpha_size;
    }
    for(; recursion_count < n_; --recursion_count) {
        if(recursion_count == 0) {
            LMS_count = n;
            n = n_;
            alpha_size = alpha_size_;
            T = T_;
        } else {
            LMS_count = n;
            n = T + n - SA;
            for(i = n; SA[i] > n; ++i) {}
            T = SA + i;
            alpha_size = 0;
            for(i = 0; i < n; ++i) {
                alpha_size = (alpha_size > T[i]) ? alpha_size : T[i];
            }
            alpha_size++;
        }
        step_two_b(T, SA, n, LMS_count);
        step_three(T, SA, n, alpha_size);
    }

    const auto t_end = now();
    std::cerr << "Constructed the suffix array. Time taken: " << duration(t_end - t_start) << " seconds.\n";
}


template <typename T_idx_>
void Suffix_Array<T_idx_>::dump(std::ofstream& output)
{
    const auto t_start = now();

    const std::size_t n = n_;
    output.write(reinterpret_cast<const char*>(&n), sizeof(std::size_t));
    output.write(reinterpret_cast<const char*>(SA_), n_ * sizeof(idx_t));

    const auto t_end = now();
    std::cerr << "Dumped the suffix array. Time taken: " << duration(t_end - t_start) << " seconds.\n";
}


}





// Template instantiations for the required instances.
template class Goto_SA::Suffix_Array<uint64_t>;
