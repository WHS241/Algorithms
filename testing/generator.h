#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>

#include <structures/Graph.h>

Graph<int> random_graph(std::mt19937_64& gen, bool directed, bool weighted, bool cyclic = true,
    GraphType type = AdjList);

std::vector<double> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine);

#endif // GENERATOR_H