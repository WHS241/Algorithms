#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <functional>

/*
All algorithms require random access iterators for O(log n)
*/
namespace Sequence {
    /*
     Straightforward binary search
     Values iterators point to should be sorted with Compare
     */
    template <typename RandomIt, typename T, typename Compare = std::less<>>
    RandomIt binarySearch(RandomIt first, RandomIt last, const T &item,
                  Compare comp = Compare());

    /*
    Assuming values are ordered such that all elements x for which pred(x) is true
    come before those for which pred(x) is false, find the first element after that cutoff.
    */
    template <typename RandomIt, typename Function>
    RandomIt findCutoff(RandomIt first, RandomIt last, Function pred);

    /*
    Find minimum of cyclically sorted list
    Requirements:
    For some 0 <= x < (last-first), if i != x, comp(*(first + i), *(first + i + 1))
    If x != 0, comp(*first, *(last - 1))
    */
    template<typename RandomIt, typename Compare = std::less<>>
    RandomIt cyclicFindMin(RandomIt first, RandomIt last, Compare comp = Compare());

    /*
    Special Binary Search
    Find x such that *(first + x) = x
    Requirements:
    RandomIt::operator*() returns type int or equivalent
    Values are sorted with std::less (or equivalent) and distinct
    return -1 if not found
    */
    template<typename RandomIt>
    uint32_t specialIndex(RandomIt first, RandomIt last);
} // namespace BinarySearch

#include "../../src/sequence/BinarySearch.cpp"

#endif // !BINARY_SEARCH_H
