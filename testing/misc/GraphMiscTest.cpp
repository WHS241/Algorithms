#include <stdexcept>

#include <gtest/gtest.h>

#include <misc/GraphMisc.h>

#include "../generator.h"

class GraphMiscTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(GraphMiscTest, MinDegMISTest) {
    for (uint32_t i = 0; i < 20; ++i) {
        Graph<int> input = random_graph(engine, false, false);
        std::uniform_int_distribution<uint32_t> parameter(0, input.order());
        uint32_t limit = parameter(engine);
        Graph<int> result = GraphMisc::minDegreeInducedSubgraph(input, limit);
        auto subgraphVert = result.vertices();
        for (auto vertex : input.vertices()) {
            if (std::find(subgraphVert.begin(), subgraphVert.end(), vertex) == subgraphVert.end()) {
                auto neighbors = input.neighbors(vertex);
                EXPECT_LT(std::count_if(neighbors.begin(), neighbors.end(), [&subgraphVert](int x) {
                    return std::find(subgraphVert.begin(), subgraphVert.end(), x) != subgraphVert.end();
                    })
                , limit);
            }
            else {
                EXPECT_GE(result.degree(vertex), limit);
            }
        }
    }
}

TEST_F(GraphMiscTest, CelebrityTest) {
    for (uint32_t i = 0; i < 20; ++i) {
        Graph<int> input = random_graph(engine, true, false, true, AdjMatrix);
        bool addCelebrity = (i % 2 == 0);

        if (addCelebrity) {
            input.addVertex(input.order());

            for (uint32_t j = 0; j < input.order() - 1; ++j) {
                input.setEdge(j, input.order() - 1);
            }

            EXPECT_EQ(GraphMisc::celebrity(input), input.order() - 1);
        }
        else {
            try {
                int celebrity = GraphMisc::celebrity(input);
                EXPECT_EQ(input.degree(celebrity), 0);
                for (int vertex : input.vertices())
                    if (vertex != celebrity) {
                        EXPECT_TRUE(input.hasEdge(vertex, celebrity));
                    }
            } catch(std::invalid_argument&) {}
        }
    }
}