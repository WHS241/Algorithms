#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <graph/components.h>
#include <graph/path.h>

#include <structures/RedBlackTree.h>

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

TEST_F(AlgorithmTest, ConnectivityTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        Graph<int> input = random_graph(engine, false, false);
        auto result = GraphAlg::connectedComponents(input);
        for (auto& set : result) {
            // verify path
            auto it2 = set.begin();
            for (auto it1 = it2++; it2 != set.end(); ++it1, ++it2) {
                EXPECT_NO_THROW(GraphAlg::leastEdgesPath(input, *it1, *it2));
            }
        }
    }
}

TEST_F(AlgorithmTest, BiconnectivityTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        Graph<int> input = random_graph(engine, false, false);

        auto result = GraphAlg::articulationPoints(input);
        auto numComponents = GraphAlg::connectedComponents(input).size();
        for (auto& vertex : input.vertices()) {
            Graph<int> copy(input);
            copy.remove(vertex);
            auto newComponents = GraphAlg::connectedComponents(copy).size();

            if (input.degree(vertex) == 0) {
                EXPECT_EQ(result.find(vertex), result.end());
            } else if (result.find(vertex) == result.end()) {
                EXPECT_EQ(newComponents, numComponents);
            } else {
                EXPECT_GT(newComponents, numComponents);
            }
        }
    }
}

TEST_F(AlgorithmTest, StronglyConnectedTest)
{
    for (uint32_t i = 0; i < 50; ++i) {
        Graph<int> input = random_graph(engine, true, false);

        auto result = GraphAlg::stronglyConnectedComponents(input);
        uint32_t j = 0;
        std::vector<uint32_t> compOf(input.order());
        for (auto& set : result) {
            for (auto& member : set) {
                compOf[member] = j;
            }
            ++j;
        }

        auto checkStrongConnect = [&input](int u, int v) {
            GraphAlg::leastEdgesPath(input, u, v);
            GraphAlg::leastEdgesPath(input, v, u);
        };

        for (uint32_t k = 0; k < compOf.size(); ++k) {
            for (uint32_t x = k; x < compOf.size(); ++x) {
                if (compOf[k] == compOf[x])
                    EXPECT_NO_THROW(checkStrongConnect(k, x));
                else
                    EXPECT_THROW({ checkStrongConnect(k, x); }, std::domain_error);
            }
        }
    }
}

template <typename T, typename C> void verifySearchTree(const BinarySearchTree<T, C>& tree)
{
    C tester;
    for (auto it(tree.begin(InOrder)); it != tree.end(); ++it) {
        auto it2(it);
        ++it2;
        if (it2 != tree.end()) {
            EXPECT_TRUE(tester(*it, *it2) || *it == *it2);
        }
    }
}

static std::vector<double> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine)
{
    std::vector<double> result(size);
    std::uniform_real_distribution<> dist(0, bound);
    std::generate(result.begin(), result.end(), [&dist, &engine]() { return dist(engine); });
    return result;
}

TEST_F(AlgorithmTest, RedBlackConstructor)
{
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        RedBlackTree<double> tree(data.begin(), data.end());
        verifySearchTree(tree);
        for (auto val : data) {
            EXPECT_TRUE(tree.contains(val));
        }
    }
}

TEST_F(AlgorithmTest, RedBlackInsertTest)
{
    for (uint32_t i = 5; i < 20; ++i) {
        auto data(generateData(2 * i, i, engine));
        RedBlackTree<double> tree(data.begin(), data.end());

        std::uniform_real_distribution<> dist(0, i + 1);
        for (uint32_t j = 0; j < 30; ++j) {
            double insertVal = dist(engine);
            tree.insert(insertVal);
            EXPECT_TRUE(tree.contains(insertVal));
        }

        verifySearchTree(tree);
    }
}

TEST_F(AlgorithmTest, RedBlackRemoveTest)
{
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        std::unique_ptr<BinarySearchTree<double>> tree(
            new RedBlackTree<double>(data.begin(), data.end()));
        for (double x : data) {
            tree->remove(x);
            verifySearchTree(*tree);
        }
    }
}