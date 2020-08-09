#include <cmath>
#include <random>
#include <unordered_set>

#include <gtest/gtest.h>

#include <structures/graph.h>

#include <graph/closure.h>
#include <graph/order_dimension.h>

#include <special_case/model.h>

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
        
TEST_F(AlgorithmTest, Circle_Graph_Clique) {
    std::uniform_real_distribution<double> point_dist;
    for(int i = 0; i < 100; ++i) {
        std::unordered_set<double> used_points;
        std::vector<std::pair<double, double>> chords(100);
        for(int j = 0; j < 100; ++j) {
            std::pair<double, double> next;
            while (next.first < 0. || next.first > 1. || used_points.count(next.first) != 0)
                next.first = point_dist(engine);
            used_points.insert(next.first);
            while (next.second < 0. || next.second > 1. || used_points.count(next.second) != 0)
                next.second = point_dist(engine);
            used_points.insert(next.second);
            if (next.second < next.first)
                std::swap(next.first, next.second);
            chords[j] = next;
        }

        std::vector<std::size_t> result = special_case::model_max_clique_circle_graph(chords.begin(), chords.end());
        ASSERT_LE(result.size(), chords.size());
        ASSERT_NE(result.size(), 0);

        std::vector<bool> in_set(100, false);
        for (std::size_t x : result)
            in_set.at(x) = true;

        for (std::size_t j = 0; j < 100; ++j) {
            bool intersects_all = true;
            for (std::size_t k = 0; intersects_all && k < 100; ++k)
                if (j != k && in_set[k]) 
                    intersects_all = ((chords[j].first < chords[k].first) && (chords[k].first < chords[j].second) && (chords[j].second < chords[k].second))
                        || ((chords[k].first < chords[j].first) && (chords[j].first < chords[k].second) && (chords[k].second < chords[j].second));
            ASSERT_EQ(intersects_all, in_set[j]);
        }
    }

    }



