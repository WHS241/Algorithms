#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>

#include <Graph.h>

Graph<int> random_graph(std::mt19937_64& gen, bool directed, bool weighted, bool cyclic = true);

#endif // GENERATOR_H