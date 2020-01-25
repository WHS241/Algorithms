//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_SPANNING_TREE_CPP
#define GRAPH_ALG_SPANNING_TREE_CPP

#include <set>

#include <structures/disjoint_set.h>
#include <structures/graph.h>
#include <structures/heap>

#include <graph/components.h>

namespace graph_alg {
template <typename T>
graph::graph<T, false, true> minimum_spanning_Boruvka(const graph::graph<T, false, true>& input)
{
    std::vector<T> input_vertices = input.vertices();
    uint32_t num_input_components = connected_components(input).size();

    struct edge {
        T start;
        T terminal;
        double weight;
    };

    graph::graph<T, false, true> result;
    for (T& vertex : input_vertices) {
        result.add_vertex(vertex);
    }

    // Treat each vertex as its own subtree initially
    disjoint_set<T> tree_components(input_vertices.begin(), input_vertices.end());

    std::unordered_map<T, edge> to_add; // keeps track of what edges to add to MST

    while (tree_components.num_sets() > num_input_components) { // O(log V iterations)
        for (const T& vertex : input_vertices) {
            // each iteration, add shortest edge out of every subtree
            T component = tree_components.find(vertex);
            auto edges = input.edges(vertex);

            // ignore edges that create cycles or are already added
            for (auto it = edges.begin(); it != edges.end();) {
                if (component == tree_components.find(it->first)) {
                    it = edges.erase(it);
                } else {
                    ++it;
                }
            }

            if (!edges.empty()) {
                auto least_weight_edge = *std::min_element(edges.begin(), edges.end(),
                    [](const auto& x, const auto& y) { return x.second < y.second; });

                edge found_edge;
                found_edge.start = vertex;
                found_edge.terminal = least_weight_edge.first;
                found_edge.weight = least_weight_edge.second;

                // not adding yet because we need to find all shortest edges first
                if (to_add.find(component) == to_add.end()
                    || found_edge.weight < to_add[component].weight) {
                    to_add[component] = found_edge;
                }
            }
        }

        // add edge and update connected components
        for (const auto& edge_to_insert : to_add) {
            result.set_edge(edge_to_insert.second.start, edge_to_insert.second.terminal,
                edge_to_insert.second.weight);
            tree_components.union_(edge_to_insert.second.start, edge_to_insert.second.terminal);
        }
        to_add.clear();
    }

    return result;
}

template <typename T>
graph::graph<T, false, true> minimum_spanning_Prim(const graph::graph<T, false, true>& input)
{
    struct edge {
        T current;
        T from;
        double weight;
    };

    std::vector<T> vertices = input.vertices();
    graph::graph<T, false, true> result;

    for (T& vertex : vertices)
        result.add_vertex(vertex);

    std::vector<edge> data_map(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data_map.begin(), [](const T& value) {
        edge result;
        result.current = value;
        result.from = value;
        result.weight
            = std::numeric_limits<double>::max(); // orders unconnected vertices after connected
        return result;
    });

    // the heap structure used here determines the runtime of the algorithm
    // must be a node_base
    // Peculiarities with binomial heap make it incompatible.
    auto compare = [](const edge& x, const edge& y) { return x.weight < y.weight; };
    typedef typename heap::node_base<edge, decltype(compare)>::node node;
    heap::Fibonacci<edge, decltype(compare)> heap(compare);
    std::unordered_map<T, node*> tracker;

    auto it1 = vertices.begin();
    for (auto& vertex_data : data_map) {
        tracker[*it1] = heap.add(vertex_data);
        ++it1;
    }

    auto heap_ptr = &heap;

    while (!heap_ptr->empty()) {
        // every iteration, add the shortest edge out of the current MST
        edge to_add = heap.remove_root();
        tracker.erase(to_add.current);

        // don't allow self-loops; may add unconnected vertex if graph is not connected
        if (to_add.from != to_add.current)
            result.set_edge(to_add.from, to_add.current, to_add.weight);

        // update heap values with newly added vertex regardless
        for (const T& neighbor : input.neighbors(to_add.current)) {
            auto it = tracker.find(neighbor);
            if (it != tracker.end()) {
                node* ptr = it->second;
                const node* read_ptr = ptr;
                double edge_cost = input.edge_cost(to_add.current, neighbor);

                if (edge_cost < (*read_ptr)->weight) {
                    edge replace(**read_ptr);
                    replace.from = to_add.current;
                    replace.weight = edge_cost;
                    heap.decrease(ptr, replace);
                }
            }
        }
    }

    return result;
}

template <typename T>
graph::graph<T, false, true> minimum_spanning_Kruskal(const graph::graph<T, false, true>& input)
{
    std::vector<T> vertices = input.vertices();

    struct edge {
        T start;
        T terminal;
        double weight;
    };
    std::unordered_set<T> processed_vertices;

    auto edge_compare = [](const edge& x, const edge& y) { return x.weight < y.weight; };
    std::set<edge, decltype(edge_compare)> edges(edge_compare); // all edges, ordered by weight

    graph::graph<T, false, true> result;

    for (T& vertex : vertices) {
        result.add_vertex(vertex);
        processed_vertices.insert(vertex);

        // insert all edges into set
        for (auto curr_edge : input.edges(vertex))
            if (processed_vertices.find(curr_edge.first) == processed_vertices.end()) {
                edge current;
                current.start = vertex;
                current.terminal = curr_edge.first;
                current.weight = curr_edge.second;
                edges.insert(current);
            }
    }

    disjoint_set<T> components(vertices.begin(), vertices.end());
    uint32_t num_input_components = graph_alg::connected_components(input).size();

    // only add edges between disjoint components: use disjoint set and union-find
    for (const edge& current : edges) {
        if (components.size() == num_input_components)
            break;

        if (components.find(current.start) != components.find(current.terminal)) {
            result.set_edge(current.start, current.terminal, current.weight);
            components.union_(current.start, current.terminal);
        }
    }

    return result;
}
}

#endif // GRAPH_ALG_SPANNING_TREE_CPP
