#ifndef SEQUENCE_MAJORITY_H
#define SEQUENCE_MAJORITY_H
#include <iterator>
#include <list>
#include <stdexcept>

namespace sequence {
template<typename It>
typename std::iterator_traits<It>::value_type find_majority(It first, It last) {
    if (first == last)
        throw std::invalid_argument("Empty container");

    std::list<typename std::iterator_traits<It>::value_type> values(first, last);

    // removing two different elements preserves majorities (though new one may
    // pop up) use this to eliminate all but one candidate
    for (auto temp = ++values.begin(); temp != values.end();) {
        if (*temp != *values.begin()) {
            values.erase(values.begin());
            temp = values.erase(temp);

            if (values.empty()) {
                throw std::invalid_argument("No majority");
            }
        } else {
            ++temp;
        }
    }

    // verify majority
    auto size = std::distance(first, last);
    uint32_t count = 0;
    for (; count < (size + 1) / 2 && first != last; ++first)
        if (values.front() == *first)
            ++count;

    if (count < (size + 1) / 2)
        throw std::invalid_argument("No majority");
    return values.front();
}
} // namespace sequence

#endif // SEQUENCE_MAJORITY_H