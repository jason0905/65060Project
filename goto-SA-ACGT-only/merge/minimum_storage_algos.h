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

/* subalgorithms that are embedded in _Merge
*/

template <class _RandomAccessIter, class _ValueTp>
_RandomAccessIter bsearch_lower
	(_RandomAccessIter first, 
	 _RandomAccessIter last, 
	 _ValueTp x) 
{
	_RandomAccessIter middle;

	while (first < last) {
		middle = first + (last - first) / 2;

		if (x <= * middle) // (x <= A[middle]) COMPARISON
			last = middle;
		else
			first = middle + 1;
	}
	return first;
}

template <class _RandomAccessIter, class _ValueTp>
_RandomAccessIter bsearch_upper
	(_RandomAccessIter first, 
	 _RandomAccessIter last, 
	 _ValueTp x) 
{
	_RandomAccessIter middle;

	while (first < last) {
		middle = first + (last - first) / 2;

		if (x < *middle) // (x < A[middle]) COMPARISON
			last = middle;
		else
			first = middle + 1;
	}
	return first;
}

template <class _RandomAccessIter>
inline _RandomAccessIter minimum 
	(_RandomAccessIter first, 
	 _RandomAccessIter last) 
{

	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	
	assert(first < last);

	_RandomAccessIter current_min = first++;

	for (; first < last; first++) 
		if (*first < *current_min)
			current_min = first;
	
	return current_min;
}

/* The merging algorithm from Dudzinski and Dydek */

template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
void dyd_original
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last) 
{
	typedef typename std::iterator_traits<_RandomAccessIter>::difference_type distance; 
	
	distance len1 = first2 - first1;
	distance len2 = last - first2;	
	
	if (len1 == 0 || len2 == 0) 
		return;

	_RandomAccessIter first_cut, second_cut;

	if (len1 <= len2) {
		
		// LEFT SIDE SHORTER
		first_cut = first1 + (((first2 - first1) /* - 1 */) / 2); // Pointer addition is impossible in C(++) :-(
		second_cut = bsearch_lower (first2, last, *first_cut); // binary search 

		__rotate(first_cut, first2, second_cut); 
		_RandomAccessIter new_mid = first_cut + (second_cut - first2);
	
		dyd_original<_RandomAccessIter, __rotate>(first1, first_cut, new_mid);
		dyd_original<_RandomAccessIter, __rotate>(new_mid + 1, second_cut, last);
	} else { 
		
		// RIGHT SIDE SHORTER
		second_cut = first2 + (((last - first2) /* - 1 */) / 2); // Pointer addition is impossible in C(++) :-(
		first_cut = bsearch_upper (first1, first2, *second_cut); // binary search
	
		__rotate(first_cut, first2, second_cut + 1);
		_RandomAccessIter new_mid = first_cut + (second_cut - first2);
	
		dyd_original<_RandomAccessIter, __rotate>(first1, first_cut, new_mid);
		dyd_original<_RandomAccessIter, __rotate>(new_mid + 1, second_cut + 1, last);
	}
} // dyd_original

/* The ESA 2004 algorithm */

template <class _RandomAccessIter>	
inline _RandomAccessIter bsearch (_RandomAccessIter l, 
								  _RandomAccessIter r, 
								  const _RandomAccessIter& middle, 
								  const _RandomAccessIter& first2)
{
	_RandomAccessIter m;
	while (l < r) {
		m = l + ((r - l) / 2);
		if (*m <= *(middle + (first2 - m) - 1))
			l = m + 1;
		else
			r = m;
	}
	return l;
}

template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter)>
void symmerge 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last) 
{
	if (first1 >= first2 || first2 >= last)
		return;

	_RandomAccessIter middle = first1 + ((last - first1) / 2); // (first1 + last) / 2;
	// int n = middle + first2;
	
	_RandomAccessIter first_cut;
	if (first2 > middle)  
		// ** left side longer **
		first_cut = bsearch (middle - (last - first2), middle, middle, first2); 
	else             
		// ** right side longer **
		first_cut = bsearch (first1, first2, middle, first2);

	_RandomAccessIter second_cut = middle + (first2 - first_cut);
			
	__rotate (first_cut, first2, second_cut);
		
	symmerge<_RandomAccessIter, __rotate> (first1, first_cut, middle); // left recursion
	symmerge<_RandomAccessIter, __rotate> (middle, second_cut, last); // right recursion
}

#if 0

/* The SOFSEM 2007 algorithm 
*/

template <class _RandomAccessIter, class _ValueTp>
void _Merge<_RandomAccessIter, _ValueTp>::simple_merge 
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last) {
	
	if (first1 >= first2 || first2 >= last)
		return;

	_RandomAccessIter l = first1, r = first2;
	_RandomAccessIter l_ = r, r_ = last;
	_RandomAccessIter m, m_;

	do {	
		if (l < r)
			m = l + (r - l) / 2;  // (l + r) / 2; Pointer addition is impossible in C(++) :-(
		if (l_ < r_)
			m_ = l_ + (r_ - l_) / 2; // (l_ + r_) / 2; Pointer addition is impossible in C(++) :-(
		
#ifdef COUNT_COMPARISONS
		if (!__smaller_than(*m_, *m)) { // (A[m] <= A[m_])
#else
		if (*m <= *m_) {
#endif
			l = m + 1;
			r_ = m_;
		} else {
			
			l_ = m_ + 1;
			r = m;
		}
		/* if ((r < first2) && (l_ > first2) && (A[r] <= A[l_ - 1])) {
			printf("Violation Assertion (A[r] <= A[l_ - 1])\n");
			exit(1);
		}
		if ((l > first1) && (r_ <last) && (A[l - 1] > A[r_]))  {
			printf("Violation Assertion (A[l - 1] > A[r_])\n");
			exit(1);
		} */
	} while ((l < r) || (l_ < r_));
	
	_RandomAccessIter left_end = r + (r_ - first2);
	_RandomAccessIter right_start = l + (l_ - first2);
	
	/* if (abs (last - first2 - (first2 - first1)) > 1) {
		 if (left_end != right_start)
			printf("Found difference A\n");
	}
	else 
		if (left_end == right_start)
			printf("Found difference B\n"); */

	__rotate (r, first2, l_);
	simple_merge (first1, r, left_end);
	simple_merge (right_start, l_, last);
}
#endif