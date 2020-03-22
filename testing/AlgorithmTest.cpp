
#include <unordered_set>

#include <gtest/gtest.h>

#include <structures/graph.h>
#include <graph/closure.h>

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

TEST_F(AlgorithmTest, TransitiveClosureTest) {
    for(uint32_t i = 0; i < 100; ++i) {
        graph::graph<int, false, false> input = random_graph<false, false>(engine);
        graph::graph<int, false, false> closure = graph_alg::transitive_closure(input);
        for(int& v : input.vertices()) {
            graph_alg::depth_first(input, v, [&closure, &v](int u) {
                if(v != u)
                    EXPECT_TRUE(closure.has_edge(u, v));
            },[](int, int){});
        }
    }
}

TEST_F(AlgorithmTest, ChvatalBondyTest) {
    for(uint32_t i = 0; i < 100; ++i) {
        std::cout << "Next" << std::endl;
        graph::graph<int, false, false> input = random_graph<false, false>(engine);
        uint32_t highest_degree = 0;
        for(int v : input.vertices())
            highest_degree = std::max(highest_degree, input.degree(v));
        std::cout << input << std::endl;
        for(uint32_t j = 0; j < highest_degree; ++j) {
            graph::graph<int, false, false> result = graph_alg::Chvatal_Bondy_closure(input, j);

            for(int u : input.vertices()) {
                for(int v : input.vertices()) {
                    if(u != v && result.degree(u) + result.degree(v) >= j) {
                        if(!result.has_edge(u,v)) {
                            std::cout << j << "\n" << result << std::endl;
                        }
                        EXPECT_TRUE(result.has_edge(u, v));
                    } else
                        EXPECT_EQ(input.has_edge(u, v), result.has_edge(u, v));
                }
            }
        }
    }

}