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
template <typename T, bool Directed, bool Weighted, typename F1 = std::function<void(const T&)>,
    typename F2 = std::function<void(const T&, const T&)>>
void depth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival = [](const T&) {},
    F2 on_backtrack = [](const T& parent, const T& child) {});

/*
Perform DFS, restarting if necessary
*/
template <typename T, bool Directed, bool Weighted, typename F1 = std::function<void(const T&)>,
    typename F2 = std::function<void(const T&, const T&)>,
    typename F3 = std::function<void(const T&)>>
void depth_first_forest(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival = [](const T&) {},
    F2 on_backtrack = [](const T& parent, const T& child) {}, F3 on_finish_root = [](const T&) {});

/*
 * Tree-style depth-first
 * Infinite run-time unless DAG or F1/F2 return values
 */
template <typename T, bool Directed, bool Weighted, typename F1 = std::function<void(const T&)>,
    typename F2 = std::function<void(const T&, const T&)>>
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
template <typename T, bool Directed, bool Weighted, typename F = std::function<void(const T&)>>
void breadth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F on_visit = [](const T&) {});

/*
Returns a topological sort of the input graph
Throws if input is not directed acyclic.
Arthur B. Kahn
Topological sorting of large networks
(1962) doi:10.1145/368996.369025
Θ(V+E)
*/
template <typename T, bool Weighted>
std::vector<T> topological_sort(const graph::graph<T, true, Weighted>& src);
} // namespace graph_alg

#include "../../src/graph/search.tpp"

#endif // GRAPH_TRAVERSAL_H