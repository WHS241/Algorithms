#ifndef COMPARE_SORT_CPP
#define COMPARE_SORT_CPP

#include <iterator>
#include <list>
#include <type_traits>

#include <structures/heap_base.h>

template <typename ForwardIt, typename Compare>
void sequence::mergesort(ForwardIt first, ForwardIt last, Compare compare)
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

template <typename BiDirIt, typename Compare>
void sequence::quicksort(BiDirIt first, BiDirIt last, Compare compare)
{
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

template <typename BiDirIt, typename Compare>
BiDirIt sequence::partition(BiDirIt first, BiDirIt last, BiDirIt partition, Compare compare)
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

template <typename It, typename Compare> void sequence::heapsort(It first, It last, Compare compare)
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

#endif // !COMPARE_SORT_CPP
