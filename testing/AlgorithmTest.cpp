
#include <unordered_set>

#include <gtest/gtest.h>

#include <sequence/subsequence.h>

#include "generator.h"

class AlgorithmTest : public ::testing::Test {
protected:
    std::mt19937_64 engine;

    void SetUp() override
    {
        std::random_device base;
        engine = std::mt19937_64(base());
    }
};

TEST_F(AlgorithmTest, LISTest)
{
    for (uint32_t j = 0; j < 100; ++j) {
        // strictly increasing values
        std::vector<uint32_t> input(100);

        uint32_t last = -1;
        for (uint32_t i = 0; i < 100; ++i) {
            std::uniform_int_distribution<uint32_t> dist(last + 1, 9901 + i);
            input[i] = last = dist(engine);
        }

        // shuffle
        for (uint32_t i = 0; i < 100; ++i) {
            std::uniform_int_distribution<uint32_t> index_dist(i, 99);
            std::swap(input[i], input[index_dist(engine)]);
        }

        auto result1 = sequence::longest_ordered_subsequence(input.begin(), input.end());
        auto int_res = sequence::longest_increasing_integer_subsequence(input.begin(), input.end());

        std::list<uint32_t> list_form;
        std::copy(input.begin(), input.end(), std::back_inserter(list_form));

        auto list_res = sequence::longest_ordered_subsequence(list_form.begin(), list_form.end());

        EXPECT_EQ(result1.size(), int_res.size());
        EXPECT_EQ(list_res.size(), int_res.size());

        for (auto it = ++result1.begin(); it != result1.end(); ++it) {
            auto temp(it);
            --temp;
            EXPECT_LT(**temp, **it);
        }
        for (auto it = ++int_res.begin(); it != int_res.end(); ++it) {
            auto temp(it);
            --temp;
            EXPECT_LT(**temp, **it);
        }
        for (auto it = ++list_res.begin(); it != list_res.end(); ++it) {
            auto temp(it);
            --temp;
            EXPECT_LT(**temp, **it);
        }
    }
}