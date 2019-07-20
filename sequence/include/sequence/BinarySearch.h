#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <functional>

/*
All algorithms require random access iterators
*/
namespace BinarySearch {
    /* Straightforward binary search
    Values iterators point to should be sorted with Compare
    */
    template<typename RandomIt, typename T, typename Compare = std::less<>>
    RandomIt binarySearch(RandomIt first, RandomIt last, T item, Compare comp = Compare());

    /*
    Cyclie binary search
    Requirements:
    For some 0 <= x < (last-first), if i != x, comp(*(first + i), *(first + i + 1))
    If x != 0, comp(*first, *(last - 1))
    */
    template<typename RandomIt, typename T, typename Compare = std::less<>>
    RandomIt cyclicBinarySearch(RandomIt first, RandomIt last, T item, Compare comp = Compare());

    /*
    Special Binary Search
    Find x such that *(first + x) = x
    Requirements:
    RandomIt::operator*() returns type int
    Values are sorted and distinct
    */
    template<typename RandomIt, typename Compare = std::less<>>
    int specialIndex(RandomIt first, RandomIt last, Compare comp = Compare());
}

#endif // !BINARY_SEARCH_H
