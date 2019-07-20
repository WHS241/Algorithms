#include <random>

#include <gtest/gtest.h>

#include <induction/graphic.h>

class GraphicTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::random_device base;
        engine = std::mt19937_64(base());
    }

    std::mt19937_64 engine;
};

// currenty requires manual testing via debugger
// TODO: automate test
TEST_F(GraphicTest, SkylineTest) {
    for (uint32_t i = 0; i < 20; ++i) {
        std::uniform_int_distribution<uint32_t> sizeDist(1, 100);
        std::vector<Graphic::Silhouette> input(5);
        std::uniform_int_distribution<int32_t> inputGen(1, 10);
        for (auto& building : input) {
            building.height = inputGen(engine);
            building.leftIndex = inputGen(engine);
            do
                building.rightIndex = inputGen(engine);
            while (building.rightIndex == building.leftIndex);
            
            if (building.leftIndex > building.rightIndex)
                std::swap(building.leftIndex, building.rightIndex);
        }

        auto result = Graphic::generateSkyline(input.begin(), input.end());
    }
}