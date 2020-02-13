#ifndef SUBSEQUENCE_H
#define SUBSEQUENCE_H

#include <iterator>

namespace sequence {
/*
Verify if [targetFirst, targetLast) is a subsequence of [masterFirst,
masterLast)
Conditions: It1::value_type is equivalent to It2::value_type
Θ(distance(targetFirst, targetLast) + distance(masterFirst, masterLast))
*/
template <typename It1, typename It2>
bool is_subsequence(It1 target_first, It1 target_last, It2 master_first, It2 master_last);

/*
Stuttering subsequence problem:
Find the maximum value of x such that the sequence a_i described by the
following is a subsequence of the master a_i = targetFirst[i % x]
Andranik Mirzaian (1987)
Θ((n+m) log(n/m)) where m = distance(targetFirst, targetLast),
                        n = distance(masterFirst, masterLast)
*/
template <typename It1, typename It2>
uint32_t max_stutter(It1 target_first, It1 target_last, It2 master_first, It2 master_last);

/*
Longest ordered (increasing) subsequence
Find the longest sequence n_0, n_2, ..., n_x such that 0 <= n_0 <= n_1 <= ... <=
n_x <= (last - first)
and compare(first[n_(i-1)], first[n_i]) is true for any 0 < i <= x
Θ(n log n)
*/
template <typename It, typename Compare = std::less<>>
std::list<It> longest_ordered_subsequence(It first, It last, Compare comp = Compare());

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
} // namespace Sequence

#include "../../src/sequence/subsequence.cpp"

#endif // SUBSEQUENCE_H