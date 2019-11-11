#ifndef GRAPH_SEARCH_H
#define GRAPH_SEARCH_H
#include <functional>

#include <structures/graph.h>

namespace graph_alg {
/*
Depth-first search on src starting with startVertex
On each vertex:
1. performs on_arrival(vertex)
2. checks for early termination (F1 returns a bool value of true; skips if no return value)
3. uses depth-first search to go to next vertex
4. performs on_backtrack(parent, child) when backtracking to parent

Requirements: F1::operator()(T param) and F2::operator()(T param1, T param2) are
defined
*/
template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
void depth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival = [](const T&) {},
    F2 on_backtrack = [](const T& parent, const T& child) {});

/*
Perform DFS, restarting if necessary
*/
template <typename T, bool Directed, bool Weighted, typename F1, typename F2, typename F3>
void depth_first_forest(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival = [](const T&) {},
    F2 on_backtrack = [](const T& parent, const T& child) {}, F3 on_finish_root = [](const T&) {});

/*
 * Tree-style depth-first
 * May visit a vertex multiple times
 */
template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
void depth_first_tree(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival = [](const T&) {},
    F2 on_backtrack = [](const T& parent, const T& child) {});

/*
Breadth-first search on src
On each vertex:
1. performs on_visit(vertex)
2. Goes to next vertex BFS

Requirements: F::operator()(T param) is defined
*/
template <typename T, bool Directed, bool Weighted, typename F>
void breadth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F on_visit = [](const T&) {});
} // namespace graph_alg

#include <src/graph/search.cpp>

#endif // GRAPH_TRAVERSAL_H