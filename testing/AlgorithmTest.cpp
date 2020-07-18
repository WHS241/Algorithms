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

TEST_F(AlgorithmTest, Dynamic_Matrix_Test) {
    dynamic_matrix<int> test = {{0,1, 2}, {3,4,5}};
    dynamic_matrix<int> test_2 = {{4,5,8, 9,10,12},{13,14,15, 1,5,4}, {8,-1, 4,8,0,2}};
    dynamic_matrix<int> exp = {{29, 12, 23, 17, 5, 8},{104, 66, 104, 71, 50, 62}};

    dynamic_matrix<int> result = test * test_2;
    if (result != exp) {

            for(std::size_t j = 0; j < 2; ++j) {
                for(std::size_t k = 0; k < 3; ++k)
                    std::cout << test[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < 3; ++j) {
                for(std::size_t k = 0; k < 6; ++k)
                    std::cout << test_2[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < 2; ++j) {
                for(std::size_t k = 0; k < 6; ++k)
                    std::cout << exp[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < 2; ++j) {
                for(std::size_t k = 0; k < 6; ++k)
                    std::cout << result[j][k] << " ";
                std::cout << std::endl;
            }
    }
    
    ASSERT_EQ(test * test_2, exp);

    std::uniform_int_distribution<std::size_t> gen(1, 100);
    for (int i = 0; i < 100; ++i) {
        std::size_t m = gen(engine);
        std::size_t n = gen(engine);
        std::size_t p = gen(engine);
        dynamic_matrix<std::size_t> lhs(m, n);
        dynamic_matrix<std::size_t> rhs(n, p);
        for(std::size_t j = 0; j < m; ++j)
            for(std::size_t k = 0; k < n; ++k) 
                lhs[j][k] = gen(engine);

        for(std::size_t j = 0; j < n; ++j)
            for(std::size_t k = 0; k < p; ++k)
                rhs[j][k] = gen(engine);

        dynamic_matrix<std::size_t> brute_force(m, p);
        for(std::size_t j = 0; j < m; ++j)
            for(std::size_t k = 0; k < p; ++k) {
                brute_force[j][k] = 0;

                for(std::size_t l = 0; l < n; ++l)
                    brute_force[j][k] += lhs[j][l] * rhs[l][k];
            }
        
        dynamic_matrix<std::size_t> Strassen_result = lhs * rhs;
        EXPECT_EQ(brute_force, Strassen_result);
        if(brute_force != Strassen_result) {
            for(std::size_t j = 0; j < m; ++j) {
                for(std::size_t k = 0; k < n; ++k)
                    std::cout << lhs[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < n; ++j) {
                for(std::size_t k = 0; k < p; ++k)
                    std::cout << rhs[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < m; ++j) {
                for(std::size_t k = 0; k < p; ++k)
                    std::cout << brute_force[j][k] << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for(std::size_t j = 0; j < m; ++j) {
                for(std::size_t k = 0; k < p; ++k)
                    std::cout << Strassen_result[j][k] << " ";
                std::cout << std::endl;
            }
            break;
        }
    }

}
