#ifndef UTIL_IT_HASH_H
#define UTIL_IT_HASH_H

#include <iterator>

namespace util {
template <typename It>
struct it_hash {
    std::hash<typename std::iterator_traits<It>::pointer> pointer_hash;

    std::size_t operator()(It it) const {
        return pointer_hash(it.operator->());
    }
};
}

#endif // UTIL_IT_HASH_H