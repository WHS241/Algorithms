#include <algorithm>
#include <fstream>
#include <queue>
#include <random>

#include <gtest/gtest.h>

#include <structures/graph.h>

#include <graph/closure.h>
#include <graph/order_dimension.h>

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

TEST_F(AlgorithmTest, Transitive_Closure_Test) {
    for (int i = 0; i < 100; ++i) {
        graph::graph<int, true, false> input = random_graph<true, false>(engine, false);
        graph::graph<int, true, false> closure = graph_alg::transitive_closure(input);
        EXPECT_TRUE(graph_alg::is_transitive_closure(closure));

        dynamic_matrix<int> input_matrix(input.order(), input.order(), 0), closure_matrix(input.order(), input.order(), 0);
        for (int v : input.vertices()) {
            for (int w : input.neighbors(v))
                input_matrix[v][w] = 1;
            for (int w : closure.neighbors(v))
                closure_matrix[v][w] = 1;
        }
        EXPECT_EQ(input_matrix == closure_matrix, graph_alg::is_transitive_closure(input));
    }
}

TEST_F(AlgorithmTest, Order_Dimension_Test) {
    graph::unweighted_graph<std::string, true> input(graph::graph_type::adj_matrix);
    std::initializer_list<std::string> vertices = { 
        "u12", "u13", "u1a", "u1b", "u21", "u23", "u2a", "u2b", "u31", "u32", "u3a", "u3b", 
        "v12", "v13", "v1a", "v1b", "v21", "v23", "v2a", "v2b", "v31", "v32", "v3a", "v3b"
    };
    std::list<std::pair<std::string, std::string>> relations = {
        {"u1a", "v12"}, {"u1a", "v13"}, {"u1a", "v1b"}, {"u2a", "v21"}, {"u2a", "v23"}, {"u2a", "v2b"}, {"u3a", "v31"}, {"u3a", "v32"}, {"u3a", "v3b"},
        {"u1b", "v12"}, {"u1b", "v13"}, {"u1b", "v1a"}, {"u2b", "v21"}, {"u2b", "v23"}, {"u2b", "v2a"}, {"u3b", "v31"}, {"u3b", "v32"}, {"u3b", "v3a"},

        {"u12", "v13"}, {"u12", "v1a"}, {"u12", "v1b"}, {"u12", "v23"}, {"u12", "v2a"}, {"u12", "v2b"}, {"u12", "v31"}, {"u12", "v32"}, {"u12", "v3a"}, {"u12", "v3b"},
        {"u21", "v13"}, {"u21", "v1a"}, {"u21", "v1b"}, {"u21", "v23"}, {"u21", "v2a"}, {"u21", "v2b"}, {"u21", "v31"}, {"u21", "v32"}, {"u21", "v3a"}, {"u21", "v3b"},
        {"u23", "v13"}, {"u23", "v1a"}, {"u23", "v1b"}, {"u23", "v2a"}, {"u23", "v2b"}, {"u23", "v31"}, {"u23", "v3a"}, {"u23", "v3b"},
        {"u32", "v13"}, {"u32", "v1a"}, {"u32", "v1b"}, {"u32", "v2a"}, {"u32", "v2b"}, {"u32", "v31"}, {"u32", "v3a"}, {"u32", "v3b"},
        {"u13", "v1a"}, {"u13", "v1b"}, {"u13", "v2a"}, {"u13", "v2b"}, {"u13", "v3a"}, {"u13", "v3b"},
        {"u31", "v1a"}, {"u31", "v1b"}, {"u31", "v2a"}, {"u31", "v2b"}, {"u31", "v3a"}, {"u31", "v3b"},
        {"u12", "v21"}, {"u21", "v12"}, {"u23", "v32"}, {"u32", "v23"}, {"u13", "v31"}, {"u31", "v13"},
        {"u12", "u23"}, {"u12", "u32"}, {"u12", "u13"}, {"u12", "u31"}, {"u21", "u23"}, {"u21", "u32"}, {"u21", "u13"}, {"u21", "u31"},
        {"u23", "u13"}, {"u23", "u31"}, {"u32", "u13"}, {"u32", "u31"}
    };

    for (const std::string& v : vertices)
        input.add_vertex(v);
    for (const std::pair<std::string, std::string>& edge: relations)
        input.force_add(edge.first, edge.second);

    ASSERT_TRUE(graph_alg::is_transitive_closure(input));
    std::initializer_list<std::string> gen[3] = {
        {"u1a", "u1b", "u21", "v12", "u12", "u23", "u32", "u31", "v13", "u13", "v1a", "v1b", "u2b", "v2a", "u2a", "v2b", "v21", "v23", "u3a", "v3b", "u3b", "v3a", "v31", "v32"},
        {"u2a", "u2b", "u12", "v21", "u21", "u32", "v23", "u23", "u13", "u31", "v2a", "v2b", "u3b", "v3a", "u3a", "v3b", "v32", "v31", "u1a", "v1b", "u1b", "v1a", "v13", "v12"},
        {"u3a", "u3b", "u12", "u21", "u23", "v32", "u32", "u13", "v31", "u31", "v3a", "v3b", "u1b", "v1a", "u1a", "v1b", "v12", "v13", "u2a", "v2b", "u2b", "v2a", "v23", "v21"}
    };

    graph::unweighted_graph<std::string, true> result = graph_alg::three_dimensional_transitive_reduction(gen[0].begin(), gen[0].end(), gen[1].begin(), gen[1].end(), gen[2].begin(), gen[2].end());

    EXPECT_TRUE(graph_alg::is_transitive_reduction(result));
    graph::unweighted_graph<std::string, true> new_closure = graph_alg::transitive_closure(result);
    for (const std::string& u : new_closure.vertices())
        for (const std::string& v : new_closure.vertices()) {
            if (new_closure.has_edge(u, v) != input.has_edge(u, v)) {
                std::cout << u << " " << v << std::endl;
                std::cout << new_closure << std::endl;
                std::cout << input << std::endl;
            }
            ASSERT_EQ(new_closure.has_edge(u, v), input.has_edge(u, v));
        }
} 
        
