#ifndef SUBSEQUENCE_CPP
#define SUBSEQUENCE_CPP

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <sequence/BinarySearch.h>

template <typename It1, typename It2>
bool Sequence::isSubsequence(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast)
{
    for (; targetFirst != targetLast; ++targetFirst, ++masterFirst) {
        masterFirst = std::find(masterFirst, masterLast, *targetFirst);
        if (masterFirst == masterLast)
            return false;
    }
    return true;
}

template <typename It1, typename It2>
uint32_t Sequence::maxStutter(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast)
{
    uint32_t lower = 0,
             upper
        = std::distance(masterFirst, masterLast) / std::distance(targetFirst, targetLast);

    // binary search style
    while (lower < upper) {
        uint32_t mid = (lower + 1 == upper) ? upper : (lower + upper) / 2;
        StutterIterator<It1> stutterFirst(targetFirst, mid), stutterLast(targetLast, mid);
        if (isSubsequence(stutterFirst, stutterLast, masterFirst, masterLast))
            lower = mid;
        else
            upper = mid - 1;
    }
    return lower;
}

template <typename It, typename Compare>
std::list<It> Sequence::longestOrderedSubsequence(It first, It last, Compare comp)
{
    if constexpr (std::is_same<typename std::iterator_traits<It>::iterator_category,
                      std::random_access_iterator_tag>::value) {
        std::vector<It> predecessor(last - first, last);
        std::vector<It> tail(predecessor.size(),
            last); // all non-last entries point to elements in ascending order

        // populate vectors
        uint32_t index = 0;
        for (auto temp(first); temp != last; ++temp, ++index) {
            auto replaceIt = findCutoff(tail.begin(), tail.begin() + index,
                [&temp, &last, &comp](const It& it) { return it != last && comp(*it, *temp); });

            if (replaceIt != tail.begin())
                predecessor[index] = *(replaceIt - 1);
            *replaceIt = temp;
        }

        // backtrack through predecessor starting with last element of tail
        std::list<It> result;
        for (auto temp
             = *(findCutoff(tail.begin(), tail.end(), [&last](const It& it) { return it != last; })
                 - 1);
             temp != last; temp = predecessor[temp - first]) {
            result.push_front(temp);
        }
        return result;
    } else {
        // transfer something with random access iterators
        std::vector<typename std::iterator_traits<It>::value_type> buffer(first, last);
        auto tempResult = longestOrderedSubsequence(buffer.begin(), buffer.end(), comp);

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
