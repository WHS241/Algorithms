#include <induction/algebra.h>

std::complex<double> Algebraic::polynomialEval(const std::vector<std::complex<double>>& coefficients, std::complex<double> x) {
    std::complex<double> result = 0;
    for (auto it(coefficients.rbegin()); it != coefficients.rend(); ++it) {
        result *= x;
        result += *it;
    }
    return result;
}