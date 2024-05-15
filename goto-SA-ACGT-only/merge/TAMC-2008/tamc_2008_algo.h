/*
	MERGING BENCHMARKING TOOL
	Copyright 2007, 2008, 2009 Arne Kutzner

    This code is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this code. If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>

#include "buffer_extraction.h"
#include "block_rotation_merge.h"

#ifndef CHECK_INTERMEDIATE_RESULTS // DEBUG STUFF
#define CHECK_INTERMEDIATE_RESULTS
#endif

#ifndef OPTIMIZED_MI_BUFFER // delay of rotations in the mi-buffer
#define OPTIMIZED_MI_BUFFER
#endif

/* 
	LOCAL MERGES (SUBOPERATION OF THE TAMC 2008 ALGORITHM)
*/
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter), class _ValueTp, class _Distance, class _BlockCounter>
_RandomAccessIter local_merges
	(_RandomAccessIter first, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buf, // 'buf' must be a buffer of size 'block_size' or NULL
	 _RandomAccessIter bds1, 
	 _RandomAccessIter bds2, 
	 _Distance k, 
	 _BlockCounter numLeftBlocks
	) 

{
#ifndef NDEBUG
	_RandomAccessIter orig_last = last; // DEBUG variable
#endif 
	
	_BlockCounter index = ((last - first) / k) - 1; // index gets the number of complete blocks between first1 and last minus 1
	for (; numLeftBlocks > 0; numLeftBlocks--) {
		for (; *(bds1 + index) < *(bds2 + index); index--); // find the first block orginating from the left side
		
		_RandomAccessIter first2 = first + ((index + 1) * k);

		assert(first2 <= last);

		if (first2 < last) {
			// the right side of the current block isn't empty
			_RandomAccessIter b = bsearch_lower(first2, last, *(first2 - k));

			assert((_Distance)(b - first2) <= k);
			
			// START VARIANT 1
			if (buf != NULL && ((_Distance)(last - b) >= k)) {
				// START OPTIMIZATION (floating hole technique)
				_Distance delta = b - first2;
				float_hole(buf, buf + delta, first2 - k, first2);
				std::swap_ranges(buf + delta, buf + k, b - k);
				hwang_and_lin_left<_RandomAccessIter, __rotate>(b - k, b, last, buf, OMIT_BUFFER_COPY);
				// END OPTIMIZATION
			} else {
				__rotate(first2 - k, first2, b);
				hwang_and_lin<_RandomAccessIter, __rotate>(b - k, b, last, buf); 
			}
			// END VARIANT 1

#if 0
			// START VARIANT 2 (very simple but requires additional comparisons)
			hwang_and_lin(first2 - k, first2, last, buf); 
			// END VARIANT 2
#endif

			last = b - k;
		} // if
		std::iter_swap(bds1 + index, bds2 + index); // restore the original order of the elements in the BD-storage
		index--;
		
	} // for

#ifdef CHECK_INTERMEDIATE_RESULTS
		assert(verify(last, orig_last));
#endif

	return last;
}

/* 
	BLOCK DISTRIBUTION (SUBOPERATION OF THE TAMC 2008 ALGORITHM)
*/
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter), class _Distance>
void distribute_blocks
	(_RandomAccessIter left, 
	 _RandomAccessIter right,
	 _RandomAccessIter last, 
	 _RandomAccessIter mi_buf, // movement imitation buffer
	 _RandomAccessIter bds1, // first part of the block distribution storage
	 _RandomAccessIter bds2, // second part of the block distribution storage
	 _Distance k
#ifndef NDEBUG
	 , _RandomAccessIter bds1_end, // DEBUG STUFF
	 _RandomAccessIter bds2_end, // DEBUG STUFF
	 _RandomAccessIter buffer_end // DEBUG STUFF
#endif					   
	) 
{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	
	if (left >= right)
		// nothing to do 
		return;	
	
	_RandomAccessIter mi_buf_end = mi_buf + (right - left) / k; // points to first element following the mi-buffer
	
	assert(verify(mi_buf, mi_buf_end));
	assert(mi_buf_end <= buffer_end);

#ifdef OPTIMIZED_MI_BUFFER
	_Distance delta_mi_buf = 0; // delta inside the movement imitation buffer
#endif

	_RandomAccessIter min_block = left; // the current minimal block

	while (true) {

		assert((right - left) % k == 0);
		assert((min_block - left) % k == 0);
		assert(bds1 < bds1_end && bds2 < bds2_end);

#ifndef OPTIMIZED_MI_BUFFER
		assert(*mi_buf >= *(mi_buf + (min_block - left) / k));
#endif // OPTIMIZED_MI_BUFFER
	
		if (right + k < last) {
			// There is still a complete block that originates form the right side
		
			assert(min_block <= right + k - 1);

			if (*(right + k - 1) < *min_block) {
				// the right block occurs before the minimal one
				std::swap_ranges(left, left + k, right); // exchange blocks
				bds1++; bds2++; // encode block distribution in storage
#ifdef OPTIMIZED_MI_BUFFER
				delta_mi_buf++;
#else
				__rotate(mi_buf, mi_buf + 1, mi_buf + ((right - left) / k)); // simulate movement - expensive
#endif
			
				if (min_block == left)
					min_block = right;

				left += k; right += k; // we can't get (left >= right) at this position

				continue; // process the next block (please note that we don't have to check (left >= right)
			}
		} // if
		
		// there is still at least one block that originates from the left side, and the left side contains the current minimal block
		if (min_block != left)
			// HINT: __std::swap_ranges checks for block overlapping 
			std::swap_ranges(min_block, min_block + k, left); // exchange blocks
		
		std::iter_swap(bds1++, bds2++); // encode block distribution in storage

#ifdef OPTIMIZED_MI_BUFFER
		__rotate(mi_buf, mi_buf + (delta_mi_buf % ((right - left) / k)), mi_buf + ((right - left) / k));
		delta_mi_buf = 0;
#endif
		std::iter_swap(mi_buf, mi_buf + (min_block - left) / k); 
		left += k;
		if (left >= right)
			// no more blocks left
			break;

		mi_buf++; // decrease the size of the mi-buffer
		
		// search the new minimal block in the permutation storage
		min_block = left + (minimum (mi_buf, mi_buf_end) - mi_buf) * k;
	} // while(true)
	// END BLOCK DISTRIBUTION
}


/* 
	THE CORE OF THE TAMC 2008 ALGORITHM
*/
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
void TAMC2008_merge
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last) 
{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	typedef typename std::iterator_traits<_RandomAccessIter>::difference_type distance;

	if (first1 >= first2 || first2 >= last)
		return;

	// assert(first2 - first1 <= last - first2); // TO DO: implement the symmetric case
	assert(verify(first1, first2)); // Left input sequence sorted ?
	assert(verify(first2, last)); // Right input sequence sorted ?

#ifndef NDEBUG
	_RandomAccessIter orig_first1 = first1; // DEBUG variable
	_RandomAccessIter orig_first2 = first2; // DEBUG variable
	_RandomAccessIter orig_last = last; // DEBUG variable
#endif 

	distance k = (distance)sqrt((double)(first2 - first1)); // sqrt(m)
	distance orig_block_size = k; // block_size can change in a special case. However, later we will need the original size once again.
	
	unsigned int n = (last - first1) / k; // Here is some difference with the TAMC 2008 paper

	assert(n >= 1);
	
	// START BD-STORAGE ALLOCATION
	_RandomAccessIter bds1 = first1; 
	_RandomAccessIter bde1 = first1 + n;

	// Calculation of the start of the second section of the block-distribution storage
	_RandomAccessIter bds2 = bde1 >= first2  ?  first2  :  bsearch_upper(bde1, first2, *(bde1 - 1));
	_RandomAccessIter bde2 = bds2 + n;
	
	if (bde2 >= first2) {
		// SPECIAL CASE: The size of the block-distribution storage exceeds the size of the left input sequence
		// In the TAMC 2008 paper occurs an additional rotation in this case. However, this roation is not necessary.
		block_rotation_merge<_RandomAccessIter, __rotate, _ValueTp>(first1, first2 ,last, k, (_RandomAccessIter)NULL);
		return;	
	}
	
	first1 = bde2; // we set first1 to the first element after the block distribution storage
	_RandomAccessIter original_last = last;
	// END BD-STORAGE ALLOCATION
	
	assert(first1 < first2); // The left side must now contain at least one element
	assert(verify(first1, first2));
	assert((distance)(n * k) >= (distance)(last - first1));
	
	// START BUFFER EXTRACTION	
	distance bSize = extract_buffer_left_right<_RandomAccessIter, __rotate, _ValueTp>(first1, first2, k);
	_RandomAccessIter bStart = first1; // internal buffer for local merges and movement imitation

#ifndef NDEBUG
	bool buffer_undersized = bSize < k;
#endif

	_RandomAccessIter bStart_adapted;
	distance k_adapted;
	if (bSize < k) {
		// SPECIAL CASE: We have an undersized buffer (SOLUTION: we adapt the block-size)

#ifndef NDEBUG
		// distance orig_block_size = k;
#endif
		bStart_adapted = NULL;
		k_adapted = (first2 - bds1) / bSize; // orig_left_size / bSize; We set a new block_size - WARNING: first2 - first1 results in trouble !!

		assert(k_adapted >= k);
	} else {
		bStart_adapted = bStart;
		k_adapted = k;
	}

	assert(verify(bStart, bStart + bSize) && verify(bStart + bSize, first2));
	// END BUFFER EXTRACTION
	
	first1 = (first1 + bSize) + ((first2 - (first1 + bSize)) % k_adapted);	// the first block of the left side can be smaller than block_size

	assert((first2 - first1) % k_adapted == 0); // very important property;
	assert(bSize * k_adapted >= (distance)(first2 - first1));

#ifndef NDEBUG
	int real_number_of_blocks = (last - first1) / k_adapted;
#endif
	assert(bde1 - bds1 >= real_number_of_blocks); // is the movement storage big enough?
	
	// START BLOCK DISTRIBUTION
#ifndef NDEBUG
	distribute_blocks<_RandomAccessIter, __rotate>(first1, first2, last, bStart, bds1, bds2, k_adapted, bde1, bde2, bStart + bSize);
#else
	distribute_blocks<_RandomAccessIter, __rotate>(first1, first2, last, bStart, bds1, bds2, k_adapted);
#endif
	// END BLOCK DISTRIBUTION

	// START LOCAL MERGES
	
	assert(verify(bStart, bStart + bSize));

#ifndef NDEBUG
	_RandomAccessIter last_before_local_merges = last; // the last pointer before the start of the local merges
#endif
	
	_RandomAccessIter lMerged 
		= local_merges<_RandomAccessIter, __rotate, _ValueTp>(first1, last, bStart_adapted, bds1, bds2, k_adapted, (first2 - first1) / k_adapted);
    //^fixed bug here. Second occurence of k_adapted was originally just k

	// In the context of the further algorithm it isn't neccessary that the buffer is sorted, however we check it
	assert(!(buffer_undersized && !verify(bStart, bStart + bSize)));
	// END LOCAL MERGES

#ifndef NDEBUG	
	bool remaing_small_left_block_exists = first1 > bStart + bSize;
#endif

	assert(remaing_small_left_block_exists || (bStart + bSize == first1));
	assert((!remaing_small_left_block_exists) || (verify(bStart + bSize, first1))); // (a => b) represneted as (!a || b)
	
	// BEGIN SWEEPING UP
	_RandomAccessIter b = bsearch_lower(first1, lMerged, *(bStart - 1)); // we have n >= 1
	__rotate(bStart - 1, first1, b); // We keep the stability because we respected the minimum
	bStart += b - first1; // adjustment the start of the buffer (delta)

#if 0 // UNOPTIMIZED CODE
	if (bStart_adapted != NULL)
		hwang_and_lin<_RandomAccessIter, __rotate>(bStart + bSize, b, lMerged, bStart);
	else
		hwang_and_lin<_RandomAccessIter, __rotate>(bStart + bSize, b, lMerged, (_RandomAccessIter)NULL);

	assert((b - first1 <= 0) /* at least one element moved in front? */ || (*(bStart - 2) < *(bStart - 1)));

	// remaining small left side originating block has to be merged

	assert(verify(bStart + bSize, last));

	std::sort(bStart, bStart + bSize);

	assert(verify(bStart, bStart + bSize));
	assert(verify(bStart + bSize, last));

	hwang_and_lin<_RandomAccessIter, __rotate>(bStart, bStart + bSize, last, (_RandomAccessIter)NULL); // quite expensive
	lMerged = bStart - 1;

	// finally we merge the BD-storage
	
	assert(verify(bStart + bSize, last));
	assert(verify(bds1, bds1 + n - 1));
	assert(lMerged - (b - first1) == bde2 - 1);

	b = bsearch_lower (lMerged - (b - first1), lMerged, *(bde1 - 1));
	__rotate(bde1 - 1, bde2 - 1, b);
	
	assert(verify(bds1, bde1 - 1));
	assert(verify(bde1 - 1, bde1 - 1 + (b - (bde2 - 1))));

	// We merge the remaining smaller left block
	block_rotation_merge<_RandomAccessIter, __rotate, _ValueTp>(bds1, bde1 - 1, bde1 - 1 + (b - (bde2 - 1)), k, (_RandomAccessIter)NULL); // sqrt(m) is our original block_size

	// We merge the remaining left BD-storage
	block_rotation_merge<_RandomAccessIter, __rotate, _ValueTp>(b - (n - 1), b, lMerged, k, (_RandomAccessIter)NULL); // sqrt(m) is our original block_size

	
#else // OPTIMIZED CODE
	if (bStart_adapted != NULL)
		bStart_adapted = bStart;

	assert((b - first1 <= 0) /* at least one element moved in front? */ || (*(bStart - 2) < *(bStart - 1)));

	// remaining small left side originating block has to be merged
	hwang_and_lin<_RandomAccessIter, __rotate>(bStart + bSize, b, lMerged, bStart_adapted);	
	
	lMerged = bStart - 1;
	
	assert(verify(bStart + bSize, last));
	assert(verify(bds1, bds1 + n - 1));
	assert(lMerged - (b - first1) == bde2 - 1);

	b = bsearch_lower (lMerged - (b - first1), lMerged, *(bde1 - 1));
	__rotate(bde1 - 1, bde2 - 1, b);
	
	assert(verify(bds1, bde1 - 1));
	assert(verify(bde1 - 1, bde1 - 1 + (b - (bde2 - 1))));

	// We merge the remaining smaller left block
	block_rotation_merge<_RandomAccessIter, __rotate, _ValueTp>(bds1, bde1 - 1, bde1 - 1 + (b - (bde2 - 1)), k, bStart_adapted); // sqrt(m) is our original block_size

	// We merge the remaining left BD-storage
	block_rotation_merge<_RandomAccessIter, __rotate, _ValueTp>(b - (n - 1), b, lMerged, k, bStart_adapted); // sqrt(m) is our original block_size

	assert(verify(bStart + bSize, last));

	std::sort(bStart, bStart + bSize);

	assert(verify(bStart, bStart + bSize));
	assert(verify(bStart + bSize, last));

	hwang_and_lin<_RandomAccessIter, __rotate>(bStart, bStart + bSize, last, (_RandomAccessIter)NULL); // quite expensive
#endif
	
	
	// END SWEEPING UP

#if 0
	assert(verify(orig_first1, orig_last));
#endif
	
}
