#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>

#include <structures/graph.h>

template <bool Directed, bool Weighted>
graph::graph<int, Directed, Weighted> random_graph(
    std::mt19937_64& gen, bool cyclic = true, graph::graph_type type = graph::adj_list)
{
    std::uniform_int_distribution<uint32_t> dist(1, 50);
    std::uniform_real_distribution<double> weight(0, 1000);

    auto num_vertices = dist(gen);
    graph::graph<int, Directed, Weighted> graph(type);

    if (Directed && cyclic) {
        for (uint32_t i = 0; i < num_vertices; ++i)
            graph.add_vertex(graph.order());

        if (num_vertices <= 1)
            return graph;

        std::uniform_int_distribution<uint32_t> out_degree(0, num_vertices - 2);
        for (uint32_t i = 0; i < num_vertices; ++i) {
            uint32_t degree(out_degree(gen));
            for (uint32_t j = 0; j < degree; ++j) {
                uint32_t dest(out_degree(gen));
                if (dest >= i)
                    ++dest;

                if constexpr (Weighted)
                    graph.set_edge(i, dest, weight(gen));
                else
                    graph.set_edge(i, dest);
            }
        }
    } else {
        for (uint32_t i = 0; i < num_vertices; ++i) {
            uint32_t current_size = graph.add_vertex(graph.order());
            if (current_size <= 1)
                continue;

            std::uniform_int_distribution<uint32_t> low_degree(0, current_size - 2);
            uint32_t edges = low_degree(gen);
            for (uint32_t i = 0; i < edges; ++i)
                if constexpr (Weighted)
                    graph.set_edge(current_size - 1, low_degree(gen), weight(gen));
                else
                    graph.set_edge(current_size - 1, low_degree(gen));
        }
    }

    return graph;
}

template <bool Weighted>
graph::graph<int, false, Weighted> random_bipartite_graph(
    std::mt19937_64& gen, graph::graph_type type = graph::adj_list) {

    std::uniform_int_distribution<uint32_t> vert_dist(1, 50);
    std::uniform_real_distribution<double> weight(0, 1000);

    uint32_t num_vertices_first = vert_dist(gen);
    uint32_t num_vertices_second = vert_dist(gen);
    graph::graph<int, false, Weighted> graph(type);

    for (uint32_t i = 0; i < num_vertices_first + num_vertices_second; ++i)
        graph.add_vertex(graph.order());
    
    // shuffle vertices, so it is not obvious this is bipartite
    std::uniform_int_distribution<> random_bool(0,1);
    std::vector<uint32_t> first_set, second_set;
    first_set.reserve(num_vertices_first);
    second_set.reserve(num_vertices_second);

    for (uint32_t i = 0; i < graph.order(); ++i) {
        if (num_vertices_first > 0 && num_vertices_second > 0) {
            if (random_bool(gen)) {
                first_set.push_back(i);
                --num_vertices_first;
            } else {
                second_set.push_back(i);
                --num_vertices_second;
            }
        } else if (num_vertices_first > 0) {
            first_set.push_back(i); // don't care about num_vertices anymore
        } else {
            second_set.push_back(i);
        }
    }

    for (uint32_t u : first_set)
        for (uint32_t v : second_set)
            if (random_bool(gen))
                if constexpr (Weighted)
                    graph.force_add(u, v, weight(gen));
                else
                    graph.force_add(u, v);

    return graph;
}

std::vector<double> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine)
{
    std::vector<double> result(size);
    std::uniform_real_distribution<> dist(0, bound);
    std::generate(result.begin(), result.end(), [&dist, &engine]() { return dist(engine); });
    return result;
}

#endif // GENERATOR_H