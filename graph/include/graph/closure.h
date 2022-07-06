// whs241, 2020-02-12

#ifndef GRAPH_CLOSURE_H
#define GRAPH_CLOSURE_H

#include <cstdint>

#include <graph/components.h>
#include <structures/dynamic_matrix.h>
#include <structures/graph.h>
#include <unordered_set>

#include "search.h"

namespace graph_alg {
/**
 * An assortment of closure algorithms on graphs
 */

/**
 * find k-core: maximum induced subgraph with all degrees >= k
 * David W. Matula and Leland D. Beck:
 * Smallest-Last Ordering and Clustering and Graph Coloring Algorithms
 * (1983) doi:10.1145/2402.322385
 * Θ(V)
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>
  k_core(graph::graph<Vertex, false, Weighted, EdgeWeight, Args...> src, uint32_t k) {
    // Since we are strictly removing edges, if a vertex falls below
    // threshold, we know it has to go
    std::unordered_map<Vertex, uint32_t, Args...> degree;
    std::list<Vertex> to_remove;
    for (const Vertex& vertex : src.vertices()) {
        degree[vertex] = src.degree(vertex);
        if (degree[vertex] < k)
            to_remove.push_back(vertex);
    }

    while (!to_remove.empty()) {
        Vertex target = to_remove.back();
        to_remove.pop_front();
        for (const Vertex& neighbor : src.neighbors(target)) {
            --degree[neighbor];
            if (degree[neighbor] < k)
                to_remove.push_back(neighbor);
        }
        src.remove(target);
    }

    return src;
}

/**
 * Find the transitive closure of a graph
 * Θ(mn), good for sparse graphs
 */
template<typename Vertex, bool Directed, bool Weighted, typename... Args>
graph::graph<Vertex, Directed, false, Args...>
  transitive_closure_sparse(const graph::graph<Vertex, Directed, Weighted, Args...>& src) {
    // Strategy: From each vertex, find all reachable vertices
    graph::graph<Vertex, Directed, Weighted, Args...> result;
    std::vector<Vertex> vertices = src.vertices();
    for (const Vertex& vertex : vertices)
        result.add_vertex(vertex);
    for (const Vertex& vertex : vertices)
        graph_alg::depth_first(src, vertex, [&result, &vertex](const Vertex& dest) {
            if (vertex != dest) {
                if constexpr (Directed) {
                    result.force_add(vertex, dest);
                } else {
                    result.set_edge(vertex, dest);
                }
            }
        });

    return result;
}

// helper functions for transitive_closure()
namespace {
    /**
     * Recursive step for finding the transitive closure of a DAG
     */
    dynamic_matrix<int> transitive_closure_DAG_recurse(const dynamic_matrix<int>& src) {
        if (src.num_rows() <= 1)
            return src;

        // Since DAG has already been topologically sorted, src is always upper triangular
        std::size_t div_size = src.num_rows() / 2;
        dynamic_matrix<int> front(div_size, div_size),
          back(src.num_rows() - div_size, src.num_cols() - div_size);
        for (std::size_t i = 0; i < div_size; ++i) {
            for (std::size_t j = i; j < div_size; ++j) {
                front[i][j] = src[i][j];
            }
        }
        for (std::size_t i = 0; i < src.num_rows() - div_size; ++i) {
            for (std::size_t j = i; j < src.num_cols() - div_size; ++j) {
                back[i][j] = src[i + div_size][j + div_size];
            }
        }
        front = transitive_closure_DAG_recurse(front);
        back = transitive_closure_DAG_recurse(back);

        dynamic_matrix<int> result(src.num_rows(), src.num_cols());
        for (std::size_t i = 0; i < div_size; ++i) {
            for (std::size_t j = i; j < div_size; ++j) {
                result[i][j] = front[i][j];
            }
        }
        for (std::size_t i = 0; i < src.num_rows() - div_size; ++i) {
            for (std::size_t j = i; j < src.num_cols() - div_size; ++j) {
                result[i + div_size][j + div_size] = back[i][j];
            }
        }
        for (std::size_t i = 0; i < div_size; ++i) {
            for (std::size_t j = div_size; j < src.num_cols(); ++j) {
                result[i][j] = src[i][j];
            }
        }

        // Number of closures decreases to
        // Current: front1 -> front2 -> back1 -> back2
        // Edges to add: front1 -> back1, front1->back2, front2->back2
        //
        // Since result[i][i] != 0 for all i
        // Can do everything in one pass with 2 matrix multiplications
        result = result * result * result;

        // Slight cleanup: Reset non-zero entries to 1
        for (std::size_t i = 0; i < result.num_rows(); ++i) {
            for (std::size_t j = i; j < result.num_cols(); ++j) {
                if (result[i][j] != 0) {
                    result[i][j] = 1;
                }
            }
        }
        return result;
    }
} // namespace

/**
 * Find the transitive closure of a graph
 * Θ(matrix multiplication), good for dense graphs
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
graph::unweighted_graph<Vertex, true, Args...>
  transitive_closure(const graph::graph<Vertex, true, Weighted, EdgeWeight, Args...>& src) {
    std::list<std::unordered_set<Vertex, Args...>> comps =
      graph_alg::strongly_connected_components(src);
    std::vector<std::unordered_set<Vertex, Args...>> comps_vec(comps.begin(), comps.end());
    std::unordered_map<Vertex, std::size_t, Args...> vert_to_comp;

    std::size_t i = 0;
    for (auto& s : comps) {
        for (const Vertex& v : s) {
            vert_to_comp[v] = i;
        }
        ++i;
    }

    // Generate equivalent DAG
    graph::unweighted_graph<std::size_t, true, Args...> dag;
    for (std::size_t i = 0; i < comps.size(); ++i) {
        dag.add_vertex(i);
    }

    for (const Vertex& v : src.vertices()) {
        for (const Vertex& u : src.neighbors(v)) {
            if (vert_to_comp[v] != vert_to_comp[u]) {
                dag.force_add(vert_to_comp[v], vert_to_comp[u]);
            }
        }
    }

    std::vector<std::size_t> top_sort = topological_sort(dag);
    std::vector<std::size_t> reverse_lookup(top_sort.size());
    for (std::size_t i = 0; i < top_sort.size(); ++i) {
        reverse_lookup[top_sort[i]] = i;
    }

    dynamic_matrix<int> aug_mat(dag.order(), dag.order());
    for (std::size_t i = 0; i < dag.order(); ++i) {
        aug_mat[i][i] = 1; // Needed for matrix multiplication to work
        for (std::size_t v : dag.neighbors(top_sort[i])) {
            aug_mat[i][reverse_lookup[v]] = 1;
        }
    }

    dynamic_matrix<int> closed = transitive_closure_DAG_recurse(aug_mat);

    // Convert back to original graph
    graph::unweighted_graph<Vertex, true, Args...> result;
    for (const Vertex& v : src.vertices())
        result.add_vertex(v);
    for (std::size_t i = 0; i < closed.num_rows(); ++i) {
        for (std::size_t j = i + 1; j < closed.num_cols(); ++j) {
            if (closed[i][j] != 0) {
                // Every vertex corresponding to j can be reached from each corresponding to i
                for (const Vertex& u : comps_vec[top_sort[i]]) {
                    for (const Vertex& v : comps_vec[top_sort[j]]) {
                        result.force_add(u, v);
                    }
                }
            }
        }
    }

    return result;
}

/**
 * Chvatal-Bondy closure
 * If the sum of the degrees of two vertices exceed k, there should be an edge between them.
 *
 * Jeremy Spinrad
 * A note on computing graph closures
 * (2004) doi.org/10.1016/S0012-365X(03)00316-9
 * Θ(output size)
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>
  Chvatal_Bondy_closure(graph::graph<Vertex, false, Weighted, EdgeWeight, Args...> src,
                        uint32_t k) {
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
        std::vector<Vertex> vertices = src.vertices();
        std::unordered_map<Vertex, uint32_t, Args...> reverse_lookup;
        for (uint32_t i = 0; i < vertices.size(); ++i)
            reverse_lookup[vertices[i]] = i;

        // Bucket sort the vertices and populate contains_edge
        std::vector<std::list<uint32_t>> bucketed_vertices(src.order());
        for (const Vertex& v : vertices) {
            uint32_t index = reverse_lookup[v];
            bucketed_vertices[src.degree(v)].push_back(index);
            uint32_t offset = index * src.order();
            for (const Vertex& w : src.neighbors(v)) {
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

        // Using space-initialization: returns 0 if uninitialized, contents if initialized
        auto check_entry = [check_array, check_stack, contains_edge,
                            num_edges](uint32_t pos) -> uint8_t {
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
                            to_add.emplace_back(bucket_vert({*it, it}), bucket_vert({*it2, it2}));

        // actually add edges
        while (!to_add.empty()) {
            std::pair<bucket_vert, bucket_vert> next = to_add.front();
            to_add.pop_front();

            src.force_add(vertices[next.first.value], vertices[next.second.value]);
            uint32_t first_pos = next.first.value * src.order() + next.second.value;
            uint32_t second_pos = next.second.value * src.order() + next.first.value;

            if (check_entry(first_pos) || check_entry(second_pos))
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
                        to_add.emplace_back(next.first, bucket_vert({*it, it}));

            new_degree = src.degree(next.second.value);
            bucketed_vertices[new_degree].splice(bucketed_vertices[new_degree].end(),
                                                 bucketed_vertices[new_degree - 1],
                                                 next.second.pos);
            if (k >= new_degree)
                for (auto it = bucketed_vertices[k - new_degree].begin();
                     it != bucketed_vertices[k - new_degree].end(); ++it)
                    if (it != next.second.pos &&
                        !check_entry(*it * src.order() + next.second.value))
                        to_add.emplace_back(next.second, bucket_vert({*it, it}));
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
}

/*
 * Check if a graph is transitively oriented
 * O(M(n)), M(n) is matrix multiplication
 */
template<typename Vertex, bool Weighted, typename... Args>
bool is_transitive_closure(const graph::graph<Vertex, true, Weighted, Args...>& src) {
    dynamic_matrix<int> adj_matrix(src.order(), src.order(), 0);
    for (const Vertex& v : src.vertices()) {
        adj_matrix[src.get_translation().at(v)][src.get_translation().at(v)] = 1;
        for (const Vertex& w : src.neighbors(v))
            adj_matrix[src.get_translation().at(v)][src.get_translation().at(w)] = 1;
    }
    dynamic_matrix<int> path_matrix = adj_matrix * adj_matrix;
    for (std::size_t i = 0; i < src.order(); ++i)
        for (std::size_t j = 0; j < src.order(); ++j)
            if ((adj_matrix[i][j] == 0) != (path_matrix[i][j] == 0))
                return false;
    return true;
}

template<typename Vertex, bool Weighted, typename... Args>
bool is_transitive_reduction(const graph::graph<Vertex, true, Weighted, Args...>& src) {
    dynamic_matrix<int> adj_matrix(src.order(), src.order(), 0);
    for (const Vertex& v : src.vertices()) {
        adj_matrix[src.get_translation().at(v)][src.get_translation().at(v)] = 1;
        for (const Vertex& w : src.neighbors(v))
            adj_matrix[src.get_translation().at(v)][src.get_translation().at(w)] = 1;
    }
    dynamic_matrix<int> path_matrix = adj_matrix * adj_matrix;
    for (std::size_t i = 0; i < src.order(); ++i)
        for (std::size_t j = 0; j < src.order(); ++j)
            if (i != j && path_matrix[i][j] > 2)
                return false;
    return true;
}

} // namespace graph_alg

#endif // GRAPH_CLOSURE_H
