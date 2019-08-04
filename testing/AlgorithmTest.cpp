#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <sequence/CompareSort.h>
#include <sequence/OrderStatistics.h>
#include <sequence/SequenceComp.h>

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(AlgorithmTest, ExtremaTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 10000);
        std::uniform_int_distribution<uint32_t> inputDist;
        std::vector<uint32_t> input(sizeDist(engine));
        std::generate(input.begin(), input.end(), [&inputDist, this]() {return inputDist(engine); });

        auto result = OrderStatistics::extrema(input.begin(), input.end());
        EXPECT_EQ(result, std::minmax_element(input.begin(), input.end()));
    }
}

TEST_F(AlgorithmTest, SelectionTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 1000);
        std::uniform_int_distribution<uint32_t> inputDist;
        std::vector<uint32_t> input(sizeDist(engine));
        std::generate(input.begin(), input.end(), [&inputDist, this]() {return inputDist(engine); });

        std::uniform_int_distribution<uint32_t> targetDist(0, input.size() - 1);
        auto target = targetDist(engine);
        auto result = OrderStatistics::selection(input.begin(), input.end(), target);
        EXPECT_EQ(target, std::count_if(input.begin(), input.end(), [result](uint32_t x) {return x < result; }));
        EXPECT_NE(input.end(), std::find(input.begin(), input.end(), result));
    }
}

TEST_F(AlgorithmTest, SubstringTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::bernoulli_distribution selector;
        std::uniform_int_distribution<uint32_t> sizeDist(1, 1000);
        std::string input(sizeDist(engine), 'x');
        for (auto& c : input)
            if (selector(engine))
                c = 'y';
        
        std::uniform_int_distribution<uint32_t> targetChoice(0, input.size() - 1);
        uint32_t beginIndex = targetChoice(engine);
        targetChoice = std::uniform_int_distribution<uint32_t>(beginIndex, input.size());
        std::string target(input.begin() + beginIndex, input.begin() + targetChoice(engine));

        auto result = SequenceComp::findSubstring(input, target);
        ASSERT_NE(result, input.end());
        EXPECT_TRUE(std::equal(target.begin(), target.end(), result));
    }
}

TEST_F(AlgorithmTest, DistanceTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> selector(0, 2);
        std::uniform_int_distribution<uint32_t> sizeDist(1, 300);
        std::string src(sizeDist(engine), 'x'), target(sizeDist(engine), 'x');
        for (auto& c : src)
            c += selector(engine);
        for (auto& c : target)
            c += selector(engine);

        auto result = SequenceComp::editDistance(src, target);
        std::sort(result.begin(), result.end(), [](auto& x, auto& y) {
            return x.srcIndex < y.srcIndex;
            });

        std::for_each(result.rbegin(), result.rend(), [&src, &target](const SequenceComp::Instruction& x) {
            if (x.directive == SequenceComp::Instruction::Delete) {
                src.erase(src.begin() + x.srcIndex);
            }
            else if (x.directive == SequenceComp::Instruction::Replace) {
                src[x.srcIndex] = target[x.targetIndex];
            }
        });

        std::sort(result.begin(), result.end(), [](auto& x, auto& y) {
            return x.targetIndex < y.targetIndex;
        });
        for(auto& x : result) {
            if(x.directive == SequenceComp::Instruction::Insert) {
                src.insert(src.begin() + x.targetIndex, 1, target[x.targetIndex]);
            }
        }

        EXPECT_EQ(src, target);
    }
}