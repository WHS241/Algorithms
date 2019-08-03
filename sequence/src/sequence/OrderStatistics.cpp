#ifndef ORDER_STATISTICS_CPP
#define ORDER_STATISTICS_CPP
#include <array>
#include <list>
#include <stdexcept>
#include <vector>

#include <sequence/CompareSort.h>

template <typename It, typename Compare>
std::pair<It, It> OrderStatistics::extrema(It first, It last, Compare compare) {
    if (first == last)
        return std::make_pair(first, first);

    It temp(first);
    ++temp;
    if (temp == last)
        return std::make_pair(first, first);

    std::pair<It, It> result = compare(*first, *temp)
                               ? std::make_pair(first, temp)
                               : std::make_pair(temp, first);
    first = ++temp;

    while (first != last) {
        ++temp;
        if (temp == last) {
            if (compare(*first, *result.first))
                result.first = first;
            if (compare(*result.second, *first))
                result.second = first;
            first = last;
        } else {
            auto lower = first, upper = temp;
            if (compare(*temp, *first))
                std::swap(lower, upper);

            if (compare(*lower, *result.first))
                result.first = lower;
            if (compare(*result.second, *upper))
                result.second = upper;

            first = ++temp;
        }
    }

    return result;
}

template <typename It, typename Compare>
typename std::iterator_traits<It>::value_type OrderStatistics::selection(It first, It last, uint32_t rank, 
                              Compare compare, uint32_t partitionSize) {
    typedef typename std::iterator_traits<It>::value_type T;
    std::list<T> buffer(first, last);
    if (rank >= buffer.size())
        throw std::out_of_range("Rank exceeds size");
    if (partitionSize < 2)
        throw std::invalid_argument("Will result in infinite loop");
    if (partitionSize > buffer.size())
        partitionSize = buffer.size();

    while (buffer.size() > partitionSize) {
        std::vector<T> subset;
        std::list<T> medians;
        for (auto &item : buffer) {
            subset.push_back(item);
            if (subset.size() == partitionSize) {
                CompareSort::quicksort(subset.begin(), subset.end(), compare);
                medians.push_back(subset[partitionSize / 2]);
                subset.clear();
            }
        }

        if (!subset.empty()) {
            CompareSort::quicksort(subset.begin(), subset.end(), compare);
            medians.push_back(subset[subset.size() / 2]);
        }

        auto partition = selection(medians.begin(), medians.end(),
                                   medians.size() / 2, compare, partitionSize);
        auto partitionIt = std::find(buffer.begin(), buffer.end(), partition);
        partitionIt = CompareSort::partition(buffer.begin(), buffer.end(),
                                             partitionIt, compare);

        // eliminate candidates
        std::list<T> tempBuffer;
        tempBuffer.splice(tempBuffer.begin(), buffer, buffer.begin(), partitionIt);
        if (rank < tempBuffer.size()) {
            std::swap(buffer, tempBuffer);
        } else if (rank == tempBuffer.size()) {
            return *partitionIt;
        } else {
            rank -= (tempBuffer.size() + 1);
            buffer.pop_front(); // known to be partitionIt
        }

        // tempBuffer cleans up, eliminating members
    }
    CompareSort::quicksort(buffer.begin(), buffer.end(), compare);
    auto it = buffer.begin();
    std::advance(it, rank);
    return *it;
}

#endif // ORDER_STATISTICS_CPP