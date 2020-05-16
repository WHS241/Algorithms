#ifndef APPROX_GRAPH_H
#define APPROX_GRAPH_H

#include <list>
#include <unordered_map>

#include <graph/bipartite.h>

#include <structures/graph.h>
#include <structures/heap>

namespace approx {

// NPC problem: Vertex Cover
// Provides a vertex cover of size < 2 * optimal
// Fanica Gavril and Mihalis Yannakakis
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::list<Vertex> vertex_cover_edge_double(
    graph::graph<Vertex, false, Weighted, EdgeWeight, Args...> input)
{
    std::list<Vertex> result;

    // Candidate edge: store endpoints and cumulative degree of endpoints
    struct candidate {
        Vertex first_end;
        Vertex second_end;
        uint32_t cumul_degree;
    };

    // Need a hash function for std::pair
    struct pair_hash {
        std::size_t operator()(const std::pair<Vertex, Vertex>& x) const
        {
            return std::hash<Vertex>()(x.first) ^ std::hash<Vertex>()(x.second);
        }
    };

    // Chances are when we use this algorithm we want a lower bound on the optimal size
    // so let's try to run it as badly as possible: select as many vertices as we can.
    // Choose the edge whose vertices have smallest sum of degrees
    auto compare
        = [](const candidate& x, const candidate& y) { return x.cumul_degree < y.cumul_degree; };
    heap::Fibonacci<candidate, decltype(compare)> edge_list(compare);
    auto heap_ptr = &edge_list;
    std::unordered_map<std::pair<Vertex, Vertex>,
        typename heap::node_base<candidate, decltype(compare)>::node*, pair_hash>
        processed;

    for (const Vertex& u : input.vertices()) {
        for (const Vertex& v : input.neighbors(u)) {
            if (processed.find({ v, u }) != processed.end()) {
                processed[{ u, v }] = heap_ptr->add({ u, v, input.degree(v) + input.degree(u) });
            }
        }
    }

    // find an edge to remove and remove it as well as the edges the endpoints cover
    while (heap_ptr->size() != 0) {
        candidate to_remove = heap_ptr->remove_root();

        // update degrees of the neighbors of each vertex we are adding
        for (const Vertex& v : input.neighbors(to_remove.first_end)) {
            if (v != to_remove.second_end) {
                auto covered_edge_it = processed.find({ to_remove.first_end, v });
                if (covered_edge_it == processed.end())
                    covered_edge_it = processed.find({ v, to_remove.first_end });
                heap_ptr->decrease(covered_edge_it->second, { v, to_remove.first_end, 0 });
            }
        }
        for (const Vertex& v : input.neighbors(to_remove.second_end)) {
            if (v != to_remove.first_end) {
                auto covered_edge_it = processed.find({ to_remove.second_end, v });
                if (covered_edge_it == processed.end())
                    covered_edge_it = processed.find({ v, to_remove.second_end });
                heap_ptr->decrease(covered_edge_it->second, { v, to_remove.second_end, 0 });
            }
        }

        // pop off edges that go between already-added vertices
        while (heap_ptr->get_root().cumul_degree == 0)
            heap_ptr->remove_root();

        // add vertices to cover and remove from graph
        input.remove(to_remove.first_end);
        input.remove(to_remove.second_end);
        result.push_back(to_remove.first_end);
        result.push_back(to_remove.second_end);
    }

    return result;
}

// NPC problem: Graph 3-coloring
// Colors a 3-colorable graph with n vertices using O(sqrt(n)) colors
// Avi Wigderson (1983)
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... MapArgs>
std::unordered_map<Vertex, uint32_t, MapArgs...> three_color_Wigderson(
    graph::graph<Vertex, false, Weighted, EdgeWeight, MapArgs...> input)
{
    double bound
        = std::sqrt(2 * input.order()); // determined through balancing: 2n/bound(n) + bound(n) + 1
    std::list<Vertex> candidates;
    uint32_t num_colors = 0;
    std::unordered_map<Vertex, uint32_t, MapArgs...> result;

    // If a vertex has a high degree, we two-color its neighbors and throw out the colors and
    // vertices we used Since we strictly remove vertices, we never get new candidates as algorithm
    // progresses
    for (const Vertex& v : input.vertices())
        if (input.degree(v) > bound)
            candidates.push_back(v);
    for (const Vertex& v : candidates) {
        // degree may have changed as we throw out vertices
        if (result.find(v) == result.end() && input.degree(v) > bound) {
            std::list<Vertex> neighbors = input.neighbors(v);
            graph::graph<Vertex, false, Weighted, EdgeWeight, MapArgs...> subgraph
                = input.generate_induced_subgraph(neighbors.begin(), neighbors.end());
            std::pair<std::unordered_set<Vertex, MapArgs...>,
                std::unordered_set<Vertex, MapArgs...>>
                sets = graph_alg::verify_bipartite(subgraph);

            // if 3-colorable, the neighbors of a single vertex are two colorable
            // we also know by the bound that there are vertices here
            if (sets.first.empty() && sets.second.empty())
                throw std::domain_error("Not 3-colorable");
            for (const Vertex& u : sets.first) {
                result[u] = num_colors + 1;
                input.remove(u);
            }
            for (const Vertex& u : sets.second) {
                result[u] = num_colors + 2;
                input.remove(u);
            }
            num_colors += 2;
        }
    }

    // and now we brute-force/greedy-color the remainder
    for (const Vertex& v : input.vertices()) {
        if (result.find(v) == result.end()) {
            uint32_t color = num_colors + 1;
            std::unordered_set<uint32_t> neighbor_colors;
            for (const Vertex& w : input.neighbors(v))
                if (result.find(w) != result.end())
                    neighbor_colors.insert(result[w]);

            while (neighbor_colors.find(color) != neighbor_colors.end())
                ++color;
            result[v] = color;
        }
    }
    return result;
}
} // namespace approx

#endif // APPROX_GRAPH_H