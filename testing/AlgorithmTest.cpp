#include <algorithm>
#include <array>
#include <climits>
#include <random>
#include <string>

#include <gtest/gtest.h>

#include <sequence/BinarySearch.h>
#include <sequence/subsequence.h>

class BinarySearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(BinarySearchTest, BasicTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> dist;
        std::array<uint32_t, 1000> input;
        std::generate(input.begin(), input.end(), [&dist, this]() {return dist(engine); });
        std::sort(input.begin(), input.end());

        for (uint32_t j = 0; j < 100; ++j) {
            uint32_t target = dist(engine);
            auto found = BinarySearch::find(input.begin(), input.end(), target);
            if (found != input.end())
                EXPECT_EQ(target, *found);
        }

        std::uniform_int_distribution<uint32_t> range(0, input.size() - 1);
        for (uint32_t j = 0; j < 100; ++j) {
            uint32_t index = range(engine);
            auto found = BinarySearch::find(input.begin(), input.end(), input[index]);
            EXPECT_NE(found, input.end());
            EXPECT_EQ(*found, input[index]);
        }
    }
}

TEST_F(BinarySearchTest, CyclicMinTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> dist;
        std::array<uint32_t, 1000> values;
        std::generate(values.begin(), values.end(), [&dist, this]() {return dist(engine); });
        std::sort(values.begin(), values.end());

        std::uniform_int_distribution<uint32_t> range(0, values.size() - 1);
        uint32_t shift = range(engine);
        std::array<uint32_t, 1000> input;
        std::copy(values.begin(), values.end() - shift, input.begin() + shift);
        std::copy(values.end() - shift, values.end(), input.begin());

        auto minIt = BinarySearch::cyclicFindMin(input.begin(), input.end());
        EXPECT_EQ(minIt - input.begin(), shift);
    }
}

TEST_F(BinarySearchTest, SpecialIndexTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::array<int, 1000> input;

        std::uniform_int_distribution<uint32_t> range(1, input.size() - 1);
        uint32_t index = range(engine);
        input[index] = index;

        std::uniform_int_distribution<int> dist(INT_MIN, index - 1);
        std::generate_n(input.begin(), index - 1, [&dist, this]() { return dist(engine); });
        std::sort(input.begin(), input.begin() + index);
        for (uint32_t j = index; j > 0; --j)
            if (input[j - 1] >= input[j])
                input[j - 1] = input[j] - 1;

        dist = std::uniform_int_distribution<int>(index + 1);
        std::generate(input.begin() + index + 1, input.end(), [&dist, this]() { return dist(engine); });
        std::sort(input.begin() + index + 1, input.end());
        for (uint32_t j = index + 1; j < input.size(); ++j)
            if (input[j] <= input[j - 1])
                input[j] = input[j - 1] + 1;

        auto resultIndex = BinarySearch::specialIndex(input.begin(), input.end());
        if (resultIndex != index) {
            EXPECT_EQ(resultIndex, input[resultIndex]);
        }
    }
}

class SubsequenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(SubsequenceTest, StutterTest) {
    for (uint32_t j = 0; j < 100; ++j) {
        std::string master(1000, '0');
        std::uniform_int_distribution<uint32_t> dist(0, 25);
        std::string target(10, '\0');
        std::generate_n(target.begin(), 10, [&dist, this]() {return 'a' + dist(engine); });

        std::uniform_int_distribution<uint32_t> stutterGen(0, 100);
        uint32_t stutter = stutterGen(engine);
        uint32_t stutterLength = stutter * target.size();

        uint32_t index = 0;
        for (uint32_t i = 0; i < stutterLength; ++i) {
            dist = std::uniform_int_distribution<uint32_t>(index, master.size() - stutterLength + i);
            index = dist(engine);
            master[index++] = target[i / stutter];
        }

        EXPECT_TRUE(Subsequence::isSubsequence(target.begin(), target.end(), master.begin(), master.end()) || (stutter == 0));
        EXPECT_GE(Subsequence::maxStutter(target.begin(), target.end(), master.begin(), master.end()), stutter);
    }
}