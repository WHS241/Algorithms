// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_H
#define GRAPH_CLOSURE_H

#include <cstdint>

// Forward declarations
namespace graph {
template <typename T, bool Directed, bool Weighted> class graph;
}

namespace graph_alg {
/**
 * An assortment of closure algorithms on graphs
 */

/**
 * find k-core: maximum induced subgraph with all degrees >= k
 * David W. Matula and Leland D. Beck:
 * Smallest-Last Ordering and Clustering and Graph Coloring Algorithms
 * (1983) doi:10.1145/2402.322385
 * Θ(V)
 */
template <typename T, bool Weighted>
graph::graph<T, false, Weighted> k_core(graph::graph<T, false, Weighted> src, uint32_t k);

/**
 * Find the transitive closure of a graph
 * Θ(mn)
 */
template <typename T, bool Directed, bool Weighted>
graph::graph<T, Directed, Weighted> transitive_closure(
    const graph::graph<T, Directed, Weighted>& src);
}

#include "../../src/graph/closure.cpp"

#endif // GRAPH_CLOSURE_H