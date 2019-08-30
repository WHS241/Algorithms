#include <algorithm>

#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <graph/path.h>

#include <structures/heap>

#include "AllocationTracker.h"
#include "generator.h"

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(AlgorithmTest, BinaryHeapDestructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        NodeBinaryHeap<AllocationTracker> heap(values.begin(), values.end());
        EXPECT_EQ(heap.size(), values.size());
        EXPECT_EQ(AllocationTracker::count(), values.size() * 2);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, BinaryHeapCopyTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        NodeBinaryHeap<AllocationTracker> heap(values.begin(), values.end());
        NodeBinaryHeap<AllocationTracker> copy(heap);
        EXPECT_EQ(AllocationTracker::count(), values.size() * 3);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, BinaryHeapConstructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        NodeBinaryHeap<double> heap(input.begin(), input.end());
        EXPECT_EQ(heap.getRoot(), *std::min_element(input.begin(), input.end()));
    }
}

TEST_F(AlgorithmTest, BinaryHeapAddTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        NodeBinaryHeap<double> heap;
        double minimum = input[0];
        uint32_t j = 0;
        for (auto value : input) {
            if (value < minimum)
                minimum = value;
            heap.add(value);

            EXPECT_EQ(heap.size(), ++j);
            EXPECT_EQ(minimum, heap.getRoot());
        }
    }
}

TEST_F(AlgorithmTest, BinaryHeapRemoveTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        NodeBinaryHeap<double> heap(input.begin(), input.end());
        std::vector<double> output(input.size());
        std::generate(output.begin(), output.end(), [&heap]() { return heap.removeRoot(); });
        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, BinaryHeapExceptionTest)
{
    NodeBinaryHeap<int> heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_THROW(heap.getRoot(), std::underflow_error);
    EXPECT_THROW(heap.removeRoot(), std::underflow_error);
}

TEST_F(AlgorithmTest, BinaryHeapMergeTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input1 = generateData(100, 1000, engine);
        std::vector<double> input2 = generateData(100, 1000, engine);
        NodeBinaryHeap<double> heap1(input1.begin(), input1.end());
        NodeBinaryHeap<double> heap2(input2.begin(), input2.end());
        std::list<double> concat(input1.begin(), input1.end());
        std::copy(input2.begin(), input2.end(), std::back_inserter(concat));
        concat.sort();
        std::vector<double> output(concat.size());
        heap1.merge(heap2);

        std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        EXPECT_TRUE(std::equal(concat.begin(), concat.end(), output.begin()));
    }

    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        NodeBinaryHeap<double> heap1(input.begin(), input.end());
        NodeBinaryHeap<double> emptyHeap;
        std::vector<double> output(input.size());
        if (i % 2) {
            heap1.merge(emptyHeap);
            std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        } else {
            emptyHeap.merge(heap1);
            std::generate(
                output.begin(), output.end(), [&emptyHeap]() { return emptyHeap.removeRoot(); });
        }

        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, BinomialHeapDestructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        BinomialHeap<AllocationTracker> heap(values.begin(), values.end());
        EXPECT_EQ(heap.size(), values.size());
        EXPECT_EQ(AllocationTracker::count(), values.size() * 2);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, BinomialHeapCopyTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        BinomialHeap<AllocationTracker> heap(values.begin(), values.end());
        BinomialHeap<AllocationTracker> copy(heap);
        EXPECT_EQ(AllocationTracker::count(), values.size() * 3);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, BinomialHeapConstructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        BinomialHeap<double> heap(input.begin(), input.end());
        EXPECT_EQ(heap.getRoot(), *std::min_element(input.begin(), input.end()));
    }
}

TEST_F(AlgorithmTest, BinomialHeapAddTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        BinomialHeap<double> heap;
        double minimum = input[0];
        uint32_t j = 0;
        for (auto value : input) {
            if (value < minimum)
                minimum = value;
            heap.add(value);

            EXPECT_EQ(heap.size(), ++j);
            EXPECT_EQ(minimum, heap.getRoot());
        }
    }
}

TEST_F(AlgorithmTest, BinomialHeapRemoveTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        BinomialHeap<double> heap(input.begin(), input.end());
        std::vector<double> output(input.size());
        std::generate(output.begin(), output.end(), [&heap]() { return heap.removeRoot(); });
        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, BinomialHeapExceptionTest)
{
    BinomialHeap<int> heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_THROW(heap.getRoot(), std::underflow_error);
    EXPECT_THROW(heap.removeRoot(), std::underflow_error);
}

TEST_F(AlgorithmTest, BinomialHeapMergeTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input1 = generateData(100, 1000, engine);
        std::vector<double> input2 = generateData(100, 1000, engine);
        BinomialHeap<double> heap1(input1.begin(), input1.end());
        BinomialHeap<double> heap2(input2.begin(), input2.end());
        std::list<double> concat(input1.begin(), input1.end());
        std::copy(input2.begin(), input2.end(), std::back_inserter(concat));
        concat.sort();
        std::vector<double> output(concat.size());
        heap1.merge(heap2);

        std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        EXPECT_TRUE(std::equal(concat.begin(), concat.end(), output.begin()));
    }

    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        BinomialHeap<double> heap1(input.begin(), input.end());
        BinomialHeap<double> emptyHeap;
        std::vector<double> output(input.size());
        if (i % 2) {
            heap1.merge(emptyHeap);
            std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        } else {
            emptyHeap.merge(heap1);
            std::generate(
                output.begin(), output.end(), [&emptyHeap]() { return emptyHeap.removeRoot(); });
        }

        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, FibonacciHeapDestructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        FibonacciHeap<AllocationTracker> heap(values.begin(), values.end());
        EXPECT_EQ(heap.size(), values.size());
        EXPECT_EQ(AllocationTracker::count(), values.size() * 2);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, FibonacciHeapCopyTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<AllocationTracker> values(dist(engine));
        ASSERT_EQ(values.size(), AllocationTracker::count());
        FibonacciHeap<AllocationTracker> heap(values.begin(), values.end());
        FibonacciHeap<AllocationTracker> copy(heap);
        EXPECT_EQ(AllocationTracker::count(), values.size() * 3);
    }
    EXPECT_EQ(AllocationTracker::count(), 0U);
}

TEST_F(AlgorithmTest, FibonacciHeapConstructorTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        FibonacciHeap<double> heap(input.begin(), input.end());
        EXPECT_EQ(heap.getRoot(), *std::min_element(input.begin(), input.end()));
    }
}

TEST_F(AlgorithmTest, FibonacciHeapAddTest)
{
    std::uniform_int_distribution dist(0, 1000);
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        FibonacciHeap<double> heap;
        double minimum = input[0];
        uint32_t j = 0;
        for (auto value : input) {
            if (value < minimum)
                minimum = value;
            heap.add(value);

            EXPECT_EQ(heap.size(), ++j);
            EXPECT_EQ(minimum, heap.getRoot());
        }
    }
}

TEST_F(AlgorithmTest, FibonacciHeapRemoveTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        FibonacciHeap<double> heap(input.begin(), input.end());
        std::vector<double> output(input.size());
        std::generate(output.begin(), output.end(), [&heap]() { return heap.removeRoot(); });
        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, FibonacciHeapExceptionTest)
{
    FibonacciHeap<int> heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_THROW(heap.getRoot(), std::underflow_error);
    EXPECT_THROW(heap.removeRoot(), std::underflow_error);
}

TEST_F(AlgorithmTest, FibonacciHeapMergeTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input1 = generateData(100, 1000, engine);
        std::vector<double> input2 = generateData(100, 1000, engine);
        FibonacciHeap<double> heap1(input1.begin(), input1.end());
        FibonacciHeap<double> heap2(input2.begin(), input2.end());
        std::list<double> concat(input1.begin(), input1.end());
        std::copy(input2.begin(), input2.end(), std::back_inserter(concat));
        concat.sort();
        std::vector<double> output(concat.size());
        heap1.merge(heap2);

        std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        EXPECT_TRUE(std::equal(concat.begin(), concat.end(), output.begin()));
    }

    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<double> input = generateData(100, 1000, engine);
        FibonacciHeap<double> heap1(input.begin(), input.end());
        FibonacciHeap<double> emptyHeap;
        std::vector<double> output(input.size());
        if (i % 2) {
            heap1.merge(emptyHeap);
            std::generate(output.begin(), output.end(), [&heap1]() { return heap1.removeRoot(); });
        } else {
            emptyHeap.merge(heap1);
            std::generate(
                output.begin(), output.end(), [&emptyHeap]() { return emptyHeap.removeRoot(); });
        }

        std::sort(input.begin(), input.end());
        EXPECT_TRUE(std::equal(input.begin(), input.end(), output.begin()));
    }
}

TEST_F(AlgorithmTest, DijkstraAllPairTest)
{
    // Due to the complexity of this algorithm/verification, only using a known-answer problem
    // To an extent, this also serves as a test for decrease() on the heap structure used
    Graph<char> input(true, true);
    std::vector<char> vertices({ 's', 'a', 'b', 'c', 'd', 'e', 'f' });
    for (char& vert : vertices)
        input.addVertex(vert);

    input.setEdge('s', 'b', 7);
    input.setEdge('s', 'c', 20);
    input.setEdge('s', 'd', 2);
    input.setEdge('s', 'e', 13);
    input.setEdge('a', 'd', 1);
    input.setEdge('b', 'a', 7);
    input.setEdge('b', 'c', 10);
    input.setEdge('b', 'd', 8);
    input.setEdge('b', 'f', 8);
    input.setEdge('d', 'c', 8);
    input.setEdge('d', 'f', 5);
    input.setEdge('e', 'f', 12);

    auto result = GraphAlg::pathDijkstra(input, 's');
    EXPECT_EQ(result['s'].first, 0);
    EXPECT_EQ(result['a'], std::make_pair(14., 'b'));
    EXPECT_EQ(result['b'], std::make_pair(7., 's'));
    EXPECT_EQ(result['c'], std::make_pair(10., 'd'));
    EXPECT_EQ(result['d'], std::make_pair(2., 's'));
    EXPECT_EQ(result['e'], std::make_pair(13., 's'));
    EXPECT_EQ(result['f'], std::make_pair(7., 'd'));
}

TEST_F(AlgorithmTest, DijkstraSinglePathTest)
{
    // Due to the complexity of this algorithm/verification, only using a known-answer problem
    Graph<int> input(false, true);
    std::vector<int> vertices({ 1, 2, 3, 4, 5, 6 });
    for (int& vert : vertices)
        input.addVertex(vert);

    input.setEdge(1, 2, 7);
    input.setEdge(1, 3, 9);
    input.setEdge(1, 6, 14);
    input.setEdge(2, 3, 10);
    input.setEdge(2, 4, 15);
    input.setEdge(3, 4, 11);
    input.setEdge(3, 6, 2);
    input.setEdge(4, 5, 6);
    input.setEdge(5, 6, 9);

    auto result = GraphAlg::pathDijkstra(input, 1, 5);
    EXPECT_EQ(result.first, 20);
    std::vector<int> correctPath({3, 6, 5 });
    EXPECT_EQ(result.second.size(), correctPath.size());
    EXPECT_TRUE(std::equal(result.second.begin(), result.second.end(), correctPath.begin()));
}