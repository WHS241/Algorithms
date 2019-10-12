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
    auto Floyd_result = graph_alg::Floyd_Warshall_all_pairs(input);
    auto Johnson_result = graph_alg::Johnson_all_pairs(input, 0U);

    for (uint32_t start : input.vertices()) {
        for (uint32_t end : input.vertices()) {
            EXPECT_DOUBLE_EQ(Johnson_result[start][end].first, Floyd_result[start][end].first);

            // build path from Floyd-Warshall
            auto Floyd_build_result = buildPath(Floyd_result, start, end);
            auto Johnson_build_result = buildPath(Johnson_result, start, end);

            // possibility of the two algorithms getting different paths of equal lengths
            // verify independently
            double generated_path_Johnson = 0;
            double generated_path_Floyd = 0;

            if (!Johnson_build_result.empty()) {
                auto it1 = Johnson_build_result.begin();
                generated_path_Johnson += input.edge_cost(start, *it1);
                ++it1;
                for (auto it2 = Johnson_build_result.begin(); it1 != Johnson_build_result.end();
                     ++it1, ++it2) {
                    generated_path_Johnson += input.edge_cost(*it2, *it1);
                }
            }

            if (!Floyd_build_result.empty()) {
                auto it1 = Floyd_build_result.begin();
                generated_path_Floyd += input.edge_cost(start, *it1);
                ++it1;
                for (auto it2 = Floyd_build_result.begin(); it1 != Floyd_build_result.end();
                     ++it1, ++it2) {
                    generated_path_Floyd += input.edge_cost(*it2, *it1);
                }
            }

            EXPECT_DOUBLE_EQ(generated_path_Johnson, Johnson_result[start][end].first);
            EXPECT_DOUBLE_EQ(generated_path_Floyd, Floyd_result[start][end].first);
        }
    }
}

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

TEST_F(AlgorithmTest, Spanning_Tree_Comparison)
{
    for (uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto Boruvka_result = graph_alg::minimum_spanning_Boruvka(input);
        auto Prim_result = graph_alg::minimum_spanning_Prim(input);
        auto Kruskal_result = graph_alg::minimum_spanning_Kruskal(input);

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

        EXPECT_DOUBLE_EQ(Boruvka_total, Prim_total);
        EXPECT_DOUBLE_EQ(Prim_total, Kruskal_total);
    }
}