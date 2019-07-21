#ifndef SUBSEQUENCE_H
#define SUBSEQUENCE_H

#include <iterator>

namespace Subsequence {
    /*
    Verify if [targetFirst, targetLast) is a subsequence of [masterFirst, masterLast)
    Conditions: It1::value_type is equivalent to It2::value_type
    Θ(distance(targetFirst, targetLast) + distance(masterFirst, masterLast))
    */
    template<typename It1, typename It2>
    bool isSubsequence(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast);

    /*
    Stuttering subsequence problem:
    Find the maximum value of x such that the sequence a_i described by the following is a subsequence of the master
    a_i = targetFirst[i % x]

    Θ((n+m)log(n/m)) where m = distance(targetFirst, targetLast), n = distance(masterFirst, masterLast)
    */
    template<typename It1, typename It2>
    uint32_t maxStutter(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast);

    namespace {
        /*
        Iterator adaptor:
        Simulate stuttering without having to create a new container
        */
        template<typename It>
        class StutterIterator {
        public:
            typedef typename std::iterator_traits<It>::value_type value_type;

            StutterIterator(const It it, uint32_t stutterAmount) : iterator(it), SETTING(stutterAmount), counter(0) {};

            bool operator==(const StutterIterator& rhs) {
                return SETTING == rhs.SETTING && counter == rhs.counter && iterator == rhs.iterator;
            }
            bool operator!=(const StutterIterator& rhs) {
                return !operator==(rhs);
            }
            StutterIterator& operator++() {
                counter = (counter + 1) % SETTING;
                if (counter == 0)
                    ++iterator;
                return *this;
            }
            StutterIterator operator++(int) {
                StutterIterator temp(*this);
                ++*this;
                return temp;
            }

            const value_type& operator*() const {
                return *iterator;
            }

            const value_type* operator->() const {
                return iterator;
            }

        private:
            const uint32_t SETTING;
            uint32_t counter;
            It iterator;
        };
    }
}

#include "../../src/sequence/subsequence.cpp"

#endif // SUBSEQUENCE_H