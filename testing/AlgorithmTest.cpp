#include <algorithm>

#include <queue>
#include <random>

#include <gtest/gtest.h>

#include <structures/graph.h>

#include <graph/components.h>
#include <graph/min_flow_max_cut.h>
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

/*
 * For testing the max flow algorithms
 */
template <typename F> void test_case_one(F target_algorithm)
{
    graph::graph<char, true, true> input;
    input.add_vertex('A');
    input.add_vertex('B');
    input.add_vertex('C');
    input.add_vertex('D');
    input.add_vertex('E');
    input.add_vertex('F');
    input.add_vertex('G');

    input.set_edge('A', 'B', 3);
    input.set_edge('A', 'D', 3);
    input.set_edge('B', 'C', 4);
    input.set_edge('C', 'A', 3);
    input.set_edge('C', 'D', 1);
    input.set_edge('C', 'E', 2);
    input.set_edge('D', 'E', 2);
    input.set_edge('D', 'F', 6);
    input.set_edge('E', 'B', 1);
    input.set_edge('E', 'G', 1);
    input.set_edge('F', 'G', 9);

    graph::graph<char, true, true> result = target_algorithm(input, 'A', 'G');
    EXPECT_DOUBLE_EQ(result.edge_cost('A', 'B'), 2);
    EXPECT_DOUBLE_EQ(result.edge_cost('A', 'D'), 3);
    EXPECT_DOUBLE_EQ(result.edge_cost('B', 'C'), 2);
    EXPECT_DOUBLE_EQ(result.edge_cost('C', 'D'), 1);
    EXPECT_DOUBLE_EQ(result.edge_cost('C', 'E'), 1);
    EXPECT_DOUBLE_EQ(result.edge_cost('D', 'F'), 4);
    EXPECT_DOUBLE_EQ(result.edge_cost('E', 'G'), 1);
    EXPECT_DOUBLE_EQ(result.edge_cost('F', 'G'), 4);

    EXPECT_EQ(result.degree('A'), 2);
    EXPECT_EQ(result.degree('B'), 1);
    EXPECT_EQ(result.degree('C'), 2);
    EXPECT_EQ(result.degree('D'), 1);
    EXPECT_EQ(result.degree('E'), 1);
    EXPECT_EQ(result.degree('F'), 1);
}
template <typename F> void test_case_two(F target_algorithm)
{
    graph::graph<char, true, true> input;
    input.add_vertex('s');
    input.add_vertex('1');
    input.add_vertex('2');
    input.add_vertex('3');
    input.add_vertex('4');
    input.add_vertex('t');

    input.set_edge('s', '1', 10);
    input.set_edge('s', '2', 10);
    input.set_edge('1', '2', 2);
    input.set_edge('1', '3', 4);
    input.set_edge('1', '4', 8);
    input.set_edge('2', '4', 9);
    input.set_edge('3', 't', 10);
    input.set_edge('4', '3', 6);
    input.set_edge('4', 't', 10);

    graph::graph<char, true, true> result = target_algorithm(input, 's', 't');
    EXPECT_DOUBLE_EQ(result.edge_cost('s', '1'), 10);
    EXPECT_DOUBLE_EQ(result.edge_cost('s', '2'), 9);
    EXPECT_DOUBLE_EQ(result.edge_cost('1', '3'), 4);
    EXPECT_DOUBLE_EQ(result.edge_cost('1', '4'), 6);
    EXPECT_DOUBLE_EQ(result.edge_cost('2', '4'), 9);
    EXPECT_DOUBLE_EQ(result.edge_cost('3', 't'), 9);
    EXPECT_DOUBLE_EQ(result.edge_cost('4', '3'), 5);
    EXPECT_DOUBLE_EQ(result.edge_cost('4', 't'), 10);

    EXPECT_EQ(result.degree('s'), 2);
    EXPECT_EQ(result.degree('1'), 2);
    EXPECT_EQ(result.degree('2'), 1);
    EXPECT_EQ(result.degree('3'), 1);
    EXPECT_EQ(result.degree('4'), 2);
    EXPECT_EQ(result.degree('t'), 0);
}

TEST_F(AlgorithmTest, Min_Flow_Edmonds_Karp)
{
    test_case_one([](const auto& a, const auto& b, const auto& c) {
        return graph_alg::Edmonds_Karp(a, b, c);
    });
    test_case_two([](const auto& a, const auto& b, const auto& c) {
        return graph_alg::Edmonds_Karp(a, b, c);
    });
}

TEST_F(AlgorithmTest, Min_Flow_Dinitz)
{
    test_case_one(
        [](const auto& a, const auto& b, const auto& c) { return graph_alg::Dinitz(a, b, c); });
    test_case_two(
        [](const auto& a, const auto& b, const auto& c) { return graph_alg::Dinitz(a, b, c); });
}