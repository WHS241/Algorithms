//
// Created by whs241 on 2019/10/11.
//

#ifndef GRAPH_ALG_MIN_FLOW_H
#define GRAPH_ALG_MIN_FLOW_H

#include <structures/graph.h>

#include "path.h"

namespace graph_alg {
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
 * Must return augmenting path(s) or throw a graph_alg::no_path_exception
 * in order to signal no augmenting path found
 */
template <typename Vertex, bool Directed, typename EdgeWeight, typename Function, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...> Ford_Fulkerson(
    const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input, const Vertex& source,
    const Vertex& sink, Function f)
{
    typedef graph::graph<Vertex, true, true, EdgeWeight, Args...> flow_graph_type;

    static const EdgeWeight zero = EdgeWeight();
    auto is_zero = [&zero](const EdgeWeight& x) {
        if constexpr (std::is_floating_point_v<EdgeWeight>)
            return std::abs(x) <= 1e-10;
        else
            return x == zero;
    };

    static_assert(
        std::is_invocable_v<Function, flow_graph_type, Vertex, Vertex>, "Incompatible function");

    // All vertices accounted for in result
    flow_graph_type result;
    std::vector<Vertex> vertices = input.vertices();
    for (const Vertex& vertex : vertices)
        result.add_vertex(vertex);

    // Build initial residual graph (for zero flow)
    flow_graph_type residual;
    if constexpr (Directed) {
        residual = input;
    } else {
        // convert from undirected to directed
        for (const Vertex& vertex : vertices)
            residual.add_vertex(vertex);
        for (const Vertex& vertex : vertices)
            for (const std::pair<Vertex, EdgeWeight>& edge : input.edges(vertex))
                residual.set_edge(vertex, edge.first, edge.second);
    }

    try {
        while (true) {
            // use function to find augment paths
            std::list<std::list<Vertex>> paths;
            if constexpr (std::is_invocable_r_v<std::list<Vertex>, Function, flow_graph_type,
                              Vertex, Vertex>) {
                // function returns exactly one path
                paths.push_back(f(residual, source, sink));
            } else if constexpr (std::is_invocable_r_v<std::list<std::list<Vertex>>, Function,
                                     flow_graph_type, Vertex, Vertex>) {
                paths = f(residual, source, sink);
            } else {
                // function may return multiple paths
                // make sure these paths are extractable
                static_assert(
                    std::is_same_v<std::list<Vertex>,
                        typename std::iterator_traits<typename std::invoke_result_t<Function,
                            flow_graph_type, Vertex, Vertex>::iterator>::value_type>,
                    "Function does not return paths");

                auto found_paths = f(residual, source, sink);
                paths = std::list<std::list<Vertex>>(found_paths.begin(), found_paths.end());
            }

            for (const std::list<Vertex>& augment_path : paths) {
                // Determine how much flow can go down this path
                // or if it is blocked (not always unlikely)
                EdgeWeight allowed_flow = zero;
                bool blocked = false;
                for (auto it = augment_path.cbegin(); !blocked && it != augment_path.cend(); ++it) {
                    if (it == augment_path.cbegin()) {
                        blocked = !residual.has_edge(source, *it);
                        if (!blocked)
                            allowed_flow = residual.edge_cost(source, *it);
                    } else {
                        auto temp(it);
                        --temp;
                        blocked = !residual.has_edge(*temp, *it);
                        if (!blocked)
                            allowed_flow = std::min(allowed_flow, residual.edge_cost(*temp, *it));
                    }
                }

                if (!blocked) {
                    // add flow to result graph and update residual
                    for (auto edge_end = augment_path.cbegin(); edge_end != augment_path.cend();
                         ++edge_end) {

                        // Get the two endpoints of the augmenting edge
                        const Vertex* edge_start;
                        if (edge_end == augment_path.cbegin()) {
                            edge_start = &source;
                        } else {
                            auto temp(edge_end);
                            --temp;
                            edge_start = &(*temp);
                        }

                        EdgeWeight forward_residual
                            = residual.edge_cost(*edge_start, *edge_end); // must exist
                        EdgeWeight backward_residual = residual.has_edge(*edge_end, *edge_start)
                            ? residual.edge_cost(*edge_end, *edge_start)
                            : zero;

                        // Current and new amount of flow between these vertices
                        EdgeWeight current_flow = zero;
                        if (result.has_edge(*edge_end, *edge_start))
                            current_flow -= result.edge_cost(*edge_end, *edge_start);
                        else if (result.has_edge(*edge_start, *edge_end))
                            current_flow += result.edge_cost(*edge_start, *edge_end);
                        EdgeWeight new_flow = current_flow + allowed_flow;

                        // Calculate new residuals
                        EdgeWeight new_forward = forward_residual - allowed_flow;
                        EdgeWeight new_backward = backward_residual + allowed_flow;

                        // Set new flow
                        if (is_zero(new_flow)) {
                            if (current_flow > zero)
                                result.remove_edge(*edge_start, *edge_end);
                            else if (current_flow < zero)
                                result.remove_edge(*edge_end, *edge_start);
                        } else if (new_flow > zero) {
                            if (current_flow < zero)
                                result.remove_edge(*edge_end, *edge_start);
                            result.set_edge(*edge_start, *edge_end, new_flow);
                        } else if (new_flow < zero) {
                            if (current_flow > zero)
                                result.remove_edge(*edge_start, *edge_end);
                            result.set_edge(*edge_end, *edge_start, -new_flow);
                        }

                        // Set new residuals
                        if (is_zero(new_forward))
                            residual.remove_edge(*edge_start, *edge_end);
                        else
                            residual.set_edge(*edge_start, *edge_end, new_forward);
                        residual.set_edge(*edge_end, *edge_start, new_backward);
                    }
                }
            }
        }
    } catch (graph_alg::no_path_exception&) {
        // indicates no augmenting flow found
        // simply absorb the exception and use as an exit out of while loop
    }

    if constexpr (Directed) {
        return result;
    } else {
        // convert back to undirected
        graph::graph<Vertex, false, true, EdgeWeight, Args...> undirected_result;
        for (const Vertex& vertex : vertices)
            undirected_result.add_vertex(vertex);

        for (const Vertex& vertex : vertices)
            for (const std::pair<Vertex, EdgeWeight>& flow_edge : result.edges(vertex))
                undirected_result.force_add(vertex, flow_edge.first, flow_edge.second);

        return undirected_result;
    }
}

/**
 * Jack Edmonds, Richard Karp
 * Theoretical Improvements in Algorithmic Efficiency for Network Flow Problems
 * (1972) doi:10.1145/321694.321699
 * Θ(V E^2)
 */
template <typename Vertex, bool Directed, typename... Args>
graph::graph<Vertex, Directed, true, Args...> Edmonds_Karp(
    const graph::graph<Vertex, Directed, true, Args...>& input, const Vertex& source,
    const Vertex& sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<Vertex, true, true, Args...>& residual, const Vertex& source,
            const Vertex& sink) { return graph_alg::least_edges_path(residual, source, sink); });
}

/**
 * Yefim Dinitz
 * An algorithm for the solution of the max-flow problem with the polynomial estimation
 * (1970) - Soviet Doklady
 * Θ(V^2 E)
 */
template <typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
graph::graph<Vertex, Directed, true, EdgeWeight, Args...> Dinitz(
    const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input, const Vertex& source,
    const Vertex& sink)
{
    return Ford_Fulkerson(input, source, sink,
        [](const graph::graph<Vertex, true, true, EdgeWeight, Args...>& residual,
            const Vertex& source, const Vertex& sink) {
            // construct a level graph: start by assigning each vertex to a BFS layer
            std::unordered_map<Vertex, uint32_t, Args...> layer;
            layer[source] = 0;
            graph_alg::breadth_first(
                residual, source, [&residual, &layer, &sink](const Vertex& vertex) {
                    if (vertex == sink)
                        return true;

                    for (const Vertex& neighbor : residual.neighbors(vertex))
                        if (layer.find(neighbor) == layer.end())
                            layer[neighbor] = layer[vertex] + 1;

                    return false;
                });

            if (layer.find(sink) == layer.end())
                throw graph_alg::no_path_exception();

            // Add the edges: only add edges where layer(start) + 1 = layer(end)
            graph::graph<Vertex, true, true, EdgeWeight, Args...> layer_graph;
            for (const std::pair<Vertex, uint32_t>& pair : layer)
                layer_graph.add_vertex(pair.first);

            for (const Vertex& vertex : layer_graph.vertices())
                for (const std::pair<Vertex, EdgeWeight>& edge : residual.edges(vertex))
                    if (layer.find(edge.first) != layer.end()
                        && layer[edge.first] == layer[vertex] + 1)
                        layer_graph.force_add(vertex, edge.first, edge.second);

            // layer graph constructed, now to find the blocking path
            // essentially just add all possible paths from source to sink in layer graph
            // O(layer of sink * E) which is O(VE)
            // can do with DFS, but we need to be able to visit a vertex multiple times if we want
            // multiple paths
            std::list<std::list<Vertex>> result;
            std::list<Vertex> current;
            graph_alg::depth_first_tree(
                layer_graph, source,
                [&current, &source](const Vertex& vertex) {
                    if (vertex != source)
                        current.push_back(vertex);
                },
                [&result, &current, &sink](const Vertex& parent, const Vertex& child) {
                    if (child == sink)
                        result.push_back(current);
                    current.pop_back();
                });

            return result;
        });
}

template <typename T> struct cut_edge {
    T start;
    T end;
};

/**
 * Minimum cut from start to terminal
 * Max-flow-min-cut theorem: minimum cut is the edges that are saturated in maximum flow
 */
template <typename Vertex, bool Directed, typename EdgeWeight, typename... Args>
std::list<cut_edge<Vertex>> minimum_cut(
    const graph::graph<Vertex, Directed, true, EdgeWeight, Args...>& input, const Vertex& start,
    const Vertex& terminal)
{
    // Use max-flow-min-cut theorem
    graph::graph<Vertex, Directed, true, EdgeWeight, Args...> flow_graph
        = Dinitz(input, start, terminal);
    std::list<cut_edge<Vertex>> result;

    // Using theorem, all edges in min-cut must be saturated in max-flow: start by adding these to a
    // set of potential results.
    // Create a copy of graph without potential edges, then see which of them actually are
    // involved in the cut
    graph::graph<Vertex, Directed, true, EdgeWeight, Args...> partition_graph(input);
    for (const Vertex& vertex : input.vertices())
        for (const std::pair<Vertex, EdgeWeight>& edge : flow_graph.edges(vertex))
            if (std::abs(edge.second - input.edge_cost(vertex, edge.first)) < 1e-5) {
                result.emplace_back({ vertex, edge.first });
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
        return !((reachable_vertices.find(edge.start) != reachable_vertices.end())
            && (reachable_vertices.find(edge.end) == reachable_vertices.end()));
    });

    return result;
}

}

#endif // GRAPH_ALG_MIN_FLOW_H
