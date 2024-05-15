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

#ifndef USE_LINEAR_SEARCH
#define USE_LINEAR_SEARCH // use a linear search instead of a binary search 
#endif

// returns the size of the actually extracted buffer
template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter), class _ValueTp, class _Distance>
_Distance extract_buffer_left_right(_RandomAccessIter first, 
								    _RandomAccessIter last, 
								    _Distance buffer_size)
{
	
	_RandomAccessIter bstart = first, bend = first;
	
	while ((_Distance(bend - bstart) < buffer_size) && (bend < last)) {

#ifdef USE_LINEAR_SEARCH
		_RandomAccessIter b = bend;
		do {b++;} while ((b < last) && (*b == *bend));
#else
		_RandomAccessIter b = bsearch_upper(bend + 1, last, *bend);
#endif

		__rotate (bstart, bend + 1, b); 
		bstart = b - (bend - bstart + 1); 
		bend = b;
	} // while

	assert(verify(bstart, bend));

	__rotate(first, bstart, bend); // we move the extracted buffer to the front

	return bend - bstart;
}