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

#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <cassert>

/* Some general basic algorithms
*/

template <class BidirectionalIter1, class BidirectionalIter2>
inline BidirectionalIter2 copy_forward(BidirectionalIter1 first, 
                                       BidirectionalIter1 last, 
                                       BidirectionalIter2 result)
{
	while (first != last) {
		*result++ = *first++;
	}
	return result; // we return the first position after the last copied element
}

template <class ValueTp>
inline void swap_and_move (ValueTp *first1, ValueTp *last1, ValueTp *first2) {
	ValueTp buf = *(first1 - 1);
	
	while (first1 != last1) {
		*(first1 - 1) = *first2;
		*(first2++) = *(first1++);
	}
	*(first1 - 1) = buf;
}

template <class _RandomAccessIter>
inline void float_hole (_RandomAccessIter first1, _RandomAccessIter last1, _RandomAccessIter first2, _RandomAccessIter first3) {
	
	typedef typename std::iterator_traits<_RandomAccessIter>::value_type _ValueTp;
	
	while (first1 != last1) {
		_ValueTp buf = *first1;
		*(first1++) = *first2;
		*(first2++) = *first3;
		*(first3++) = buf;
	}	
}

// Computation of logarithm (basis 2)

template <class _IntType>
unsigned int log2_floor(_IntType x) {
	
	assert(!(x <= 0));

	_IntType pat = 2;
	unsigned int log = 0; // (or int log = 1; if we want to have log 1 => 1)
	for (unsigned int i = 1; pat != 0; pat <<= 1, i++)
		if ((x & pat) != 0)
			log = i;
	return log;
}

template <class _IntType>
unsigned int log2_ceil(_IntType x) {

	assert(!(x <= 0));
	
	_IntType pat = 1;
	unsigned int ones = 0;
	unsigned int log = 0; // (or int log = 1; if we want to have log 1 => 1)
	for (unsigned int i = 0; pat != 0; pat <<= 1, i++)
		if ((x & pat) != 0) {
			log = i;
			ones++;
		}

	return (ones > 1) ? log + 1 : log;
}

// A extension of ostream for the output of vectors
template <class _Tp>
inline std::string to_string (const _Tp& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

#endif