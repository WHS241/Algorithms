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
 *
 * Ford-Fulkerson method: runtime dependent on flow-finding strategy
 * Bad strategies may not necessarily converge on true cost for irrational values (infinite runtime)
 *
 * L. R. Ford, D. R. Fulkerson
 * Maximal flow through a network
 * (1956) doi:10.4153/CJM-1956-045-5
 *
 * Conditions: Function takes in the residual graph and source/sink vertices
 * Must return augmenting path(s) or throw a graph_alg::no_path_exception
 * in order to signal no augmenting path found
 */
template <typename T, bool Directed, typename Function>
graph::graph<T, Directed, true> Ford_Fulkerson(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink, Function f);

/**
 * Jack Edmonds, Richard Karp
 * Theoretical Improvements in Algorithmic Efficiency for Network Flow Problems
 * (1972) doi:10.1145/321694.321699
 * Θ(VE^2)
 */
template <typename T, bool Directed>
graph::graph<T, Directed, true> Edmonds_Karp(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink);

/**
 * Yefim Dinitz
 * An algorithm for the solution of the max-flow problem with the polynomial estimation
 * (1970) - Soviet Doklady
 * Θ(V^2 E)
 */
template <typename T, bool Directed>
graph::graph<T, Directed, true> Dinitz(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink);

template <typename T> struct cut_edge {
    cut_edge(const T& start, const T& end)
        : start(start)
        , end(end) {};
    T start;
    T end;
};

/**
 * Minimum cut from start to terminal
 * Max-flow-min-cut theorem: minimum cut is the edges that are saturated in maximum flow
 */
template <typename T, bool Directed>
std::list<cut_edge<T>> minimum_cut(
    const graph::graph<T, Directed, true>& input, const T& start, const T& terminal);

}

#include "../../src/graph/max_flow_min_cut.tpp"

#endif // GRAPH_ALG_MIN_FLOW_H
