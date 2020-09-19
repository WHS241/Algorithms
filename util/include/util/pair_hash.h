#ifndef UTIL_PAIR_HASH_H
#define UTIL_PAIR_HASH_H
#include <functional>
#include <utility>

namespace util {
    std::size_t asym_combine_hash(std::size_t hash_1, std::size_t hash_2) {
        return hash_1 ^ (hash_2 + 0x1de3d42a + (hash_1 << 6) + (hash_1 >> 2)); // Using fine-structure constant
    }

    template <typename T1, typename T2, typename H1 = std::hash<T1>, typename H2 = std::hash<T2>>
    struct pair_hash {
        H1 hash_1;
        H2 hash_2;
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            return asym_combine_hash(hash_1(p.first), hash_2(p.second));
        }
    };

    template <typename T, typename H = std::hash<T>>
    struct pair_hash_unordered {
        H hash;
        std::size_t operator()(const std::pair<T, T>& p) const {
            return hash(p.first) + hash(p.second);
        }
    };
}
            

#endif // UTIL_PAIR_HASH_H
