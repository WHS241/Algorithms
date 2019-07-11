#ifndef ALGEBRA_H
#define ALGEBRA_H
#include <complex>
#include <functional>
#include <utility>
#include <vector>

namespace Algebraic {
    // Evaluate a[0] + a[1]x + a[2]x^2 + a[3]x^3 + ...
    // O(coefficients.size())
    std::complex<double> polynomialEval(const std::vector<std::complex<double>>& coefficients, std::complex<double> x);

    // Evaluate the consecutive series of iterators such that sum of *first-*last is a maximum
    // Jay Kadane: O(distance(first, last))
    template<typename It, typename Compare = std::less<>>
    std::pair<It, It> maximumConsecutiveSum(It first, It last, Compare compare = Compare());
}

#include "../../src/misc/algebraTemp.cpp"

#endif //ALGEBRA_H