//Extracted from minimum_storage_algos.h


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
