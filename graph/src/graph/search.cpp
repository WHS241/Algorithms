#ifndef GRAPH_SEARCH_CPP
#define GRAPH_SEARCH_CPP
#include <list>
#include <queue>
#include <stdexcept>
#include <unordered_map>

// Recursive helpers for DFS
template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
static bool depth_first_helper(const graph::graph<T, Directed, Weighted>& src, const T& current,
    F1& on_visit, F2& on_backtrack, std::unordered_map<T, bool>& visited)
{
    visited[current] = true;
    if constexpr (std::is_convertible_v<std::result_of_t<F1(T)>, bool>) {
        if (on_visit(current))
            return true;
    } else {
        on_visit(current);
    }
    for (const T& neighbor : src.neighbors(current)) {
        if (!visited[neighbor]) {
            depth_first_helper(src, neighbor, on_visit, on_backtrack, visited);
            on_backtrack(current, neighbor);
        }
    }
    return false;
}
template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
static bool depth_first_tree_helper(const graph::graph<T, Directed, Weighted>& src,
    const T& current, F1& on_visit, F2& on_backtrack)
{
    if constexpr (std::is_convertible_v<std::result_of_t<F1(T)>, bool>) {
        if (on_visit(current))
            return true;
    } else {
        on_visit(current);
    }
    for (const T& neighbor : src.neighbors(current)) {
        depth_first_tree_helper(src, neighbor, on_visit, on_backtrack);
        on_backtrack(current, neighbor);
    }
    return false;
}

template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
void graph_alg::depth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival, F2 on_backtrack)
{
    static_assert(
        std::is_invocable_v<F1, T> && std::is_invocable_v<F2, T, T>, "incompatible functions");
    std::unordered_map<T, bool> visited;
    std::vector<T> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    depth_first_helper(src, start, on_arrival, on_backtrack, visited);
}

template <typename T, bool Directed, bool Weighted, typename F1, typename F2, typename F3>
void graph_alg::depth_first_forest(const graph::graph<T, Directed, Weighted>& src, const T& start,
    F1 on_arrival, F2 on_backtrack, F3 on_finish_root)
{
    static_assert(
        std::is_invocable_v<F1, T> && std::is_invocable_v<F2, T, T> && std::is_invocable_v<F3, T>,
        "incompatible functions");
    std::unordered_map<T, bool> visited;
    std::vector<T> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    if (depth_first_helper(src, start, on_arrival, on_backtrack, visited))
        return;

    on_finish_root(start);

    auto findUnvisited = [](const std::pair<T, bool>& x) { return !x.second; };

    for (auto it = std::find_if(visited.cbegin(), visited.cend(), findUnvisited);
         it != visited.cend(); it = std::find_if(visited.cbegin(), visited.cend(), findUnvisited)) {
        if (depth_first_helper(src, it->first, on_arrival, on_backtrack, visited))
            return;
        on_finish_root(it->first);
    }
}

template <typename T, bool Directed, bool Weighted, typename F1, typename F2>
void graph_alg::depth_first_tree(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F1 on_arrival, F2 on_backtrack)
{
    static_assert(
        std::is_invocable_v<F1, T> && std::is_invocable_v<F2, T, T>, "incompatible functions");
    std::vector<T> vertices = src.vertices();
    if (vertices.empty())
        return;

    if (std::find(vertices.begin(), vertices.end(), start) == vertices.end())
        throw std::out_of_range("Vertex does not exist");

    depth_first_tree_helper(src, start, on_arrival, on_backtrack);
}

template <typename T, bool Directed, bool Weighted, typename F>
void graph_alg::breadth_first(
    const graph::graph<T, Directed, Weighted>& src, const T& start, F on_visit)
{
    static_assert(std::is_invocable_v<F, T>, "incompatible functions");
    std::unordered_map<T, bool> visited;
    std::vector<T> vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(start) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    std::queue<T, std::list<T>> toVisit;
    toVisit.push(start);
    while (!toVisit.empty()) {
        T current = toVisit.front();
        toVisit.pop();
        if (!visited[current]) {
            if constexpr (std::is_convertible_v<std::result_of_t<F(T)>, bool>) {
                if (on_visit(current))
                    return;
            } else {
                on_visit(current);
            }
            visited[current] = true;

            for (const T& neighbor : src.neighbors(current)) {
                if (!visited[neighbor]) {
                    toVisit.push(neighbor);
                }
            }
        }
    }
}

template <typename T, bool Weighted>
std::vector<T> graph_alg::topological_sort(const graph::graph<T, true, Weighted>& src) {
    std::unordered_map<T, uint32_t> in_degree;
    std::list<T> candidates; // vertices with in-degree 0
    std::vector<T> result; // stores topological sort

    // populate map
    for (const T& vert : src.vertices())
        in_degree[vert] = 0;

    // correct map values
    for (const T& source : src.vertices())
        for (const T& terminal : src.neighbors(source))
            ++in_degree[terminal];

    for (const std::pair<T, uint32_t>& value : in_degree)
        if (value.second == 0)
            candidates.push_back(value.first);

    while (!candidates.empty()) {
        T current = candidates.front();
        candidates.pop_front();
        result.push_back(current);
        for (const T& terminal : src.neighbors(current)) {
            --in_degree[terminal];
            if (in_degree[terminal] == 0)
                candidates.push_back(terminal);
        }
    }

    if (result.size() != src.order())
        throw std::invalid_argument("Not DAG");
    
    return result;
}

#endif // GRAPH_SEARCH_CPP