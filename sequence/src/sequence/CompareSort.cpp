#ifndef COMPARE_SORT_CPP
#define COMPARE_SORT_CPP

#include <iterator>
#include <list>
#include <type_traits>

#include <Heap.h>

template <typename ForwardIt, typename Compare>
void CompareSort::mergesort(ForwardIt first, ForwardIt last, Compare compare) {
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
    std::list<typename std::iterator_traits<ForwardIt>::value_type> firstHalf(
            first, temp),
            secondHalf(temp, last);
    auto it1 = firstHalf.begin(), it2 = secondHalf.begin();
    while (it1 != firstHalf.end() && it2 != secondHalf.end()) {
        if (compare(*it1, *it2)) {
            *first++ = std::move(*it1);
            ++it1;
        } else {
            *first++ = std::move(*it2);
            ++it2;
        }
    }

    // one of these is empty
    std::copy(it1, firstHalf.end(), first);
    std::copy(it2, secondHalf.end(), first);
}

template <typename BiDirIt, typename Compare>
void CompareSort::quicksort(BiDirIt first, BiDirIt last, Compare compare) {
    // base case
    auto size = std::distance(first, last);
    if (size <= 1)
        return;

    // random pivot, swap with first element
    auto forwardTemp(first);
    std::uniform_int_distribution<uint32_t> indexGen(0, size - 1);
    std::advance(forwardTemp, indexGen(engine));

    auto partitionIt = partition(first, last, forwardTemp, compare);

    quicksort(first, partitionIt, compare);
    quicksort(++partitionIt, last, compare);
}

template <typename BiDirIt, typename Compare>
BiDirIt CompareSort::partition(BiDirIt first, BiDirIt last, BiDirIt partition,
                               Compare compare) {
    std::iter_swap(partition, first);

    // setup for partition
    auto forwardTemp(first), backwardTemp(last);
    ++forwardTemp;
    --backwardTemp;
    bool processedBack =
            false; // "false" indicates we have not yet considered backwardTemp

    // partition: on each pass, find two values that need to be swapped
    while (true) {
        while (((processedBack && forwardTemp != backwardTemp) ||
                (!processedBack && forwardTemp != last)) &&
               compare(*forwardTemp, *first))
            ++forwardTemp;
        if ((processedBack && forwardTemp == backwardTemp) ||
              (!processedBack && forwardTemp == last)) {
            break;
        }

        processedBack = true;
        while (forwardTemp != backwardTemp && compare(*first, *backwardTemp))
            --backwardTemp;
        if (forwardTemp == backwardTemp)
            break;

        std::iter_swap(forwardTemp, backwardTemp);
    }

    // move partition value to correct place
    std::iter_swap(first, --forwardTemp);
    return forwardTemp;
}

template <typename It, typename Compare>
void CompareSort::heapsort(It first, It last, Compare compare) {
    if (first == last)
        return;

    // Random access: can sort without extra space
    if constexpr (std::is_same<
            typename std::iterator_traits<It>::iterator_category,
            std::random_access_iterator_tag>::value) {
        auto distance = last - first;

        // heapify in opposite direction
        for (auto position = distance - 1; position + 1 > 0; --position) {
            auto current(position);
            It currentIt = first + current;

            while (2 * current + 1 < distance) {
                auto child(2 * current + 1);
                It childIt = first + child;
                if (2 * current + 2 < distance && compare(childIt[0], childIt[1])) {
                    ++child;
                    ++childIt;
                }

                if (compare(*currentIt, *childIt)) {
                    std::iter_swap(currentIt, childIt);
                    current = child;
                    currentIt = childIt;
                } else {
                    break;
                }
            }
        }

        // swap root to end
        for (--last; last != first; --last) {
            std::iter_swap(first, last);
            --distance;

            typename std::iterator_traits<It>::difference_type current = 0;
            It currentIt = first;
            while (2 * current + 1 < distance) {
                auto child(2 * current + 1);
                It childIt = first + child;
                if (2 * current + 2 < distance && compare(*childIt, childIt[1])) {
                    ++child;
                    ++childIt;
                }

                if (compare(*currentIt, *childIt)) {
                    std::iter_swap(currentIt, childIt);
                    current = child;
                    currentIt = childIt;
                } else {
                    break;
                }
            }
        }
    } else {
        BinaryHeap<typename std::iterator_traits<It>::value_type, Compare> heap(
                first, last, compare);
        std::generate(first, last, [&heap]() { return heap.removeRoot(); });
    }
}

#endif // !COMPARE_SORT_CPP
