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
            // Note that since we iterate over all vertices every time, the depth of UNION-FIND tree
            // <= 3 so this is constant time
            T component = tree_components.find(vertex);
            std::list<std::pair<T, double>> edges = input.edges(vertex);

            // ignore edges that create cycles or are already added
            for (auto it = edges.begin(); it != edges.end();) {
                if (component == tree_components.find(it->first)) {
                    it = edges.erase(it);
                } else {
                    ++it;
                }
            }

            if (!edges.empty()) {
                std::pair<T, double> least_weight_edge = *std::min_element(edges.begin(),
                    edges.end(), [](const std::pair<T, double>& x, const std::pair<T, double>& y) {
                        return x.second < y.second;
                    });

                edge found_edge;
                found_edge.start = vertex;
                found_edge.terminal = least_weight_edge.first;
                found_edge.weight = least_weight_edge.second;

                // check if it is the smallest out of current set so far
                if (to_add.find(component) == to_add.end()
                    || found_edge.weight < to_add[component].weight) {
                    to_add[component] = found_edge;
                }
            }
        }

        // add edge and update connected components
        for (const std::pair<T, edge>& edge_to_insert : to_add) {
            // tie-breaker: make sure we haven't already connected these two components
            if (tree_components.find(edge_to_insert.second.start)
                != tree_components.find(edge_to_insert.second.terminal)) {
                result.force_add(edge_to_insert.second.start, edge_to_insert.second.terminal,
                    edge_to_insert.second.weight);
                tree_components.union_sets(edge_to_insert.second.start, edge_to_insert.second.terminal);
            }
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

    // Keep track of the shortest edge to each vertex so far
    std::vector<edge> data_map(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data_map.begin(), [](const T& value) {
        // double max orders unconnected vertices after connected
        return edge { value, value, std::numeric_limits<double>::max() };
    });

    // the heap structure used here determines the runtime of the algorithm
    // must be a node_base
    auto compare = [](const edge& x, const edge& y) { return x.weight < y.weight; };
    typedef typename heap::node_base<edge, decltype(compare)>::node node;
    heap::Fibonacci<edge, decltype(compare)> heap(compare);
    std::unordered_map<T, node*> tracker;

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
    std::unordered_set<T> processed_vertices; // used to prevent double-counting edges

    auto edge_compare = [](const edge& x, const edge& y) { return x.weight < y.weight; };
    std::vector<edge> edges; // all edges, ordered by weight
    edges.reserve(vertices.size());

    graph::graph<T, false, true> result;

    for (T& vertex : vertices) {
        result.add_vertex(vertex);
        processed_vertices.insert(vertex);

        // insert all edges into set
        for (const std::pair<T, double>& curr_edge : input.edges(vertex))
            if (processed_vertices.find(curr_edge.first) == processed_vertices.end()) {
                edge current;
                current.start = vertex;
                current.terminal = curr_edge.first;
                current.weight = curr_edge.second;
                edges.push_back(current);
            }
    }

    std::sort(edges.begin(), edges.end(), edge_compare);

    disjoint_set<T> components(vertices.begin(), vertices.end());
    uint32_t num_input_components = graph_alg::connected_components(input).size();

    // only add edges between disjoint components: use disjoint set and union-find
    for (const edge& current : edges) {
        if (components.size() == num_input_components)
            break;

        T root_a = components.find(current.start);
        T root_b = components.find(current.terminal);
        if (root_a != root_b) {
            result.force_add(current.start, current.terminal, current.weight);
            components.union_sets(root_a, root_b);
        }
    }

    return result;
}
}

#endif // GRAPH_ALG_SPANNING_TREE_CPP
