//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_MIN_FLOW_H
#define GRAPH_ALG_MIN_FLOW_H
#include <functional>

#include <structures/graph.h>

#include <util/exposed_graph.h>

#include "path.h"

namespace graph_alg {

template<typename EdgeWeight>
using exp_graph = std::vector<std::list<std::pair<std::size_t, EdgeWeight>>>;

template<typename EdgeWeight> class zero_check {
    public:
    static constexpr EdgeWeight zero = EdgeWeight();
    static constexpr double epsilon = 1e-10;
    bool operator()(const EdgeWeight& x) {
        if constexpr (std::is_floating_point_v<EdgeWeight>)
            return std::abs(x) < epsilon;
        else
            return x == zero;
    }
};

/**
 * Max flow algorithm
 * The weight on each edge of input denotes the capacity
 * Returns a weighted graph where each weight denotes flow
 *
 * Ford-Fulkerson method: runtime dependent on flow-finding strategy
 * Bad strategies may not necessarily converge on true cost for irrational values (infinite runtime)
 *
 * L. R. Ford, D. R. Fulkerson
 * Maximal flow through a network
 * (1956) doi:10.4153/CJM-1956-045-5
 *
 * Conditions:
 * Invoking default constructor of EdgeType gives the "0" value
 * EdgeType supports addition and comparison
 *
 * Function takes in the residual graph and source/sink vertices
 * Returns all the edges from the augmenting path(s) in tuple form (edge source, edge target,
 * augmenting flow) or throw a graph_alg::no_path_exception in order to signal no augmenting path
 * found
 */
template<typename EdgeWeight, typename Function>
std::vector<std::list<std::pair<std::size_t, EdgeWeight>>>
  Ford_Fulkerson(std::vector<std::list<std::pair<std::size_t, EdgeWeight>>>& input,
                 std::size_t source, std::size_t sink, Function f) {
    typedef exp_graph<EdgeWeight> graph;
    typedef util::exposed_edge_ptr<EdgeWeight> edge_ptr;

    graph result(input.size());

    static EdgeWeight zero = zero_check<EdgeWeight>::zero;
    static zero_check<EdgeWeight> is_zero;

    if (source == sink)
        return result;

    graph residual_graph = input;
    std::vector<std::vector<std::pair<bool, edge_ptr>>> flow_ptrs = util::list_pointer_map(result);
    std::vector<std::vector<std::pair<bool, edge_ptr>>> residual_ptrs =
      util::list_pointer_map(residual_graph);

    try {
        while (true) {
            // use function to find augment paths
            std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>> next_path =
              f(residual_graph, source, sink);

            // verify path
            std::vector<EdgeWeight> net_flow(input.size(), EdgeWeight());
            for (const std::tuple<std::size_t, std::size_t, EdgeWeight>& to_augment : next_path) {
                net_flow[std::get<0>(to_augment)] -= std::get<2>(to_augment);
                net_flow[std::get<1>(to_augment)] += std::get<2>(to_augment);
            }
            for (std::size_t i = 0; i < net_flow.size(); ++i)
                if (i != source && i != sink && !is_zero(net_flow[i]))
                    throw std::domain_error("Non-zero net flow");

            // Update flow and residual graphs
            for (const std::tuple<std::size_t, std::size_t, EdgeWeight>& e : next_path) {
                auto [from_v, to_v, flow] = e;
                EdgeWeight residual_flow = flow;

                std::pair<bool, edge_ptr>& flow_edge_ptr = flow_ptrs[from_v][to_v];
                std::pair<bool, edge_ptr>& rev_flow_edge_ptr = flow_ptrs[to_v][from_v];

                if (rev_flow_edge_ptr.first) {
                    // Removing/reversing an existing flow?
                    if (flow > rev_flow_edge_ptr.second->second ||
                        is_zero(rev_flow_edge_ptr.second->second - flow)) {
                        flow -= rev_flow_edge_ptr.second->second;
                        rev_flow_edge_ptr.first = false;
                        result[to_v].erase(rev_flow_edge_ptr.second);
                    } else {
                        rev_flow_edge_ptr.second->second -= flow;
                        flow = zero;
                    }
                }
                if (!is_zero(flow)) {
                    // Augment an existing flow or adding a new one?
                    if (flow_edge_ptr.first) {
                        flow_edge_ptr.second->second += flow;
                    } else {
                        result[from_v].push_back(std::make_pair(to_v, flow));
                        flow_edge_ptr = std::make_pair(true, --result[from_v].end());
                    }
                }

                // Residual flows
                std::pair<bool, edge_ptr>& res_ptr = residual_ptrs[from_v][to_v];
                std::pair<bool, edge_ptr>& res_back_ptr = residual_ptrs[to_v][from_v];

                // Start with back residual: augment, or add if non-existent
                if (res_back_ptr.first) {
                    res_back_ptr.second->second += residual_flow;
                } else {
                    residual_graph[to_v].push_back(std::make_pair(from_v, residual_flow));
                    res_back_ptr = std::make_pair(true, --residual_graph[to_v].end());
                }

                // Front residual: check validity, then decrease or remove
                if (!res_ptr.first || (res_ptr.second->second < residual_flow &&
                                       !is_zero(res_ptr.second->second - residual_flow)))
                    throw std::domain_error("Invalid augmenting edge");
                if (is_zero(res_ptr.second->second - residual_flow)) {
                    res_ptr.first = false;
                    residual_graph[from_v].erase(res_ptr.second);
                } else {
                    res_ptr.second->second -= residual_flow;
                }
            }
        }
    } catch (graph_alg::no_path_exception&) {
        // indicates no augmenting flow found
        // simply absorb the exception and use as an exit out of while loop
    }

    return result;
}

template<typename Vertex, bool Directed, typename EdgeWeight, typename Function, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...>
  max_flow(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input,
           const Vertex& source, const Vertex& sink, Function flow_finder) {
    exp_graph<EdgeWeight> temp = util::get_list_rep(input);
    exp_graph<EdgeWeight> temp_result =
      flow_finder(temp, input.get_translation().at(source), input.get_translation().at(sink));

    graph::graph<Vertex, Directed, true, EdgeWeight, Args...> output(graph::adj_list);
    std::vector<Vertex> vertices = input.vertices();
    for (const Vertex& v : vertices)
        output.add_vertex(v);

    for (std::size_t i = 0; i < vertices.size(); ++i)
        for (const std::pair<std::size_t, EdgeWeight>& e : temp_result[i])
            output.force_add(vertices[i], vertices[e.first], e.second);

    return output;
}

/**
 * Jack Edmonds, Richard Karp
 * Theoretical Improvements in Algorithmic Efficiency for Network Flow Problems
 * (1972) doi:10.1145/321694.321699
 * Θ(V E^2)
 */
template<typename EdgeWeight>
std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>>
  Edmonds_Karp_helper(const exp_graph<EdgeWeight>& residual, std::size_t source, std::size_t sink) {
    // Essentially BFS to find shortest path from source to sink
    std::vector<std::pair<std::size_t, EdgeWeight>> bfs_tracker(
      residual.size(), std::make_pair(residual.size(), EdgeWeight()));
    std::list<std::size_t> frontier;
    frontier.push_back(source);
    while (!frontier.empty() && bfs_tracker[sink].first == residual.size()) {
        std::size_t next = frontier.front();
        frontier.pop_front();
        for (const std::pair<std::size_t, EdgeWeight>& e : residual[next]) {
            if (bfs_tracker[e.first].first == residual.size()) {
                frontier.push_back(e.first);
                bfs_tracker[e.first].first = next;
                if (next == source)
                    bfs_tracker[e.first].second = e.second;
                else
                    bfs_tracker[e.first].second = std::min(bfs_tracker[next].second, e.second);
            }
        }
    }

    if (bfs_tracker[sink].first == residual.size())
        throw graph_alg::no_path_exception();

    std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>> output;
    EdgeWeight flow = bfs_tracker[sink].second;
    for (std::size_t curr = sink; curr != source; curr = bfs_tracker[curr].first)
        output.push_front(std::make_tuple(bfs_tracker[curr].first, curr, flow));

    return output;
}
template<typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...>
  Edmonds_Karp_max_flow(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input,
                        const Vertex& source, const Vertex& sink) {
    auto f = std::bind(Ford_Fulkerson<EdgeWeight, decltype(Edmonds_Karp_helper<EdgeWeight>)>,
                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                       Edmonds_Karp_helper<EdgeWeight>);
    return max_flow(input, source, sink, f);
}

/*
 * Build a layer graph from a residual graph
 */
template<typename EdgeWeight>
exp_graph<EdgeWeight> build_layer_graph(const exp_graph<EdgeWeight>& residual, std::size_t source,
                                        std::size_t sink) {
    exp_graph<EdgeWeight> output(residual.size());
    std::vector<std::size_t> bfs_layer(residual.size(), residual.size());
    std::list<std::size_t> frontier;
    frontier.push_back(source);
    bfs_layer[source] = 0;

    std::size_t current_layer = 0;
    while (!frontier.empty() && bfs_layer[sink] > current_layer) {
        std::size_t next_v = frontier.front();
        frontier.pop_front();
        current_layer = bfs_layer[next_v];
        for (const std::pair<std::size_t, EdgeWeight>& e : residual[next_v]) {
            if (bfs_layer[e.first] > current_layer) {
                output[next_v].push_back(e);
            }
            if (bfs_layer[e.first] == residual.size()) {
                bfs_layer[e.first] = current_layer + 1;
                frontier.push_back(e.first);
            }
        }
    }

    if (bfs_layer[sink] == residual.size())
        throw graph_alg::no_path_exception();

    for (std::list<std::pair<std::size_t, EdgeWeight>>& adj_list : output)
        adj_list.remove_if([&bfs_layer, &sink](const std::pair<std::size_t, EdgeWeight>& e) {
            return e.first != sink && bfs_layer[e.first] >= bfs_layer[sink];
        });

    return output;
}

/**
 * Yefim Dinitz
 * An algorithm for the solution of the max-flow problem with the polynomial estimation
 * (1970) - Soviet Doklady
 * Θ(V^2 E)
 */
template<typename EdgeWeight>
std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>>
  Dinic_helper(const exp_graph<EdgeWeight>& residual, std::size_t source, std::size_t sink) {
    exp_graph<EdgeWeight> layer_graph = build_layer_graph(residual, source, sink);

    // Each entry: [from vertex, current spot in DFS, current allowed flow]
    std::list<std::tuple<
      std::size_t, typename std::list<std::pair<std::size_t, EdgeWeight>>::iterator, EdgeWeight>>
      curr_path;

    std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>> output;

    static EdgeWeight zero = zero_check<EdgeWeight>::zero;
    static zero_check<EdgeWeight> is_zero;

    curr_path.emplace_back(source, layer_graph[source].begin(), layer_graph[source].front().second);
    while (!curr_path.empty()) {
        auto [curr_v, it, curr_flow] = curr_path.back();
        std::size_t next_v = it->first;
        if (!layer_graph[next_v].empty()) {
            curr_path.emplace_back(next_v, layer_graph[next_v].begin(),
                                   std::min(curr_flow, layer_graph[next_v].front().second));
        } else if (next_v == sink) {
            // Add the path
            std::transform(
              curr_path.begin(), curr_path.end(), std::back_inserter(output),
              [&curr_flow](
                const std::tuple<std::size_t,
                                 typename std::list<std::pair<std::size_t, EdgeWeight>>::iterator,
                                 EdgeWeight>& step) {
                  return std::make_tuple(std::get<0>(step), std::get<1>(step)->first, curr_flow);
              });

            // Modify the capacity of each edge in path, find first saturated edge
            auto step_it = curr_path.end();
            for (auto it = curr_path.begin(); it != curr_path.end(); ++it) {
                auto edge_ptr = std::get<1>(*it);
                edge_ptr->second -= curr_flow;
                std::get<2>(*it) -= curr_flow;
                if (is_zero(edge_ptr->second) && step_it == curr_path.end())
                    step_it = it;
            }

            // Remove all saturated edges
            auto [new_head, new_head_ptr, ignored] = *step_it;
            ++new_head_ptr;
            for (auto it = step_it; it != curr_path.end(); ++it)
                if (is_zero(std::get<1>(*it)->second))
                    layer_graph[std::get<0>(*it)].erase(std::get<1>(*it));
            curr_path.erase(step_it, curr_path.end());

            if (new_head_ptr != layer_graph[new_head].end()) {
                curr_path.emplace_back(new_head, new_head_ptr,
                                       new_head == source ? new_head_ptr->second
                                                          : std::min(std::get<2>(curr_path.back()),
                                                                     new_head_ptr->second));
            } else {
                // start backtracking
                // We can also delete edges as we backtrack, because we know they would lead to a
                // dead end
                bool added_next = false;
                while (!added_next && !curr_path.empty()) {
                    std::tie(new_head, new_head_ptr, std::ignore) = curr_path.back();
                    curr_path.pop_back();
                    new_head_ptr = layer_graph[new_head].erase(new_head_ptr);
                    if (new_head_ptr != layer_graph[new_head].end()) {
                        added_next = true;
                        curr_path.emplace_back(
                          new_head, new_head_ptr,
                          new_head == source
                            ? new_head_ptr->second
                            : std::min(std::get<2>(curr_path.back()), new_head_ptr->second));
                    }
                }
            }
        } else {
            // Backtrack and delete
            bool added_next = false;
            while (!added_next && !curr_path.empty()) {
                auto [new_head, new_head_ptr, ignore] = curr_path.back();
                curr_path.pop_back();
                new_head_ptr = layer_graph[new_head].erase(new_head_ptr);
                if (new_head_ptr != layer_graph[new_head].end()) {
                    added_next = true;
                    curr_path.emplace_back(
                      new_head, new_head_ptr,
                      new_head == source
                        ? new_head_ptr->second
                        : std::min(std::get<2>(curr_path.back()), new_head_ptr->second));
                }
            }
        }
    }

    return output;
}
template<typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...>
  Dinic_max_flow(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input,
                 const Vertex& source, const Vertex& sink) {
    auto f = std::bind(Ford_Fulkerson<EdgeWeight, decltype(Dinic_helper<EdgeWeight>)>,
                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                       Dinic_helper<EdgeWeight>);
    return max_flow(input, source, sink, f);
}

/*
 * Aleksandr V. Karzanov
 * Determining a maximal flow in a network by the method of preflows
 * (1974) - Soviet Doklady
 * O(V^3)
 */
template<typename EdgeWeight>
std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>>
  Karzanov_helper(const exp_graph<EdgeWeight>& residual, std::size_t source, std::size_t sink) {
    exp_graph<EdgeWeight> layer_graph = build_layer_graph(residual, source, sink);

    static EdgeWeight zero = zero_check<EdgeWeight>::zero;
    static zero_check<EdgeWeight> is_zero;

    // generate the BFS number for reachable vertices
    std::vector<bool> found(residual.size(), false);
    std::list<std::size_t> bfs_order;
    bfs_order.push_back(source);
    found[source] = true;

    for (auto it = bfs_order.begin(); it != bfs_order.end(); ++it) {
        for (const std::pair<std::size_t, EdgeWeight>& e : layer_graph[*it]) {
            if (!found[e.first]) {
                found[e.first] = true;
                bfs_order.push_back(e.first);
            }
        }
    }

    exp_graph<EdgeWeight> augmenting_flow(layer_graph.size()); // to map the augmenting flow
    std::vector<EdgeWeight> excess(layer_graph.size(),
                                   zero); // in-flow that has not been pushed through
    std::vector<bool> frozen(layer_graph.size(),
                             false); // if true, ignore all future edges to this vertex
    exp_graph<typename exp_graph<EdgeWeight>::value_type::iterator> in_flow_stacks(
      layer_graph.size()); // push blocked flows in augmenting_flow back to source.
    std::vector<typename exp_graph<EdgeWeight>::value_type::iterator> sweep_pos(
      layer_graph.size()); // current sweep position
    typedef
      typename exp_graph<typename exp_graph<EdgeWeight>::value_type::iterator>::iterator stack_ptr;
    std::vector<bool> partially_filled(layer_graph.size(),
                                       false); // is the current active edge only partially filled?
                                               // if so, second points into corresponding stack

    // initialization
    for (const std::pair<std::size_t, EdgeWeight>& e : layer_graph[source]) {
        augmenting_flow[source].push_back(e);
        in_flow_stacks[e.first].emplace_back(source, --augmenting_flow[source].end());
        excess[e.first] += e.second;
    }
    for (std::size_t i = 0; i < layer_graph.size(); ++i)
        sweep_pos[i] = layer_graph[i].begin();

    bool is_blocking = false;
    while (!is_blocking) {
        // push flow through all vertices
        for (std::size_t v : bfs_order) {
            if (!is_zero(excess[v]) && v != source && v != sink) {
                while (sweep_pos[v] != layer_graph[v].end() && !is_zero(excess[v])) {
                    if (frozen[sweep_pos[v]->first]) {
                        ++sweep_pos[v];
                        partially_filled[v] = false;
                    } else {
                        EdgeWeight capacity = sweep_pos[v]->second;

                        // if partially full, the capacity drops
                        if (partially_filled[v]) {
                            capacity -= augmenting_flow[v].back().second;
                        }

                        if (capacity < excess[v] || is_zero(capacity - excess[v])) {
                            if (partially_filled[v]) {
                                // update current and reset partially_filled
                                augmenting_flow[v].back().second = sweep_pos[v]->second;
                                partially_filled[v] = false;
                            } else {
                                augmenting_flow[v].push_back(*sweep_pos[v]);
                                in_flow_stacks[sweep_pos[v]->first].emplace_back(
                                  v, --augmenting_flow[v].end());
                            }
                            excess[v] -= capacity;
                            excess[sweep_pos[v]->first] += capacity;
                            ++sweep_pos[v];
                        } else {
                            if (partially_filled[v]) {
                                augmenting_flow[v].back().second += excess[v];
                            } else {
                                augmenting_flow[v].emplace_back(sweep_pos[v]->first, excess[v]);
                                in_flow_stacks[sweep_pos[v]->first].emplace_back(
                                  v, --augmenting_flow[v].end());
                            }
                            excess[sweep_pos[v]->first] += excess[v];
                            excess[v] = zero;
                            partially_filled[v] = true;
                        }
                    }
                }
            }
        }

        // pull excess flow back through one vertex
        is_blocking = true;
        for (auto rev_it = bfs_order.rbegin(); rev_it != bfs_order.rend() && is_blocking;
             ++rev_it) {
            std::size_t v = *rev_it;
            if (v != source && v != sink && !is_zero(excess[v])) {
                is_blocking = false;
                while (!is_zero(excess[v])) {
                    std::pair<std::size_t, typename exp_graph<EdgeWeight>::value_type::iterator>
                      pull_back = in_flow_stacks[v].back();

                    if (pull_back.second->second < excess[v] ||
                        is_zero(pull_back.second->second - excess[v])) {
                        excess[v] -= pull_back.second->second;
                        excess[pull_back.first] += pull_back.second->second;
                        augmenting_flow[pull_back.first].erase(pull_back.second);
                        in_flow_stacks[v].pop_back();
                    } else {
                        excess[pull_back.first] += excess[v];
                        pull_back.second->second -= excess[v];
                        excess[v] = zero;
                    }
                }
                frozen[v] = true;
            }
        }
    }

    // Generate output paths from augmenting_flow
    std::list<std::tuple<std::size_t, std::size_t, EdgeWeight>> output;
    for (std::size_t i = 0; i < augmenting_flow.size(); ++i)
        for (const std::pair<std::size_t, EdgeWeight>& e : augmenting_flow[i])
            output.emplace_back(i, e.first, e.second);

    return output;
}
template<typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...>
  Karzanov_max_flow(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input,
                    const Vertex& source, const Vertex& sink) {
    auto f = std::bind(Ford_Fulkerson<EdgeWeight, decltype(Karzanov_helper<EdgeWeight>)>,
                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                       Karzanov_helper<EdgeWeight>);
    return max_flow(input, source, sink, f);
}

template<typename T> struct cut_edge {
    T start;
    T end;
};

/**
 * Minimum cut from start to terminal
 * Max-flow-min-cut theorem: minimum cut is the edges that are saturated in maximum flow
 */
template<typename Vertex, bool Directed, typename EdgeWeight, typename Function, typename... Args>
std::list<cut_edge<Vertex>>
  minimum_cut(const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input,
              const Vertex& start, const Vertex& terminal,
              Function max_flow_alg = Karzanov_max_flow<Vertex, Directed, EdgeWeight, Args...>) {
    // Use max-flow-min-cut theorem
    graph::graph<Vertex, Directed, true, EdgeWeight, Args...> flow_graph =
      max_flow_alg(input, start, terminal);
    std::list<cut_edge<Vertex>> result;

    // Using theorem, all edges in min-cut must be saturated in max-flow: start by adding these to a
    // set of potential results.
    // Create a copy of graph without potential edges, then see which of them actually are
    // involved in the cut
    graph::graph<Vertex, Directed, true, EdgeWeight, Args...> partition_graph(input);
    for (const Vertex& vertex : input.vertices())
        for (const std::pair<Vertex, EdgeWeight>& edge : flow_graph.edges(vertex))
            if (std::abs(edge.second - input.edge_cost(vertex, edge.first)) < 1e-5) {
                result.emplace_back(cut_edge<Vertex>({vertex, edge.first}));
                partition_graph.remove_edge(vertex, edge.first);
            } else {
                // flow should not flow back from cut region to uncut region (double-counting error)
                // pseudo-residual: make sure start of edge is reachable
                partition_graph.force_add(edge.first, vertex);
            }

    std::unordered_set<Vertex, Args...> reachable_vertices;
    breadth_first(partition_graph, start,
                  [&reachable_vertices](const Vertex& v) { reachable_vertices.insert(v); });

    result.remove_if([&reachable_vertices](const cut_edge<Vertex>& edge) {
        // Only allow edges that cross the reachable-unreachable boundary
        return !((reachable_vertices.find(edge.start) != reachable_vertices.end()) &&
                 (reachable_vertices.find(edge.end) == reachable_vertices.end()));
    });

    return result;
}

} // namespace graph_alg

#endif // GRAPH_ALG_MIN_FLOW_H
