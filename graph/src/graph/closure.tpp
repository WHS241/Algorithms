// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_CPP
#define GRAPH_CLOSURE_CPP

#include <graph/search.h>
#include <structures/graph.h>

namespace graph_alg {

template <typename T, bool Weighted, typename EdgeType, typename... Args>
graph::graph<T, false, Weighted, EdgeType, Args...> k_core(
    graph::graph<T, false, Weighted, EdgeType, Args...> src, uint32_t k)
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
                if (vertex != dest) {
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

template <typename T, bool Weighted, typename EdgeType, typename... Args>
graph::graph<T, false, Weighted, EdgeType, Args...> Chvatal_Bondy_closure(
    graph::graph<T, false, Weighted, EdgeType, Args...> src, uint32_t k)
{
    uint32_t num_edges = 0;
    uint32_t *check_array, *check_stack;
    uint8_t* contains_edge;
    std::allocator<uint32_t> check_allocator;
    std::allocator<uint8_t> edge_allocator;
    bool first_success = false, second_success = false;
    uint32_t allocate_size = src.order() * src.order();

    // Allocate memory, clean up on exception
    try {
        check_array = check_allocator.allocate(allocate_size);
        first_success = true;
        check_stack = check_allocator.allocate(allocate_size);
        second_success = true;
        contains_edge = edge_allocator.allocate(allocate_size);
    } catch (...) {
        if (first_success)
            check_allocator.deallocate(check_array, allocate_size);
        if (second_success)
            check_allocator.deallocate(check_stack, allocate_size);
        throw;
    }

    try {
        // Reduce to integers 0 - (n-1) for compatibility with array
        std::vector<T> vertices = src.vertices();
        std::unordered_map<T, uint32_t, Args...> reverse_lookup;
        for (uint32_t i = 0; i < vertices.size(); ++i)
            reverse_lookup[vertices[i]] = i;

        // Bucket sort the vertices and populate contains_edge
        std::vector<std::list<uint32_t>> bucketed_vertices(src.order());
        for (const T& v : vertices) {
            uint32_t index = reverse_lookup[v];
            bucketed_vertices[src.degree(v)].push_back(index);
            uint32_t offset = index * src.order();
            for (const T& w : src.neighbors(v)) {
                uint32_t location = offset + reverse_lookup[w];
                contains_edge[location] = 1;
                check_array[location] = num_edges;
                check_stack[num_edges] = location;
                ++num_edges;
            }
        }

        struct bucket_vert {
            uint32_t value;
            typename std::list<uint32_t>::iterator pos;
        };
        std::list<std::pair<bucket_vert, bucket_vert>> to_add;

        // Using space-initialization: initializes the entry if not already, then returns
        auto check_entry
            = [check_array, check_stack, contains_edge, num_edges](uint32_t pos) -> uint8_t {
            return (check_array[pos] < num_edges && check_stack[check_array[pos]] == pos)
                ? contains_edge[pos]
                : 0;
        };

        // go through the buckets and find vertices whose degree exceed threshold and are not yet
        // existing edges
        for (uint32_t i = 0; i < bucketed_vertices.size(); ++i)
            for (auto it = bucketed_vertices[i].begin(); it != bucketed_vertices[i].end(); ++it)
                for (uint32_t j = src.order() - 1; j != -1 && j >= i && i + j >= k; --j)
                    for (auto it2 = bucketed_vertices[j].begin();
                         it != it2 && it2 != bucketed_vertices[j].end(); ++it2)
                        if (!check_entry(*it * src.order() + *it2))
                            to_add.emplace_back(
                                bucket_vert({ *it, it }), bucket_vert({ *it2, it2 }));

        // actually add edges
        while (!to_add.empty()) {
            std::pair<bucket_vert, bucket_vert> next = to_add.front();
            to_add.pop_front();
            
            src.force_add(vertices[next.first.value], vertices[next.second.value]);
            uint32_t first_pos = next.first.value * src.order() + next.second.value;
            uint32_t second_pos = next.second.value * src.order() + next.first.value;

            if(check_entry(first_pos) || check_entry(second_pos))
                throw std::logic_error("Multigraph detected");
            
            contains_edge[first_pos] = contains_edge[second_pos] = 1;
            check_array[first_pos] = num_edges;
            check_stack[num_edges] = first_pos;
            ++num_edges;
            check_array[second_pos] = num_edges;
                check_stack[num_edges] = second_pos;
                ++num_edges;

            // update
            uint32_t new_degree = src.degree(vertices[next.first.value]);
            bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(),
                bucketed_vertices[new_degree - 1], next.first.pos);
            if (k >= new_degree)
                for (auto it = bucketed_vertices[k - new_degree].begin();
                     it != bucketed_vertices[k - new_degree].end(); ++it)
                    if (it != next.first.pos && !check_entry(*it * src.order() + next.first.value))
                        to_add.emplace_back(next.first, bucket_vert({ *it, it }));

            new_degree = src.degree(next.second.value);
            bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(),
                bucketed_vertices[new_degree - 1], next.second.pos);
            if (k >= new_degree)
                for (auto it = bucketed_vertices[k - new_degree].begin();
                     it != bucketed_vertices[k - new_degree].end(); ++it)
                    if (it != next.second.pos && !check_entry(*it * src.order() + next.second.value))
                        to_add.emplace_back(next.second, bucket_vert({ *it, it }));
        }


        check_allocator.deallocate(check_array, allocate_size);
        check_allocator.deallocate(check_stack, allocate_size);
        edge_allocator.deallocate(contains_edge, allocate_size);
        return src;

    } catch (...) {
        check_allocator.deallocate(check_array, allocate_size);
        check_allocator.deallocate(check_stack, allocate_size);
        edge_allocator.deallocate(contains_edge, allocate_size);
        throw;
    }
    /*

    struct bucket_vert {
        T value;
        typename std::list<T>::iterator pos;
    };
    std::list<std::pair<bucket_vert,bucket_vert>> to_add;

    // go through the buckets and find vertices whose degree exceed threshold and are not yet
    existing edges for(uint32_t i = 0; i < bucketed_vertices.size(); ++i) for(auto it =
    bucketed_vertices[i].begin(); it != bucketed_vertices[i].end(); ++it) for(uint32_t j =
    src.order() - 1; j != -1 && j >= i && i + j >= k; --j) for(auto it2 =
    bucketed_vertices[j].begin(); it != it2 && it2 != bucketed_vertices[j].end(); ++it2)
                    if(!src.has_edge(*it, *it2)) // TODO: Switch to optimized adjacency matrix
                        to_add.emplace_back(bucket_vert({*it, it}), bucket_vert({*it2, it2}));

    // actually add edges
    while(!to_add.empty()) {
        std::pair<bucket_vert, bucket_vert> next = to_add.front();
        to_add.pop_front();
        src.force_add(next.first.value, next.second.value);

        // update
        uint32_t new_degree = src.degree(next.first.value);
        bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(),
    bucketed_vertices[new_degree - 1], next.first.pos); if(k >= new_degree) for(auto it =
    bucketed_vertices[k - new_degree].begin(); it != bucketed_vertices[k - new_degree].end(); ++it)
                if(it != next.first.pos && !src.has_edge(*it, next.first.value))
                    to_add.emplace_back(next.first, bucket_vert({*it, it}));

        new_degree = src.degree(next.second.value);
        bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(),
    bucketed_vertices[new_degree - 1], next.second.pos); if(k >= new_degree) for(auto it =
    bucketed_vertices[k - new_degree].begin(); it != bucketed_vertices[k - new_degree].end(); ++it)
                if(it != next.second.pos && !src.has_edge(*it, next.second.value))
                    to_add.emplace_back(next.second, bucket_vert({*it, it}));
    }

    return src;
    */
}
}

#endif // GRAPH_CLOSURE_CPP