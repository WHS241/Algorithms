#ifndef GRAPH_PATH_H
#define GRAPH_PATH_H
#include <functional>
#include <list>
#include <unordered_map>
namespace graph {
    template<typename T, bool Directed, bool Weighted>
    class graph;
}

/*
Supplies algorithms for finding the shortest path(s) in a graph
All functions throw std::domain_error if no path exists
*/
namespace graph_alg {

/*
Use BFS to find path with fewest edges
*/
    template<typename T, bool Directed, bool Weighted>
std::list<T> least_edges_path(const graph::graph<T, Directed, Weighted>& src, const T& start, const T& dest);

/*
Use topological sort on a weighted DAG
O(V+E)
*/
template <typename T, typename Compare = std::less<>>
std::pair<double, std::list<T>> shortest_path_DAG(
    const graph::graph<T,true, true>& src, const T& start, const T& dest, Compare compare = Compare());
template <typename T, typename Compare = std::less<>>
std::unordered_map<T, std::pair<double, T>> shortest_path_DAG_all(
    const graph::graph<T,true, true>& src, const T& start, Compare compare = Compare());

/*
Single source shortest path
Non-negative weights
Dijkstra's algorithm: O(V^2)
Binary or Binomial heap: O((V+E) log V)
Fibonacci heap: O(V log V + E)
*/
template <typename T, bool Directed>
std::pair<double, std::list<T>> Dijkstra_single_target(const graph::graph<T, Directed, true>& src, const T& start, const T& dest);
template <typename T, bool Directed>
std::unordered_map<T, std::pair<double, T>> Dijkstra_all_targets(const graph::graph<T, Directed, true>& src, const T& start);

/*
Single source shortest path
Directed graph negative weights
Bellman-Ford: O(VE)
*/
template <typename T>
std::pair<double, std::list<T>> Bellman_Ford_single_target(const graph::graph<T, true, true>& src, const T& start, const T& dest);
template <typename T>
std::unordered_map<T, std::pair<double, T>> Bellman_Ford_all_targets(const graph::graph<T, true, true>& src, const T& start);

/*
All-pairs shortest path
Floyd-Warshall: O(V^3)
result[i][j] gives vertex k in middle of path, such that shortest path from i to
j is (shortest path from i to k) + (shortest path from k to j)
*/
template <typename T>
std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> Floyd_Warshall_all_pairs(const graph::graph<T, true, true>& src);
} // namespace graph_alg

#include <src/graph/path.cpp>

#endif // GRAPH_PATH_H
