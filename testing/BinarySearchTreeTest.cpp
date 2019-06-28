#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <structures/AVLTree.h>
#include <structures/PrimitiveBinarySearchTree.h>

template<typename T, typename C>
void verifySearchTree(const BinarySearchTree<T, C>& tree) {
    C tester;
    for (auto it(tree.begin(InOrder)); it != tree.end(); ++it) {
        auto it2(it);
        ++it2;
        if (it2 != tree.end()) {
            EXPECT_TRUE(tester(*it, *it2) || *it == *it2);
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

TEST_F(PrimitiveBinarySearchTreeTest, Constructor) {
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        PrimitiveBinarySearchTree<double> tree(data.begin(), data.end());
        verifySearchTree(tree);
        for (auto val : data) {
            EXPECT_TRUE(tree.contains(val));
        }
    }
}

TEST_F(PrimitiveBinarySearchTreeTest, InsertTest) {
    for (uint32_t i = 5; i < 20; ++i) {
        auto data(generateData(2 * i, i, engine));
        PrimitiveBinarySearchTree<double> tree(data.begin(), data.end());

        std::uniform_real_distribution<> dist(0, i + 1);
        for (uint32_t j = 0; j < 30; ++j) {
            double insertVal = dist(engine);
            tree.insert(insertVal);
            EXPECT_TRUE(tree.contains(insertVal));
        }

        verifySearchTree(tree);
    }
}

TEST_F(PrimitiveBinarySearchTreeTest, RemoveTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        std::unique_ptr<BinarySearchTree<double>> tree(new PrimitiveBinarySearchTree<double>(data.begin(), data.end()));
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->remove(data[j]);
            verifySearchTree(*tree);
        }
    }
}

/*
// timing test
TEST_F(PrimitiveBinarySearchTreeTest, SortedTest) {
    for (uint32_t i = 0; i < 500; ++i) {
        auto data(generateData(1000, 1000, engine));
        std::sort(data.begin(), data.end());
        std::unique_ptr<BinarySearchTree<double>> tree(new PrimitiveBinarySearchTree<double>());
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->insert(data[j]);
        }
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->remove(data[j]);
        }
    }
}*/


class AVLTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

TEST_F(AVLTreeTest, Constructor) {
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        AVLSearchTree<double> tree(data.begin(), data.end());
        verifySearchTree(tree);
        for (auto val : data) {
            EXPECT_TRUE(tree.contains(val));
        }
    }
}

TEST_F(AVLTreeTest, InsertTest) {
    for (uint32_t i = 5; i < 20; ++i) {
        auto data(generateData(2 * i, i, engine));
        AVLSearchTree<double> tree(data.begin(), data.end());

        std::uniform_real_distribution<> dist(0, i + 1);
        for (uint32_t j = 0; j < 30; ++j) {
            double insertVal = dist(engine);
            tree.insert(insertVal);
            EXPECT_TRUE(tree.contains(insertVal));
        }

        verifySearchTree(tree);
    }
}

TEST_F(AVLTreeTest, RemoveTest) {
    for (uint32_t i = 0; i < 100; ++i) {
        auto data(generateData(100, 100, engine));
        std::unique_ptr<BinarySearchTree<double>> tree(new AVLSearchTree<double>(data.begin(), data.end()));
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->remove(data[j]); 
            verifySearchTree(*tree);
        }
    }
}

/*
// timing test
TEST_F(AVLTreeTest, SortedTest) {
    for (uint32_t i = 0; i < 500; ++i) {
        auto data(generateData(1000, 1000, engine));
        std::sort(data.begin(), data.end());
        std::unique_ptr<BinarySearchTree<double>> tree(new AVLSearchTree<double>());
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->insert(data[j]);
        }
        for (uint32_t j = 0; j < data.size(); ++j) {
            tree->remove(data[j]);
        }
    }
}*/