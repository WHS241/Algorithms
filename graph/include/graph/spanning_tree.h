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
 * Otakar Borůvka (1926)
 * Θ(E log V)
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Boruvka(const graph::graph<T, false, true>& input);

/*
 * Vojtěch Jarník (1930), Robert Prim (1957), Edsger Dijkstra (1959)
 * Θ(E + V log V) with Fibonacci heap
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Prim(const graph::graph<T, false, true>& input);

/*
 * Joseph Kruskal (1956)
 * Θ(E log V) with sorting and UNION-FIND
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Kruskal(const graph::graph<T, false, true>& input);
}

#include <src/graph/spanning_tree.cpp>

#endif // GRAPH_ALG_SPANNING_TREE_H
