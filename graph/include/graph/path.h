#ifndef GRAPH_PATH_H
#define GRAPH_PATH_H
#include <functional>
#include <list>
#include <unordered_map>
namespace graph {
template <typename T, bool Directed, bool Weighted> class graph;
}

/*
Supplies algorithms for finding the shortest path(s) in a graph
All functions throw no_path_exception (std::domain_error) if no path exists

Algorithms on weighted graphs:
Single target: result.first is length, result.second is path (does not include start)
Single source, all targets: result[v] = (total length, immediate predecessor)
All pairs:
    result[v][v] = (0, v)
    result[u][v] = (total length u -> v,
                            vertex m s.t. result[u][v] = result[u][m] + result[m][v])
                            m != v, and m != u unless single-edge path is shortest
*/
namespace graph_alg {

class no_path_exception : public std::domain_error {
public:
    no_path_exception()
        : std::domain_error("No path") {};
};

/*
Use BFS to find path with fewest edges
*/
template <typename T, bool Directed, bool Weighted>
std::list<T> least_edges_path(
    const graph::graph<T, Directed, Weighted>& src, const T& start, const T& dest);

/*
Use topological sort on a weighted DAG
Can be used with other comparison operators (e.g. find longest path)
*/
template <typename T, typename Compare = std::less<>>
std::pair<double, std::list<T>> shortest_path_DAG(const graph::graph<T, true, true>& src,
    const T& start, const T& dest, Compare compare = Compare());
template <typename T, typename Compare = std::less<>>
std::unordered_map<T, std::pair<double, T>> shortest_path_DAG_all_targets(
    const graph::graph<T, true, true>& src, const T& start, Compare compare = Compare());

/*
Single source shortest path
Non-negative weights

Edsger Dijkstra 
A note on two problems in connexion with graphs
(1959) doi:10.1007/BF01386390
Θ(V^2)

Binary heap: Θ((V+E) log V)
Fibonacci heap: Θ(V log V + E)
*/
template <typename T, bool Directed>
std::pair<double, std::list<T>> Dijkstra_single_target(
    const graph::graph<T, Directed, true>& src, const T& start, const T& dest);
template <typename T, bool Directed>
std::unordered_map<T, std::pair<double, T>> Dijkstra_all_targets(
    const graph::graph<T, Directed, true>& src, const T& start);
/*
 F function determines whether to halt early (returns true to halt)
 */
template <typename T, bool Directed, typename F>
std::unordered_map<T, std::pair<double, T>> Dijkstra_partial(
    const graph::graph<T, Directed, true>& src, const T& start, F function);

/*
Single source shortest path
Directed graph negative weights
Alfonso Shimbel
Structure in communication nets
(1955)
Lester Ford Jr. 
Network Flow Theory
(1956) RAND Corporation, Paper P-923
Edward F. Moore
The shortest path through a maze
(1957) MR 0114710
Richard Bellman
On a routing problem
(1958) MR 0102435

Θ(VE)
*/
template <typename T>
std::pair<double, std::list<T>> Bellman_Ford_single_target(
    const graph::graph<T, true, true>& src, const T& start, const T& dest);
template <typename T>
std::unordered_map<T, std::pair<double, T>> Bellman_Ford_all_targets(
    const graph::graph<T, true, true>& src, const T& start);

/*
All-pairs shortest path

Bernard Roy 
Transitivité et connexité
(1959) C. R. Acad. Sci. Paris
Stephen Warshall 
A theorem on Boolean matrices
(1962) doi:10.1145/321105.321107
Robert Floyd
Algorithm 97: Shortest Path
(1962) doi:10.1145/367766.368168

Θ(V^3)
*/
template <typename T>
std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> Floyd_Warshall_all_pairs(
    const graph::graph<T, true, true>& src);

/*
All-pairs shortest path
Donald B. Johnson
Efficient algorithms for shortest paths in sparse networks
(1977) doi:10.1145/321992.321993

Θ(V^2 log V + VE)

This algorithm requires a vertex name for a temporary additional vertex
*/
template <typename T>
std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> Johnson_all_pairs(
    const graph::graph<T, true, true>& src, const T& new_vertex);
} // namespace graph_alg

#include <src/graph/path.cpp>

#endif // GRAPH_PATH_H
