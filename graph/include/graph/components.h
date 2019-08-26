#ifndef GRAPH_COMPONENTS_H
#define GRAPH_COMPONENTS_H
#include <list>
#include <unordered_set>

#include <structures/Graph.h>

namespace GraphAlg {
/*
Determine the connected components using DFS
*/
template <typename T> std::list<std::unordered_set<T>> connectedComponents(const Graph<T>& src);

/*
Find all articulation points of a graph
John Hopcroft, Robert Tarjan: O(V+E)
*/
template <typename T> std::unordered_set<T> articulationPoints(const Graph<T>& src);

/*
Find all strongly connected components
Tarjan: O(V+E)
*/
template <typename T>
std::list<std::unordered_set<T>> stronglyConnectedComponents(const Graph<T>& src);
} // namespace GraphAlg

#include <src/graph/components.cpp>

#endif // GRAPH_COMPONENTS_H