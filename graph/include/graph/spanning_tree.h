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
 * Otakar Boruvka (1926): O(E log V)
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Boruvka(const graph::graph<T, false, true>& input);

/*
 * Vojtech Jarnik (1930) and Robert Prim (1957)
 * O(E + V log V) with Fibonacci heap
 */
template <typename T>
graph::graph<T, false, true> minimum_spanning_Prim(const graph::graph<T, false, true>& input);

template <typename T>
graph::graph<T, false, true> minimum_spanning_Kruskal(const graph::graph<T, false, true>& input);
}

#include <src/graph/spanning_tree.cpp>

#endif // GRAPH_ALG_SPANNING_TREE_H
