#ifndef ALGEBRA_TEMP_CPP
#define ALGEBRA_TEMP_CPP

#include <algorithm>
#include <iterator>

template<typename It, typename Compare = std::less<>>
std::pair<It, It> Algebraic::maximumConsecutiveSum(It first, It last, Compare compare) {
    if (first == last)
        return std::make_pair(first, first);

    typedef typename std::iterator_traits<It>::value_type T;
    struct Decider {
        It current;
        It start;
        T sum;
    };
    
    typename std::iterator_traits<It>::difference_type size = std::distance(first, last);
    std::vector<Decider> subResults(size);
    T defaultValue = T();

    subResults[0].current = first;
    subResults[0].start = first;
    subResults[0].sum = *first;
    ++first;

    // The maximum consecutive sum where the final element is at the particular iterator
    for (uint32_t i = 1; i < subResults.size(); ++i, ++first) {
        subResults[i].current = first;
        if (compare(defaultValue, subResults[i - 1].sum)) {
            subResults[i].start = subResults[i - 1].start;
            subResults[i].sum = subResults[i - 1].sum + *first;
        }
        else {
            subResults[i].start = first;
            subResults[i].sum = *first;
        }
    }

    // find max among the sub-answers; would an empty array yield a greater sum?
    auto answer = std::max_element(subResults.begin(), subResults.end(), [&compare](const Decider& x, const Decider& y) {
        return compare(x.sum, y.sum);
        });

    if (compare(answer->sum, defaultValue))
        return std::make_pair(first, first);
    else
        return std::make_pair(answer->start, ++answer->current);
    
}

#endif // !ALGEBRA_TEMP_CPP
