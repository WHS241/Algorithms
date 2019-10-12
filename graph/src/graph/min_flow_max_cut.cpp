//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_MIN_FLOW_CPP
#define GRAPH_ALG_MIN_FLOW_CPP
#include <iostream>

#include <structures/graph.h>

#include <graph/path.h>

namespace graph_alg {
template <typename T, bool Directed, typename Function>
graph::graph<T, Directed, true> Ford_Fulkerson(
    const graph::graph<T, Directed, true>& input, T source, T sink, Function f)
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
        for (const T& vertex : vertices) {
            residual.add_vertex(vertex);
            for (const auto& edge : input.edges(vertex))
                residual.set_edge(vertex, edge.first, edge.second);
        }
    }

    try {
        while (true) {
            // use function to find augment paths
            std::list<std::list<T>> paths;
            if constexpr (std::is_invocable_r_v<std::list<T>, Function, graph::graph<T, true, true>,
                              T, T>) {
                // function returns exactly one path
                paths.push_back(f(residual, source, sink));
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
                        blocked = std::isnan(allowed_flow);
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

                        if (result.has_edge(*edge_end, *edge_start)) {
                            // cancel out backwards flow before adding forward flow
                            double current_back_flow = result.edge_cost(*edge_end, *edge_start);
                            if (current_back_flow - allowed_flow > 1e-10) {
                                result.set_edge(
                                    *edge_end, *edge_start, current_back_flow - allowed_flow);
                                // residuals and both directions must be non-NaN
                                residual.set_edge(
                                    *edge_start, *edge_end, forward_residual - allowed_flow);
                                residual.set_edge(
                                    *edge_end, *edge_start, backward_residual + allowed_flow);
                            } else {
                                result.remove_edge(*edge_end, *edge_start);
                                if (std::abs(current_back_flow - allowed_flow) > 1e-10) {
                                    result.set_edge(
                                        *edge_start, *edge_end, allowed_flow - current_back_flow);
                                    // special case if we've saturated the forward edge
                                    if (allowed_flow - current_back_flow - forward_residual
                                        <= 1e-10) {
                                        residual.remove_edge(*edge_start, *edge_end);
                                    } else {
                                        residual.set_edge(*edge_start, *edge_end,
                                            forward_residual - allowed_flow);

                                        // unsaturating back-edge? need a check
                                        residual.set_edge(*edge_end, *edge_start,
                                            allowed_flow
                                                + (std::isnan(backward_residual)
                                                        ? 0
                                                        : backward_residual));
                                    }
                                } else {
                                    // no flow in improved solution along this edge
                                    // residuals are equal to corresponding edge weight
                                    // forward edge may not necessarily exist, but backward must
                                    residual.set_edge(*edge_end, *edge_start,
                                        input.edge_cost(*edge_end, *edge_start));
                                    double input_forward = input.edge_cost(*edge_start, *edge_end);
                                    if (std::isnan(input_forward))
                                        residual.remove_edge(*edge_start, *edge_end);
                                    else
                                        residual.set_edge(*edge_start, *edge_end, input_forward);
                                }
                            }
                        } else if (result.has_edge(*edge_start, *edge_end)) {
                            // augment flow
                            result.set_edge(*edge_start, *edge_end,
                                allowed_flow + result.edge_cost(*edge_start, *edge_end));
                            // backwards residual must exist, but not necessarily forward
                            residual.set_edge(
                                *edge_end, *edge_start, backward_residual + allowed_flow);
                            if (forward_residual - allowed_flow > 1e-10)
                                residual.set_edge(
                                    *edge_start, *edge_end, forward_residual - allowed_flow);
                            else
                                residual.remove_edge(*edge_start, *edge_end);
                        } else {
                            // add forward flow
                            result.set_edge(*edge_start, *edge_end, allowed_flow);

                            // forward residual must exist BEFORE adding flow, might not after
                            // backward residual must exist AFTER adding flow, might not before
                            if (forward_residual - allowed_flow > 1e-10)
                                residual.set_edge(
                                    *edge_start, *edge_end, forward_residual - allowed_flow);
                            else
                                residual.remove_edge(*edge_start, *edge_end);

                            residual.set_edge(*edge_end, *edge_start,
                                allowed_flow
                                    + (std::isnan(backward_residual) ? 0 : backward_residual));
                        }
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
            for (const auto& flow_edge : result.edges(vertex))
                undirected_result.set_edge(vertex, flow_edge.first, flow_edge.second);

        return undirected_result;
    }
}

template <typename T, bool Directed>
graph::graph<T, Directed, true> Edmonds_Karp(
    const graph::graph<T, Directed, true>& input, T source, T sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<T, true, true>& residual, const T& source, const T& sink) {
            return graph_alg::least_edges_path(residual, source, sink);
        });
}

template <typename T, bool Directed>
graph::graph<T, Directed, true> Dinitz(
    const graph::graph<T, Directed, true>& input, T source, T sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<T, true, true>& residual, const T& source, const T& sink) {
            // construct a level graph
            std::unordered_map<T, uint32_t> layer;
            layer[source] = 0;
            graph_alg::breadth_first(residual, source, [&residual, &layer, &sink](const T& vertex) {
                if (vertex == sink)
                    return true;

                for (const auto& neighbor : residual.neighbors(vertex))
                    if (layer.find(neighbor) == layer.end())
                        layer[neighbor] = layer[vertex] + 1;

                return false;
            });

            if (layer.find(sink) == layer.end())
                throw graph_alg::no_path_exception();

            graph::graph<T, true, true> layer_graph;
            for (const auto& pair : layer)
                layer_graph.add_vertex(pair.first);

            for (const T& vertex : layer_graph.vertices())
                for (const auto& edge : residual.edges(vertex))
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
}

#endif // GRAPH_ALG_MIN_FLOW_CPP
