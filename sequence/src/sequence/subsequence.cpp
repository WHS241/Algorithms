#ifndef SUBSEQUENCE_CPP
#define SUBSEQUENCE_CPP

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <sequence/binary_search.h>

template <typename It1, typename It2>
bool sequence::is_subsequence(It1 target_first, It1 target_last, It2 master_first, It2 master_last)
{
    for (; target_first != target_last; ++target_first, ++master_first) {
        master_first = std::find(master_first, master_last, *target_first);
        if (master_first == master_last)
            return false;
    }
    return true;
}

template <typename It1, typename It2>
uint32_t sequence::max_stutter(It1 target_first, It1 target_last, It2 master_first, It2 master_last)
{
    uint32_t lower = 0,
             upper
        = std::distance(master_first, master_last) / std::distance(target_first, target_last);

    // binary search style
    while (lower < upper) {
        uint32_t mid = (lower + 1 == upper) ? upper : (lower + upper) / 2;
        stutter_iterator<It1> stutterFirst(target_first, mid), stutterLast(target_last, mid);
        if (is_subsequence(stutterFirst, stutterLast, master_first, master_last))
            lower = mid;
        else
            upper = mid - 1;
    }
    return lower;
}

template <typename It, typename Compare>
std::list<It> sequence::longest_ordered_subsequence(It first, It last, Compare comp)
{
    if constexpr (std::is_same<typename std::iterator_traits<It>::iterator_category,
                      std::random_access_iterator_tag>::value) {
        std::vector<It> predecessor(last - first, last);
        // all non-last entries point to elements in ascending order
        std::vector<It> tail(predecessor.size(), last);

        // populate vectors
        uint32_t index = 0;
        for (auto temp(first); temp != last; ++temp, ++index) {
            auto replace_iterator = find_cutoff(tail.begin(), tail.begin() + index,
                [&temp, &last, &comp](const It& it) { return it != last && comp(*it, *temp); });

            if (replace_iterator != tail.begin())
                predecessor[index] = *(replace_iterator - 1);
            *replace_iterator = temp;
        }

        // backtrack through predecessor starting with last element of tail
        std::list<It> result;
        for (auto temp
             = *(find_cutoff(tail.begin(), tail.end(), [&last](const It& it) { return it != last; })
                 - 1);
             temp != last; temp = predecessor[temp - first]) {
            result.push_front(temp);
        }
        return result;
    } else {
        // transfer to something with random access iterators
        std::vector<typename std::iterator_traits<It>::value_type> buffer(first, last);
        auto tempResult = longest_ordered_subsequence(buffer.begin(), buffer.end(), comp);

        // convert to input iterators
        std::list<It> result;
        auto lastTemp = buffer.begin();
        std::transform(tempResult.begin(), tempResult.end(), std::back_inserter(result),
            [&first, &lastTemp](auto it) {
                std::advance(first, it - lastTemp);
                lastTemp = it;
                return first;
            });
        return result;
    }
}

#endif // !SUBSEQUENCE_CPP
