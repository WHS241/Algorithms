#include <algebra/algebra.h>

std::complex<double>
  algebra::evaluate_polynomial(const std::vector<std::complex<double>>& coefficients,
                               std::complex<double> x) {
    std::complex<double> result = 0;
    for (auto it(coefficients.rbegin()); it != coefficients.rend(); ++it) {
        result *= x;
        result += *it;
    }
    return result;
}

// Performs FFT on v (size n) using the 2^i >= n roots of unity (stored in points) and an offset
// (jump = 2^k for some integer k <= i)
// Offset ensures we can use the same points vector in all recursive calls rather than recalculating
static std::vector<std::complex<double>> FFT_helper(const std::vector<std::complex<double>>& v,
                                                    const std::vector<std::complex<double>>& points,
                                                    std::size_t jump) {
    // base case
    if (v.size() == 1) {
        return std::vector<std::complex<double>>(points.size() / jump, v.front());
    }

    // separate polynomial into coefficients for even and odd degree terms
    std::vector<std::complex<double>> even, odd;
    for (auto& n : v) {
        if (even.size() == odd.size())
            even.push_back(n);
        else
            odd.push_back(n);
    }

    // A(x) = A_even(x^2) + x * A_odd(x^2)
    even = FFT_helper(even, points, jump * 2);
    odd = FFT_helper(odd, points, jump * 2);

    // Leverage that points contains 2^i roots of unity (points[k] = -points[2^(i - 1) + k])
    std::vector<std::complex<double>> output(points.size() / jump);
    for (std::size_t i = 0; i < even.size(); ++i) {
        auto p = even[i];
        auto q = odd[i] * points[i * jump];
        output[i] = p + q;
        output[i + even.size()] = p - q;
    }
    return output;
}

std::vector<std::complex<double>> algebra::FFT(const std::vector<std::complex<double>>& v) {
    if (v.empty())
        return v;

    // Determine roots of unity and how many
    std::size_t num_points = 1;
    while (num_points < v.size())
        num_points *= 2;
    std::vector<std::complex<double>> points(num_points);
    for (std::size_t i = 0; i < num_points; ++i) {
        points[i] = std::polar(1., std::numbers::pi * i * -2.0 / num_points);
    }

    return FFT_helper(v, points, 1);
}

std::vector<std::complex<double>> algebra::inverse_FFT(const std::vector<std::complex<double>>& v) {
    if (v.empty())
        return v;

    // FFT essentially multiplies the (column) vector v by a Vandermonde matrix with specific values
    // The nature of this Vandermonde matrix's inverse allows us to
    // invert a FFT with a second FFT, plus some post-processing
    std::vector<std::complex<double>> result = FFT(v);
    std::reverse(result.begin() + 1, result.end());
    for (auto& z : result)
        z /= result.size();
    return result;
}

std::vector<std::complex<double>>
  algebra::polynomial_product(const std::vector<std::complex<double>>& p,
                              const std::vector<std::complex<double>>& q) {
    std::vector<std::complex<double>> p_trans(p), q_trans(q);
    while (!p_trans.empty() && p_trans.back() == std::complex<double>())
        p_trans.pop_back();
    while (!q_trans.empty() && q_trans.back() == std::complex<double>())
        q_trans.pop_back();

    if (p_trans.empty() || q_trans.empty())
        return std::vector<std::complex<double>>();

    // Transform from A(x) = a_0 + a_1 x + ... + a_n x^n to [A(x_0), A(x_1), ..., A(x_n)]
    std::size_t target_degree = p.size() + q.size() - 1;
    p_trans.resize(target_degree);
    q_trans.resize(target_degree);
    p_trans = FFT(p_trans);
    q_trans = FFT(q_trans);

    // With new representation, multiplication becomes much easier
    std::transform(p_trans.begin(), p_trans.end(), q_trans.begin(), p_trans.begin(),
                   std::multiplies<>());

    // Change back to a_0 + a_1 x + ... a_n x^n notation
    std::vector<std::complex<double>> result = inverse_FFT(p_trans);
    result.resize(target_degree);
    return result;
}
