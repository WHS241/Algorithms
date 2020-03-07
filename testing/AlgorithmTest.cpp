
#include <unordered_set>

#include <gtest/gtest.h>

#include <approx/npc_graph.h>
#include <structures/van_Emde_Boas.h>
#include <structures/vEB_iterator.h>

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

TEST_F(AlgorithmTest, vanEmdeBoasTest) {
    for(uint32_t i = 0; i < 2000; ++i) {
        std::uniform_int_distribution<uint32_t> size_gen(1, 20000);
        uint32_t tree_size = size_gen(engine);
        van_Emde_Boas_tree tree(tree_size);
        std::vector<bool> included(tree_size, false);
        std::uniform_int_distribution<uint32_t> data_gen(0, tree_size - 1);
        for(uint32_t j = 0; j < tree_size; ++j) {
            uint32_t next = data_gen(engine);
            tree.insert(next);
            included[next] = true;
        }
        for(uint32_t j = 0; j < tree_size; ++j) {
            EXPECT_EQ(included[j], tree.contains(j));
        }
        std::list<uint32_t> values;
        std::copy(tree.begin(), tree.end(), std::back_inserter(values));
        for(auto it = ++values.begin(); it != values.end(); ++it) {
            auto it2 = it;
            --it2;
            EXPECT_LT(*it2, *it);
        }

        std::list<uint32_t> rev_values(values);
        std::copy(std::make_reverse_iterator(tree.end()), std::make_reverse_iterator(tree.begin()), rev_values.begin());
        EXPECT_TRUE(std::equal(rev_values.rbegin(), rev_values.rend(), values.begin(), values.end()));
    }
}