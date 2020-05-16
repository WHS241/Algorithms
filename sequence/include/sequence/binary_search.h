#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <cstdint>
#include <functional>

/*
All algorithms require random access iterators for Θ(log n)
*/
namespace sequence {
/*
 Straightforward binary search
 Values iterators point to should be sorted with Compare
 */
template <typename RandomIt, typename T, typename Compare>
RandomIt binary_search(RandomIt first, RandomIt last, const T& item, Compare comp)
{
    if (first == last)
        return first;

    RandomIt no_match = last;
    while (last - first > 1) {
        RandomIt mid = first + (last - first) / 2;
        if (*mid == item)
            return mid;

        if (comp(item, *mid))
            last = mid;
        else
            first = mid;
    }

    return (*first == item) ? first : no_match;
}
template <typename RandomIt, typename T>
RandomIt binary_search(RandomIt first, RandomIt last, const T& item)
{
    return binary_search(first, last, item, std::less<T>());
}

/*
Assuming values are ordered such that all elements x for which pred(x) is true
come before those for which pred(x) is false, find the first element after that
cutoff.
*/
template <typename RandomIt, typename Function>
RandomIt find_cutoff(RandomIt first, RandomIt last, Function pred)
{
    if (first == last || !pred(*first))
        return first;
    if (pred(*(last - 1)))
        return last;

    while (last - first > 2) {
        RandomIt mid = first + (last - first) / 2;

        if (pred(*(mid++)))
            first = mid;
        else
            last = mid;
    }

    return pred(*first) ? first + 1 : first;
}

/*
Find minimum of cyclically sorted list
Requirements:
For some 0 <= x < (last-first), if i != x, comp(*(first + i), *(first + i + 1))
If x != 0, comp(*first, *(last - 1))
*/
template <typename RandomIt, typename Compare>
RandomIt find_min_in_cyclic(RandomIt first, RandomIt last, Compare comp)
{
    if (first == last)
        return first;

    while (last - first > 2) {
        RandomIt mid = first + (last - first) / 2;
        if (comp(*(mid++), *(last - 1)))
            last = mid;
        else
            first = mid;
    }
    --last;
    return comp(*first, *last) ? first : last;
}
template <typename RandomIt, typename T> RandomIt find_min_in_cyclic(RandomIt first, RandomIt last)
{
    return find_min_in_cyclic(first, last, std::less<T>());
}

/*
Special Binary Search
Find x such that *(first + x) = x
Requirements:
RandomIt::operator*() returns type int or equivalent
Values are sorted with std::less (or equivalent) and distinct
return -1 if not found
*/
template <typename RandomIt> uint32_t special_index(RandomIt first, RandomIt last)
{
    RandomIt noMatch = last, begin = first;
    while (last - first > 1) {
        RandomIt mid = first + (last - first) / 2;
        int mid_value = mid - begin;
        if (*mid == mid_value)
            return mid_value;
        if (*mid < mid_value) {
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    if (first == last || *first != first - begin)
        return -1;
    return first - begin;
}
} // namespace Sequence

#endif // !BINARY_SEARCH_H
