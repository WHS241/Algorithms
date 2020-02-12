#include <algebra/algebra.h>

std::complex<double> algebra::evaluate_polynomial(
    const std::vector<std::complex<double>>& coefficients, std::complex<double> x)
{
    std::complex<double> result = 0;
    for (auto it(coefficients.rbegin()); it != coefficients.rend(); ++it) {
        result *= x;
        result += *it;
    }
    return result;
}