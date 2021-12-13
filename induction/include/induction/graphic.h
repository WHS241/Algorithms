#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <list>
#include <utility>

namespace induction {
struct Silhouette {
    int leftIndex;
    int rightIndex;
    int height;
};

struct Skyline {
    int leftEnd;
    std::list<std::pair<int, int>> section; // first: height, second: right end
};

// generate skyline from buildings
// divide and conquer: Θ(n log n)
// RandomIt not necessary, but helpful
// It::value_type must be of type Silhouette
template<typename It> Skyline generateSkyline(It first, It last);
} // namespace induction

#include "../../src/induction/graphic.cpp"

#endif // !GRAPHIC_H
