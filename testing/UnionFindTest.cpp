#include <random>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <structures/UnionFind.h>

std::vector<uint32_t> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine) {
        std::vector<uint32_t> result(size);
        std::uniform_int_distribution<uint32_t> dist(0, bound);
        std::generate(result.begin(), result.end(), [&dist, &engine]() {return dist(engine); });
        std::unordered_set<uint32_t> verify;

        result.erase(std::remove_if(result.begin(), result.end(), [&verify](uint32_t x) {
            bool ret = (verify.find(x) != verify.end());
            if (!ret) {
                verify.insert(x);
            }
            return ret;
            }), result.end());
        return result;
}

class UnionFindTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(UnionFindTest, InsertTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        UnionFind<uint32_t> forest;
        std::vector<uint32_t> toInsert(generateData(100, 10000, engine));
        for (auto& x : toInsert) {
            forest.insert(x);
            EXPECT_EQ(x, forest.find(x));
        }
    }
}

TEST_F(UnionFindTest, LoadConstructorTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<uint32_t> toInsert(generateData(100, 10000, engine));
        UnionFind<uint32_t> forest(toInsert.begin(), toInsert.end());
        EXPECT_EQ(forest.size(), toInsert.size());
        for (auto& x : toInsert) {
            EXPECT_EQ(x, forest.find(x));
        }
        EXPECT_THROW(forest.insert(toInsert[0]), std::invalid_argument);
    }
}

TEST_F(UnionFindTest, UnionAndFindTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        std::vector<uint32_t> toInsert(generateData(100, 10000, engine));
        UnionFind<uint32_t> forest(toInsert.begin(), toInsert.end());
        
        std::uniform_int_distribution<uint32_t> chooseSets(0, toInsert.size() - 1);
        for (uint32_t i = 0; i < 20; ++i) {
            auto x1(toInsert[chooseSets(engine)]), x2(toInsert[chooseSets(engine)]);
            forest.setUnion(x1, x2);
            EXPECT_EQ(forest.find(x1), forest.find(x2));
        }
        EXPECT_THROW(forest.find(100000), std::out_of_range);
    }
}

TEST_F(UnionFindTest, RemoveTest) {
    for (uint32_t i = 0; i < 10; ++i) {
        std::vector<uint32_t> toInsert(generateData(100, 10000, engine));
        UnionFind<uint32_t> forest(toInsert.begin(), toInsert.end());
        for (auto& x : toInsert) {
            forest.removeSet(x);
            EXPECT_THROW(forest.find(x), std::out_of_range);
        }
    }
}