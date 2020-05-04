#ifndef GRAPH_COMPONENTS_H
#define GRAPH_COMPONENTS_H
#include <list>
#include <unordered_set>

#include <structures/graph.h>

namespace graph_alg {
/*
Determine the connected components using DFS
*/
template <typename T, bool Weighted, typename EdgeType, typename... Args>
std::list<std::unordered_set<T, Args...>> connected_components(
    const graph::graph<T, false, Weighted, EdgeType, Args...>& src);

/*
Find all articulation points of a graph
John Hopcroft, Robert Tarjan
Algorithm 447: efficient algorithms for graph manipulation
(1973) doi:10.1145/362248.362272
Θ(V+E)
*/
template <typename T, bool Weighted, typename EdgeType, typename... Args>
std::unordered_set<T, Args...> articulation_points(
    const graph::graph<T, false, Weighted, EdgeType, Args...>& src);

/*
Find all strongly connected components
Tarjan
Depth-First Search and Linear Graph Algorithms
(1972) doi:10.1137/0201010
Θ(V+E)
*/
template <typename T, bool Weighted, typename EdgeType, typename... Args>
std::list<std::unordered_set<T, Args...>> strongly_connected_components(
    const graph::graph<T, true, Weighted, Args...>& src);
} // namespace GraphAlg

#include "../../src/graph/components.tpp"

#endif // GRAPH_COMPONENTS_H