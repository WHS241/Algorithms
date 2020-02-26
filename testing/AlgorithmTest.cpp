
#include <gtest/gtest.h>

#include <graph/bipartite.h>

#include <structures/graph.h>

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

TEST_F(AlgorithmTest, IdentifyBipartite)
{
    for (uint32_t i = 0; i < 200; ++i) {
        graph::graph<int, false, false> input = random_bipartite_graph<false>(engine);
        auto output = graph_alg::verify_bipartite(input);

        EXPECT_EQ(output.first.size() + output.second.size(), input.order());
        for (int u : output.first) {
            EXPECT_EQ(
                output.second.find(u), output.second.end()); // each vertex is in exactly one set
            for (int v : output.first)
                if (u != v)
                    EXPECT_FALSE(input.has_edge(u, v)); // each half is an independent set
        }

        // Repeat with second set; don't need to check against first set since we would have found
        // it already
        for (int u : output.second)
            for (int v : output.second)
                if (u != v)
                    EXPECT_FALSE(input.has_edge(u, v));
    }
}

TEST_F(AlgorithmTest, BipartiteFailTest)
{
    // Test graphs with odd cycles - should fail
    for (uint32_t i = 0; i < 200; ++i) {
        graph::graph<int, false, false> input = random_bipartite_graph<false>(engine);

        // pick a start for odd cycle
        // need a vertex with multiple children. If not found, we need a new graph
        int start = -1;
        for (uint32_t i = 0; i < input.order() && start == -1; ++i)
            if (input.degree(i) > 1)
                start = i;

        while (start == -1) {
            input = random_bipartite_graph<false>(engine);
            for (uint32_t i = 0; i < input.order() && start == -1; ++i)
                if (input.degree(i) > 1)
                    start = i;
        }

        // Create the odd cycle: link two of the neighbors together
        std::list<int> neighbors = input.neighbors(start);
        input.force_add(neighbors.front(), neighbors.back());

        auto output = graph_alg::verify_bipartite(input);
        EXPECT_EQ(output.first.size(), 0);
        EXPECT_EQ(output.second.size(), 0);
    }
}

TEST_F(AlgorithmTest, MaxBipartiteMatch)
{
    for (uint32_t i = 0; i < 200; ++i) {
        graph::graph<int, false, false> input = random_bipartite_graph<false>(engine);

        graph::graph<int, false, false> matched_graph(graph::adj_list);
        for (int v : input.vertices())
            matched_graph.add_vertex(v);
        std::list<std::pair<int, int>> result
            = graph_alg::maximum_bipartite_matching(input, -1, -2);
        for (std::pair<int, int> match : result) {
            EXPECT_TRUE(input.has_edge(match.first, match.second));
            matched_graph.force_add(match.first, match.second);
        }

        for (int v : input.vertices()) {
            EXPECT_LE(matched_graph.degree(v), 1);

            // not a perfect maximum check, but works: make sure we don't have an obvious addition.
            if (matched_graph.degree(v) == 0) {
                for (int w : input.neighbors(v)) {
                    EXPECT_EQ(matched_graph.degree(w), 1);
                }
            }
        }
    }
}