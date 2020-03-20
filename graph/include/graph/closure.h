// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_H
#define GRAPH_CLOSURE_H

#include <cstdint>

// Forward declarations
namespace graph {
template <typename VertexType, bool Directed, bool Weighted, typename EdgeType, typename Hash, typename KeyEqual> class graph;
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

/**
 * Chvatal-Bondy closure
 * If the sum of the degrees of two vertices exceed k, there should be an edge between them.
 * 
 * Jeremy Spinrad
 * A note on computing graph closures
 * (2004) doi.org/10.1016/S0012-365X(03)00316-9
 * Θ(output size)
 */
template<typename T, bool Weighted>
graph::graph<T, false, Weighted> Chvatal_Bondy_closure(graph::graph<T, false, Weighted> src, uint32_t k);
}

#include "../../src/graph/closure.tpp"

#endif // GRAPH_CLOSURE_H