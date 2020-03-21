#ifndef COMPARE_SORT_H
#define COMPARE_SORT_H

#include <functional>
#include <random>

/*
Comparison sorts (compatible with all data types, including objects)
*/
namespace sequence {
/*
 Mergesort
 Forward iterators are sufficient
 John von Neumann (1945)
 Θ(n log n) worst case
 Θ(n) extra space
 */
template <typename ForwardIt, typename Compare = std::less<>>
void mergesort(ForwardIt first, ForwardIt last, Compare compare = Compare());

/*
Quicksort
Needs bidirectional iterators
Tony Hoare
Algorithm 64: Quicksort
(1959) doi:10.1145/366622.366644

Θ(n^2) worst case, Θ(n log n) expected case
Θ(1) extra space
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
J. W. J. Williams
Algorithm 232: Heapsort
(1964) doi:10.1145/512274.512284

Θ(n log n) worst case
Θ(1) extra space for random access iterators, Θ(n) for any other type
*/
template <typename It, typename Compare = std::less<>>
void heapsort(It first, It last, Compare compare = Compare());

namespace {
    std::random_device rd;
    std::mt19937_64 engine(rd());
} // namespace
} // namespace Sequence

#include "src/sequence/compare_sort.tpp"

#endif // !COMPARE_SORT_H
