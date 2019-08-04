#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <sequence/Huffman.h>
#include <sequence/OrderStatistics.h>

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

// Tested manually
TEST_F(AlgorithmTest, HuffmanTest) {
    std::string input("this is an example of a huffman tree");
    std::unique_ptr<BinaryTree<char>::BinaryNode> result(Huffman::generateTree(input));
    result.reset();
}