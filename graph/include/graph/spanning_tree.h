//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_SPANNING_TREE_H
#define GRAPH_ALG_SPANNING_TREE_H

#include <structures/graph.h>

/**
 * Algorithms to find the minimum spanning tree
 * Runtimes are given on adjacency lists
 */
namespace graph_alg {
/*
 * Otakar Borůvka
 * O jistém problému minimálním
 * (1926) Práce Mor. Přírodověd. Spol. V Brně III
 * Θ(E log V)
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Boruvka(const graph::graph<T, false, true>& input);

/*
 * Vojtěch Jarník
 * O jistém problému minimálním
 * (1930) hdl:10338.dmlcz/500726
 * Robert Prim
 * Shortest connection networks and some generalizations
 * (1957) doi:10.1002/j.1538-7305.1957.tb01515.x
 * Edsger Dijkstra
 * A note on two problems in connexion with graphs
 * (1959) doi:10.1007/BF01386390
 *
 * Θ(V^2) with array
 * Θ(E + V log V) with Fibonacci heap
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Prim(const graph::graph<T, false, true>& input);

/*
 * Joseph Kruskal
 * On the shortest spanning subtree of a graph and the traveling salesman problem
 * (1956) doi:10.1090/S0002-9939-1956-0078686-7
 * Θ(E log V) with sorting and UNION-FIND
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Kruskal(const graph::graph<T, false, true>& input);
}

#include <src/graph/spanning_tree.cpp>

#endif // GRAPH_ALG_SPANNING_TREE_H
