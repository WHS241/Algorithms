#ifndef ALGEBRA_H
#define ALGEBRA_H
#include <complex>
#include <vector>

namespace Algebraic {
    // Evaluate a[0] + a[1]x + a[2]x^2 + a[3]x^3 + ...
    // O(coefficients.size())
    std::complex<double> polynomialEval(const std::vector<std::complex<double>>& coefficients, std::complex<double> x);
}
#endif //ALGEBRA_H