//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_SPANNING_TREE_H
#define GRAPH_ALG_SPANNING_TREE_H

#include <set>

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
template <typename Vertex, typename EdgeWeight, typename... Args, typename VertexComp>
graph::graph<Vertex, false, true, EdgeWeight, Args...> minimum_spanning_Boruvka(
    const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input, VertexComp comp)
{
    std::vector<Vertex> input_vertices = input.vertices();
    uint32_t num_input_components = connected_components(input).size();

    graph::graph<Vertex, false, true, EdgeWeight, Args...> result;
    for (Vertex& vertex : input_vertices) {
        result.add_vertex(vertex);
    }

    struct edge_info {
        Vertex start;
        Vertex terminal;
        EdgeWeight weight;
        uint32_t analogue;
    };

    auto order_verts = [&comp](std::pair<Vertex, Vertex>& vert_pair) {
        if (comp(vert_pair.second, vert_pair.first)) {
            std::swap(vert_pair.first, vert_pair.second);
        }
    };

    for (std::list<std::unordered_set<Vertex, Args...>> current_components
         = connected_components(result);
         current_components.size() != num_input_components;
         current_components = connected_components(result)) {
        // O(log V iterations)

        // contain a list of edges to add
        std::vector<edge_info> to_add(current_components.size(),
            edge_info({ input_vertices.front(), input_vertices.front(), EdgeWeight(), -1 }));

        // determine which component each vertex is a member of
        std::unordered_map<Vertex, uint32_t, Args...> member_of;
        uint32_t i = 0;
        for (const std::unordered_set<Vertex, Args...>& comp : current_components) {
            for (const Vertex& v : comp)
                member_of[v] = i;
            ++i;
        }

        for (const Vertex& v : input_vertices) {
            for (const std::pair<Vertex, EdgeWeight>& edge : input.edges(v)) {
                uint32_t first_comp(member_of[edge.first]), second_comp(member_of[v]);
                if (first_comp != second_comp) {
                    edge_info &first_edge = to_add[first_comp], &second_edge = to_add[second_comp];
                    std::pair<Vertex, Vertex> curr_pair({ edge.first, v }),
                        first_pair({ first_edge.start, first_edge.terminal }),
                        second_pair({ second_edge.start, second_edge.terminal });
                    order_verts(curr_pair);
                    order_verts(first_pair);
                    order_verts(second_pair);
                    // Compare edge to first to see if we should swap it out
                    if (first_edge.start == first_edge.terminal // assign if no edge found yet
                        || edge.second < first_edge.weight // smaller-weight edge
                        || (edge.second == first_edge.weight // tiebreaker: compare vertices
                            && !comp(first_pair.first, curr_pair.first)
                            && (comp(curr_pair.first, first_pair.first)
                                || comp(curr_pair.second, first_pair.second)))) {
                        first_edge = { edge.first, v, edge.second, second_comp };
                    }

                    // same for second
                    if (second_edge.start == second_edge.terminal
                        || edge.second < second_edge.weight
                        || (edge.second == second_edge.weight
                            && !comp(second_pair.first, curr_pair.first)
                            && (comp(curr_pair.first, second_pair.first)
                                || comp(curr_pair.second, second_pair.second)))) {
                        second_edge = { v, edge.first, edge.second, first_comp };
                    }
                }
            }
        }

        // add the edges we found
        for (const edge_info& e : to_add) {
            // check existence
            if (e.start != e.terminal) {
                edge_info& e2 = to_add[e.analogue];
                // don't double add
                if (!(e.start == e2.terminal && e.terminal == e2.start
                        && e.analogue < e2.analogue)) {
                    result.force_add(e.start, e.terminal, e.weight);
                }
            }
        }
    }

    return result;
}
template <typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...> minimum_spanning_Boruvka(
    const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input)
{
    return minimum_spanning_Boruvka(input, std::less<Vertex>());
}

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
template <typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...> minimum_spanning_Prim(
    const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input)
{
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
        return edge { value, value, EdgeWeight() };
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
    typedef typename heap::node_base<edge, decltype(compare)>::node node;
    heap::Fibonacci<edge, decltype(compare)> heap(compare);
    std::unordered_map<Vertex, node*, Args...> tracker;

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
                node* ptr = it->second;
                const node& current_node = *ptr;
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
template <typename Vertex, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, true, EdgeWeight, Args...> minimum_spanning_Kruskal(
    const graph::graph<Vertex, false, true, EdgeWeight, Args...>& input)
{
    std::vector<Vertex> vertices = input.vertices();

    struct edge {
        Vertex start;
        Vertex terminal;
        EdgeWeight weight;
    };
    std::unordered_set<Vertex, Args...> processed_vertices; // used to prevent double-counting edges

    auto edge_compare = [](const edge& x, const edge& y) { return x.weight < y.weight; };
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
}

#endif // GRAPH_ALG_SPANNING_TREE_H
