#ifndef COMPARE_SORT_H
#define COMPARE_SORT_H

#include <functional>
#include <list>
#include <random>

#include <structures/heap>

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
template <typename ForwardIt, typename Compare>
void mergesort(ForwardIt first, ForwardIt last, Compare compare)
{
    // base case
    if (first == last)
        return;
    ForwardIt temp(first);
    if (++temp == last)
        return;

    // recursive step
    temp = first;
    std::advance(temp, std::distance(first, last) / 2);

    mergesort(first, temp, compare);
    mergesort(temp, last, compare);

    // merge sub-solutions
    std::list<typename std::iterator_traits<ForwardIt>::value_type> first_half(first, temp),
        second_half(temp, last);
    auto it1 = first_half.begin(), it2 = second_half.begin();
    while (it1 != first_half.end() && it2 != second_half.end()) {
        if (compare(*it1, *it2)) {
            *first++ = std::move(*(it1++));
        } else {
            *first++ = std::move(*(it2++));
        }
    }

    // one of these is empty
    std::copy(it1, first_half.end(), first);
    std::copy(it2, second_half.end(), first);
}
template <typename ForwardIt> void mergesort(ForwardIt first, ForwardIt last)
{
    mergesort(first, last, std::less<>());
}

/*
 Partition a list of numbers for ordering
 Used for quicksort and selection
 Returns an iterator pointing to the partition value
 */
template <typename BiDirIt, typename Compare>
BiDirIt partition(BiDirIt first, BiDirIt last, BiDirIt partition, Compare compare)
{
    // setup for partition
    std::iter_swap(partition, first);
    auto forward_it(first), backward_it(last);
    ++forward_it;
    --backward_it;
    bool processed_last = false; // we have not yet considered backward_it

    // partition: on each pass, find two values that need to be swapped
    while (true) {
        while (((processed_last && forward_it != backward_it)
                   || (!processed_last && forward_it != last))
            && compare(*forward_it, *first))
            ++forward_it;
        if ((processed_last && forward_it == backward_it)
            || (!processed_last && forward_it == last)) {
            break;
        }

        processed_last = true;
        while (forward_it != backward_it && compare(*first, *backward_it))
            --backward_it;
        if (forward_it == backward_it)
            break;

        std::iter_swap(forward_it, backward_it);
        ++forward_it;
    }

    // move partition value to correct place
    std::iter_swap(first, --forward_it);
    return forward_it;
}

/*
Quicksort
Needs bidirectional iterators
Tony Hoare
Algorithm 64: Quicksort
(1959) doi:10.1145/366622.366644

Θ(n^2) worst case, Θ(n log n) expected case
Θ(1) extra space
*/
template <typename BiDirIt, typename Compare>
void quicksort(BiDirIt first, BiDirIt last, Compare compare)
{
    static std::random_device rd;
    static std::mt19937_64 engine(rd());
    // base case
    auto size = std::distance(first, last);
    if (size <= 1)
        return;

    // random pivot, swap with first element
    auto forward_temp(first);
    std::uniform_int_distribution<uint32_t> index_generator(0, size - 1);
    std::advance(forward_temp, index_generator(engine));

    auto partition_index = partition(first, last, forward_temp, compare);

    quicksort(first, partition_index, compare);
    quicksort(++partition_index, last, compare);
}
template <typename BiDirIt> void quicksort(BiDirIt first, BiDirIt last) { quicksort(first, last); }

/*
Heapsort
J. W. J. Williams
Algorithm 232: Heapsort
(1964) doi:10.1145/512274.512284

Θ(n log n) worst case
Θ(1) extra space for random access iterators, Θ(n) for any other type
*/
template <typename It, typename Compare> void heapsort(It first, It last, Compare compare)
{
    if (first == last)
        return;

    // Random access: can sort without extra space
    if constexpr (std::is_same_v<typename std::iterator_traits<It>::iterator_category,
                      std::random_access_iterator_tag>) {
        typedef typename std::iterator_traits<It>::difference_type diff_type;
        diff_type distance = last - first;

        // heapify in opposite direction
        for (diff_type position = distance - 1; position + 1 > 0; --position) {
            diff_type current(position);

            while (2 * current + 1 < distance) {
                diff_type child(2 * current + 1);
                if (2 * current + 2 < distance && compare(first[child], first[child + 1])) {
                    ++child;
                }

                if (compare(first[current], first[child])) {
                    std::iter_swap(first + current, first + child);
                    current = child;
                } else {
                    break;
                }
            }
        }

        // swap root to end
        for (--last; last != first; --last) {
            std::iter_swap(first, last);
            --distance;

            diff_type current = 0;
            while (2 * current + 1 < distance) {
                diff_type child(2 * current + 1);
                if (2 * current + 2 < distance && compare(first[child], first[child + 1])) {
                    ++child;
                }

                if (compare(first[current], first[child])) {
                    std::iter_swap(first + current, first + child);
                    current = child;
                } else {
                    break;
                }
            }
        }
    } else {
        heap::priority_queue<typename std::iterator_traits<It>::value_type, Compare> heap(
            first, last, compare);
        std::generate(first, last, [&heap]() { return heap.remove_root(); });
    }
}
template <typename It> void heapsort(It first, It last) { heapsort(first, last, std::less<>()); }

} // namespace Sequence

#endif // !COMPARE_SORT_H
