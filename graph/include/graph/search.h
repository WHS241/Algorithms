#ifndef GRAPH_SEARCH_H
#define GRAPH_SEARCH_H
#include <functional>

#include <structures/graph.h>

namespace graph_alg {
/*
Depth-first search on src starting with startVertex
On each vertex:
1. performs onVisitFunction(vertex)
2. uses depth-first search to go to next vertex
3. performs onBacktrackFunction(parent, child) when backtracking to parent

Requirements: F1::operator()(T param) and F2::operator()(T param1, T param2) are
defined
*/
template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
void depthFirst(
    const graph::graph<T, Directed, Weighted>& src, const T& startVertex, F1 onVisitFunction = [](const T&) {},
    F2 onBacktrackFunction = [](const T& parent, const T& child) {});

/*
Perform DFS, restarting if necessary
*/
template <typename T, bool Directed, bool Weighted, typename F1, typename F2, typename F3>
void depthFirstForest(
    const graph::graph<T, Directed, Weighted>& src, const T& startVertex, F1 onVisitFunction = [](const T&) {},
    F2 onBacktrackFunction = [](const T& parent, const T& child) {},
    F3 onBacktrackRoot = [](const T&) {});

/*
Breadth-first search on src
On each vertex:
1. performs F(vertex)
2. Goes to next vertex BFS

Requirements: F::operator()(T param) is defined
*/
    template <typename T, bool Directed, bool Weighted, typename F>
void breadthFirst(
    const graph::graph<T, Directed, Weighted>& src, const T& startVertex, F function = [](const T&) {});
} // namespace graph_alg

#include <src/graph/search.cpp>

#endif // GRAPH_TRAVERSAL_H