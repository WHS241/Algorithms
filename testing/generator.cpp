
#include "generator.h"

Graph<int> random_graph(
    std::mt19937_64& gen, bool directed, bool weighted, bool cyclic, GraphType type)
{
    std::uniform_int_distribution<uint32_t> dist(0, 20);
    std::uniform_real_distribution<double> weight(0, 1000);

    auto num_vertices = dist(gen);
    Graph<int> graph(directed, weighted, type);

    if (directed && cyclic) {
        for (uint32_t i = 0; i < num_vertices; ++i)
            graph.addVertex(graph.order());

        if (num_vertices <= 1)
            return graph;

        std::uniform_int_distribution<uint32_t> out_degree(0, num_vertices - 2);
        for (uint32_t i = 0; i < num_vertices; ++i) {
            uint32_t degree(out_degree(gen));
            for (uint32_t j = 0; j < degree; ++j) {
                uint32_t dest(out_degree(gen));
                if (dest >= i)
                    ++dest;

                if (weighted)
                    graph.setEdge(i, dest, weight(gen));
                else
                    graph.setEdge(i, dest);
            }
        }
    } else {
        for (uint32_t i = 0; i < num_vertices; ++i) {
            uint32_t current_size = graph.addVertex(graph.order());
            if (current_size <= 1)
                continue;

            std::uniform_int_distribution<uint32_t> low_degree(0, current_size - 2);
            uint32_t edges = low_degree(gen);
            for (uint32_t i = 0; i < edges; ++i)
                if (weighted)
                    graph.setEdge(current_size - 1, low_degree(gen), weight(gen));
                else
                    graph.setEdge(current_size - 1, low_degree(gen));
        }
    }

    return graph;
}

std::vector<double> generateData(uint32_t size, uint32_t bound, std::mt19937_64& engine)
{
    std::vector<double> result(size);
    std::uniform_real_distribution<> dist(0, bound);
    std::generate(result.begin(), result.end(), [&dist, &engine]() { return dist(engine); });
    return result;
}