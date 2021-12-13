//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_SPANNING_TREE_H
#define GRAPH_ALG_SPANNING_TREE_H

#include <set>

#include <sequence/order_stats.h>

#include <structures/disjoint_set.h>
#include <structures/graph.h>
#include <structures/heap>

#include "components.h"

/**
 * Algorithms to find the minimum spanning tree
 * Runtimes are given on adjacency lists
 */
namespace graph_alg {
/*
 * Optional second parameter is a comparison operator between vertices (for tie-breaking)
 * Otakar Borůvka
 * O jistém problému minimálním
 * (1926) Práce Mor. Přírodověd. Spol. V Brně III
 * Θ(E log V)
 */
template<typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...>
  minimum_spanning_Boruvka(const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input) {
    struct edge_info {
        Vertex start;
        Vertex terminal;
        EdgeWeight weight;
    };

    std::vector<Vertex> input_vertices = input.vertices();
    uint32_t num_input_components = connected_components(input).size();

    graph::graph<Vertex, false, true, EdgeWeight, Args...> result;
    disjoint_set<Vertex, Args...> tree_components;
    std::unordered_map<Vertex, std::list<std::pair<Vertex, EdgeWeight>>, Args...>
      mutable_list; // need to be able to iterate through and delete edges quickly

    for (Vertex& v : input_vertices) {
        result.add_vertex(v);
        tree_components.insert(v);
        mutable_list[v] = input.edges(v);
    }

    while (tree_components.num_sets() > num_input_components) {
        std::unordered_map<Vertex, edge_info, Args...> to_add;
        for (const Vertex& v : input_vertices) {
            // Each iteration, add shortest edge out of every subtree
            // Since we iterate over all vertices every time, depth of UNION-FIND tree<=3
            // So find() is essentially constant
            Vertex component_root = tree_components.find(v);
            std::list<std::pair<Vertex, EdgeWeight>>& edge_list = mutable_list[v];

            for (auto it = edge_list.begin(); it != edge_list.end();)
                if (component_root == tree_components.find(it->first))
                    it = edge_list.erase(it);
                else
                    ++it;

            if (!mutable_list[v].empty()) {
                std::pair<Vertex, EdgeWeight> least_weight_edge = *std::min_element(
                  edge_list.begin(), edge_list.end(),
                  [](const std::pair<Vertex, EdgeWeight>& x,
                     const std::pair<Vertex, EdgeWeight>& y) { return x.second < y.second; });
                edge_info candidate{v, least_weight_edge.first, least_weight_edge.second};

                // check that it is the smallest out of current set so far
                if (to_add.find(component_root) == to_add.end() ||
                    candidate.weight < to_add[component_root].weight)
                    to_add[component_root] = std::move(candidate);
            }
        }

        // add edges and update connected components
        for (const std::pair<Vertex, edge_info>& candidate : to_add)
            if (tree_components.find(candidate.second.start) !=
                tree_components.find(candidate.second.terminal)) {
                result.force_add(candidate.second.start, candidate.second.terminal,
                                 candidate.second.weight);
                tree_components.union_sets(candidate.second.start, candidate.second.terminal);
            }
    }
    return result;
}
/*
template <typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...> minimum_spanning_Boruvka(
    const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input)
{
    return minimum_spanning_Boruvka(input, [&input](const Vertex& u, const Vertex& v){ return
input.get_translation().at(u) < input.get_translation().at(v); });
}*/

/*
 * Vojtěch Jarník
 * O jistém problému minimálním
 * (1930) hdl:10338.dmlcz/500726
 * Robert Prim
 * Shortest connection networks and some generalizations
 * (1957) doi:10.1002/j.1538-7305.1957.tb01515.x
 * Edsger Dijkstra
 * A note on two problems in connexion with graphs
 * (1959) doi:10.1007/BF01386390
 *
 * Θ(V^2) with array
 * Θ(E + V log V) with Fibonacci heap
 */
template<typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...>
  minimum_spanning_Prim(const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input) {
    struct edge {
        Vertex current;
        Vertex from;
        EdgeWeight weight;
    };

    std::vector<Vertex> vertices = input.vertices();
    graph::graph<Vertex, false, true, EdgeWeight, Args...> result;

    for (const Vertex& vertex : vertices)
        result.add_vertex(vertex);

    // Keep track of the shortest edge to each vertex so far
    std::vector<edge> data_map(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data_map.begin(), [](const Vertex& value) {
        // double max orders unconnected vertices after connected
        return edge{value, value, EdgeWeight()};
    });

    // ordering for edges in heap below; visited before unvisited
    auto compare = [](const edge& x, const edge& y) {
        if (x.current == x.from)
            return false;
        if (y.current == y.from)
            return true;
        return x.weight < y.weight;
    };

    // the heap structure used here determines the runtime of the algorithm
    // must be a node_base
    typedef typename heap::node_base<edge, decltype(compare)>::node_wrapper node_wrapper;
    typedef typename heap::node_base<edge, decltype(compare)>::node node;
    heap::Fibonacci<edge, decltype(compare)> heap(compare);
    std::unordered_map<Vertex, node_wrapper, Args...> tracker;

    auto it1 = vertices.begin();
    for (edge& vertex_data : data_map)
        tracker[*(it1++)] = heap.add(vertex_data);

    auto heap_ptr = &heap;

    while (!heap_ptr->empty()) {
        // every iteration, add the shortest edge out of the current MST
        edge to_add = heap.remove_root();
        tracker.erase(to_add.current);

        // don't allow self-loops; may add unconnected vertex if graph is not connected
        if (to_add.from != to_add.current)
            result.force_add(to_add.from, to_add.current, to_add.weight);

        // update heap values with edges from newly added vertex
        for (const Vertex& neighbor : input.neighbors(to_add.current)) {
            auto it = tracker.find(neighbor);
            if (it != tracker.end()) {
                node_wrapper ptr = it->second;
                const node& current_node = *ptr.get();
                double edge_cost = input.edge_cost(to_add.current, neighbor);

                if (current_node->current == current_node->from // unvisited vertex
                    || edge_cost < current_node->weight) {
                    edge replace(*current_node);
                    replace.from = to_add.current;
                    replace.weight = edge_cost;
                    heap.decrease(ptr, replace);
                }
            }
        }
    }

    return result;
}

/*
 * Joseph Kruskal
 * On the shortest spanning subtree of a graph and the traveling salesman problem
 * (1956) doi:10.1090/S0002-9939-1956-0078686-7
 * Θ(E log V) with sorting and UNION-FIND
 */
template<typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...>
  minimum_spanning_Kruskal(const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input) {
    std::vector<Vertex> vertices = input.vertices();

    struct edge {
        Vertex start;
        Vertex terminal;
        EdgeWeight weight;
    };
    std::unordered_set<Vertex, Args...> processed_vertices; // used to prevent double-counting edges

    auto edge_compare = [](const edge& x, const edge& y) {
        return x.weight < y.weight;
    };
    std::vector<edge> edges; // all edges, ordered by weight
    edges.reserve(vertices.size());

    graph::graph<Vertex, false, true, EdgeWeight, Args...> result;

    for (const Vertex& vertex : vertices) {
        result.add_vertex(vertex);
        processed_vertices.insert(vertex);

        // insert all edges into set
        for (const std::pair<Vertex, EdgeWeight>& curr_edge : input.edges(vertex))
            if (processed_vertices.find(curr_edge.first) == processed_vertices.end()) {
                edge current;
                current.start = vertex;
                current.terminal = curr_edge.first;
                current.weight = curr_edge.second;
                edges.push_back(current);
            }
    }

    std::sort(edges.begin(), edges.end(), edge_compare);

    disjoint_set<Vertex> components(vertices.begin(), vertices.end());
    uint32_t num_input_components = graph_alg::connected_components(input).size();

    // only add edges between disjoint components: use disjoint set and union-find
    for (const edge& current : edges) {
        if (components.size() == num_input_components)
            break;

        Vertex root_a = components.find(current.start);
        Vertex root_b = components.find(current.terminal);
        if (root_a != root_b) {
            result.force_add(current.start, current.terminal, current.weight);
            components.union_sets(root_a, root_b);
        }
    }

    return result;
}

/*
 * Andrew Chi-Chih Yao (姚期智)
 * An O(|E|loglog|V|) algorithm for finding minimum spanning trees
 * (1975) doi:10.1016/0020-0190(75)90056-3
 */
template<typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...>
  minimum_spanning_Yao(const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input) {
    // as long as there are no multi-edges, any graph with <= 2 vertices is its own MST
    if (input.order() < 3)
        return input;

    typedef std::list<std::pair<Vertex, EdgeWeight>> edge_list;

    std::vector<Vertex> input_vertices = input.vertices();

    // Improvement comes from partial-sorting each adjacency list into log V levels
    std::unordered_map<Vertex, std::vector<edge_list>, Args...> leveled_edges;
    uint32_t num_levels = static_cast<uint32_t>(std::round(std::log(input.order())));
    if (num_levels == 0)
        ++num_levels;

    for (const Vertex& v : input_vertices) {
        // Divide-and-conquer to partial sort: Find the middle cutoff, then partition to split the
        // levels
        // Keep track of which levels the current group contains, add to final set if only
        // one level remains
        std::vector<edge_list> final_levels(num_levels);

        // On each pair, first.first is lowest level, first.second is highest, second is the actual
        // list
        std::list<std::pair<std::pair<uint32_t, uint32_t>, edge_list>> processor;

        processor.push_back({{0, num_levels - 1}, input.edges(v)});
        uint32_t current_vertex_degree = input.degree(v);

        while (!processor.empty()) {
            std::pair<std::pair<uint32_t, uint32_t>, edge_list> current =
              std::move(processor.front());
            processor.pop_front();
            if (current.first.first == current.first.second) {
                final_levels[current.first.first] = std::move(current.second);
            } else {
                uint32_t split = (current.first.first + current.first.second) / 2;
                std::pair<std::pair<uint32_t, uint32_t>, edge_list> next_1(
                  {{current.first.first, split}, edge_list()}),
                  next_2({{split + 1, current.first.second}, edge_list()});
                uint32_t cutoff_point = (current_vertex_degree * (split + 1)) / num_levels -
                                        (current_vertex_degree * current.first.first) / num_levels;

                if (cutoff_point != current.second.size()) {
                    std::pair<Vertex, EdgeWeight> pivot = sequence::selection(
                      current.second.begin(), current.second.end(), cutoff_point,
                      [](const std::pair<Vertex, EdgeWeight>& x,
                         const std::pair<Vertex, EdgeWeight>& y) { return x.second < y.second; });

                    // Partition current list based on our median, skip any that are equal to the
                    // pivot
                    for (auto it = current.second.begin(); it != current.second.end();) {
                        if (it->second == pivot.second) {
                            ++it;
                        } else {
                            auto temp(it++);
                            if (temp->second < pivot.second)
                                next_1.second.splice(next_1.second.end(), current.second, temp);
                            else
                                next_2.second.splice(next_2.second.end(), current.second, temp);
                        }
                    }

                    // For ones that are equal, split so that the groups are roughly proportional to
                    // the number of edges they should have
                    double expected_ratio = static_cast<double>(split - current.first.first + 1) /
                                            (current.first.second - split);
                    for (auto it = current.second.begin(); it != current.second.end();) {
                        auto temp(it++);
                        if (next_1.second.size() < expected_ratio * next_2.second.size())
                            next_1.second.splice(next_1.second.end(), current.second, temp);
                        else
                            next_2.second.splice(next_2.second.end(), current.second, temp);
                    }
                } else {
                    // we're just pushing everything into one
                    next_1.second.splice(next_1.second.end(), current.second);
                }
                processor.push_back(std::move(next_1));
                processor.push_back(std::move(next_2));
            }
        }

        leveled_edges.insert(std::make_pair(v, std::move(final_levels)));
    }

    // Preprocessing done: only look at lowest non-discarded level on each vertex
    struct edge_info {
        Vertex start;
        Vertex terminal;
        EdgeWeight weight;
    };

    uint32_t num_input_components = connected_components(input).size();

    graph::graph<Vertex, false, true, EdgeWeight, Args...> result;
    disjoint_set<Vertex, Args...> tree_components;
    std::unordered_map<Vertex, uint32_t, Args...> current_level;

    for (Vertex& v : input_vertices) {
        result.add_vertex(v);
        tree_components.insert(v);
        current_level[v] = 0;
    }

    while (tree_components.num_sets() > num_input_components) {
        std::unordered_map<Vertex, edge_info, Args...> to_add;
        for (const Vertex& v : input_vertices) {
            // Each iteration, add shortest edge out of every subtree
            // Since we iterate over all vertices every time, depth of UNION-FIND tree<=3
            // So find() is essentially constant
            Vertex component_root = tree_components.find(v);
            std::vector<edge_list>& edge_list = leveled_edges.at(v);
            edge_info candidate;
            bool found_edge = false;
            uint32_t& level = current_level[v];

            while (!found_edge && level < num_levels) {
                for (auto it = edge_list[level].begin(); it != edge_list[level].end();)
                    if (component_root == tree_components.find(it->first)) {
                        it = edge_list[level].erase(it);
                    } else {
                        if (!found_edge) {
                            found_edge = true;
                            candidate = {v, it->first, it->second};
                        } else if (it->second < candidate.weight) {
                            candidate = {v, it->first, it->second};
                        }
                        ++it;
                    }

                if (!found_edge) // need to go to the next level
                    ++level;
            }

            // compare with what we have for this component, assuming we have an edge
            if (found_edge && (to_add.find(component_root) == to_add.end() ||
                               candidate.weight < to_add[component_root].weight))
                to_add[component_root] = std::move(candidate);
        }

        // add edges and update connected components
        for (const std::pair<Vertex, edge_info>& candidate : to_add)
            if (tree_components.find(candidate.second.start) !=
                tree_components.find(candidate.second.terminal)) {
                result.force_add(candidate.second.start, candidate.second.terminal,
                                 candidate.second.weight);
                tree_components.union_sets(candidate.second.start, candidate.second.terminal);
            }
    }
    return result;
}
} // namespace graph_alg

#endif // GRAPH_ALG_SPANNING_TREE_H
