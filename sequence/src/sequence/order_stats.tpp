#ifndef ORDER_STATISTICS_CPP
#define ORDER_STATISTICS_CPP
#include <array>
#include <list>
#include <stdexcept>
#include <vector>

#include <sequence/compare_sort.h>

template <typename It, typename Compare>
std::pair<It, It> sequence::extrema(It first, It last, Compare compare)
{
    if (first == last)
        return std::make_pair(first, first);

    It temp(first);
    ++temp;
    if (temp == last)
        return std::make_pair(first, first);

    std::pair<It, It> result
        = compare(*first, *temp) ? std::make_pair(first, temp) : std::make_pair(temp, first);
    first = ++temp;

    // extend two at a time: compare against each other before result
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
typename std::iterator_traits<It>::value_type sequence::selection(
    It first, It last, uint32_t rank, Compare compare, uint32_t partition_size)
{
    typedef typename std::iterator_traits<It>::value_type T;
    std::list<T> buffer(first, last);
    if (rank >= buffer.size())
        throw std::out_of_range("Rank exceeds size");
    if (partition_size < 2)
        throw std::invalid_argument("Will result in infinite loop");
    if (partition_size > buffer.size())
        partition_size = buffer.size();

    while (buffer.size() > partition_size) {
        std::vector<T> subset;

        // put into columns and find median of each
        std::list<T> medians;
        for (auto& item : buffer) {
            subset.push_back(item);
            if (subset.size() == partition_size) {
                quicksort(subset.begin(), subset.end(), compare);
                medians.push_back(subset[partition_size / 2]);
                subset.clear();
            }
        }
        if (!subset.empty()) {
            quicksort(subset.begin(), subset.end(), compare);
            medians.push_back(subset[subset.size() / 2]);
        }

        // median of medians, then partition
        auto pivot_value = selection(
            medians.begin(), medians.end(), medians.size() / 2, compare, partition_size);
        auto partition_iterator = std::find(buffer.begin(), buffer.end(), pivot_value);
        partition_iterator = partition(buffer.begin(), buffer.end(), partition_iterator, compare);

        // eliminate candidates
        std::list<T> temp_buffer;
        temp_buffer.splice(temp_buffer.begin(), buffer, buffer.begin(), partition_iterator);
        if (rank < temp_buffer.size()) {
            std::swap(buffer, temp_buffer);
        } else if (rank == temp_buffer.size()) {
            return *partition_iterator;
        } else {
            rank -= (temp_buffer.size() + 1);
            buffer.pop_front(); // known to be partition_iterator
        }

        // temp_buffer cleans up, eliminating members
    }
    quicksort(buffer.begin(), buffer.end(), compare);
    auto it = buffer.begin();
    std::advance(it, rank);
    return *it;
}

#endif // ORDER_STATISTICS_CPP