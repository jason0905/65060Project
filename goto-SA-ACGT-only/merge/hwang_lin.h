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

#ifdef CHECK_INTERMEDIATE_RESULTS
#undef CHECK_INTERMEDIATE_RESULTS
#endif

#define OMIT_BUFFER_COPY true
#define PERFORM_BUFFER_COPY false

#define APPLY_OPTIMIZATIONS

// Hwang and Lin for the case where the  r i g h t  side is copied to the buffer (right side shorter)
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
void hwang_and_lin_right 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buffer, 
	 bool omit_buffer_copy) 

{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;

#ifndef NDEBUG
	_RandomAccessIter orig_last = last;
#endif
	
	int len = last - first2;

	int f = (first2 - first1) / len;
	assert(f >= 1);

#if 0
	if (f < 1) {
		printf("VIOLATION ASSERTION (hwang_and_lin_left): (f < 1)\n");
		exit(1);
	}
#endif
	
	int k = 1 << log2_floor(f); // k = 2 ^ (log (n/m))
	assert(k >= 1);

#if 0
	if (k < 1) {
		printf("VIOLATION ASSERTION (hwang_and_lin_left): (k < 1)\n");
		exit(1);
	}
#endif
	
	// START BUFFER RELATED CODE
	if (buffer != NULL) {

#ifdef APPLY_OPTIMIZATIONS
		if (k == 1) {
			linear_swap_buffer_right(first1, first2, last, buffer, omit_buffer_copy);
			assert(verify(first1, last));

			return; 
		}
#endif // APPLY_OPTIMIZATIONS
		if (!omit_buffer_copy)
			std::swap_ranges(first2, last, buffer); // copy the shorter sequence to the buffer
	} // if
	// END BUFFER RELATED CODE

    do {

	assert(last - first2 == len);

#if 0
		if (last - first2 != len) {
			printf("VIOALTION hwang_and_lin_right: (last - first2 != len)\n");
			exit (1);
		}
#endif

		_RandomAccessIter p = first2 - k;
		_RandomAccessIter start = first2;
		
		_ValueTp element = (buffer != NULL) ? *(buffer + len - 1) : *(last - 1); // BUFFER RELATED CODE

		while (true) {
			if (p < first1) {
				p = first1 - 1;
				break;
			}
			if (!(element < *p)) // A[p] <= A[last - 1]
				break;
			start = p;
			p = p - k;
		}

		_RandomAccessIter b = bsearch_upper(p + 1, start, element);

		assert((b >= first2) || (element < *b));
		assert((b <= first1) || (element >= *(b - 1)));

#if 0
		if ((b <= first2 - 1) && (!(element < *b))) {
			printf("Hwang and Lin / right side shorter / Error with binary search (A)\n");
			exit (1);
		}
		if (b - 1 > first1 - 1 && (element < *(b - 1))) {
			printf("Hwang and Lin / right side shorter / Error with binary search (B)\n");
			exit (1);
		}
#endif
	    
		// START BUFFER RELATED CODE
		if (buffer != NULL) {
			swap_backward(b, first2, last);
			std::iter_swap(b + len - 1, buffer + len - 1);
		} else
			// We apply a rotation (because there is no buffer)
			__rotate(b, first2, last);
		// END BUFFER RELATED CODE

		first2 = b;
		last = b + (--len);

#ifdef CHECK_INTERMEDIATE_RESULTS
		assert(verify(last, orig_last));
#endif
// START OPTIMIZATION (the algorithm works also without this optimization ...)
		if (first1 >= first2) {
			if (buffer != NULL) {
				std::swap_ranges(first1, last, buffer);
			}
			break;
		} // if
// END OPTIMIZATION

	} while (first2 < last);
	
#ifdef CHECK_INTERMEDIATE_RESULTS
	assert(verify(first1, orig_last));
#endif
}

// Hwang and Lin for the case where the  l e f t  side is copied to the buffer (left side shorter)
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
void hwang_and_lin_left 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buffer, 
	 bool omit_buffer_copy) 

{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;

#ifndef NDEBUG
	_RandomAccessIter orig_first1 = first1;
#endif	

	int len = first2 - first1;

	int f = (last - first2) / len;
	assert(f >= 1);

#if 0
		if (f < 1) {
			printf("VIOLATION ASSERTION (hwang_and_lin_left): (f < 1)\n");
			exit(1);
		}
#endif
		
	int k = 1 << log2_floor(f); // k = 2 ^ (log (n/m))
	
	assert(k >= 1);

#if 0
	if (k < 1) {
		printf("VIOLATION ASSERTION (hwang_and_lin_left): (k < 1)\n");
		exit(1);
	}
#endif
	
	// START BUFFER RELATED CODE
	if (buffer != NULL) {

#ifdef APPLY_OPTIMIZATIONS
		if (k == 1) {
			linear_swap_buffer_left(first1, first2, last, buffer, omit_buffer_copy);

			assert(verify(first1, last));
			return; 
		}
#endif // APPLY_OPTIMIZATIONS

		if (!omit_buffer_copy)
			std::swap_ranges(first1, first2, buffer);
		buffer += len;
	} // if
	// END BUFFER RELATED CODE

    do {
		_RandomAccessIter p = first2 - 1 + k;
		_RandomAccessIter start = first2;
		
		_ValueTp element = (buffer != NULL) ? *(buffer - len) : *(first1); // BUFFER RELATED CODE

		while (true) {
			if (p >= last) {
				p = last;
				break;
			}
			if (!(*p < element)) // A[first1] <= A[p]
				break;
			start = p + 1;
			p = p + k;
		}

		_RandomAccessIter b = bsearch_lower(start, p, element);

		assert((b >= last) || (*b >= element));
		assert((b <= first2) || (*(b - 1) < element));

#if 0
		if ((b < last) && (*b < element)) {
			printf("Hwang and Lin / left side shorter / Error with binary search (A)\n");
			exit (1);
		}
		if ((b > first2) && (!(*(b - 1) < element))) {
			printf("Hwang and Lin / left side shorter / Error with binary search (B)\n");
			exit (1);
		}
#endif
	    
		// START BUFFER RELATED CODE
		if (buffer != NULL) {
			std::swap_ranges(first2, b, first1);
			std::iter_swap(b - len, buffer - len);
		} else
			// We apply a rotation (because there is no buffer)
			__rotate(first1, first2, b);
		// END BUFFER RELATED CODE

		first2 = b;
	    first1 = b - (--len);

#ifdef CHECK_INTERMEDIATE_RESULTS
		assert(verify(orig_first1, first1));
#endif
// START OPTIMIZATION (the algorithm works also without this optimization ...)
		if (first2 >= last) {
			if (buffer != NULL) {
				std::swap_ranges(first1, last, buffer - len);
			}
			break;
		} // if
// END OPTIMIZATION

	} while (first1 < first2);

#ifdef CHECK_INTERMEDIATE_RESULTS
	assert(verify(orig_first1, last));
#endif
}

template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
inline void hwang_and_lin
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buffer) 

{
	if (first1 >= first2 || first2 >= last)
		return;
	
	if (first2 - first1 > last - first2)
		hwang_and_lin_right<_RandomAccessIter, __rotate>(first1, first2, last, buffer, PERFORM_BUFFER_COPY);
	else
		hwang_and_lin_left<_RandomAccessIter, __rotate>(first1, first2, last, buffer, PERFORM_BUFFER_COPY);
}

