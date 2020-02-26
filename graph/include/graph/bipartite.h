#ifndef GRAPH_BIPARTITE_H
#define GRAPH_BIPARTITE_H
#include <unordered_set>
#include <utility>

#include <structures/graph.h>

namespace graph_alg {
/**
 * Check if a graph is bipartite
 * If graph is bipartite, returns the two independent sets that make it up
 * If not, returns empty sets
 *
 * Θ(m+n)
 */
template <typename T, bool Weighted>
std::pair<std::unordered_set<T>, std::unordered_set<T>> verify_bipartite(
    const graph::graph<T, false, Weighted>& input);

/**
 * On a bipartite graph, find the maximum matching
 * Essentially Dinitz's algorithm, but with fixed unit-cost edges
 *
 * John Hopcroft, Richard Karp
 * An n^5/2 algorithm for maximum matchings in bipartite graphs
 * (1973) doi:10.1137/0202019
 *
 * Alexander Karzanov
 * An exact estimate of an algorithm for finding a maximum flow, applied to the problem on
 * representatives (1973) Problems in Cybernetics
 *
 * Θ(sqrt(V) E)
 *
 * This algorithm requires names for two dummy vertices to add to the graph during the max-flow
 * portion
 */
template <typename T, bool Weighted>
std::list<std::pair<T, T>> maximum_bipartite_matching(
    const graph::graph<T, false, Weighted>& input, const T& dummy_1, const T& dummy_2);
}

#include "../../src/graph/bipartite.cpp"

#endif // GRAPH_BIPARTITE_H