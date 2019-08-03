#include <algorithm>
#include <array>
#include <climits>
#include <random>
#include <string>

#include <gtest/gtest.h>

#include <sequence/CompareSort.h>
#include <sequence/NonCompareSort.h>

class SortTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(SortTest, RadixSort) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        std::vector<uint32_t> input(sizeDist(engine));
        std::uniform_int_distribution<uint32_t> dist(0, 1000000);
        std::generate(input.begin(), input.end(), [&dist, this]() { return dist(engine); });

        std::uniform_int_distribution<uint32_t> baseDist(2, 16);
        NonCompareSort::radixSort(input.begin(), input.end(), baseDist(engine));

        for (auto it = input.begin() + 1; it != input.end(); ++it)
            EXPECT_LE(*(it - 1), *it);    
    }
}

TEST_F(SortTest, MergeSort) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        std::vector<uint32_t> input(sizeDist(engine));
        std::uniform_int_distribution<uint32_t> dist;
        std::generate(input.begin(), input.end(), [&dist, this]() { return dist(engine); });
        
        CompareSort::mergesort(input.begin(), input.end());

        for (auto it = input.begin() + 1; it != input.end(); ++it)
            EXPECT_LE(*(it - 1), *it);
    }
}

TEST_F(SortTest, QuickSort) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        std::vector<uint32_t> input(sizeDist(engine));
        std::uniform_int_distribution<uint32_t> dist;
        std::generate(input.begin(), input.end(), [&dist, this]() { return dist(engine); });

        CompareSort::quicksort(input.begin(), input.end());

        for (auto it = input.begin() + 1; it != input.end(); ++it)
            EXPECT_LE(*(it - 1), *it);
    }
}

TEST_F(SortTest, RandomAccessHeapSort) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        std::vector<uint32_t> input(sizeDist(engine));
        std::uniform_int_distribution<uint32_t> dist;
        std::generate(input.begin(), input.end(), [&dist, this]() { return dist(engine); });

        CompareSort::heapsort(input.begin(), input.end());

        for (auto it = input.begin() + 1; it != input.end(); ++it)
            EXPECT_LE(*(it - 1), *it);
    }
}

TEST_F(SortTest, LimitedAccessHeapSort) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        auto size = sizeDist(engine);
        std::list<uint32_t> input;
        std::uniform_int_distribution<uint32_t> dist;
        for (uint32_t j = 0; j < size; ++j)
            input.push_back(dist(engine));

        CompareSort::heapsort(input.begin(), input.end());

        for (auto it = ++input.begin(); it != input.end(); ++it) {
            auto temp = it;
            --temp;
            EXPECT_LE(*temp, *it);
        }
    }
}