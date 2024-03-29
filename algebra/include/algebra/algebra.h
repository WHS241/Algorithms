#ifndef ALGEBRA_H
#define ALGEBRA_H
#include <complex>
#include <functional>
#include <utility>
#include <vector>

namespace algebra {
// Evaluate a[0] + a[1]x + a[2]x^2 + a[3]x^3 + ...
// Θ(coefficients.size()) with Horner's method
std::complex<double> evaluate_polynomial(const std::vector<std::complex<double>>& coefficients,
                                         std::complex<double> x);

// Evaluate the Discrete Fourier Transform for a given sequence of values
// Transforms polynomial A(x) = a[0] + a[1]x + a[2]x^2 + ... + a[n - 1]x^(n - 1)
// into vector (c_0, c_1, ..., c_(2^k - 1))
// where c_i = A(e^(-2 * pi * i / 2^k)) and k is the smallest number such that 2^k >= n
//
// Θ(n log n) using Gauss/Cooley-Tukey algorithm
std::vector<std::complex<double>> FFT(const std::vector<std::complex<double>>& v);
std::vector<std::complex<double>> inverse_FFT(const std::vector<std::complex<double>>& v);

// Calculate the product of (p[0] + p[1]x + ...)(q[0] + q[1]x + ...)
// Θ(n log n) using FFT/inverse FFT
std::vector<std::complex<double>> polynomial_product(const std::vector<std::complex<double>>& p,
                                                     const std::vector<std::complex<double>>& q);

/*
Evaluate the consecutive series of iterators such that
func(func(...func(*first, *(first + 1)), *(first + 2), ...), *(last -1 )) is a
maximum
Jay Kadane: Θ(distance(first, last))

Requirements:
compare is an order relation (antisymmetric, transitive) between values of type
It::value_type It::value_type has a default constructor or is a primitive
(henceforth called "zero") func obeys the following properties for any a, b, x:
1. compare(a, b) is true iff compare(func(a, x), func(b, x)) is true
2. compare(zero, x) is true iff compare(a, func(a, x)) is true
*/
template<typename It, typename Compare = std::less<>, typename Function = std::plus<>>
std::pair<It, It> maximum_consecutive_sum(It first, It last, Compare compare = Compare(),
                                          Function func = Function()) {
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
    auto answer = std::max_element(
      sub_results.begin(), sub_results.end(),
      [&compare](const Decider& x, const Decider& y) { return compare(x.sum, y.sum); });

    return std::make_pair(answer->start, ++(answer->current));
}
} // namespace algebra

#endif // ALGEBRA_H
