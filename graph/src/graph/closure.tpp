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
        if (degree[vertex] < k)
            to_remove.push_back(vertex);
    }

    while (!to_remove.empty()) {
        T target = to_remove.back();
        to_remove.pop_front();
        for (const T& neighbor : src.neighbors(target)) {
            --degree[neighbor];
            if (degree[neighbor] < k)
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
            [&result, &vertex](const T& dest) {
                if(vertex != dest) {
                if constexpr (Directed) {
                    result.force_add(vertex, dest);
                } else {
                    result.set_edge(vertex, dest);
                }
                }
            },
            [](const T&, const T&) {});

    return result;
}

template<typename T, bool Weighted>
graph::graph<T, false, Weighted> Chvatal_Bondy_closure(graph::graph<T, false, Weighted> src, uint32_t k) {
    // bucket sort vertices by their degree
    std::vector<std::list<T>> bucketed_vertices(src.order());
    for(const T& v : src.vertices())
        bucketed_vertices[src.degree(v)].push_back(v);
    
    // go through the buckets and link vertices whose degree exceed threshold
    for(uint32_t i = 0; i < bucketed_vertices.size(); ++i) {
        for(auto it = bucketed_vertices[i].begin(); it != bucketed_vertices[i].end();) {
            bool added = false;
            for(uint32_t j = src.order() - 1; j != -1 && i + j >= k; --j) {
                for(auto it2 = bucketed_vertices[j].begin(); it2 != bucketed_vertices[j].end();) {
                    if(*it != *it2 && !src.has_edge(*it, *it2)) {
                        added = true;
                        src.force_add(*it, *it2);
                        auto temp = it2;
                        ++it2;
                        bucketed_vertices[j + 1].splice(bucketed_vertices[j + 1].end(), bucketed_vertices[j + 1], temp);
                    } else {
                        ++it2;
                    }
                }
            }
            if(added) {
                bucketed_vertices[src.degree(*it)].push_back(*it);
                it = bucketed_vertices[i].erase(it);
            } else {
                ++it;
            }
        }
    }

    return src;
}
}

#endif // GRAPH_CLOSURE_CPP