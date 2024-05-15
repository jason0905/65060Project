#include <stdio.h>
//#include <stdlib.h>

#include <algorithm>
#include <string>
#include <sstream>

#define NDEBUG
#include <algobase.h>

//Using only these subsets because the full files are not needed and raise other compile errors
//That I don;t want to deal with 
#include <bsearch.h>
//#include <minimum_storage_algos.h>
#include <swap.h>
//#include <linear_merge_algos.h>

#include <rotation_algos.h>
#include <hwang_lin.h>
#include <TAMC-2008/tamc_2008_algo.h>

//Adapted from https://stackoverflow.com/questions/12092448/code-for-a-basic-random-access-iterator-based-on-pointers
template<typename Type>
class ReverseIterator : public std::iterator<
                        std::random_access_iterator_tag, //iterator_category
                        Type, //value_type
                        std::ptrdiff_t, //different_type
                        Type*,//pointer
                        Type&>{ //reference
public:
    using difference_type = typename std::iterator<std::random_access_iterator_tag, Type>::difference_type;
    ReverseIterator() : _ptr(nullptr) {}
    ReverseIterator(Type* rhs) : _ptr(rhs) {}
    ReverseIterator(const ReverseIterator &rhs) : _ptr(rhs._ptr) {}
    /* inline ReverseIterator& operator=(Type* rhs) {_ptr = rhs; return *this;} */
    /* inline ReverseIterator& operator=(const ReverseIterator &rhs) {_ptr = rhs._ptr; return *this;} */
    inline ReverseIterator& operator+=(difference_type rhs) {_ptr -= rhs; return *this;}
    inline ReverseIterator& operator-=(difference_type rhs) {_ptr += rhs; return *this;}
    inline Type& operator*() const {return *_ptr;}
    inline Type* operator->() const {return _ptr;}
    inline Type& operator[](difference_type rhs) const {return _ptr[-rhs];}

    inline ReverseIterator& operator++() {--_ptr; return *this;}
    inline ReverseIterator& operator--() {++_ptr; return *this;}
    inline ReverseIterator operator++(int) {ReverseIterator tmp(*this); --_ptr; return tmp;}
    inline ReverseIterator operator--(int) {ReverseIterator tmp(*this); ++_ptr; return tmp;}
    /* inline ReverseIterator operator+(const ReverseIterator& rhs) {return ReverseIterator(_ptr+rhs.ptr);} */
    inline difference_type operator-(const ReverseIterator& rhs) const {return rhs._ptr-_ptr;}
    inline ReverseIterator operator+(difference_type rhs) const {return ReverseIterator(_ptr-rhs);}
    inline ReverseIterator operator-(difference_type rhs) const {return ReverseIterator(_ptr+rhs);}
    friend inline ReverseIterator operator+(difference_type lhs, const ReverseIterator& rhs)
        {return ReverseIterator(rhs._ptr-lhs);}
    friend inline ReverseIterator operator-(difference_type lhs, const ReverseIterator& rhs)
        {return ReverseIterator(lhs+rhs._ptr);}

    inline bool operator==(const ReverseIterator& rhs) const {return _ptr == rhs._ptr;}
    inline bool operator!=(const ReverseIterator& rhs) const {return _ptr != rhs._ptr;}
    inline bool operator>(const ReverseIterator& rhs) const {return _ptr < rhs._ptr;}
    inline bool operator<(const ReverseIterator& rhs) const {return _ptr > rhs._ptr;}
    inline bool operator>=(const ReverseIterator& rhs) const {return _ptr <= rhs._ptr;}
    inline bool operator<=(const ReverseIterator& rhs) const {return _ptr >= rhs._ptr;}
private:
    Type* _ptr;
};


template <typename idx_t>
struct desc_comp { //Descending-order comparisons
    idx_t index;

    bool operator<(const desc_comp &other) const {
        return index > other.index;
    }
    bool operator<=(const desc_comp &other) const {
        return index >= other.index;
    }
    bool operator>(const desc_comp &other) const {
        return index < other.index;
    }
    bool operator>=(const desc_comp &other) const {
        return index <= other.index;
    }
    bool operator!=(const desc_comp &other) const {
        return index != other.index;
    }
    bool operator==(const desc_comp &other) const {
        return index == other.index;
    }
};


template <class dtype> void merge_inplace(dtype *start, size_t mid, size_t len) {
    if (mid == 0 || len - mid == 0) return;
    if (mid <= len - mid) { //Right side is smaller - proceed as normal
        TAMC2008_merge<dtype*,&__rotate_swapping>(start, start+mid, start+len);
    } else { //Right side is larger, so merge reversed array in descending order
        typedef desc_comp<dtype> elem;
        typedef ReverseIterator<elem> iter;
        iter rev_array = ReverseIterator<elem>(((elem*) start) + (len-1));
        TAMC2008_merge<iter,&__rotate_swapping>(rev_array, rev_array+(len-mid), rev_array+len);
    }
}

//template <class dtype> void merge_inplace(dtype *start, size_t l1, size_t l2) {
//    if (l1 > l2) {
//        __rotate_swapping(start, start+l1, start+l1+l2);
//        l1 = l1 + l2; l2 = l1 - l2; l1 = l1 - l2;
//    }
//    TAMC2008_merge<dtype*,&__rotate_swapping>(start, start+l1, start+l1+l2);
//}
