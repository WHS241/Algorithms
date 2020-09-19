#ifndef UTIL_KEY_EQ_WRAPPER_H
#define UTIL_KEY_EQ_WRAPPER_H
#include <functional>

namespace util {
    template <typename T1, typename T2, typename K1 = std::equal_to<T1>, typename K2 = std::equal_to<T2>>
    struct key_eq_wrapper {
        K1 eq1;
        K2 eq2;
        bool operator()(const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
            return eq1(lhs.first, rhs.first) && eq2(lhs.second, rhs.second);
        }
    };

    template <typename T, typename K = std::equal_to<T>>
    struct key_eq_unordered {
        K eq;
        bool operator()(const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const {
            return (eq(lhs.first, rhs.first) && eq(lhs.second, rhs.second)) || (eq(lhs.first, rhs.second) && eq(lhs.second, rhs.first));
        }
    };
}

#endif // UTIL_KEY_EQ_WRAPPER_H
