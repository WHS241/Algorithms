#ifndef ORDER_STATISTICS_H
#define ORDER_STATISTICS_H
#include <functional>

namespace sequence {
/*
Find both min and max elements
result.first is min, result.second is max
3n/2 comparisons (compared to 2n for finding each separately)
*/
template <typename It, typename Compare = std::less<>>
std::pair<It, It> extrema(It first, It last, Compare compare = Compare());

/*
 Selection
 Select the k-th smallest element (k passed as parameter rank)
 Parameter 4 (optional) is the size of subproblems algorithm divides into
 Must be >=5 for algorithm to be Θ(n), preferably odd. Infinite
 recursion/looping if 1
 Manuel Blum, Robert Floyd, Vaughan Pratt, Ronald Rivest, Robert Tarjan (1972)
 */
template <typename It, typename Compare = std::less<>>
typename std::iterator_traits<It>::value_type selection(
    It first, It last, uint32_t rank, Compare compare = Compare(), uint32_t partition_size = 5);
} // namespace Sequence

#include "src/sequence/order_stats.cpp"

#endif // !ORDER_STATISTICS_H
