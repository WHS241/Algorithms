#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <functional>

/*
All algorithms require random access iterators for Θ(log n)
*/
namespace sequence {
/*
 Straightforward binary search
 Values iterators point to should be sorted with Compare
 */
template <typename RandomIt, typename T, typename Compare = std::less<>,
    typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
RandomIt binary_search(RandomIt first, RandomIt last, const T& item)
{
    return binary_search(first, last, item, Compare());
}
template <typename RandomIt, typename T, typename Compare>
RandomIt binary_search(RandomIt first, RandomIt last, const T& item, Compare comp);

/*
Assuming values are ordered such that all elements x for which pred(x) is true
come before those for which pred(x) is false, find the first element after that
cutoff.
*/
template <typename RandomIt, typename Function>
RandomIt find_cutoff(RandomIt first, RandomIt last, Function pred);

/*
Find minimum of cyclically sorted list
Requirements:
For some 0 <= x < (last-first), if i != x, comp(*(first + i), *(first + i + 1))
If x != 0, comp(*first, *(last - 1))
*/
template <typename RandomIt, typename T, typename Compare = std::less<>,
    typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
RandomIt find_min_in_cyclic(RandomIt first, RandomIt last)
{
    return find_min_in_cyclic(first, last, Compare());
}
template <typename RandomIt, typename Compare>
RandomIt find_min_in_cyclic(RandomIt first, RandomIt last, Compare comp);

/*
Special Binary Search
Find x such that *(first + x) = x
Requirements:
RandomIt::operator*() returns type int or equivalent
Values are sorted with std::less (or equivalent) and distinct
return -1 if not found
*/
template <typename RandomIt> uint32_t special_index(RandomIt first, RandomIt last);
} // namespace Sequence

#include "src/sequence/binary_search.tpp"

#endif // !BINARY_SEARCH_H
