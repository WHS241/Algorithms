#ifndef GRAPH_COMPONENTS_H
#define GRAPH_COMPONENTS_H
#include <list>
#include <unordered_set>

#include <structures/graph.h>

namespace graph_alg {
/*
Determine the connected components using DFS
*/
template <typename T, bool Weighted> std::list<std::unordered_set<T>> connected_components(const graph::graph<T, false, Weighted>& src);

/*
Find all articulation points of a graph
John Hopcroft, Robert Tarjan: O(V+E)
*/
    template <typename T, bool Weighted> std::unordered_set<T> articulation_points(const graph::graph<T, false, Weighted>& src);

/*
Find all strongly connected components
Tarjan: O(V+E)
*/
    template <typename T, bool Weighted>
    std::list<std::unordered_set<T>> strongly_connected_components(const graph::graph<T, true, Weighted>& src);
} // namespace GraphAlg

#include <src/graph/components.cpp>

#endif // GRAPH_COMPONENTS_H