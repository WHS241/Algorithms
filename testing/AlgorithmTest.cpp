#include <algorithm>

#include <queue>
#include <random>

#include <gtest/gtest.h>

#include <structures/graph.h>

#include <graph/components.h>
#include <graph/path.h>
#include <graph/spanning_tree.h>

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

TEST_F(AlgorithmTest, Boruvka_Spanning_Tree)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto result = graph_alg::minimum_spanning_Boruvka(input);

        auto connected = graph_alg::connected_components(input);
        EXPECT_EQ(connected.size(), graph_alg::connected_components(result).size());

        auto mst_vertices = result.vertices();
        auto mst_edge_count = std::accumulate(mst_vertices.begin(), mst_vertices.end(), 0U,
            [&result](uint32_t i, int j) { return i + result.degree(j); });
        EXPECT_EQ(mst_edge_count / 2 + connected.size(), input.order());

        for (const int& start : input.vertices()) {
            for (const auto& edge : input.edges(start)) {
                if (result.has_edge(start, edge.first)) {
                    EXPECT_EQ(result.edge_cost(start, edge.first), edge.second);
                } else {
                    auto mst_path = graph_alg::least_edges_path(result, start, edge.first);
                    for (auto it = mst_path.begin(); it != mst_path.end(); ++it) {
                        if (it == mst_path.begin()) {
                            EXPECT_LE(result.edge_cost(start, *it), edge.second);
                        } else {
                            auto temp(it);
                            --temp;
                            EXPECT_LE(result.edge_cost(*temp, *it), edge.second);
                        }
                    }
                }
            }
        }
    }
}

TEST_F(AlgorithmTest, Prim_Spanning_Tree)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto result = graph_alg::minimum_spanning_Prim(input);

        auto connected = graph_alg::connected_components(input);
        EXPECT_EQ(connected.size(), graph_alg::connected_components(result).size());

        auto mst_vertices = result.vertices();
        auto mst_edge_count = std::accumulate(mst_vertices.begin(), mst_vertices.end(), 0U,
            [&result](uint32_t i, int j) { return i + result.degree(j); });
        EXPECT_EQ(mst_edge_count / 2 + connected.size(), input.order());

        for (const int& start : input.vertices()) {
            for (const auto& edge : input.edges(start)) {
                if (result.has_edge(start, edge.first)) {
                    EXPECT_EQ(result.edge_cost(start, edge.first), edge.second);
                } else {
                    auto mst_path = graph_alg::least_edges_path(result, start, edge.first);
                    for (auto it = mst_path.begin(); it != mst_path.end(); ++it) {
                        if (it == mst_path.begin()) {
                            EXPECT_LE(result.edge_cost(start, *it), edge.second);
                        } else {
                            auto temp(it);
                            --temp;
                            EXPECT_LE(result.edge_cost(*temp, *it), edge.second);
                        }
                    }
                }
            }
        }
    }
}

TEST_F(AlgorithmTest, Kruskal_Spanning_Tree)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto result = graph_alg::minimum_spanning_Kruskal(input);

        auto connected = graph_alg::connected_components(input);
        EXPECT_EQ(connected.size(), graph_alg::connected_components(result).size());

        auto mst_vertices = result.vertices();
        auto mst_edge_count = std::accumulate(mst_vertices.begin(), mst_vertices.end(), 0U,
            [&result](uint32_t i, int j) { return i + result.degree(j); });
        EXPECT_EQ(mst_edge_count / 2 + connected.size(), input.order());

        for (const int& start : input.vertices()) {
            for (const auto& edge : input.edges(start)) {
                if (result.has_edge(start, edge.first)) {
                    EXPECT_EQ(result.edge_cost(start, edge.first), edge.second);
                } else {
                    auto mst_path = graph_alg::least_edges_path(result, start, edge.first);
                    for (auto it = mst_path.begin(); it != mst_path.end(); ++it) {
                        if (it == mst_path.begin()) {
                            EXPECT_LE(result.edge_cost(start, *it), edge.second);
                        } else {
                            auto temp(it);
                            --temp;
                            EXPECT_LE(result.edge_cost(*temp, *it), edge.second);
                        }
                    }
                }
            }
        }
    }
}

TEST_F(AlgorithmTest, Yao_Spanning_Tree)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto result = graph_alg::minimum_spanning_Yao(input);

        auto connected = graph_alg::connected_components(input);
        EXPECT_EQ(connected.size(), graph_alg::connected_components(result).size());

        auto mst_vertices = result.vertices();
        auto mst_edge_count = std::accumulate(mst_vertices.begin(), mst_vertices.end(), 0U,
            [&result](uint32_t i, int j) { return i + result.degree(j); });
        EXPECT_EQ(mst_edge_count / 2 + connected.size(), input.order());

        for (const int& start : input.vertices()) {
            for (const auto& edge : input.edges(start)) {
                if (result.has_edge(start, edge.first)) {
                    EXPECT_EQ(result.edge_cost(start, edge.first), edge.second);
                } else {
                    auto mst_path = graph_alg::least_edges_path(result, start, edge.first);
                    for (auto it = mst_path.begin(); it != mst_path.end(); ++it) {
                        if (it == mst_path.begin()) {
                            EXPECT_LE(result.edge_cost(start, *it), edge.second);
                        } else {
                            auto temp(it);
                            --temp;
                            EXPECT_LE(result.edge_cost(*temp, *it), edge.second);
                        }
                    }
                }
            }
        }
    }
}

TEST_F(AlgorithmTest, Spanning_Tree_Comparison)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto Boruvka_result = graph_alg::minimum_spanning_Boruvka(input);
        auto Prim_result = graph_alg::minimum_spanning_Prim(input);
        auto Kruskal_result = graph_alg::minimum_spanning_Kruskal(input);
        auto Yao_result = graph_alg::minimum_spanning_Yao(input);

        auto vertices = input.vertices();
        double Boruvka_total = std::accumulate(
            vertices.begin(), vertices.end(), 0., [&Boruvka_result](double prev, int curr_vertex) {
                auto edges = Boruvka_result.edges(curr_vertex);
                return std::accumulate(edges.begin(), edges.end(), prev,
                    [](double current, auto x) { return current + x.second; });
            });

        double Prim_total = std::accumulate(
            vertices.begin(), vertices.end(), 0., [&Prim_result](double prev, int curr_vertex) {
                auto edges = Prim_result.edges(curr_vertex);
                return std::accumulate(edges.begin(), edges.end(), prev,
                    [](double current, auto x) { return current + x.second; });
            });

        double Kruskal_total = std::accumulate(
            vertices.begin(), vertices.end(), 0., [&Kruskal_result](double prev, int curr_vertex) {
                auto edges = Kruskal_result.edges(curr_vertex);
                return std::accumulate(edges.begin(), edges.end(), prev,
                    [](double current, auto x) { return current + x.second; });
            });

        double Yao_total = std::accumulate(
            vertices.begin(), vertices.end(), 0., [&Yao_result](double prev, int curr_vertex) {
                auto edges = Yao_result.edges(curr_vertex);
                return std::accumulate(edges.begin(), edges.end(), prev,
                    [](double current, auto x) { return current + x.second; });
            });

        EXPECT_DOUBLE_EQ(Boruvka_total, Prim_total);
        EXPECT_DOUBLE_EQ(Prim_total, Kruskal_total);
        EXPECT_DOUBLE_EQ(Kruskal_total, Yao_total);
    }
}
