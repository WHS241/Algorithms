//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_MIN_FLOW_CPP
#define GRAPH_ALG_MIN_FLOW_CPP
#include <structures/graph.h>

#include <graph/path.h>

namespace graph_alg {
template <typename T, bool Directed, typename Function>
graph::graph<T, Directed, true> Ford_Fulkerson(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink, Function f)
{
    static_assert(
        std::is_invocable_v<Function, graph::graph<T, true, true>, T, T>, "Incompatible function");

    graph::graph<T, true, true> result;
    std::vector<T> vertices = input.vertices();
    for (const T& vertex : vertices)
        result.add_vertex(vertex);

    graph::graph<T, true, true> residual;
    if constexpr (Directed) {
        residual = input;
    } else {
        // convert from undirected to directed
        for (const T& vertex : vertices)
            residual.add_vertex(vertex);
        for (const T& vertex : vertices)
            for (const std::pair<T, double>& edge : input.edges(vertex))
                residual.set_edge(vertex, edge.first, edge.second);
    }

    try {
        while (true) {
            // use function to find augment paths
            std::list<std::list<T>> paths;
            if constexpr (std::is_invocable_r_v<std::list<T>, Function, graph::graph<T, true, true>,
                              T, T>) {
                // function returns exactly one path
                paths.push_back(f(residual, source, sink));
            } else if constexpr (std::is_invocable_r_v<std::list<std::list<T>>, Function,
                                     graph::graph<T, true, true>, T, T>) {
                paths = f(residual, source, sink);
            } else {
                // function may return multiple paths
                // make sure these paths are extractable
                static_assert(
                    std::is_same_v<std::list<T>,
                        typename std::iterator_traits<typename std::invoke_result_t<Function,
                            graph::graph<T, true, true>, T, T>::iterator>::value_type>,
                    "Function does not return paths");

                auto found_paths = f(residual, source, sink);
                paths = std::list<std::list<T>>(found_paths.begin(), found_paths.end());
            }
            for (const std::list<T>& augment_path : paths) {
                double allowed_flow = 0;
                bool blocked = false;
                for (auto it = augment_path.begin(); !blocked && it != augment_path.end(); ++it) {
                    if (it == augment_path.begin()) {
                        allowed_flow = residual.edge_cost(source, *it);
                        blocked = std::isnan(allowed_flow);
                    } else {
                        auto temp(it);
                        --temp;
                        double current_edge = residual.edge_cost(*temp, *it);
                        blocked = std::isnan(current_edge);
                        if (!blocked)
                            allowed_flow = std::min(allowed_flow, current_edge);
                    }
                }

                if (!blocked) {
                    // add flow to result graph and update residual
                    for (auto edge_end = augment_path.begin(); edge_end != augment_path.end();
                         ++edge_end) {
                        const T* edge_start;
                        if (edge_end == augment_path.begin()) {
                            edge_start = &source;
                        } else {
                            auto temp(edge_end);
                            --temp;
                            edge_start = &(*temp);
                        }

                        double forward_residual = residual.edge_cost(*edge_start, *edge_end);
                        double backward_residual = residual.edge_cost(*edge_end, *edge_start);
                        if (std::isnan(backward_residual))
                            backward_residual = 0;

                        double current_flow = 0;
                        if (result.has_edge(*edge_end, *edge_start))
                            current_flow -= result.edge_cost(*edge_end, *edge_start);
                        else if (result.has_edge(*edge_start, *edge_end))
                            current_flow += result.edge_cost(*edge_start, *edge_end);

                        double new_flow = current_flow + allowed_flow;
                        double new_forward = forward_residual - allowed_flow;
                        double new_backward = backward_residual + allowed_flow;

                        // Set flow
                        if (new_flow > 1e-10) {
                            if (current_flow < 0)
                                result.remove_edge(*edge_end, *edge_start);
                            result.set_edge(*edge_start, *edge_end, new_flow);
                        } else if (new_flow < -1e-10) {
                            if (current_flow > 0)
                                result.remove_edge(*edge_start, *edge_end);
                            result.set_edge(*edge_end, *edge_start, -new_flow);
                        } else {
                            if (current_flow > 0)
                                result.remove_edge(*edge_start, *edge_end);
                            else if (current_flow < 0)
                                result.remove_edge(*edge_end, *edge_start);
                        }

                        // Set residuals
                        if (std::abs(new_forward) <= 1e-10)
                            residual.remove_edge(*edge_start, *edge_end);
                        else
                            residual.set_edge(*edge_start, *edge_end, new_forward);
                        residual.set_edge(*edge_end, *edge_start, new_backward);
                    }
                }
            }
        }
    } catch (graph_alg::no_path_exception& exception) {
        // indicates no augmenting flow found
        // simply absorb the exception and use as an exit out of while loop
    }

    if constexpr (Directed) {
        return result;
    } else {
        // convert back to undirected
        graph::graph<T, false, true> undirected_result;
        for (const T& vertex : vertices)
            undirected_result.add_vertex(vertex);

        for (const T& vertex : vertices)
            for (const std::pair<T, double>& flow_edge : result.edges(vertex))
                undirected_result.set_edge(vertex, flow_edge.first, flow_edge.second);

        return undirected_result;
    }
}

template <typename T, bool Directed>
graph::graph<T, Directed, true> Edmonds_Karp(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<T, true, true>& residual, const T& source, const T& sink) {
            return graph_alg::least_edges_path(residual, source, sink);
        });
}

template <typename T, bool Directed>
graph::graph<T, Directed, true> Dinitz(
    const graph::graph<T, Directed, true>& input, const T& source, const T& sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<T, true, true>& residual, const T& source, const T& sink) {
            // construct a level graph: start by assigning each vertex to a BFS layer
            std::unordered_map<T, uint32_t> layer;
            layer[source] = 0;
            graph_alg::breadth_first(residual, source, [&residual, &layer, &sink](const T& vertex) {
                if (vertex == sink)
                    return true;

                for (const T& neighbor : residual.neighbors(vertex))
                    if (layer.find(neighbor) == layer.end())
                        layer[neighbor] = layer[vertex] + 1;

                return false;
            });

            if (layer.find(sink) == layer.end())
                throw graph_alg::no_path_exception();

            // Add the edges: only add edges where layer(start) + 1 = layer(end)
            graph::graph<T, true, true> layer_graph;
            for (const std::pair<T, uint32_t>& pair : layer)
                layer_graph.add_vertex(pair.first);

            for (const T& vertex : layer_graph.vertices())
                for (const std::pair<T, double>& edge : residual.edges(vertex))
                    if (layer.find(edge.first) != layer.end()
                        && layer[edge.first] == layer[vertex] + 1)
                        layer_graph.set_edge(vertex, edge.first, edge.second);

            // layer graph constructed, now to find the blocking path
            // essentially just add all possible paths from source to sink in layer graph
            // O(layer of sink * E) which is O(VE)
            // can do with DFS, but we need to be able to visit a vertex multiple times if we want
            // multiple paths
            std::list<std::list<T>> result;
            std::list<T> current;
            graph_alg::depth_first_tree(
                layer_graph, source,
                [&current, &source](const T& vertex) {
                    if (vertex != source)
                        current.push_back(vertex);
                },
                [&result, &current, &sink](const T& parent, const T& child) {
                    if (child == sink)
                        result.push_back(current);
                    current.pop_back();
                });

            return result;
        });
}

template <typename T, bool Directed>
std::list<cut_edge<T>> minimum_cut(
    const graph::graph<T, Directed, true>& input, const T& start, const T& terminal)
{
    // Use max-flow-min-cut theorem
    graph::graph<T, Directed, true> flow_graph = Dinitz(input, start, terminal);
    std::list<cut_edge<T>> result;

    // Using theorem, all edges in min-cut must be saturated in max-flow: start by adding these to a
    // set of potential results Create a copy of graph without potential edges, then see which of
    // them actually are involved in the cut
    graph::graph<T, Directed, true> partition_graph(input);
    for (const T& vertex : input.vertices())
        for (const std::pair<T, double>& edge : flow_graph.edges(vertex))
            if (std::abs(edge.second - input.edge_cost(vertex, edge.first)) < 1e-5) {
                result.push_back(cut_edge<T>(vertex, edge.first));
                partition_graph.remove_edge(vertex, edge.first);
            } else {
                // flow should not flow back from cut region to uncut region (double-counting error)
                // pseudo-residual: make sure start of edge is reachable
                partition_graph.set_edge(edge.first, vertex);
            }

    std::unordered_set<T> reachable_vertices;
    breadth_first(partition_graph, start,
        [&reachable_vertices](const T& v) { reachable_vertices.insert(v); });
    result.remove_if([&reachable_vertices](const cut_edge<T>& edge) {
        // Only allow edges that cross the reachable-unreachable boundary
        return (reachable_vertices.find(edge.start) == reachable_vertices.end())
            || (reachable_vertices.find(edge.end) != reachable_vertices.end());
    });

    return result;
}
}

#endif // GRAPH_ALG_MIN_FLOW_CPP
