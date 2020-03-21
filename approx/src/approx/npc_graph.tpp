#ifndef APPROX_GRAPH_CPP
#define APPROX_GRAPH_CPP
#include <list>
#include <unordered_map>

#include <graph/bipartite.h>

#include <structures/heap>

namespace approx {

template <typename T, bool Weighted>
std::list<T> vertex_cover_edge_double(graph::graph<T, false, Weighted> input)
{
    // Chances are when we use this algorithm we want a lower bound on the optimal size
    // so let's try to run it as badly as possible: select as many vertices as we can.

    // Candidate edge: store endpoints and cumulative degree of endpoints
    struct candidate {
        T first_end;
        T second_end;
        uint32_t cumul_degree;
        candidate() = default;
    };

    struct pair_hash {
        std::size_t operator()(const std::pair<T, T>& x) const
        {
            return std::hash<T>()(x.first) ^ std::hash<T>()(x.second);
        }
    };

    auto compare
        = [](const candidate& x, const candidate& y) { return x.cumul_degree < y.cumul_degree; };
    heap::Fibonacci<candidate, decltype(compare)> edge_list(compare);
    auto heap_ptr = &edge_list;
    std::unordered_map<std::pair<T, T>,
        typename heap::node_base<candidate, decltype(compare)>::node*, pair_hash>
        processed;

    for (const T& u : input.vertices()) {
        for (const T& v : input.neighbors(u)) {
            if (processed.find({ v, u }) != processed.end()) {
                processed[{ u, v }] = heap_ptr->add({ u, v, input.degree(v) + input.degree(u) });
            }
        }
    }

    std::list<T> result;

    // find edge to remove and remove it as well as the edges the endpoints cover
    while (heap_ptr->size() != 0) {
        candidate to_remove = heap_ptr->remove_root();
        for (const T& v : input.neighbors(to_remove.first_end)) {
            if (v != to_remove.second_end) {
                auto covered_edge_it = processed.find({ to_remove.first_end, v });
                if (covered_edge_it == processed.end())
                    covered_edge_it = processed.find({ v, to_remove.first_end });
                heap_ptr->decrease(covered_edge_it->second, { v, to_remove.first_end, 0 });
            }
        }
        for (const T& v : input.neighbors(to_remove.second_end)) {
            if (v != to_remove.first_end) {
                auto covered_edge_it = processed.find({ to_remove.second_end, v });
                if (covered_edge_it == processed.end())
                    covered_edge_it = processed.find({ v, to_remove.second_end });
                heap_ptr->decrease(covered_edge_it->second, { v, to_remove.second_end, 0 });
            }
        }

        // pop off
        while (heap_ptr->get_root().cumul_degree == 0)
            heap_ptr->remove_root();
        // remove vertices from graph
        input.remove(to_remove.first_end);
        input.remove(to_remove.second_end);

        result.push_back(to_remove.first_end);
        result.push_back(to_remove.second_end);
    }

    return result;
}

template <typename T, bool Weighted>
std::unordered_map<T, uint32_t> three_color_Wigderson(graph::graph<T, false, Weighted> input)
{
    double bound
        = std::sqrt(2 * input.order()); // determined through balancing: 2n/bound(n) + bound(n) + 1
    std::list<T> candidates;
    uint32_t num_colors = 0;
    std::unordered_map<T, uint32_t> result;
    for (const T& v : input.vertices())
        if (input.degree(v) > bound)
            candidates.push_back(v);
    for (const T& v : candidates) {
        // may have changed
        if (result.find(v) == result.end() && input.degree(v) > bound) {
            std::list<T> neighbors = input.neighbors(v);
            graph::graph<T, false, Weighted> subgraph
                = input.generate_induced_subgraph(neighbors.begin(), neighbors.end());
            std::pair<std::unordered_set<T>, std::unordered_set<T>> sets
                = graph_alg::verify_bipartite(subgraph);
            // if 3-colorable, the neighbors of a single vertex are two colorable
            // we also know by the bound that there are vertices here
            if (sets.first.empty() && sets.second.empty())
                throw std::domain_error("Not 3-colorable");
            for (const T& u : sets.first) {
                result[u] = num_colors + 1;
                input.remove(u);
            }
            for (const T& u : sets.second) {
                result[u] = num_colors + 2;
                input.remove(u);
            }
            num_colors += 2;
        }
    }

    // and now we brute-force/greedy-color the remainder
    for (const T& v : input.vertices()) {
        if (result.find(v) == result.end()) {
            uint32_t color = num_colors + 1;
            std::unordered_set<uint32_t> neighbor_colors;
            for (const T& w : input.neighbors(v))
                if (result.find(w) != result.end())
                    neighbor_colors.insert(result[w]);

            while (neighbor_colors.find(color) != neighbor_colors.end())
                ++color;
            result[v] = color;
        }
    }
    return result;
}
}

#endif // APPROX_GRAPH_CPP