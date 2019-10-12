#ifndef NON_COMPARE_SORT_H
#define NON_COMPARE_SORT_H

#include <functional>

namespace sequence {
/*
 Radix sort, customizable base
 Herman Hollerth (1887)
 Time: O(n log_base(max)) where n is distance(first, last), max is largest
 element Space: O(n + base)
 */
template <typename It> void radix_sort(It first, It last, uint32_t base);
} // namespace Sequence

#include "src/sequence/non_compare_sort.cpp"

#endif // NON_COMPARE_SORT_H
