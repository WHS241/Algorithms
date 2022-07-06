#ifndef FUNCTION_H
#define FUNCTION_H

#include <functional>
#include <type_traits>
#include <utility>

namespace misc {
// Find ordered pair (m, n) such that for all i >= m, f^i(x) = f^{i + n}(x)
// where x is the input value and m is minimum
// O(m + n) runtime, O(1) space
template<typename T, typename Function, typename Equality>
std::pair<std::size_t, std::size_t> find_cycle(T start, Function f, Equality equal) {
    static_assert(std::is_invocable_r_v<T, Function, const T&>);
    static_assert(std::is_invocable_r_v<bool, Equality, const T&, const T&>);

    std::pair<std::size_t, std::size_t> result;

    // k s.t. f^k(x) = f^{2k}(x). k does not need to be tracked
    T turtle = f(start), hare = f(turtle);
    while (!equal(turtle, hare)) {
        turtle = f(turtle);
        hare = f(f(hare));
    }

    // n divides k, so for all i >= m, f^i(x) = f^{2k + i}(x)
    // find m
    turtle = start;
    result.first = 0;
    while (!equal(turtle, hare)) {
        turtle = f(turtle);
        hare = f(hare);
        ++result.first;
    }

    // find n
    hare = f(turtle);
    result.second = 1;
    while (!equal(turtle, hare)) {
        hare = f(hare);
        ++result.second;
    }

    return result;
}
template<typename T, typename Function>
std::pair<std::size_t, std::size_t> find_cycle(T start, Function f) {
    return find_cycle(start, f, std::equal_to<T>());
}
} // namespace misc

#endif // FUNCTION_H
