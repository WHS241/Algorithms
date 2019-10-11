#include <algorithm>

#include <queue>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <graph/path.h>

#include <structures/graph.h>

#include "generator.h"

class AlgorithmTest : public ::testing::Test {
protected:
    std::mt19937_64 engine;
    std::priority_queue<double> heap;

    graph::graph<uint32_t, true, true> input;

    void SetUp() override
    {
        std::random_device base;
        engine = std::mt19937_64(base());

        for (uint32_t i = 1; i <= 6; ++i) {
            input.add_vertex(i);
        }

        input.set_edge(1, 2, -6);
        input.set_edge(1, 4, 1);
        input.set_edge(2, 1, 11);
        input.set_edge(2, 3, -4);
        input.set_edge(3, 6, 20);
        input.set_edge(4, 3, -11);
        input.set_edge(4, 5, 2);
        input.set_edge(5, 2, 3);
        input.set_edge(5, 4, -1);
        input.set_edge(5, 6, 7);
        input.set_edge(6, 1, 5);
        input.set_edge(6, 5, -3);
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
    auto allPairResult = graph_alg::Floyd_Warshall_all_pairs(input);

    for (uint32_t start : input.vertices()) {
        for (uint32_t end : input.vertices()) {
            auto resultBellmanFord = graph_alg::Bellman_Ford_single_target(input, start, end);
            EXPECT_DOUBLE_EQ(resultBellmanFord.first, allPairResult[start][end].first);

            // build path from Floyd-Warshall
            auto buildResult = buildPath(allPairResult, start, end);

            // possibility of the two algorithms getting different paths of equal lengths
            // verify independently
            double generatedPathBF = 0;
            double generatedPathFW = 0;

            if (!resultBellmanFord.second.empty()) {
                auto it1 = resultBellmanFord.second.begin();
                generatedPathBF += input.edge_cost(start, *it1);
                ++it1;
                for (auto it2 = resultBellmanFord.second.begin();
                     it1 != resultBellmanFord.second.end(); ++it1, ++it2) {
                    generatedPathBF += input.edge_cost(*it2, *it1);
                }
            }

            if (!buildResult.empty()) {
                auto it1 = buildResult.begin();
                generatedPathFW += input.edge_cost(start, *it1);
                ++it1;
                for (auto it2 = buildResult.begin(); it1 != buildResult.end(); ++it1, ++it2) {
                    generatedPathFW += input.edge_cost(*it2, *it1);
                }
            }

            EXPECT_DOUBLE_EQ(generatedPathBF, resultBellmanFord.first);
            EXPECT_DOUBLE_EQ(generatedPathFW, allPairResult[start][end].first);
        }
    }
}
TEST_F(AlgorithmTest, DijkstraAllPairTest)
{
    // Due to the complexity of this algorithm/verification, only using a known-answer problem
    // To an extent, this also serves as a test for decrease() on the heap structure used
    graph::graph<char, true, true> input;
    std::vector<char> vertices({ 's', 'a', 'b', 'c', 'd', 'e', 'f' });
    for (char& vert : vertices)
        input.add_vertex(vert);

    input.set_edge('s', 'b', 7);
    input.set_edge('s', 'c', 20);
    input.set_edge('s', 'd', 2);
    input.set_edge('s', 'e', 13);
    input.set_edge('a', 'd', 1);
    input.set_edge('b', 'a', 7);
    input.set_edge('b', 'c', 10);
    input.set_edge('b', 'd', 8);
    input.set_edge('b', 'f', 8);
    input.set_edge('d', 'c', 8);
    input.set_edge('d', 'f', 5);
    input.set_edge('e', 'f', 12);

    auto result = graph_alg::Dijkstra_all_targets(input, 's');
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
    graph::graph<int, false, true> input;
    std::vector<int> vertices({ 1, 2, 3, 4, 5, 6 });
    for (int& vert : vertices)
        input.add_vertex(vert);

    input.set_edge(1, 2, 7);
    input.set_edge(1, 3, 9);
    input.set_edge(1, 6, 14);
    input.set_edge(2, 3, 10);
    input.set_edge(2, 4, 15);
    input.set_edge(3, 4, 11);
    input.set_edge(3, 6, 2);
    input.set_edge(4, 5, 6);
    input.set_edge(5, 6, 9);

    auto result = graph_alg::Dijkstra_single_target(input, 1, 5);
    EXPECT_EQ(result.first, 20);
    std::vector<int> correctPath({3, 6, 5 });
    EXPECT_EQ(result.second.size(), correctPath.size());
    EXPECT_TRUE(std::equal(result.second.begin(), result.second.end(), correctPath.begin()));
}