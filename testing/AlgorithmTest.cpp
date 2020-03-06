
#include <unordered_set>

#include <gtest/gtest.h>

#include <approx/npc_graph.h>
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

TEST_F(AlgorithmTest, VertexCoverTest)
{
    for (uint32_t i = 0; i < 200; ++i) {
        graph::graph<uint32_t, false, false> input(graph::adj_list);
        std::uniform_int_distribution<uint32_t> size_gen(1, 100);
        uint32_t graph_size = size_gen(engine);
        for (uint32_t j = 0; j < graph_size; ++j) {
            input.add_vertex(j);
        }
        std::uniform_int_distribution<uint32_t> cover_gen(1, graph_size);
        uint32_t max_cover_size
            = cover_gen(engine); // may not be minimal, but certainly an upper bound
        std::unordered_set<uint32_t> cover_members;
        for (uint32_t j = 0; j < max_cover_size; ++j)
            cover_members.insert(cover_gen(engine) - 1);

        for (uint32_t v : cover_members) {
            uint32_t neighbor = cover_gen(engine) - 1;
            if (neighbor != v)
                input.set_edge(v, neighbor);
        }

        std::list<uint32_t> output = approx::vertex_cover_edge_double(input);
        EXPECT_LE(output.size(), 2 * cover_members.size());
    }
}

TEST_F(AlgorithmTest, WigdersonTest)
{
    for (uint32_t i = 0; i < 200; ++i) {
        graph::graph<uint32_t, false, false> input(graph::adj_list);
        std::uniform_int_distribution<uint32_t> size_gen(1, 200);
        uint32_t graph_size = size_gen(engine);
        std::vector<uint32_t> sets[3];
        std::uniform_int_distribution<uint32_t> color_pick(0, 2);
        for (uint32_t j = 0; j < graph_size; ++j) {
            input.add_vertex(j);
            sets[color_pick(engine)].push_back(j);
        }

        // wire everything up
        std::uniform_int_distribution<uint32_t> colored_chooser[]
            = { std::uniform_int_distribution<uint32_t>(0, sets[0].size() - 1),
                  std::uniform_int_distribution<uint32_t>(0, sets[1].size() - 1),
                  std::uniform_int_distribution<uint32_t>(0, sets[2].size() - 1) };

        for (uint32_t j = 0; j < 3; ++j) {
            for (uint32_t v : sets[j]) {
                for (uint32_t k = 0; k < 3; ++k) {
                    if (j != k && !sets[k].empty()) {
                        for (uint32_t m = 0; m < sets[j].size(); ++m) { // make this dense, I guess
                            input.set_edge(v, sets[k][colored_chooser[k](engine)]);
                        }
                    }
                }
            }
        }

        std::unordered_map<uint32_t, uint32_t> result = approx::three_color_Wigderson(input);

        // verify coloring
        for (uint32_t u : input.vertices())
            for (uint32_t v : input.neighbors(u))
                EXPECT_NE(result.at(u), result.at(v));
    }
}