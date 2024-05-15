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

// rotate and rotate_copy, and their auxiliary functions

#pragma warning(disable:4996)

template <class _EuclideanRingElement>
_EuclideanRingElement __gcd(_EuclideanRingElement __m,
                            _EuclideanRingElement __n)
{
  while (__n != 0) {
    _EuclideanRingElement __t = __m % __n;
    __m = __n;
    __n = __t;
  }
  return __m;
}

template <class _ForwardIter>
void __rotate_swapping(_ForwardIter __first,
                       _ForwardIter __middle,
                       _ForwardIter __last)
{
  if (__first == __middle)
    return; 
  if (__last  == __middle)
    return; 

  _ForwardIter __first2 = __middle;
  do {
	std::swap(*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
  } while (__first2 != __last);

  _ForwardIter __new_middle = __first;

  __first2 = __middle;

  while (__first2 != __last) {
	std::swap (*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
    else if (__first2 == __last)
      __first2 = __middle;
  }
}

template <class _RandomAccessIter>
void __reverse(_RandomAccessIter __first, _RandomAccessIter __last)
{
  while (__first < __last)
	  std::iter_swap(__first++, --__last);
}

template <class _BidirectionalIter>
void __rotate_reverse(_BidirectionalIter __first,
					  _BidirectionalIter __middle,
					  _BidirectionalIter __last) 
{
  if (__first == __middle)
    return; // __last;
  if (__last  == __middle)
    return; // __first;

  __reverse(__first,  __middle);
  __reverse(__middle, __last);

  while (__first != __middle && __middle != __last)
	  std::swap (*__first++, *--__last);

  if (__first == __middle) {
    __reverse(__middle, __last);
    return; // __last;
  }
  else {
    __reverse(__first,  __middle);
    return; // __first;
  }
}

template <class _RandomAccessIter>
void __rotate_juggling(_RandomAccessIter __first,
                       _RandomAccessIter __middle,
					   _RandomAccessIter __last)
{
  typedef typename std::iterator_traits<_RandomAccessIter>::difference_type _Distance;
  typedef typename std::iterator_traits<_RandomAccessIter>::value_type _Tp; 

  _Distance __n = __last   - __first;
  _Distance __k = __middle - __first;
  _Distance __l = __n - __k;
  _RandomAccessIter __result = __first + (__last - __middle);

  if (__k == 0)
    return; // __last;

  else if (__k == __l) {
	std::swap_ranges(__first, __middle, __middle);
    return; // __result;
  }

  _Distance __d = __gcd(__n, __k);

  for (_Distance __i = 0; __i < __d; __i++) {
    _Tp __tmp = *__first;
    _RandomAccessIter __p = __first;

    if (__k < __l) {
      for (_Distance __j = 0; __j < __l/__d; __j++) {
        if (__p > __first + __l) {
          *__p = *(__p - __l);
          __p -= __l;
        }

        *__p = *(__p + __k);
        __p += __k;
      }
    }

    else {
      for (_Distance __j = 0; __j < __k/__d - 1; __j ++) {
        if (__p < __last - __k) {
          *__p = *(__p + __k);
          __p += __k;
        }

        *__p = * (__p - __l);
        __p -= __l;
      }
    }

    *__p = __tmp;
    ++__first;
  }

  return; // __result;
}

#if 0
template <class _RandomAccessIter>
inline void float_hole__ (_RandomAccessIter first1, _RandomAccessIter last1, _RandomAccessIter first2, _RandomAccessIter first3) {
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _Tp; 

	while (first1 != last1) {
		_Tp buf = *first1;
		*(first1++) = *first2;
		*(first2++) = *first3;
		*(first3++) = buf;
#ifdef COUNT_ASSIGMENTS
		assignments += 4;
#endif
	}	
}


template <class _RandomAccessIter>
void float_hole__(_RandomAccessIter first1, 
						 const _RandomAccessIter &last1, 
						 _RandomAccessIter first2, 
						 _RandomAccessIter first3) {
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _Tp; 

	for (;first1 != last1;) {
		_Tp buf = *first1;
		*(first1++) = *first2;
		*(first2++) = *first3;
		*(first3++) = buf;
#ifdef COUNT_ASSIGMENTS
		assignments += 4;
#endif
	}	
}

/* New hybrid rotation algorithm  
*/
template <class _RandomAccessIter >
void speed_rotate (_RandomAccessIter first, _RandomAccessIter pivot, _RandomAccessIter last) {
	typedef typename std::iterator_traits<_RandomAccessIter>::difference_type _Distance;
	
	_Distance deltal = pivot - first;
	if (deltal == 0)
		return; 
	_Distance deltar = last - pivot;
	if (deltar == 0)
		return; 

	do {	
		if (deltal <= deltar) {	
			if (2 * deltal > deltar) {
				pivot = first + (deltar - deltal);
				if (first != pivot) {
					float_hole__(first, pivot, first + deltal, first + deltar);
				}
				
				__std::swap_ranges(pivot, 
								   first + deltal, 
								   pivot + deltar);

				first = pivot;
				deltar -= deltal;
				deltal -= deltar;
			}
			else {
				pivot = first + deltal;
				float_hole__(first, pivot, pivot, first + deltar);
				first = pivot;
				deltar = deltar - 2 * deltal;	
			}
		} else {
			if (2 * deltar > deltal) {
				pivot = first + deltar;
				float_hole__(pivot + deltar, pivot + deltal, pivot, pivot - (deltal - deltar));
				__std::swap_ranges(first, 
								   pivot - (deltal - deltar), 
								   first + deltal);
				first = pivot;
				deltal -= deltar;
				deltar -= deltal;
			}
			else {
				pivot = first + deltar;
				float_hole__(first + deltal, pivot + deltal, pivot, first);
				first = pivot + deltar;
				deltal = deltal - 2 * deltar; 
			}
		}
	} while (deltal > 0 && deltar > 0); // do - while
}

template <class _RandomAccessIter >
void speed_rotate_version2 (_RandomAccessIter first, _RandomAccessIter pivot, _RandomAccessIter last) {
	typedef typename std::iterator_traits<_RandomAccessIter>::difference_type _Distance;
	_Distance deltal = pivot - first;
	_Distance deltar = last - pivot;

	while (deltal > 0 && deltar > 0) {	
#ifdef CHECK_ASSERTIONS
				exit_if(pivot - first != deltal, 
					"speed-rotate: pivot - first != deltal");
#endif
		if (deltal <= deltar) {
			_Distance len = deltar - deltal;
			if (deltal > len) {	
				// len == 0 occurs in the case of equal input sizes
				if (len > 0) {
					float_hole__(first, first + len, pivot, pivot + len);
					first += len;
					deltal = deltal - len;
				} 
				
				std::swap_ranges(first, pivot, pivot + len + len);
				
				deltar = len;
			}
			else {
				float_hole__(first, pivot, pivot, pivot + len);

				deltar = len - deltal;
				first += deltal;
				pivot += deltal;
#ifdef CHECK_ASSERTIONS
				exit_if(deltal + deltar != len, 
					"speed-rotate 1: deltal + deltar != len");
#endif
			}
		} else {
 			_Distance len = deltal - deltar;  
			if (len < deltar) {
				first += deltar;
#ifdef CHECK_ASSERTIONS
				exit_if(first + len != pivot, 
					"speed-rotate : first + len != pivot");
				exit_if(first + deltar != pivot + (deltar - len), 
					"speed-rotate : first + deltar != pivot + (deltar - len)");
#endif
				float_hole__(first, pivot, first - len, first + deltar);		
				
				std::swap_ranges(first - deltar, first - len, pivot);
				
				deltal = len;
				deltar = deltar - len;
			}
			else {
				float_hole__(pivot - deltar, pivot, first, pivot);
				
				deltal = len - deltar;
				first += deltar;
				pivot = pivot - deltar;
#ifdef CHECK_ASSERTIONS
				exit_if(deltal + deltar != len, 
					"speed-rotate 2: deltal + deltar != len");
#endif
			} // inner else

#ifdef CHECK_ASSERTIONS
			exit_if(first + deltal != pivot, 
				"speed-rotate : first + deltal != pivot");
#endif
		} // outer else
	} // while
}

template <class _RandomAccessIter >
void speed_rotate_version3 (_RandomAccessIter first, _RandomAccessIter pivot, _RandomAccessIter last) {
	register _uint deltal = pivot - first;
	register _uint deltar = last - pivot;

	while (deltal > 0 && deltar > 0) {
		if (deltal <= deltar) {
			register _uint len = deltar - deltal;
			if (deltal > len) {
				// (7 Pointer Arithmetic Operations)
				float_hole(first, first + len, first + deltal, first + deltar);
				first += len;
				deltal = deltal - len;
				std::swap_ranges(first, first + deltal, first + deltar);
				
				deltar = len;
			}
			else {
				// (5 Pointer Arithmetic Operations)
				float_hole(first, first + deltal, first + deltal, first + deltar);
				
				first += deltal;
				deltar = len - deltal;
#ifdef CHECK_ASSERTIONS
				exit_if(deltal + deltar != len, 
					"speed-rotate 1: deltal + deltar != len");
#endif
			} // inner else
		} else {
 			register _uint len = deltal - deltar;  
			if (deltar > len) {
				/* VARIANT 1 (8 Pointer Arithmetic Operations)
				float_hole(first, first + len, first + deltal, first + deltar); // floating hole left
				first += deltal;
				
				__std::swap_ranges(first + len, first + deltar, first - deltar);
				
				deltar = deltar - len;
				deltal = len;
				*/
				
				// Variant 2 (7 Pointer Arithmetic Operations) -- Symmetric Varinat
				first += deltar;
				float_hole(first - len, first, first + deltar, first);
				
				std::swap_ranges(first - deltar, first - len, first + len);
				
				deltar = deltar - len;
				deltal = len;
			}
			else {
				/* VARIANT 1 (6 Pointer Arithmetic Operations)
				float_hole(first, first + deltar, first + deltal, first + deltar);
				
				first += 2 * deltar;
				deltal = len - deltar;
				*/
				// Variant 2 (5 Pointer Arithmetic Operations) -- Symmetric Variant
				float_hole(first, first + deltar, first + deltal, first + len);
				
				first += deltar;
				deltal = len - deltar;
#ifdef CHECK_ASSERTIONS
				exit_if(deltal + deltar != len, 
					"speed-rotate 1: deltal + deltar != len");
#endif
			} // inner else
		} // outer else
	} // while
}
#if 0
template <class _ForwardIter>
inline _ForwardIter rotate(_ForwardIter __first, _ForwardIter __middle,
                           _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  return __rotate(__first, __middle, __last,
                  __DISTANCE_TYPE(__first),
                  __ITERATOR_CATEGORY(__first));
}

template <class _ForwardIter, class _OutputIter>
_OutputIter rotate_copy(_ForwardIter __first, _ForwardIter __middle,
                        _ForwardIter __last, _OutputIter __result) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  return copy(__first, __middle, copy(__middle, __last, __result));
}

// Reversal rotate. If you want to use this algorithm, call the reversal_rotate()
template <class _ForwardIter>
inline void reverse(_ForwardIter i, _ForwardIter j)
{ 
	while (i < j)
		__std::iter_swap(i++, j--);
}

template <class _ForwardIter>
void reversal_rotate(_ForwardIter first, _ForwardIter middle, _ForwardIter last)
{ 	
	reverse(first, middle - 1);	
	reverse(middle, last - 1);	
	reverse(first, last - 1);
}

// STL rotate is more or less identical to swap_rotate
template <class _ForwardIter>
void STL_rotate (_ForwardIter __first, _ForwardIter __middle,
                 _ForwardIter __last )
{
	
	if (__first == __middle)
		return; //__last;
	if (__last  == __middle)
		return; // __first;

	_ForwardIter __first2 = __middle;
	do {
		__std::iter_swap(__first++, __first2++);
		if (__first == __middle)
			__middle = __first2;
	} while (__first2 != __last);

	_ForwardIter __new_middle = __first;

	__first2 = __middle;

	while (__first2 != __last) {
		__std::iter_swap(__first++, __first2++);
		if (__first == __middle)
			__middle = __first2;
		else if (__first2 == __last)
			__first2 = __middle;
	}

	// return __new_middle;
}


template <class _IntType>
_IntType gcd (_IntType m, _IntType n) {
	while (n != 0) { 
		_IntType t = m % n; 
		m = n; 
		n = t; 
	}
	return m;
}

template <class _ValueTp>
void gcd_rotate_dummy(_ValueTp *first, _ValueTp *pivot, _ValueTp *last) {
	_uint g = gcd((_uint)(last - first), (_uint)(pivot - first));
	if (g)
		*first = *pivot;
}

template <class _ValueTp>
void gcd_rotate_no_assignments(_ValueTp *first, _ValueTp *pivot, _ValueTp *last) {

	if (pivot <= first || last <= pivot) 
		return;
	
	_uint d = pivot - first, n = last - first;
	_uint g = gcd((_uint)(last - first), d);

	while (g-- != 0) {
		// _ValueTp val = *(first + g);

		_uint p1 = g, p2 = g + d;
		while (p2 != g) {
			// *(first + p1) = *(first + p2);

			p1 = p2;
			if (n - p2 > d) 
				p2 += d;
			else 
				p2 = (d - (n - p2));
		} // inner while
		// *(first + p1) = val;

	} // outer while
}

template <class _ValueTp>
void gcd_rotate(_ValueTp *first, _ValueTp *pivot, _ValueTp *last) {

	if (pivot <= first || last <= pivot) 
		return;
	
	_uint d = pivot - first, n = last - first;
	_uint g = gcd((_uint)(last - first), d);

	while (g-- != 0) {
		_ValueTp val = *(first + g);
#ifdef COUNT_ASSIGMENTS
		assignments++;
#endif
		_uint p1 = g, p2 = g + d;
		while (p2 != g) {
			*(first + p1) = *(first + p2);
#ifdef COUNT_ASSIGMENTS
			assignments++;
#endif

			p1 = p2;
			if (n - p2 > d) 
				p2 += d;
			else 
				p2 = (d - (n - p2));
		} // inner while
		*(first + p1) = val;
#ifdef COUNT_ASSIGMENTS
		assignments++;
#endif
	} // outer while
}

/* 
// pointer based implementation of the GCD-rotation algorithm
// (without compiler optimizations slower than integer implementation)

template <class _ValueTp>
void gcd_rotate_pointer_based(_ValueTp *first, _ValueTp *pivot, _ValueTp *last) {
	if (pivot <= first || last <= pivot) 
		return;
	
	_uint d = pivot - first, n = last - first;

	_ValueTp *g_ptr = first + gcd(n, d);
	while (g_ptr-- != first) {
		_ValueTp val = *g_ptr;
#ifdef COUNT_ASSIGMENTS
		assignments++;
#endif

		_ValueTp *p1_ptr = g_ptr;
		_ValueTp *p2_ptr = g_ptr + d;
		while (p2_ptr != g_ptr) {
			*p1_ptr = *p2_ptr;
#ifdef COUNT_ASSIGMENTS
			assignments++;
#endif

			p1_ptr = p2_ptr;
			p2_ptr += d;
			if (p2_ptr >= last) 
				p2_ptr = first + (p2_ptr - last);
		} // inner while
		*p1_ptr = val;
#ifdef COUNT_ASSIGMENTS
		assignments++;
#endif
	} // outer while
}
*/

template <class _RandomAccessIter>
void swap_rotate (_RandomAccessIter first, 
				  _RandomAccessIter pivot, 
				  _RandomAccessIter last) 
{	
	if (pivot <= first || last <= pivot) 
		return;
	
	_uint i = pivot - first;
	_uint j = last - pivot;
	_RandomAccessIter p = first + i;

	while (i != j)
		if (i > j) {
			__std::swap_ranges(p - i, p - i + j, p);
			i -= j;
		}
		else {
			__std::swap_ranges(p - i, p, p + j - i);
			j -= i;
		}
	__std::swap_ranges(p - i, p, p);
} 

#endif 

#endif