#include <algorithm>

#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <graph/path.h>

#include <structures/Graph.h>

#include "generator.h"

class AlgorithmTest : public ::testing::Test {
protected:
    std::mt19937_64 engine;

    Graph<uint32_t> input;

    void SetUp() override
    {
        std::random_device base;
        engine = std::mt19937_64(base());

        input = Graph<uint32_t>(true, true);
        for (uint32_t i = 1; i <= 6; ++i) {
            input.addVertex(i);
        }

        input.setEdge(1, 2, -6);
        input.setEdge(1, 4, 1);
        input.setEdge(2, 1, 11);
        input.setEdge(2, 3, -4);
        input.setEdge(3, 6, 20);
        input.setEdge(4, 3, -11);
        input.setEdge(4, 5, 2);
        input.setEdge(5, 2, 3);
        input.setEdge(5, 4, -1);
        input.setEdge(5, 6, 7);
        input.setEdge(6, 1, 5);
        input.setEdge(6, 5, -3);
    }
};

static std::list<uint32_t> buildPath(
    const std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::pair<double, uint32_t>>>&
        data,
    uint32_t start, uint32_t dest)
{
    std::list<uint32_t> result;
    if (start == dest)
        return result;

    uint32_t mid = data.at(start).at(dest).second;
    if (mid == start) {
        result.push_back(dest);
        return result;
    }
    result = buildPath(data, start, mid);
    result.splice(result.end(), buildPath(data, mid, dest));
    return result;
}

TEST_F(AlgorithmTest, PathTests)
{
    auto allPairResult = GraphAlg::allPairs(input);

    for (uint32_t start : input.vertices()) {
        for (uint32_t end : input.vertices()) {
            auto resultBellmanFord = GraphAlg::pathBellmanFord(input, start, end);
            EXPECT_DOUBLE_EQ(resultBellmanFord.first, allPairResult[start][end].first);

            // build path from Floyd-Warshall
            auto buildResult = buildPath(allPairResult, start, end);

            // possibility of the two algorithms getting different paths of equal lengths
            // verify independently
            double generatedPathBF = 0;
            double generatedPathFW = 0;

            if (!resultBellmanFord.second.empty()) {
                auto it1 = resultBellmanFord.second.begin();
                generatedPathBF += input.edgeCost(start, *it1);
                ++it1;
                for (auto it2 = resultBellmanFord.second.begin();
                     it1 != resultBellmanFord.second.end(); ++it1, ++it2) {
                    generatedPathBF += input.edgeCost(*it2, *it1);
                }
            }

            if (!buildResult.empty()) {
                auto it1 = buildResult.begin();
                generatedPathFW += input.edgeCost(start, *it1);
                ++it1;
                for (auto it2 = buildResult.begin();
                     it1 != buildResult.end(); ++it1, ++it2) {
                    generatedPathFW += input.edgeCost(*it2, *it1);
                }
            }

            EXPECT_DOUBLE_EQ(generatedPathBF, resultBellmanFord.first);
            EXPECT_DOUBLE_EQ(generatedPathFW, allPairResult[start][end].first);
        }
    }
}