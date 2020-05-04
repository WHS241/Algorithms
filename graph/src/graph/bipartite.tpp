#ifndef GRAPH_BIPARTITE_CPP
#define GRAPH_BIPARTITE_CPP

#include <graph/max_flow_min_cut.h>
#include <graph/search.h>

namespace graph_alg {
template <typename T, bool Weighted, typename EdgeType, typename Hash, typename KeyEqual>
std::pair<std::unordered_set<T, Hash, KeyEqual>, std::unordered_set<T, Hash, KeyEqual>>
verify_bipartite(const graph::graph<T, false, Weighted, EdgeType, Hash, KeyEqual>& input)
{
    std::pair<std::unordered_set<T, Hash, KeyEqual>, std::unordered_set<T, Hash, KeyEqual>> result;

    depth_first_forest(input, input.vertices()[0], [&result, &input](const T& v) {
        bool in_second = (result.second.find(v) != result.second.end());
        if (!in_second)
            // either this vertex is in the first set or is a root. Either way, we can put it in
            // first
            result.first.insert(v);

        // process all of the current's neighbors
        for (const T& w : input.neighbors(v)) {
            if ((in_second && result.second.find(w) != result.second.end())
                || (!in_second && result.first.find(w) != result.first.end())) {
                // we've forced the current vertex into the same bin as a neighbor: cannot be
                // bipartite
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

template <typename T, bool Weighted, typename... Args>
std::list<std::pair<T, T>> maximum_bipartite_matching(
    const graph::graph<T, false, Weighted, Args...>& input, const T& dummy_1, const T& dummy_2)
{
    std::vector<T> vertices = input.vertices();
    if (vertices.empty())
        return std::list<std::pair<T, T>>();
    if (std::find(vertices.begin(), vertices.end(), dummy_1) != vertices.end()
        || std::find(vertices.begin(), vertices.end(), dummy_2) != vertices.end())
        throw std::invalid_argument("Vertices already exist");

    std::pair<std::unordered_set<T>, std::unordered_set<T>> sets = verify_bipartite(input);
    if (sets.first.empty() && sets.second.empty())
        throw std::domain_error("Not a bipartite graph");
    if (sets.first.empty() || sets.second.empty()) // there are no edges
        return std::list<std::pair<T, T>>();

    // Create the max-flow graph
    graph::graph<T, true, true, Args...> flow_graph(graph::adj_list);
    for (const T& v : vertices)
        flow_graph.add_vertex(v);
    flow_graph.add_vertex(dummy_1); // source to first set
    flow_graph.add_vertex(dummy_2); // sink to second set

    // populate in correct direction with fixed cost
    const double EDGE_FLOW = 1.0;
    for (const T& v : sets.first) {
        for (const T& w : input.neighbors(v))
            flow_graph.force_add(v, w, EDGE_FLOW);

        flow_graph.force_add(dummy_1, v, EDGE_FLOW);
    }
    for (const T& v : sets.second)
        flow_graph.force_add(v, dummy_2, EDGE_FLOW);

    graph::graph<T, true, true, Args...> flow_result = Dinitz(flow_graph, dummy_1, dummy_2);
    std::list<std::pair<T, T>> result;
    for (const T& v : sets.first) {
        std::list<T> neighbors
            = flow_result.neighbors(v); // if non-empty, we found a flow to a neighbor
        if (neighbors.size() > 0)
            result.emplace_back(v, neighbors.front());
    }

    return result;
}
}

#endif // GRAPH_BIPARTITE_CPP