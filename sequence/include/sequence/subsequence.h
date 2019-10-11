#ifndef SUBSEQUENCE_H
#define SUBSEQUENCE_H

#include <iterator>

namespace sequence {
/*
Verify if [targetFirst, targetLast) is a subsequence of [masterFirst,
masterLast) Conditions: It1::value_type is equivalent to It2::value_type
Θ(distance(targetFirst, targetLast) + distance(masterFirst, masterLast))
*/
template <typename It1, typename It2>
bool isSubsequence(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast);

/*
Stuttering subsequence problem:
Find the maximum value of x such that the sequence a_i described by the
following is a subsequence of the master a_i = targetFirst[i % x] Andranik
Mirzaian (1987): Θ((n+m)log(n/m)) where m = distance(targetFirst, targetLast), n
= distance(masterFirst, masterLast)
*/
template <typename It1, typename It2>
uint32_t maxStutter(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast);

/*
Longest ordered (increasing) subsequence
Find the longest sequence n_0, n_2, ..., n_x such that 0 <= n_0 <= n_1 <= ... <=
n_x <= (last - first) and compare(first[n_(i-1)], first[n_i]) is true for any 0
< i <= x O(n log n)
*/
template <typename It, typename Compare = std::less<>>
std::list<It> longest_ordered_subsequence(It first, It last, Compare comp = Compare());

namespace {
    /*
     Iterator adaptor:
     Simulate stuttering without having to create a new container
     */
    template <typename It> class StutterIterator {
    public:
        typedef typename std::iterator_traits<It>::value_type value_type;

        StutterIterator(const It it, uint32_t stutterAmount)
            : iterator(it)
            , SETTING(stutterAmount)
            , counter(0) {};

        bool operator==(const StutterIterator& rhs)
        {
            return SETTING == rhs.SETTING && counter == rhs.counter && iterator == rhs.iterator;
        }
        bool operator!=(const StutterIterator& rhs) { return !operator==(rhs); }
        StutterIterator& operator++()
        {
            counter = (counter + 1) % SETTING;
            if (counter == 0)
                ++iterator;
            return *this;
        }
        StutterIterator operator++(int)
        {
            StutterIterator temp(*this);
            ++*this;
            return temp;
        }

        const value_type& operator*() const { return *iterator; }

        const value_type* operator->() const { return iterator; }

    private:
        const uint32_t SETTING;
        uint32_t counter;
        It iterator;
    };
} // namespace
} // namespace Sequence

#include "../../src/sequence/subsequence.cpp"

#endif // SUBSEQUENCE_H