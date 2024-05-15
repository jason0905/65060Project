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

/* block_rotation_merge is optimal up to the ratio sqrt(m)
   (Check the TAMC 2008 paper for more details)
*/

template <class _RandomAccessIter, void (* __rotate)(_RandomAccessIter, _RandomAccessIter, _RandomAccessIter), class _ValueTp, class _Distance>
void block_rotation_merge
	(_RandomAccessIter first1, 
	 _RandomAccessIter first2, 
	 _RandomAccessIter last, 
	 _Distance blocksize, 
	 _RandomAccessIter buffer
	) 

{
	if ((first1 >= first2) || (first2 >= last)) // this check is not really necessary (only an optimization)
		return;

#ifndef NDEBUG	
	_RandomAccessIter orig_first1 = first1; // DEBUG variable
#endif
	
	assert(verify(first1, first2));
	assert(verify(first2, last));

	_RandomAccessIter end = first1 + ((first2 - first1) % blocksize);
	end = end == first1 ? end + blocksize - 1 : end - 1;

	do {
		_RandomAccessIter b = bsearch_lower(first2, last, *end); // Binary Search
		__rotate(end, first2, b);

		hwang_and_lin<_RandomAccessIter, __rotate>(first1, end, end + (b - first2), buffer);

		first1 = end + 1 + (b - first2);
		first2 = b;
		end = first1 + blocksize - 1;
	} while (first1 < first2);

	assert(verify(orig_first1, last));
}