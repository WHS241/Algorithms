#ifndef GRAPH_SEARCH_H
#define GRAPH_SEARCH_H
#include <functional>

#include <structures/Graph.h>

namespace GraphAlg {
/*
Depth-first search on src starting with startVertex
On each vertex:
1. performs onVisitFunction(vertex)
2. uses depth-first search to go to next vertex
3. performs onBacktrackFunction(parent, child) when backtracking to parent

Requirements: F1::operator()(T param) and F2::operator()(T param1, T param2) are
defined
*/
template <typename T, typename F1 = std::function<void(T)>, typename F2 = std::function<void(T, T)>>
void depthFirst(
    const Graph<T>& src, const T& startVertex, F1 onVisitFunction = [](const T&) {},
    F2 onBacktrackFunction = [](const T& parent, const T& child) {});

/*
Perform DFS, restarting if necessary
*/
template <typename T, typename F1 = std::function<void(T)>, typename F2 = std::function<void(T, T)>,
    typename F3 = std::function<void(T)>>
void depthFirstForest(
    const Graph<T>& src, const T& startVertex, F1 onVisitFunction = [](const T&) {},
    F2 onBacktrackFunction = [](const T& parent, const T& child) {},
    F3 onBacktrackRoot = [](const T&) {});

/*
Breadth-first search on src
On each vertex:
1. performs F(vertex)
2. Goes to next vertex BFS

Requirements: F::operator()(T param) is defined
*/
template <typename T, typename F = std::function<void(T)>>
void breadthFirst(
    const Graph<T>& src, const T& startVertex, F function = [](const T&) {});
} // namespace GraphAlg

#include <src/graph/search.cpp>

#endif // GRAPH_TRAVERSAL_H