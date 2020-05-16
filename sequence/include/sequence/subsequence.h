#ifndef SUBSEQUENCE_H
#define SUBSEQUENCE_H

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <structures/red_black_tree.h>
#include <structures/van_Emde_Boas_map.h>

#include "binary_search.h"
#include "order_stats.h"

namespace sequence {
/*
Verify if [targetFirst, targetLast) is a subsequence of [masterFirst,
masterLast)
Conditions: It1::value_type is equivalent to It2::value_type
Θ(distance(targetFirst, targetLast) + distance(masterFirst, masterLast))
*/
template <typename It1, typename It2>
bool is_subsequence(It1 target_first, It1 target_last, It2 master_first, It2 master_last)
{
    for (; target_first != target_last; ++target_first, ++master_first) {
        master_first = std::find(master_first, master_last, *target_first);
        if (master_first == master_last)
            return false;
    }
    return true;
}

namespace {
    /*
     Iterator adaptor:
     Simulate stuttering without having to create a new container
     */
    template <typename It> class stutter_iterator {
    public:
        typedef typename std::iterator_traits<It>::value_type value_type;

        stutter_iterator(const It it, uint32_t stutter)
            : _iterator(it)
            , _stutter(stutter)
            , _counter(0) {};

        bool operator==(const stutter_iterator& rhs)
        {
            return _stutter == rhs._stutter && _counter == rhs._counter
                && _iterator == rhs._iterator;
        }
        bool operator!=(const stutter_iterator& rhs) { return !operator==(rhs); }
        stutter_iterator& operator++()
        {
            _counter = (_counter + 1) % _stutter;
            if (_counter == 0)
                ++_iterator;
            return *this;
        }
        stutter_iterator operator++(int)
        {
            stutter_iterator temp(*this);
            ++*this;
            return temp;
        }

        const value_type& operator*() const { return *_iterator; }

        const value_type* operator->() const { return _iterator; }

    private:
        const uint32_t _stutter;
        uint32_t _counter;
        It _iterator;
    };
} // namespace

/*
Stuttering subsequence problem:
Find the maximum value of x such that the sequence a_i described by the
following is a subsequence of the master a_i = targetFirst[i % x]
Andranik Mirzaian (1987)
Θ((n+m) log(n/m)) where m = distance(targetFirst, targetLast),
                        n = distance(masterFirst, masterLast)
*/
template <typename It1, typename It2>
uint32_t max_stutter(It1 target_first, It1 target_last, It2 master_first, It2 master_last)
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

/*
Longest ordered (increasing) subsequence
Find the longest sequence n_0, n_2, ..., n_x such that 0 <= n_0 <= n_1 <= ... <=
n_x <= (last - first)
and compare(first[n_(i-1)], first[n_i]) is true for any 0 < i <= x
Θ(n log n)
*/
template <typename It, typename Compare>
std::list<It> longest_ordered_subsequence(It first, It last, Compare comp)
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
        uint32_t num_elements = std::distance(first, last);
        std::vector<uint32_t> predecessor(num_elements);

        typedef std::pair<It, uint32_t> T;

        std::vector<T> it_tracker(num_elements);
        auto compare = [&comp](const T& x, const T& y) { return comp(*x.first, *y.first); };

        tree::red_black_tree<T, decltype(compare)> tree(compare);

        for (uint32_t i = 0; first != last; ++first, ++i) {
            it_tracker[i] = { first, i };
            auto current = tree.insert(it_tracker[i]).first;
            if (current == tree.begin()) {
                predecessor[i] = i;
            } else {
                auto temp(current);
                predecessor[i] = (--temp)->second;
            }

            if (++current != tree.end())
                tree.erase(current);
        }

        std::list<It> result;
        auto tracer = *tree.rbegin();
        while (predecessor[tracer.second] != tracer.second) {
            result.push_front(tracer.first);
            tracer = it_tracker[predecessor[tracer.second]];
        }
        result.push_front(tracer.first);
        return result;
        /*
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
                */
    }
}
template <typename It> std::list<It> longest_ordered_subsequence(It first, It last)
{
    return longest_ordered_subsequence(first, last, std::less<>());
}

/*
Specialized version for integral types
Uses van Emde Boas tree
For n numbers where max - min = M, O(n log log M)
*/
template <typename It,
    typename _Requires
    = std::enable_if_t<std::is_integral_v<typename std::iterator_traits<It>::value_type>, void>>
std::list<It> longest_increasing_integer_subsequence(It first, It last)
{
    if (first == last) {
        return std::list<It>();
    }

    std::pair<It, It> bounds = extrema(first, last);
    uint32_t range = *bounds.second - *bounds.first + 1;
    van_Emde_Boas_map<uint32_t> vEB(range);

    uint32_t num_elements = std::distance(first, last);
    std::vector<It> it_tracker(num_elements);
    std::vector<uint32_t> predecessor(num_elements);

    for (uint32_t i = 0; first != last; ++first, ++i) {
        it_tracker[i] = first;
        auto it = vEB.insert({ *first - *bounds.first, i }).first;
        if (it == vEB.begin()) {
            predecessor[i] = i;
        } else {
            auto temp(it);
            predecessor[i] = (--temp)->second;
        }
        if (++it != vEB.end())
            vEB.erase(it);
    }

    std::list<It> result;
    uint32_t tracer = vEB.rbegin()->second;
    while (predecessor[tracer] != tracer) {
        result.push_front(it_tracker[tracer]);
        tracer = predecessor[tracer];
    }
    result.push_front(it_tracker[tracer]);
    return result;
}

} // namespace Sequence

#endif // SUBSEQUENCE_H
