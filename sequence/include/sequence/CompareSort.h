#ifndef COMPARE_SORT_H
#define COMPARE_SORT_H

#include <functional>
#include <random>

/*
Comparison sorts (compatible with all data types, including objects)
*/
namespace Sequence {
/*
 Mergesort
 Forward iterators are sufficient
 John von Neumann (1945)
 O(n log n) worst case
 O(n) extra space
 */
template <typename ForwardIt, typename Compare = std::less<>>
void mergesort(ForwardIt first, ForwardIt last, Compare compare = Compare());

/*
Quicksort
Needs bidirectional iterators
Tony Hoare (1959)
O(n^2) worst case, O(n log n) expected case
O(1) extra space
*/
template <typename BiDirIt, typename Compare = std::less<>>
void quicksort(BiDirIt first, BiDirIt last, Compare compare = Compare());

/*
 Partition function
 Used for quicksort and selection
 Returns an iterator pointing to the partition value
 */
template <typename BiDirIt, typename Compare = std::less<>>
BiDirIt partition(BiDirIt first, BiDirIt last, BiDirIt partition, Compare compare = Compare());

/*
Heapsort
 J. W. J. Williams (1964)
O(n log n) worst case
O(1) extra space for random access iterators, O(n) for any other type
*/
template <typename It, typename Compare = std::less<>>
void heapsort(It first, It last, Compare compare = Compare());

namespace {
    std::random_device rd;
    std::mt19937_64 engine(rd());
} // namespace
} // namespace Sequence

#include "../../src/sequence/CompareSort.cpp"

#endif // !COMPARE_SORT_H
