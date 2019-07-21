#ifndef ALGEBRA_H
#define ALGEBRA_H
#include <complex>
#include <functional>
#include <utility>
#include <vector>

namespace Algebraic {
    // Evaluate a[0] + a[1]x + a[2]x^2 + a[3]x^3 + ...
    // Θ(coefficients.size()) with Horner's method
    std::complex<double> polynomialEval(const std::vector<std::complex<double>>& coefficients, std::complex<double> x);

    /* 
    Evaluate the consecutive series of iterators such that
    func(func(...func(*first, *(first + 1)), *(first + 2), ...), *(last -1 )) is a maximum
    Jay Kadane: Θ(distance(first, last))

    Requirements:
    compare is an order relation (antisymmetric, transitive) between values of type It::value_type
    It::value_type has a default constructor or is a primitive (henceforth called "zero")
    func obeys the following properties for any a, b, x:
    1. compare(a, b) is true iff compare(func(a, x), func(b, x)) is true
    2. compare(zero, x) is true iff compare(a, func(a, x)) is true
    */
    template<typename It, typename Compare = std::less<>, typename Function = std::plus<>>
    std::pair<It, It> maximumConsecutiveSum(It first, It last, Compare compare = Compare(), Function func = Function());
}

#include <src/induction/algebraTemp.cpp>

#endif //ALGEBRA_H