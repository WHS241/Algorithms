// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_CPP
#define GRAPH_CLOSURE_CPP

#include <graph/search.h>
#include <structures/graph.h>

namespace graph_alg {

template <typename T, bool Weighted, typename EdgeType, typename... Args>
graph::graph<T, false, Weighted, EdgeType, Args...> k_core(graph::graph<T, false, Weighted, EdgeType, Args...> src, uint32_t k)
{

    std::unordered_map<T, uint32_t, Args...> degree;
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

template <typename T, bool Directed, bool Weighted, typename... Args>
graph::graph<T, Directed, Weighted, Args...> transitive_closure(
    const graph::graph<T, Directed, Weighted, Args...>& src)
{
    graph::graph<T, Directed, Weighted, Args...> result;
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

template <typename T, bool Weighted, typename... Args>
graph::graph<T, false, Weighted, Args...> Chvatal_Bondy_closure(graph::graph<T, false, Weighted, Args...> src, uint32_t k) {
    // bucket sort vertices by their degree
    std::vector<std::list<T>> bucketed_vertices(src.order());
    for(const T& v : src.vertices())
        bucketed_vertices[src.degree(v)].push_back(v);

    struct bucket_vert {
        T value;
        typename std::list<T>::iterator pos;
    };
    std::list<std::pair<bucket_vert,bucket_vert>> to_add;
    
    // go through the buckets and find vertices whose degree exceed threshold and are not yet existing edges
    for(uint32_t i = 0; i < bucketed_vertices.size(); ++i)
        for(auto it = bucketed_vertices[i].begin(); it != bucketed_vertices[i].end(); ++it)
            for(uint32_t j = src.order() - 1; j != -1 && j >= i && i + j >= k; --j)
                for(auto it2 = bucketed_vertices[j].begin(); it != it2 && it2 != bucketed_vertices[j].end(); ++it2)
                    if(!src.has_edge(*it, *it2)) // TODO: Switch to optimized adjacency matrix
                        to_add.emplace_back(bucket_vert({*it, it}), bucket_vert({*it2, it2}));

    // actually add edges
    while(!to_add.empty()) {
        std::pair<bucket_vert, bucket_vert> next = to_add.front();
        to_add.pop_front();
        src.force_add(next.first.value, next.second.value);

        // update
        uint32_t new_degree = src.degree(next.first.value);
        bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(), bucketed_vertices[new_degree - 1], next.first.pos);
        if(k >= new_degree)
            for(auto it = bucketed_vertices[k - new_degree].begin(); it != bucketed_vertices[k - new_degree].end(); ++it)
                if(it != next.first.pos && !src.has_edge(*it, next.first.value))
                    to_add.emplace_back(next.first, bucket_vert({*it, it}));

        new_degree = src.degree(next.second.value);
        bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(), bucketed_vertices[new_degree - 1], next.second.pos);
        if(k >= new_degree)
            for(auto it = bucketed_vertices[k - new_degree].begin(); it != bucketed_vertices[k - new_degree].end(); ++it)
                if(it != next.second.pos && !src.has_edge(*it, next.second.value))
                    to_add.emplace_back(next.second, bucket_vert({*it, it}));
    }

    return src;
}
}

#endif // GRAPH_CLOSURE_CPP