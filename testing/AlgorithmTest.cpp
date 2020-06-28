#include <algorithm>
#include <fstream>
#include <queue>
#include <random>

#include <gtest/gtest.h>

#include <structures/dynamic_matrix.h>
#include <structures/graph.h>
#include <structures/matrix.h>

#include <graph/components.h>
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

TEST_F(AlgorithmTest, Matrix_Test) {
    matrix<int, 2, 3> test = {std::array<int, 3>{0,1, 2}, std::array<int, 3>{3,4,5}};
    matrix<int, 3, 6> test_2 = {std::array<int, 6>{4,5,8, 9,10,12},std::array<int, 6>{13,14,15, 1,5,4}, std::array<int, 6>{8,-1, 4,8,0,2}};
    matrix<int,2,6> result = {std::array<int, 6>{29, 12, 23, 17, 5, 8},std::array<int, 6>{104, 66, 104, 71, 50, 62}};
    EXPECT_EQ(test * test_2, result);

    std::uniform_int_distribution<int> gen(-10, 10);
    matrix<int, 71, 85> input_a;
    matrix<int, 85, 43> input_b;
    for(uint32_t i = 0; i < 50; ++i) {
        for(uint32_t j = 0; j < 71; ++j)
            for(uint32_t k = 0; k < 85; ++k) 
                input_a[j][k] = gen(engine);

        for(uint32_t j = 0; j < 85; ++j)
            for(uint32_t k = 0; k < 43; ++k)
                input_b[j][k] = gen(engine);

        matrix<int, 71, 43> brute_res;
        for(uint32_t j = 0; j < 71; ++j)
            for(uint32_t k = 0; k < 43; ++k) {
                brute_res[j][k] = 0;

                for(uint32_t l = 0; l < 85; ++l)
                    brute_res[j][k] += input_a[j][l] * input_b[l][k];
            }

        matrix<int, 71, 43> Strassen_result = input_a * input_b;
        EXPECT_EQ(brute_res, Strassen_result);
        if(brute_res != Strassen_result) {
            for(uint32_t j = 0; j < 71; ++j) {
                for(uint32_t k = 0; k < 85; ++k)
                    std::cout << input_a[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(uint32_t j = 0; j < 85; ++j) {
                for(uint32_t k = 0; k < 43; ++k)
                    std::cout << input_b[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(uint32_t j = 0; j < 71; ++j) {
                for(uint32_t k = 0; k < 43; ++k)
                    std::cout << brute_res[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(uint32_t j = 0; j < 71; ++j) {
                for(uint32_t k = 0; k < 43; ++k)
                    std::cout << Strassen_result[j][k] << " ";
                std::cout << std::endl;
            }

        }
    }

}
