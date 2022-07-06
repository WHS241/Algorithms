#ifndef GRAPH_PATH_H
#define GRAPH_PATH_H
#include <functional>
#include <list>
#include <stdexcept>
#include <unordered_map>

#include <structures/graph.h>
#include <structures/heap>

#include "search.h"

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
    no_path_exception() : std::domain_error("No path"){};
};

/*
Use BFS to find path with fewest edges
*/
template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
std::list<Vertex>
  least_edges_path(const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src,
                   const Vertex& start, const Vertex& dest) {
    std::list<Vertex> result;

    if (start == dest)
        return result;

    std::unordered_map<Vertex, uint32_t, Args...> search_number;
    uint32_t current_bfs_num = 0;
    bool found = false;

    std::unordered_map<Vertex, Vertex, Args...> parent;
    breadth_first(
      src, start,
      [&dest, &search_number, &current_bfs_num, &found, &parent, &src](const Vertex& curr) {
          search_number[curr] = current_bfs_num++;
          if (curr == dest) {
              found = true;
          } else if (!found) {
              std::list<Vertex> neighbors = src.neighbors(curr);
              for (const Vertex& vertex : neighbors) {
                  if (parent.find(vertex) == parent.end())
                      parent[vertex] = curr;
              }
          }
          return found;
      });

    if (!found)
        throw no_path_exception();

    for (Vertex current = dest; current != start; current = parent[current])
        result.push_front(current);

    return result;
}

/*
Use topological sort on a weighted DAG
Can be used with other comparison operators (e.g. find longest path)
*/
template<typename Vertex, typename Compare, typename EdgeWeight, typename... Args>
std::pair<EdgeWeight, std::list<Vertex>>
  shortest_path_DAG(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                    const Vertex& start, const Vertex& dest, Compare compare) {
    std::list<Vertex> result;
    if (start == dest)
        return std::make_pair(0., result);

    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> all_destinations =
      shortest_path_DAG_all_targets(src, start, compare);
    if (all_destinations.find(dest) == all_destinations.end())
        throw no_path_exception();

    for (Vertex current = dest; current != start; current = all_destinations[current].second)
        result.push_back(current);

    return std::make_pair(all_destinations[dest].first, result);
}

/*
Use topological sort on a weighted DAG
*/
template<typename Vertex, typename EdgeWeight, typename... Args>
std::pair<EdgeWeight, std::list<Vertex>>
  shortest_path_DAG(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                    const Vertex& start, const Vertex& dest) {
    return shortest_path_DAG(src, start, dest, std::less<EdgeWeight>());
}

template<typename Vertex, typename EdgeWeight, typename Compare, typename... Args>
std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>
  shortest_path_DAG_all_targets(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                                const Vertex& start, Compare compare) {
    std::vector<Vertex> topological_order = topological_sort(src);

    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> result;
    auto it = std::find(topological_order.cbegin(), topological_order.cend(), start);
    if (it == topological_order.end())
        throw std::invalid_argument("Start vertex does not exist");
    result[start].first = 0;
    result[start].second = start;

    // Traverse the sorted order, finding the shortest/longest path given possible predecessors
    for (; it != topological_order.end(); ++it) {
        if (result.find(*it) != result.end()) {
            for (const std::pair<Vertex, EdgeWeight>& edge : src.edges(*it)) {
                if (result.find(edge.first) == result.end() ||
                    compare(result[*it].first + edge.second, result[edge.first].first)) {
                    result[edge.first].first = result[*it].first + edge.second;
                    result[edge.first].second = *it;
                }
            }
        }
    }

    return result;
}

template<typename Vertex, typename EdgeWeight, typename... Args>
std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>
  shortest_path_DAG_all_targets(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                                const Vertex& start) {
    return shortest_path_DAG_all_targets(src, start, std::less<EdgeWeight>());
}

/*
 F function determines whether to halt early (returns true to halt)
 */
template<typename Vertex, bool Directed, typename EdgeWeight, typename F, typename... Args>
std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>
  Dijkstra_partial(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& src,
                   const Vertex& start, F function) {
    static_assert(std::is_invocable_r_v<bool, F, Vertex>, "incompatible function");

    // What we store in a heap; tracks predecessor and cost so far
    struct data {
        Vertex current;
        Vertex from; // != current unless current == start or we have not visited current yet
        EdgeWeight cost;
    };
    static const EdgeWeight zero = EdgeWeight();

    std::vector<Vertex> vertices = src.vertices();

    // Keep track of where the shortest path comes from and what the cost is
    // Initialize to self and 0
    std::vector<data> data_map(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data_map.begin(), [](const Vertex& value) {
        return data{value, value, zero};
    });

    // the heap structure used here determines the runtime of the algorithm
    // must be a node_base
    auto compare = [&start](const data& x, const data& y) {
        // Push start vertex to front, visited vertices are ahead of unvisited
        return x.current == start ||
               (x.current != x.from && (y.current == y.from || x.cost < y.cost));
    };
    typedef typename heap::node_base<data, decltype(compare)>::node node;
    typedef typename heap::node_base<data, decltype(compare)>::node_wrapper node_wrapper;
    heap::Fibonacci<data, decltype(compare)> heap(compare);
    std::unordered_map<Vertex, node_wrapper, Args...> tracker; // pointers to items in the heap

    auto it1 = vertices.cbegin();
    for (data& vertex_data : data_map)
        tracker[*(it1++)] = heap.add(vertex_data);

    auto heap_ptr = &heap; // need pointer-based polymorphism
    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> result;

    while (!heap_ptr->empty()) {
        data to_add = heap.remove_root();
        tracker.erase(to_add.current);
        result[to_add.current] = std::make_pair(to_add.cost, to_add.from);

        // break here once found
        if (function(to_add.current))
            return result;

        // don't want to deal with weird edge case if no path to current is found
        if (to_add.current == start || to_add.from != to_add.current) {
            // update heap values
            for (const Vertex& neighbor : src.neighbors(to_add.current)) {
                auto it = tracker.find(neighbor);
                if (it != tracker.end()) {
                    node_wrapper ptr = it->second;
                    const node* read_ptr = ptr.get();
                    EdgeWeight edge = src.edge_cost(to_add.current, neighbor);
                    if (edge < zero)
                        throw std::invalid_argument("Negative weight");

                    EdgeWeight new_cost = edge + to_add.cost;
                    if ((*read_ptr)->current == (*read_ptr)->from || new_cost < (*read_ptr)->cost) {
                        data replace(**read_ptr);
                        replace.from = to_add.current;
                        replace.cost = new_cost;
                        heap.decrease(ptr, std::move(replace));
                    }
                }
            }
        }
    }

    return result;
}

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
template<typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
std::pair<EdgeWeight, std::list<Vertex>>
  Dijkstra_single_target(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& src,
                         const Vertex& start, const Vertex& dest) {
    std::list<Vertex> path;
    if (start == dest)
        return std::make_pair(EdgeWeight(), path);

    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> all_destinations =
      Dijkstra_partial(src, start, [&dest](const Vertex& current) { return current == dest; });

    if (all_destinations.at(dest).second == dest)
        throw no_path_exception();

    for (Vertex current = dest; current != start; current = all_destinations[current].second)
        path.push_front(current);

    return std::make_pair(all_destinations[dest].first, path);
}

template<typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>>
  Dijkstra_all_targets(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& src,
                       const Vertex& start) {
    return Dijkstra_partial(src, start, [](const Vertex&) { return false; });
}

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
template<typename Vertex, typename EdgeWeight, typename... Args>
std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>
  Bellman_Ford_all_targets(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                           const Vertex& start) {
    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> result;
    std::vector<Vertex> vertices = src.vertices();
    for (const Vertex& v : vertices) {
        result[v] = std::make_pair(EdgeWeight(), v);
    }

    // relax continuously
    // a simple cycle can have at most V edges, can put off the final edge until negative cycle
    // check
    for (uint32_t i = 1; i < vertices.size(); ++i) {
        bool relaxed = false;

        // go through all the edges
        for (const Vertex& v : vertices) {
            // updating won't help if we haven't visited this vertex yet
            if (v == start || result[v].second != v) {
                for (std::pair<Vertex, EdgeWeight>& edge : src.edges(v)) {
                    // Two scenarios for relaxing:
                    // 1. We've never visited this vertex before
                    // 2. We've found a shorter path
                    if ((edge.first != start && result[edge.first].second == edge.first) ||
                        (result[v].first + edge.second < result[edge.first].first)) {
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
    for (const Vertex& v : vertices)
        if (v == start || result[v].second != v)
            for (const std::pair<Vertex, double>& edge : src.edges(v))
                if (result[v].first + edge.second < result[edge.first].first)
                    throw std::domain_error("Negative cycle");

    return result;
}
template<typename Vertex, typename EdgeWeight, typename... Args>
std::pair<EdgeWeight, std::list<Vertex>>
  Bellman_Ford_single_target(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src,
                             const Vertex& start, const Vertex& dest) {
    std::list<Vertex> path;
    if (start == dest) {
        return std::make_pair(EdgeWeight(), path);
    }

    std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...> all_destinations =
      Bellman_Ford_all_targets(src, start);
    if (all_destinations.at(dest).second == dest)
        throw no_path_exception();

    for (Vertex current = dest; current != start; current = all_destinations[current].second)
        path.push_front(current);

    return std::make_pair(all_destinations[dest].first, path);
}

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
template<typename Vertex, typename EdgeWeight, typename... Args>
std::unordered_map<Vertex, std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>,
                   Args...>
  Floyd_Warshall_all_pairs(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src) {
    std::unordered_map<Vertex, std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>,
                       Args...>
      result;
    std::vector<Vertex> vertices = src.vertices();

    static const EdgeWeight zero = EdgeWeight();

    // initialize array with already existing edges
    // distance from a vertex to itself should always be 0
    for (const Vertex& v : vertices) {
        result[v][v] = std::make_pair(zero, v);

        for (const std::pair<Vertex, EdgeWeight>& edge : src.edges(v))
            result[v][edge.first] = std::make_pair(edge.second, v);
    }

    // dynamic programming portion:
    // for each j, compare i->j->k to shortest i->k yet discovered, update if i->j->k shorter
    for (const Vertex& middle : vertices) {
        // sanity check
        if (result.find(middle) != result.end()) {
            for (const Vertex& start : vertices) {
                // worthless to check if current middle is one of the endpoints
                if (start != middle) {
                    for (const Vertex& dest : vertices) {
                        if (dest != middle
                            // sanity check
                            && result.find(start) != result.end()
                            // no path start -> middle
                            && result[start].find(middle) != result[start].end()
                            // no path middle -> end
                            && result[middle].find(dest) != result[middle].end()
                            // update if no other path
                            && (result[start].find(dest) == result[start].end()
                                // finally, we compare
                                || result[start][middle].first + result[middle][dest].first <
                                     result[start][dest].first)) {
                            result[start][dest] = std::make_pair(
                              result[start][middle].first + result[middle][dest].first, middle);
                        }
                    }
                }
            }

            // check for negative cycle: a vertex to itself in negative cost
            for (const Vertex& v : vertices)
                if (result[v][v].first < zero)
                    throw std::domain_error("Negative cycle");
        }
    }

    return result;
}

/*
All-pairs shortest path
Donald B. Johnson
Efficient algorithms for shortest paths in sparse networks
(1977) doi:10.1145/321992.321993

Θ(V^2 log V + VE)
*/
template<typename Vertex, typename EdgeWeight, typename... Args>
std::unordered_map<Vertex, std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>,
                   Args...>
  Johnson_all_pairs(const graph::graph<Vertex, true, true, EdgeWeight, Args...>& src) {
    graph::graph<uint32_t, true, true, EdgeWeight> Bellman_graph;

    for (uint32_t i = 0; i < src.order() + 1; ++i)
        Bellman_graph.add_vertex(i);
    for (const Vertex& v : src.vertices()) {
        Bellman_graph.force_add(src.order(), src.get_translation().at(v), EdgeWeight());
        for (const std::pair<Vertex, EdgeWeight>& e : src.edges(v)) {
            Bellman_graph.force_add(src.get_translation().at(v), src.get_translation().at(e.first),
                                    e.second);
        }
    }

    // O(VE)
    std::unordered_map<uint32_t, std::pair<EdgeWeight, uint32_t>, Args...> shortest_path_tree =
      Bellman_Ford_all_targets(Bellman_graph, src.order()); // where we find negative cycles

    graph::graph<Vertex, true, true, EdgeWeight, Args...> reweighted;
    for (const Vertex& v : src.vertices())
        reweighted.add_vertex(v);

    // re-weight edges based on Bellman-Ford result:
    // edge from u to v is weighted by tree_path(u) - tree_path(v)
    for (const Vertex& start : src.vertices())
        for (const std::pair<Vertex, EdgeWeight>& edge : src.edges(start))
            reweighted.force_add(start, edge.first,
                                 edge.second +
                                   shortest_path_tree[src.get_translation().at(start)].first -
                                   shortest_path_tree[src.get_translation().at(edge.first)].first);

    std::unordered_map<Vertex, std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>, Args...>,
                       Args...>
      result;

    // Use Dijkstra's algorithm to find the rest
    for (const Vertex& start : src.vertices()) {
        std::unordered_map<Vertex, std::pair<EdgeWeight, Vertex>> subresult =
          Dijkstra_all_targets(reweighted, start);
        for (const std::pair<Vertex, std::pair<EdgeWeight, Vertex>>& endpoint : subresult) {
            if (endpoint.first != endpoint.second.second) {
                // unweight
                result[start][endpoint.first] = std::make_pair(
                  endpoint.second.first +
                    shortest_path_tree[src.get_translation().at(endpoint.first)].first -
                    shortest_path_tree[src.get_translation().at(start)].first,
                  endpoint.second.second);
            }
        }

        // To match Floyd-Warshall output (and specs above)
        result[start][start] = std::make_pair(EdgeWeight(), start);
    }

    return result;
}
} // namespace graph_alg

#endif // GRAPH_PATH_H
