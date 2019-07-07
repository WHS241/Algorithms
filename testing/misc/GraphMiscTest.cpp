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
        for (auto vertex : result.vertices()) {
            EXPECT_GE(result.degree(vertex), limit);
        }
    }
}