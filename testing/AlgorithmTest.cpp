#include <algorithm>

#include <queue>
#include <random>

#include <gtest/gtest.h>

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

template <bool Directed, bool Weighted> void induced_subgraph_test(std::mt19937_64& engine, graph::graph_type type)
{
    graph::graph<int, Directed, Weighted> input = random_graph<Directed, Weighted>(engine, true, type);
    std::vector<bool> select(input.order(), false);
    std::uniform_int_distribution<uint32_t> size_gen(0, input.order());
    std::vector<int> vertices = input.vertices();
    std::vector<int> vertex_subset = vertices;
    uint32_t subgraph_size = size_gen(engine);
    for (uint32_t i = 0; i < subgraph_size; ++i) {
        std::uniform_int_distribution<uint32_t> index_gen(i, input.order() - 1);
        uint32_t index = index_gen(engine);
        std::swap(vertex_subset[i], vertex_subset[index]);
        select[vertex_subset[i]] = true;
    }
    vertex_subset.resize(subgraph_size);
    graph::graph<int, Directed, Weighted> output
        = input.generate_induced_subgraph(vertex_subset.begin(), vertex_subset.end());
    EXPECT_EQ(subgraph_size, output.order());
    for (uint32_t i = 0; i < input.order(); ++i)
        if (!select[i])
            EXPECT_THROW(output.degree(vertices[i]), std::out_of_range);
        else {
            uint32_t j;
            if constexpr (Directed)
                j = 0;
            else
                j = i + 1;
            for (; j < input.order(); ++j) {
                if (select[j]) {
                    if constexpr (Weighted) {
                        double edge = input.edge_cost(vertices[i], vertices[j]);
                        if (std::isnan(edge)) {
                            EXPECT_TRUE(std::isnan(output.edge_cost(vertices[i], vertices[j])));
                        } else {
                            EXPECT_EQ(output.edge_cost(vertices[i], vertices[j]), edge);
                        }
                    } else {
                        EXPECT_EQ(input.has_edge(vertices[i], vertices[j]), output.has_edge(vertices[i], vertices[j]));
                    }
                }
            }
        }
}

TEST_F(AlgorithmTest, InducedSubgraph)
{
    for (uint32_t i = 0; i < 500; ++i) {
        induced_subgraph_test<true, true>(engine, graph::adj_list);
        induced_subgraph_test<true, false>(engine, graph::adj_list);
        induced_subgraph_test<false, true>(engine, graph::adj_list);
        induced_subgraph_test<false, false>(engine, graph::adj_list);
        induced_subgraph_test<true, true>(engine, graph::adj_matrix);
        induced_subgraph_test<true, false>(engine, graph::adj_matrix);
        induced_subgraph_test<false, true>(engine, graph::adj_matrix);
        induced_subgraph_test<false, false>(engine, graph::adj_matrix);
    }
}