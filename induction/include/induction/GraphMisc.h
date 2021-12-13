#ifndef GRAPH_MISC_H
#define GRAPH_MISC_H

#include <structures/graph.h>

namespace induction {

// Celebrity problem
// Find a vertex (person) with in-degree V-1 and out-degree 0
// throw std::invalid_argument if none exists
// Stål Aanderaa (1973): Θ(V) for adjacency matrices, Θ(E) for adjacency lists
template<typename T, bool Weighted> T celebrity(const graph::graph<T, true, Weighted>& src);
} // namespace induction

#include <src/induction/GraphMisc.cpp>

#endif // !GRAPH_MISC_H