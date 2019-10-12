//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_MIN_FLOW_H
#define GRAPH_ALG_MIN_FLOW_H

#include <structures/graph.h>

namespace graph_alg {
/**
 * Max flow algorithm
 * The weight on each edge of input denotes the capacity
 * Returns a weighted graph where each weight denotes flow
 * Ford-Fulkerson method: runtime dependent on flow-finding strategy
 * May not necessarily converge on true cost for irrational values
 *
 * Conditions: Function takes in the residual graph and source/sink vertices
 * Must return augmenting path(s) or throw a graph_alg::no_path_exception
 * in order to signal no augmenting path found
 */
template <typename T, bool Directed, typename Function>
graph::graph<T, Directed, true> Ford_Fulkerson(
    const graph::graph<T, Directed, true>& input, T source, T sink, Function f);

/**
 * Jack Edmonds and Richard Karp (1972): O(VE^2)
 */
template <typename T, bool Directed>
graph::graph<T, Directed, true> Edmonds_Karp(
    const graph::graph<T, Directed, true>& input, T source, T sink);

/**
 * Yefim Dinitz (1972): O(V^2 E)
 */
template <typename T, bool Directed>
graph::graph<T, Directed, true> Dinitz(
    const graph::graph<T, Directed, true>& input, T source, T sink);
}

#include <src/graph/min_flow_max_cut.cpp>

#endif // GRAPH_ALG_MIN_FLOW_H
