#ifndef ALGEBRA_TEMP_CPP
#define ALGEBRA_TEMP_CPP

#include <algorithm>
#include <iterator>

template <typename It, typename Compare, typename Function>
std::pair<It, It> algebra::maximum_consecutive_sum(
    It first, It last, Compare compare, Function func)
{
    if (first == last)
        return std::make_pair(first, first);

    typedef typename std::iterator_traits<It>::value_type T;
    struct Decider {
        It current;
        It start;
        T sum;
    };

    std::vector<Decider> sub_results(std::distance(first, last));
    T default_value = T();

    sub_results[0].current = first;
    sub_results[0].start = first;
    sub_results[0].sum = *first;
    ++first;

    // The maximum consecutive sum where the final element is at the particular
    // iterator
    for (uint32_t i = 1; i < sub_results.size(); ++i, ++first) {
        sub_results[i].current = first;
        if (compare(default_value, sub_results[i - 1].sum)) {
            sub_results[i].start = sub_results[i - 1].start;
            sub_results[i].sum = func(sub_results[i - 1].sum, *first);
        } else {
            sub_results[i].start = first;
            sub_results[i].sum = *first;
        }
    }

    // find max among the sub-answers
    auto answer = std::max_element(sub_results.begin(), sub_results.end(),
        [&compare](const Decider& x, const Decider& y) { return compare(x.sum, y.sum); });

    return std::make_pair(answer->start, ++(answer->current));
}

#endif // !ALGEBRA_TEMP_CPP
