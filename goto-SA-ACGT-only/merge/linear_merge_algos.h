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

#pragma warning(disable:4996)

// the linear algorithm (slightly modified form of the STL-implementation)

template <class _RandomAccessIter, class _ValueTp>
void linear 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last,
	 _RandomAccessIter aux_buf) 
{
	assert(aux_buf != NULL); // global buffer available ?

	copy_forward(first1, last, aux_buf);
	
#if 0
	merge_backward <_RandomAccessIter, _RandomAccessIter, _RandomAccessIter, _ValueTp>
		(aux_buf, 
		 aux_buf + (first2 - first1), 
		 aux_buf + (first2 - first1), 
		 aux_buf + (last - first1), 
		 last);
#endif
	std::_Merge_backward <_RandomAccessIter>
		(aux_buf, 
		 aux_buf + (first2 - first1), 
		 aux_buf + (first2 - first1), 
		 aux_buf + (last - first1), 
		 last);
}

/* Linear merge with swap instead of copy 
 *(more assignments - no destruction of the original buffer-content)
 */
template <class _RandomAccessIter, class _ValueTp>
void linear_swap 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last,
	 _RandomAccessIter aux_buf) 
{
	assert(aux_buf != NULL); 

	if (first2 - first1 > last - first2)
		// right side shorter
		linear_swap_buffer_right<_RandomAccessIter>(first1, first2, last, aux_buf, false);
	else
		// left side shorter or equal
		linear_swap_buffer_left<_RandomAccessIter>(first1, first2, last, aux_buf, false);
}

// we copy the area between first1 and first2 into the buffer, and merge forward
template <class _RandomAccessIter>
void linear_swap_buffer_left 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buffer, 
	 bool omit_buffer_copy) 

{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	
	if (first2 - first1 <= 0)
		return; // nothing to merge
	if (!omit_buffer_copy)
		std::swap_ranges(first1, first2, buffer); // buffer copy
	merge_forward_swap<_RandomAccessIter, _ValueTp> 
		(first2,
		 last,
		 buffer, 
		 buffer + (first2 - first1),  
		 first1);
}

// we copy the area between first2 and last into the buffer, and merge backward
template <class _RandomAccessIter>
void linear_swap_buffer_right 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _RandomAccessIter buffer, 
	 bool omit_buffer_copy) 

{
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	
	if (last - first2 <= 0)
		return; // nothing to merge
	if (!omit_buffer_copy)
		std::swap_ranges(first2, last, buffer);
	merge_backward_swap<_RandomAccessIter, _ValueTp> 
		(first1,
		 first2,
		 buffer, 
		 buffer + (last - first2),  
		 last);
}

template <class _RandomAccessIter, class _ValueTp>
_RandomAccessIter merge_backward_swap
	(_RandomAccessIter first1,
	 _RandomAccessIter last1,
	 _RandomAccessIter first2,
     _RandomAccessIter last2,
	 _RandomAccessIter result) 
{
	if (first1 == last1)
		return swap_backward(first2, last2, result);
	if (first2 == last2)
		return swap_backward(first1, last1, result);
  
	--last1;
	--last2;

	while (true) {
		if (*last2 < *last1) {
			std::iter_swap(--result, last1);

			if (first1 >= last1)
				return swap_backward(first2, ++last2, result);
			
			--last1;
		} // if
		else {
			std::iter_swap(--result, last2);

			if (first2 >= last2)
				return swap_backward(first1, ++last1, result);
			
			--last2;
		} //else
	} // while
}

template <class _RandomAccessIter, class _ValueTp>
_RandomAccessIter merge_forward_swap
	(_RandomAccessIter first1,
	 _RandomAccessIter last1,
	 _RandomAccessIter first2,
     _RandomAccessIter last2,
	 _RandomAccessIter result) 
{
	if (first1 == last1)
		return _swap_forward(first2, last2, result);
	if (first2 == last2)
		return _swap_forward(first1, last1, result);

	while (true) {
		if (*first1 < *first2) {
			std::iter_swap(result, first1);

			first1++;
			result++;
			if (first1 >= last1)
				return _swap_forward(first2, last2, result);
		} // if
		else {
			std::iter_swap(result, first2);

			first2++;
			result++;
			if (first2 >= last2)
				return _swap_forward(first1, last1, result);
		} //else
	} // while
}

template <class ValueTp>
inline ValueTp* _swap_forward(ValueTp* first1, 
                              ValueTp* last1, 
                              ValueTp* first2)
{
	while (first1 != last1) {
		ValueTp buf = *first2;
		*(first2++) = *first1;
		*(first1++) = buf;
	}
	return first2; // we return the first position after the last copied element
}

template <class ValueTp>
inline ValueTp* swap_backward(ValueTp* first1, 
                              ValueTp* last1,
                              ValueTp* last2)
{
	while (first1 != last1) {
		ValueTp buf = *(--last2);
		*last2 = *(--last1);
		*last1 = buf;
	}
	return last2; 
}

