#ifndef GRAPH_BIPARTITE_H
#define GRAPH_BIPARTITE_H
#include <unordered_set>
#include <utility>

#include <structures/graph.h>

#include "max_flow_min_cut.h"
#include "search.h"

namespace graph_alg {
/**
 * Check if a graph is bipartite
 * If graph is bipartite, returns the two independent sets that make it up
 * If not, returns empty sets
 *
 * Θ(m+n)
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::pair<std::unordered_set<Vertex, Args...>, std::unordered_set<Vertex, Args...>>
  verify_bipartite(const graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>& input) {
    std::pair<std::unordered_set<Vertex, Args...>, std::unordered_set<Vertex, Args...>> result;

    // Simple forcing approach: each vertex must be in a different set than neighbors
    depth_first_forest(input, input.vertices()[0], [&result, &input](const Vertex& v) {
        bool in_second = (result.second.find(v) != result.second.end());
        if (!in_second)
            // either this vertex is in the first set or is a root.
            // Either way, we can put it in the first
            result.first.insert(v);

        for (const Vertex& w : input.neighbors(v)) {
            // If we've forced the current vertex into the same bin as a neighbor,
            // the graph cannot be bipartite
            if ((in_second && result.second.find(w) != result.second.end()) ||
                (!in_second && result.first.find(w) != result.first.end())) {
                result.first.clear();
                result.second.clear();
                return true; // no need to keep going: terminate early
            }

            // forcing step
            if (in_second)
                result.first.insert(w);
            else
                result.second.insert(w);
        }
        return false; // so far still valid
    });

    return result; // would be cleared during DFS if not bipartite
}

/**
 * On a bipartite graph, find the maximum matching
 * Essentially Dinic's algorithm, but on a simple 0-1 network
 *
 * John Hopcroft, Richard Karp
 * An n^5/2 algorithm for maximum matchings in bipartite graphs
 * (1973)
 * doi:10.1137/0202019
 *
 * Alexander Karzanov
 * An exact estimate of an algorithm for finding a maximum flow, applied to the problem on
 * representatives (1973)
 * Problems in Cybernetics
 *
 * O(sqrt(V) E)
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::list<std::pair<Vertex, Vertex>> maximum_bipartite_matching(
  const graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>& input) {
    std::vector<Vertex> vertices = input.vertices();
    if (vertices.empty()) // there are no vertices
        return std::list<std::pair<Vertex, Vertex>>();

    std::pair<std::unordered_set<Vertex, Args...>, std::unordered_set<Vertex, Args...>> sets =
      verify_bipartite(input);
    if (sets.first.empty() && sets.second.empty())
        throw std::domain_error("Not a bipartite graph");
    if (sets.first.empty() || sets.second.empty()) // there are no edges
        return std::list<std::pair<Vertex, Vertex>>();

    // Create the flow graph
    graph::graph<uint32_t, true, true, uint32_t> flow_graph(graph::adj_list);
    for (uint32_t i = 0; i < vertices.size(); ++i)
        flow_graph.add_vertex(i);

    uint32_t dummy_1 = vertices.size(), dummy_2 = vertices.size() + 1;
    flow_graph.add_vertex(dummy_1); // add source to first set
    flow_graph.add_vertex(dummy_2); // add sink to second set

    // populate in correct direction with fixed cost
    const uint32_t EDGE_FLOW = 1;
    const std::unordered_map<Vertex, uint32_t, Args...>& translation =
      input.get_translation(); // for quick lookup
    for (const Vertex& v : sets.first) {
        for (const Vertex& w : input.neighbors(v))
            flow_graph.force_add(translation.at(v), translation.at(w), EDGE_FLOW);

        flow_graph.force_add(dummy_1, translation.at(v), EDGE_FLOW);
    }
    for (const Vertex& v : sets.second)
        flow_graph.force_add(translation.at(v), dummy_2, EDGE_FLOW);

    graph::graph<uint32_t, true, true, uint32_t> flow_result =
      Dinic_max_flow(flow_graph, dummy_1, dummy_2);
    std::list<std::pair<Vertex, Vertex>> result;
    for (const Vertex& v : sets.first) {
        std::list<uint32_t> neighbors =
          flow_result.neighbors(translation.at(v)); // if non-empty, we found a flow to a neighbor
        if (neighbors.size() > 0)
            result.emplace_back(v, vertices[neighbors.front()]);
    }

    return result;
}
} // namespace graph_alg

#endif // GRAPH_BIPARTITE_H
