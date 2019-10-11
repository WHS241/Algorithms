#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <sequence/CompareSort.h>
#include <sequence/Majority.h>
#include <sequence/OrderStatistics.h>
#include <sequence/SequenceComp.h>
#include <sequence/subsequence.h>

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

        auto result = Sequence::extrema(input.begin(), input.end());
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
        auto result = Sequence::selection(input.begin(), input.end(), target);
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

        auto result = Sequence::findSubstring(input, target);
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

        auto result = Sequence::editDistance(src, target);
        std::sort(result.begin(), result.end(), [](auto& x, auto& y) {
            return x.srcIndex < y.srcIndex;
            });

        std::for_each(result.rbegin(), result.rend(), [&src, &target](const Sequence::Instruction& x) {
            if (x.directive == Sequence::Instruction::Delete) {
                src.erase(src.begin() + x.srcIndex);
            }
            else if (x.directive == Sequence::Instruction::Replace) {
                src[x.srcIndex] = target[x.targetIndex];
            }
        });

        std::sort(result.begin(), result.end(), [](auto& x, auto& y) {
            return x.targetIndex < y.targetIndex;
        });
        for(auto& x : result) {
            if(x.directive == Sequence::Instruction::Insert) {
                src.insert(src.begin() + x.targetIndex, 1, target[x.targetIndex]);
            }
        }

        EXPECT_EQ(src, target);
    }
}

TEST_F(AlgorithmTest, MajorityTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> inputDist;
        std::uniform_int_distribution<uint32_t> sizeDist(1, 1000);
        auto majoritySize = sizeDist(engine);
        auto majorityValue = inputDist(engine);
        std::vector<uint32_t> input(2 * majoritySize - 1);

        for (uint32_t j = 0; j < majoritySize; ++j)
            input[j] = majorityValue;
        for (uint32_t j = majoritySize; j < input.size(); ++j)
            input[j] = inputDist(engine);

        // shuffle
        for (uint32_t j = 0; j < input.size(); ++j) {
            std::uniform_int_distribution<uint32_t> shuffler(j, input.size() - 1);
            std::swap(input[j], input[shuffler(engine)]);
        }

        ASSERT_GE(std::count(input.begin(), input.end(), majorityValue), majoritySize);

        EXPECT_EQ(Sequence::findMajority(input.begin(), input.end()), majorityValue);

        if (input.size() != 1) {
            for (uint32_t j = 0; j < input.size(); ++j)
                if (input[j] == majorityValue)
                    input[j] = j;
                else
                    input[j] = input.size();

            EXPECT_THROW(Sequence::findMajority(input.begin(), input.end()), std::invalid_argument);
        }
    }
}

TEST_F(AlgorithmTest, LISTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> inputDist;
        std::uniform_int_distribution<uint32_t> sizeDist(1, 1000);
        std::vector<uint32_t> input(sizeDist(engine));
        std::generate(input.begin(), input.end(), [&inputDist, this]() {return inputDist(engine); });

        auto result = Sequence::longestOrderedSubsequence(input.begin(), input.end());
        auto it2 = result.begin();
        auto it1 = it2++;
        for (; it2 != result.end(); ++it1, ++it2) {
            EXPECT_GT(*it2 - *it1, 0);
            EXPECT_GT(**it2, **it1);
            auto vectIt = *it1;
            for (++vectIt; vectIt != *it2; ++vectIt)
                EXPECT_TRUE(*vectIt >= **it2 || *vectIt <= **it1);
        }

        auto vectIt = *it1;
        for (++vectIt; vectIt != input.end(); ++vectIt)
            EXPECT_LE(*vectIt, **it1);
        
    }
}

TEST_F(AlgorithmTest, LISIteratorTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> inputDist;
        std::uniform_int_distribution<uint32_t> sizeDist(1, 1000);
        auto size = sizeDist(engine);
        std::list<uint32_t> input;
        std::generate_n(std::back_inserter(input), size, [&inputDist, this]() {return inputDist(engine); });

        auto result = Sequence::longestOrderedSubsequence(input.begin(), input.end());
        auto it2 = result.begin();
        auto it1 = it2++;
        for (; it2 != result.end(); ++it1, ++it2) {
            EXPECT_GT(**it2, **it1);
            auto vectIt = *it1;
            for (++vectIt; vectIt != *it2; ++vectIt)
                EXPECT_TRUE(*vectIt >= **it2 || *vectIt <= **it1);
        }

        auto vectIt = *it1;
        for (++vectIt; vectIt != input.end(); ++vectIt)
            EXPECT_LE(*vectIt, **it1);

    }
}