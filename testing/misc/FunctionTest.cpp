#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <induction/function.h>

class FunctionTest : public ::testing::Test {
protected :
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(FunctionTest, BijectionTest) {
    std::uniform_int_distribution<uint32_t> sizeDist(0, 100);
    std::uniform_int_distribution<uint32_t> valDist;
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<uint32_t> domain(sizeDist(engine));
        std::generate(domain.begin(), domain.end(), [this, &valDist]() { return valDist(engine); });

        std::uniform_int_distribution<uint32_t> limit(0, domain.size());
        uint32_t minimumBijection = limit(engine);

        // shuffle domain
        std::vector<uint32_t> shuffledDomain(domain);
        for (uint32_t j = 0; j < domain.size(); ++j) {
            std::uniform_int_distribution<uint32_t> shuffler(j, domain.size() - 1);
            std::swap(shuffledDomain[j], shuffledDomain[shuffler(engine)]);
        }

        std::unordered_map<uint32_t, uint32_t> function;
        for (uint32_t j = 0; j < minimumBijection; ++j) {
            function[shuffledDomain[j]] = shuffledDomain[(j + 1) % minimumBijection]; // loop back
        }

        std::uniform_int_distribution<uint32_t> setter(0, shuffledDomain.size() - 1);
        for (uint32_t j = minimumBijection; j < domain.size(); ++j) {
            function[shuffledDomain[j]] = shuffledDomain[setter(engine)];
        }

        std::unordered_set<uint32_t> result = Function::subsetAutomorphism(function);

        for (uint32_t j = 0; j < minimumBijection; ++j) {
            auto found = result.find(shuffledDomain[j]);
            ASSERT_NE(found, result.end());
            result.erase(found);
        }

        while (!result.empty()) {
            uint32_t start = *result.begin();
            result.erase(result.begin());
            
            for (uint32_t current = function[start]; current != start; current = function[current]) {
                auto found = result.find(current);
                ASSERT_NE(found, result.end());
                result.erase(found);
            }
        }
    }
}