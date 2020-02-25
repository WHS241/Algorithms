// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_CPP
#define GRAPH_CLOSURE_CPP

#include <graph/search.h>
#include <structures/graph.h>

namespace graph_alg {

template <typename T, bool Weighted>
graph::graph<T, false, Weighted> k_core(graph::graph<T, false, Weighted> src, uint32_t k)
{

    std::unordered_map<T, uint32_t> degree;
    std::list<T> to_remove;
    for (const T& vertex : src.vertices()) {
        degree[vertex] = src.degree(vertex);
        if (degree[vertex] < limit)
            to_remove.push_back(vertex);
    }

    while (!to_remove.empty()) {
        T target = to_remove.back();
        to_remove.pop_front();
        for (const T& neighbor : src.neighbors(target)) {
            --degree[neighbor];
            if (degree[neighbor] < limit)
                to_remove.push_back(neighbor);
        }
        src.remove(target);
    }

    return src;
}

template <typename T, bool Directed, bool Weighted>
graph::graph<T, Directed, Weighted> transitive_closure(
    const graph::graph<T, Directed, Weighted>& src)
{
    graph::graph<T, Directed, Weighted> result;
    std::vector<T> vertices = src.vertices();
    for (const T& vertex : vertices)
        result.add_vertex(vertex);
    for (const T& vertex : vertices)
        graph_alg::depth_first(
            src, vertex,
            [&result](const T& dest) {
                if constexpr (Directed) {
                    result.force_add(vertex, dest);
                } else {
                    result.set_edge(vertex, dest);
                }
            },
            [](const T&, const T&) {});

    return result;
}
}

#endif // GRAPH_CLOSURE_CPP