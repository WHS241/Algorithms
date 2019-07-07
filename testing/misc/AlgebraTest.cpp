#include <cmath>
#include <complex>
#include <random>

#include <gtest/gtest.h>

#include <misc/algebra.h>

class AlgebraTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(AlgebraTest, PolynomialPowerTest) {
    std::uniform_int_distribution<int> constantDist(-9, 9);
    std::uniform_int_distribution<uint32_t> powerDist(0, 8);
    std::uniform_real_distribution<double> dist(-10, 10);

    for (uint32_t i = 0; i < 10; ++i) {
        int constant = constantDist(engine);
        uint32_t power = powerDist(engine);
        std::vector<int> coeffs(power + 1);
        coeffs[0] = 1;
        for (uint32_t i = 1; i <= power; ++i) {
            coeffs[i] = (coeffs[i - 1] * (power - i + 1)) / i;
        }

        std::vector<std::complex<double>> coeffInput(power + 1);
        int modifier = 1;
        for (uint32_t i = 0; i <= power; ++i) {
            coeffInput[power - i] = coeffs[i] * modifier;
            modifier *= constant;
        }

        double polyInput = dist(engine);

        double result = Algebraic::polynomialEval(coeffInput, polyInput).real();
        EXPECT_LE(std::abs(result - std::pow(polyInput + constant, power)), 1e-5);
    }
}

TEST_F(AlgebraTest, QuadraticZeroTest) {
    std::uniform_int_distribution<int> coeffDist(-1000, 1000);
    for (uint32_t i = 0; i < 20; ++i) {
        std::vector<std::complex<double>> coeff(3);
        for (uint32_t j = 0; j < 3; ++j)
            coeff[j] = coeffDist(engine);

        std::complex<double> result;

        if (std::abs(coeff[2]) < std::numeric_limits<double>::epsilon()) {
            result = Algebraic::polynomialEval(coeff, -coeff[0] / coeff[1]);
        }
        else {
            std::complex<double> vertex = -coeff[1] / (coeff[2] * 2.);
            std::complex<double> width = std::sqrt(coeff[1] * coeff[1] - 4. * coeff[0] * coeff[2])/(coeff[2] * 2.);
            EXPECT_LE(std::abs(Algebraic::polynomialEval(coeff, vertex - width)), 1e-5);
            result = Algebraic::polynomialEval(coeff, vertex + width);
        }

        EXPECT_LE(std::abs(result), 1e-5);
    }
}