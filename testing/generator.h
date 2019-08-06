#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>

#include <structures/Graph.h>

Graph<int> random_graph(std::mt19937_64& gen, bool directed, bool weighted, bool cyclic = true, GraphType type = AdjList);

#endif // GENERATOR_H