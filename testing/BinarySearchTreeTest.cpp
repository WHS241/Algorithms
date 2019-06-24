#include <structures/PrimitiveBinarySearchTree.h>

#include <gtest/gtest.h>
#include <vector>
#include <random>

template<typename T, typename C>
void verifySearchTree(const BinarySearchTree<T, C>& tree) {
    C tester;
    for (auto it(tree.begin(InOrder)); it != tree.end(); ++it) {
        auto it2(it);
        ++it2;
        if (it2 != tree.end()) {
            EXPECT_TRUE(tester(*it, *it2));
        }
    }
}

std::vector<double> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine) {
    std::vector<double> result(size);
    std::uniform_real_distribution<> dist(0, bound);
    std::generate(result.begin(), result.end(), [&dist, &engine]() {return dist(engine); });
    return result;
}


class PrimitiveBinarySearchTreeTest : public ::testing::Test {
protected :
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(PrimitiveBinarySearchTreeTest, PrimitiveConstructor) {
    for (uint32_t i = 5; i < 20; ++i) {
        auto data(generateData(i, i, engine));
        PrimitiveBinarySearchTree tree(data);
        verifySearchTree(tree);
    }
}