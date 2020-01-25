#ifndef GRAPH_PATH_CPP
#define GRAPH_PATH_CPP
#include <stdexcept>

#include <graph/search.h>

#include <structures/graph.h>
#include <structures/heap>

namespace graph_alg {
template <typename T, bool Directed, bool Weighted>
std::list<T> least_edges_path(
    const graph::graph<T, Directed, Weighted>& src, const T& start, const T& dest)
{
    std::list<T> result;

    if (start == dest)
        return result;

    std::unordered_map<T, uint32_t> search_number;
    uint32_t current_bfs_num = 0;
    bool found = false;

    std::unordered_map<T, T> parent;
    breadth_first(src, start,
        [&dest, &search_number, &current_bfs_num, &found, &parent, &src](const T& curr) {
            search_number[curr] = current_bfs_num++;
            if (curr == dest) {
                found = true;
            } else if (!found) {
                std::list<T> neighbors = src.neighbors(curr);
                for (const T& vertex : neighbors) {
                    if (parent.find(vertex) == parent.end())
                        parent[vertex] = curr;
                }
            }
            return found;
        });

    if (!found)
        throw no_path_exception();

    for (T current = dest; current != start; current = parent[current])
        result.push_front(current);

    return result;
}

template <typename T, typename Compare>
std::pair<double, std::list<T>> shortest_path_DAG(
    const graph::graph<T, true, true>& src, const T& start, const T& dest, Compare compare)
{
    std::list<T> result;

    if (start == dest) {
        return std::make_pair(0., result);
    }

    std::unordered_map<T, std::pair<double, T>> all_destinations
        = shortest_path_DAG_all(src, start, compare);
    if (all_destinations.find(dest) == all_destinations.end())
        throw no_path_exception();

    for (T current = dest; current != start; current = all_destinations[current].second) {
        result.push_back(current);
    }
    return std::make_pair(all_destinations[dest].first, result);
}

template <typename T, typename Compare>
std::unordered_map<T, std::pair<double, T>> shortest_path_DAG_all(
    const graph::graph<T, true, true>& src, const T& start, Compare compare)
{
    std::unordered_map<T, uint32_t> in_degree;
    std::list<T> candidates; // vertices with in-degree 0
    std::vector<T> topological_order; // stores topological sort

    // populate map
    for (const T& vert : src.vertices()) {
        in_degree[vert] = 0;
    }

    // correct map values
    for (const T& source : src.vertices())
        for (const T& terminal : src.neighbors(source))
            ++in_degree[terminal];

    for (const std::pair<T, uint32_t>& value : in_degree)
        if (value.second == 0)
            candidates.push_back(value.first);

    // topological sort
    while (!candidates.empty()) {
        T current = candidates.front();
        candidates.pop_front();
        topological_order.push_back(current);
        for (const T& terminal : src.neighbors(current)) {
            --in_degree[terminal];
            if (in_degree[terminal] == 0)
                candidates.push_back(terminal);
        }
    }

    if (topological_order.size() != src.order())
        throw std::invalid_argument("Not DAG");

    // start processing
    std::unordered_map<T, std::pair<double, T>> result;
    auto it = std::find(topological_order.cbegin(), topological_order.cend(), start);
    if (it == topological_order.end())
        throw std::invalid_argument("Start vertex does not exist");
    result[start].first = 0;
    result[start].second = start;

    for (; it != topological_order.end(); ++it) {
        if (result.find(*it) != result.end()) {
            for (const std::pair<T, double>& edge : src.edges(*it)) {
                if (result.find(edge.first) == result.end()
                    || compare(result[*it].first + edge.second, result[edge.first].first)) {
                    result[edge.first].first = result[*it].first + edge.second;
                    result[edge.first].second = *it;
                }
            }
        }
    }
}

template <typename T, bool Directed>
std::pair<double, std::list<T>> Dijkstra_single_target(
    const graph::graph<T, Directed, true>& src, const T& start, const T& dest)
{
    std::unordered_map<T, std::pair<double, T>> all_destinations
        = Dijkstra_partial(src, start, [&start](const T& current) { return current == start; });
    std::list<T> path;

    if (start == dest) {
        return std::make_pair(0., path);
    }

    if (all_destinations.at(dest).second == dest)
        throw no_path_exception();

    for (T current = dest; current != start; current = all_destinations[current].second) {
        path.push_front(current);
    }

    return std::make_pair(all_destinations[dest].first, path);
}

template <typename T, bool Directed>
std::unordered_map<T, std::pair<double, T>> Dijkstra_all_targets(
    const graph::graph<T, Directed, true>& src, const T& start)
{
    return Dijkstra_partial(src, start, [](const T&) { return false; });
}

template <typename T, bool Directed, typename F>
std::unordered_map<T, std::pair<double, T>> Dijkstra_partial(
    const graph::graph<T, Directed, true>& src, const T& start, F function)
{
    static_assert(std::is_invocable_r_v<bool, F, T>, "incompatible function");

    struct data {
        T current;
        T from;
        double cost;
    };

    std::vector<T> vertices = src.vertices();
    std::vector<data> data_map(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data_map.begin(), [&start](const T& value) {
        data result;
        result.current = value;
        result.from = value;
        result.cost = (value == start) ? 0 : std::numeric_limits<double>::max();
        return result;
    });

    // the heap structure used here determines the runtime of the algorithm
    // must be a node_base
    // Peculiarities with binomial heap make it incompatible.
    auto compare = [](const data& x, const data& y) { return x.cost < y.cost; };
    typedef typename heap::node_base<data, decltype(compare)>::node node;
    heap::Fibonacci<data, decltype(compare)> heap(compare);
    std::unordered_map<T, node*> tracker;

    auto it1 = vertices.cbegin();
    for (data& vertex_data : data_map) {
        tracker[*it1] = heap.add(vertex_data);
        ++it1;
    }

    auto heap_ptr = &heap;
    std::unordered_map<T, std::pair<double, T>> result;

    while (!heap_ptr->empty()) {
        data to_add = heap.remove_root();
        tracker.erase(to_add.current);
        result[to_add.current] = std::make_pair(to_add.cost, to_add.from);

        // break here once found
        if (function(to_add.current))
            return result;

        // don't want to deal with potential overflow case if no path is found
        if (to_add.current == start || to_add.from != to_add.current) {
            // update heap values
            for (const T& neighbor : src.neighbors(to_add.current)) {
                auto it = tracker.find(neighbor);
                if (it != tracker.end()) {
                    node* ptr = it->second;
                    const node* read_ptr = ptr;
                    double edge = src.edge_cost(to_add.current, neighbor);
                    if (edge < 0)
                        throw std::invalid_argument("Negative weight");

                    double to_compare = edge + to_add.cost;
                    if (to_compare < (*read_ptr)->cost) {
                        data replace(**read_ptr);
                        replace.from = to_add.current;
                        replace.cost = to_compare;
                        heap.decrease(ptr, replace);
                    }
                }
            }
        }
    }

    return result;
}

template <typename T>
std::pair<double, std::list<T>> Bellman_Ford_single_target(
    const graph::graph<T, true, true>& src, const T& start, const T& dest)
{
    std::unordered_map<T, std::pair<double, T>> all_destinations
        = Bellman_Ford_all_targets(src, start);

    std::list<T> path;
    if (start == dest) {
        return std::make_pair(0., path);
    }
    if (all_destinations.at(dest).second == dest)
        throw no_path_exception();

    for (T current = dest; current != start; current = all_destinations[current].second) {
        path.push_front(current);
    }

    return std::make_pair(all_destinations[dest].first, path);
}

template <typename T>
std::unordered_map<T, std::pair<double, T>> Bellman_Ford_all_targets(
    const graph::graph<T, true, true>& src, const T& start)
{
    std::unordered_map<T, std::pair<double, T>> result;
    std::vector<T> vertices = src.vertices();
    for (const T& v : vertices)
        result[v] = std::make_pair(v == start ? 0 : std::numeric_limits<double>::max(), v);

    // relax continuously
    // a loop can have at most V edges, can put off the final edge until negative cycle check
    for (uint32_t i = 1; i < vertices.size(); ++i) {
        bool relaxed = false;

        // go through all the edges
        for (const T& v : vertices) {
            // updating won't help if we haven't visited yet
            if (v == start || result[v].second != v) {
                for (std::pair<T, double>& edge : src.edges(v)) {
                    if ((edge.first != start && result[edge.first].second == edge.first)
                        || (result[v].first + edge.second < result[edge.first].first)) {
                        result[edge.first] = std::make_pair(result[v].first + edge.second, v);
                        relaxed = true;
                    }
                }
            }
        }

        // if no edges were updated, another iteration won't help
        // we also know there cannot be a negative cycle accessible to start,
        // because then we would go through all V-1 iterations, continuously relaxing through the
        // cycle
        if (!relaxed)
            return result;
    }

    // After V-1 iterations, any further updates must be the result of a negative cycle
    for (const T& v : vertices)
        if (v == start || result[v].second != v)
            for (const std::pair<T, double>& edge : src.edges(v))
                if (result[v].first + edge.second < result[edge.first].first)
                    throw std::domain_error("Negative cycle");

    return result;
}

template <typename T>
std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> Floyd_Warshall_all_pairs(
    const graph::graph<T, true, true>& src)
{
    std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> result;
    std::vector<T> vertices = src.vertices();

    // initialize array with already existing edges
    // distance from a vertex to itself should always be 0
    for (const T& v : vertices) {
        result[v][v] = std::make_pair(0., v);

        for (const std::pair<T, double>& edge : src.edges(v))
            result[v][edge.first] = std::make_pair(edge.second, v);
    }

    // dynamic programming portion:
    // for each j, compare i->j->k to shortest i->k yet discovered, update if i->j->k shorter
    for (const T& middle : vertices) {
        if (result.find(middle) != result.end()) { // sanity check
            for (const T& start : vertices) {
                if (start
                    != middle) { // worthless to check if current middle is one of the endpoints
                    for (const T& dest : vertices) {
                        if (dest != middle
                            && result.find(start) != result.end()
                            // no path start -> middle
                            && result[start].find(middle) != result[start].end()
                            // no path middle -> end
                            && result[middle].find(dest) != result[middle].end()
                            // update if no other path
                            && (result[start].find(dest) == result[start].end()
                                // finally, we can compare
                                || result[start][middle].first + result[middle][dest].first
                                    < result[start][dest].first)) {
                            result[start][dest] = std::make_pair(
                                result[start][middle].first + result[middle][dest].first, middle);
                        }
                    }
                }
            }

            // check for negative cycle: a vertex to itself in negative cost
            for (const T& v : vertices) {
                if (result[v][v].first < 0)
                    throw std::domain_error("Negative cycle");
            }
        }
    }

    return result;
}

template <typename T>
std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> Johnson_all_pairs(
    const graph::graph<T, true, true>& src, const T& new_vertex)
{
    graph::graph<T, true, true> temp(src);
    std::vector<T> vertices = src.vertices();
    if (std::find(vertices.begin(), vertices.end(), new_vertex) != vertices.end())
        throw std::invalid_argument("Vertex already exists");

    temp.add_vertex(new_vertex);
    for (const T& original_vertex : vertices)
        temp.set_edge(new_vertex, original_vertex, 0);

    // O(VE)
    std::unordered_map<T, std::pair<double, T>> shortest_path_tree
        = Bellman_Ford_all_targets(temp, new_vertex); // where we find negative cycles

    // O(E)
    temp.remove(new_vertex);

    // re-weight edges based on Bellman-Ford result:
    // edge from u to v is weighted by tree_path(u) - tree_path(v)
    for (const T& start : vertices)
        for (const std::pair<T, double>& edge : src.edges(start))
            temp.set_edge(start, edge.first,
                edge.second + shortest_path_tree[start].first
                    - shortest_path_tree[edge.first].first);

    std::unordered_map<T, std::unordered_map<T, std::pair<double, T>>> result;

    // Use Dijkstra's algorithm to find the rest
    for (const T& start : vertices) {
        std::unordered_map<T, std::pair<double, T>> subresult = Dijkstra_all_targets(temp, start);
        for (std::pair<T, std::pair<double, T>>& endpoint : subresult) {
            if (endpoint.first != endpoint.second.second) {
                // unweight
                result[start][endpoint.first] = std::make_pair(endpoint.second.first
                        + shortest_path_tree[endpoint.first].first
                        - shortest_path_tree[start].first,
                    endpoint.second.second);
            }
        }

        // for sake of completion
        result[start][start] = std::make_pair(0., start);
    }

    return result;
}
}

#endif // GRAPH_PATH_CPP
