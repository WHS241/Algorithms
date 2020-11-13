#ifndef GRAPH_SEARCH_H
#define GRAPH_SEARCH_H
#include <functional>
#include <list>
#include <queue>
#include <stdexcept>
#include <unordered_map>

#include <structures/graph.h>
#include <structures/partitioner.h>

// Recursive helpers for DFS
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
    typename KeyEqual, typename F1, typename F2>
static bool depth_first_helper(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>& src,
    const Vertex& current, F1& on_visit, F2& on_backtrack,
    std::unordered_map<Vertex, bool, Hash, KeyEqual>& visited)
{
    visited[current] = true;

    // account for if the user specified when to terminate early
    if constexpr (std::is_convertible_v<std::result_of_t<F1(Vertex)>, bool>) {
        if (on_visit(current))
            return true;
    } else {
        on_visit(current);
    }

    for (const Vertex& neighbor : src.neighbors(current)) {
        if (!visited[neighbor]) {
            if (depth_first_helper(src, neighbor, on_visit, on_backtrack, visited))
                return true;
            on_backtrack(current, neighbor);
        }
    }
    return false;
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
    typename KeyEqual, typename F1, typename F2>
static bool depth_first_tree_helper(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>& src,
    const Vertex& current, F1& on_visit, F2& on_backtrack)
{
    if constexpr (std::is_convertible_v<std::result_of_t<F1(Vertex)>, bool>) {
        if (on_visit(current))
            return true;
    } else {
        on_visit(current);
    }
    for (const Vertex& neighbor : src.neighbors(current)) {
        if (depth_first_tree_helper(src, neighbor, on_visit, on_backtrack))
            return true;
        on_backtrack(current, neighbor);
    }
    return false;
}

namespace graph_alg {
/*
Depth-first search on src starting with startVertex
On each vertex:
1. performs on_arrival(vertex)
2. checks for early termination (F1 returns a bool value of true; skips if no return value)
3. uses depth-first search to go to next vertex
4. performs on_backtrack(parent, child) when backtracking to parent

Requirements: F1::operator()(Vertex param) and F2::operator()(Vertex param1, Vertex param2) are
defined
*/
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args,
    typename F1 = std::function<void(const Vertex&)>,
    typename F2 = std::function<void(const Vertex&, const Vertex&)>>
void depth_first(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src, const Vertex& start,
    F1 on_arrival = [](const Vertex&) {},
    F2 on_backtrack = [](const Vertex& parent, const Vertex& child) {})
{
    static_assert(std::is_invocable_v<F1, Vertex> && std::is_invocable_v<F2, Vertex, Vertex>,
        "incompatible functions");
    std::unordered_map<Vertex, bool, Args...> visited;
    std::vector<Vertex> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const Vertex& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    depth_first_helper(src, start, on_arrival, on_backtrack, visited);
}

/*
Perform DFS, restarting if necessary
*/
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args,
    typename F1 = std::function<void(const Vertex&)>,
    typename F2 = std::function<void(const Vertex&, const Vertex&)>,
    typename F3 = std::function<void(const Vertex&)>>
void depth_first_forest(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src, const Vertex& start,
    F1 on_arrival = [](const Vertex&) {},
    F2 on_backtrack = [](const Vertex& parent, const Vertex& child) {},
    F3 on_finish_root = [](const Vertex&) {})
{
    static_assert(
        std::is_invocable_v<F1,
            Vertex> && std::is_invocable_v<F2, Vertex, Vertex> && std::is_invocable_v<F3, Vertex>,
        "incompatible functions");
    std::unordered_map<Vertex, bool, Args...> visited;
    std::vector<Vertex> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const Vertex& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    if (depth_first_helper(src, start, on_arrival, on_backtrack, visited))
        return;

    on_finish_root(start);

    auto findUnvisited = [](const std::pair<Vertex, bool>& x) { return !x.second; };

    for (auto it = std::find_if(visited.cbegin(), visited.cend(), findUnvisited);
         it != visited.cend(); it = std::find_if(visited.cbegin(), visited.cend(), findUnvisited)) {
        if (depth_first_helper(src, it->first, on_arrival, on_backtrack, visited))
            return;
        on_finish_root(it->first);
    }
}

/*
 * Tree-style depth-first
 * Infinite run-time on arbitrary graphs unless F1/F2 return values
 * O(V E) on DAGs
 */
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args,
    typename F1 = std::function<void(const Vertex&)>,
    typename F2 = std::function<void(const Vertex&, const Vertex&)>>
void depth_first_tree(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src, const Vertex& start,
    F1 on_arrival = [](const Vertex&) {},
    F2 on_backtrack = [](const Vertex& parent, const Vertex& child) {})
{
    static_assert(std::is_invocable_v<F1, Vertex> && std::is_invocable_v<F2, Vertex, Vertex>,
        "incompatible functions");
    std::vector<Vertex> vertices = src.vertices();
    if (vertices.empty())
        return;

    if (std::find(vertices.begin(), vertices.end(), start) == vertices.end())
        throw std::out_of_range("Vertex does not exist");

    depth_first_tree_helper(src, start, on_arrival, on_backtrack);
}

/*
Breadth-first search on src
On each vertex:
1. performs on_visit(vertex)
2. Goes to next vertex BFS

Requirements: F::operator()(T param) is defined
*/
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args,
    typename F = std::function<void(const Vertex&)>>
void breadth_first(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src, const Vertex& start,
    F on_visit = [](const Vertex&) {})
{
    static_assert(std::is_invocable_v<F, Vertex>, "incompatible functions");
    std::unordered_map<Vertex, bool, Args...> visited;
    std::vector<Vertex> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const Vertex& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    std::queue<Vertex, std::list<Vertex>> toVisit;
    toVisit.push(start);
    while (!toVisit.empty()) {
        Vertex current = toVisit.front();
        toVisit.pop();
        if (!visited[current]) {
            if constexpr (std::is_convertible_v<std::result_of_t<F(Vertex)>, bool>) {
                if (on_visit(current))
                    return;
            } else {
                on_visit(current);
            }
            visited[current] = true;

            for (const Vertex& neighbor : src.neighbors(current)) {
                if (!visited[neighbor]) {
                    toVisit.push(neighbor);
                }
            }
        }
    }
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
std::list<Vertex> generate_lex_bfs(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& graph,
    const Vertex& first_vertex)
{
    if (graph.get_translation().find(first_vertex) == graph.get_translation().end())
        throw std::out_of_range("Does not contain given vertex");

    partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...> setup_device(graph);
    std::list<Vertex> result;
    setup_device.remove_vertex(first_vertex);
    result.push_back(first_vertex);

    while (result.size() != graph.order()) {
        // Partition on last vertex added
        auto ptrs = setup_device.partition(result.back(), false);
        setup_device.clean(ptrs);

        const std::list<Vertex>& next_set = setup_device.get_all().front();

        // Doesn't matter which one we pick
        Vertex next = next_set.back();
        result.push_back(next);

        // Don't want to repeat vertices
        setup_device.remove_vertex(next);
    }

    return result;
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
std::list<Vertex> generate_lex_bfs(
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& graph)
{
    return graph.order() == 0 ? std::list<Vertex>()
                              : generate_lex_bfs(graph, graph.get_translation().begin()->first);
}

/*
Returns a topological sort of the input graph
Throws if input is not directed acyclic.
Arthur B. Kahn
Topological sorting of large networks
(1962) doi:10.1145/368996.369025
Θ(V+E)
*/
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::vector<Vertex> topological_sort(
    const graph::graph<Vertex, true, Weighted, EdgeWeight, Args...>& src)
{
    std::unordered_map<Vertex, uint32_t, Args...> in_degree;
    std::list<Vertex> candidates; // vertices with in-degree 0
    std::vector<Vertex> result; // stores topological sort

    // populate map
    for (const Vertex& vert : src.vertices())
        in_degree[vert] = 0;

    // correct map values
    for (const Vertex& source : src.vertices())
        for (const Vertex& terminal : src.neighbors(source))
            ++in_degree[terminal];

    for (const std::pair<Vertex, uint32_t>& value : in_degree)
        if (value.second == 0)
            candidates.push_back(value.first);

    while (!candidates.empty()) {
        Vertex current = candidates.front();
        candidates.pop_front();
        result.push_back(current);
        for (const Vertex& terminal : src.neighbors(current)) {
            --in_degree[terminal];
            if (in_degree[terminal] == 0)
                candidates.push_back(terminal);
        }
    }

    if (result.size() != src.order())
        throw std::invalid_argument("Not DAG");

    return result;
}
} // namespace graph_alg

#endif // GRAPH_TRAVERSAL_H
