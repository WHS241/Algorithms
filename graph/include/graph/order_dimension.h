#ifndef GRAPH_ORDER_DIMENSION_H
#define GRAPH_ORDER_DIMENSION_H

#include <vector>

#include <structures/graph.h>
#include <structures/van_Emde_Boas_map.h>

#include "search.h"

namespace graph_alg {
/**
 * Find generators for a partial order of dimension 2, if they exist
 * Partial order is inputted as the transitive closure of a directed acyclic graph
 *
 * Jeremy Spinrad
 * Two Dimensional Partial Orders, 1982
 * O(n^2)
 */
template<typename Vertex, bool Weighted, typename... Args>
std::pair<std::list<Vertex>, std::list<Vertex>>
  two_dimensional_order_generator_closure(const graph::graph<Vertex, true, Weighted, Args...>&);

/**
 * Verify order dimension 2 and find generators if they exist
 * Partial order need not be given as a transitive closure
 * Calculates transitive closure in the process
 *
 * Tze-Heng Ma, Jeremy Spinrad
 * Transitive closure for restricted classes of partial orders
 * O(n^2)
 */
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::pair<std::list<Vertex>, std::list<Vertex>> two_dimensional_order_generator(
  const graph::graph<Vertex, true, Weighted, EdgeWeight, Args...>& input) {
    std::vector<Vertex> top_order = topological_order(input);
    std::pair<std::list<Vertex>, std::list<Vertex>> result;
    if (input.order() == 0)
        return result;
    if (input.order() == 1) {
        result.first.push_back(input.vertices().front());
        result.second.push_back(input.vertices().front());
        return result;
    }
    if (input.order() == 2) {
        std::vector<Vertex> vertices = input.vertices();
        if (input.has_edge(vertices.front(), vertices.back())) {
            result.first.push_back(vertices.front());
            result.first.push_back(vertices.back());
            result.second = result.first;
            return result;
        }
        if (input.has_edge(vertices.back(), vertices.front())) {
            result.first.push_back(vertices.back());
            result.first.push_back(vertices.front());
            result.second = result.first;
            return result;
        }
        result.first.push_back(vertices.front());
        result.second.push_front(vertices.front());
        result.first.push_back(vertices.back());
        result.second.push_front(vertices.back());
        return result;
    }

    // divide and conquer
    std::pair<std::list<Vertex>, std::list<Vertex>> front_order = two_dimensional_order_generator(
      input.generate_induced_subgraph(top_order.begin(), top_order.begin() + input.order() / 2));
    std::pair<std::list<Vertex>, std::list<Vertex>> back_order = two_dimensional_order_generator(
      input.generate_induced_subgraph(top_order.begin() + input.order() / 2, top_order.end()));

    graph::graph<Vertex, true, false, Args...> closure(graph::adj_matrix);
    for (const Vertex& v : input.vertices())
        closure.add_vertex(v);

    // Generate transitive closure of each part
    for (const Vertex& v : front_order.first) {
        auto second_it = std::find(front_order.second.begin(), front_order.second.end(), v);
        for (auto it = second_it + 1; it != front_order.second.end(); ++it)
            closure.force_add(v, *it);
    }
    for (const Vertex& v : back_order.first) {
        auto second_it = std::find(back_order.second.begin(), back_order.second.end(), v);
        for (auto it = second_it + 1; it != back_order.second.end(); ++it)
            closure.force_add(v, *it);
    }

    // Generating transitive closure of cross edges
    std::unordered_map<Vertex, std::size_t, Args...> second_order_map;
    for (std::size_t i = 0; i < front_order.second.size(); ++i)
        second_order_map[front_order.second[i]] = i;
    for (std::size_t i = 0; i < back_order.second.size(); ++i)
        second_order_map[front_order.second[i]] = i;

    graph::graph<Vertex, true, Weighted, EdgeWeight, Args...> matrix_rep =
      input.convert(graph::graph_type::adj_matrix);

    for (const Vertex& v : front_order.first) {
        // Forward sweep: find all (v, w) from (v,u) and (u, w) where u and w are in back_order
        auto it =
          std::find_if(back_order.first.begin(), back_order.first.end(),
                       [&matrix_rep, &v](const Vertex& w) { return matrix_rep.has_edge(v, w); });
        if (it != back_order.first.end()) {
            std::size_t limit = second_order_map[*it];
            for (++it; it != back_order.first.end(); ++it) {
                if (matrix_rep.has_edge(v, *it) && second_order_map[*it] < limit)
                    limit = second_order_map[*it];
                else if (second_order_map[*it] > limit)
                    matrix_rep.set_edge(v, *it);
            }
        }
    }

    for (const Vertex& v : back_order.first) {
        // Backward sweep: find all (u,v) from (u,w) and (w,v) where u and w are in front_order
        auto it =
          std::find_if(front_order.first.rbegin(), front_order.first.rend(),
                       [&matrix_rep, &v](const Vertex& u) { return matrix_rep.has_edge(u, v); });
        if (it != front_order.first.rend()) {
            std::size_t limit = second_order_map[*it];
            for (++it; it != front_order.first.rend(); ++it) {
                if (matrix_rep.has_edge(*it, v) && second_order_map[*it] > limit)
                    limit = second_order_map[*it];
                else if (second_order_map[*it] < limit)
                    matrix_rep.set_edge(*it, v);
            }
        }
    }

    return two_dimensional_order_generator_closure(matrix_rep);
}

template<typename It1, typename It2, typename It3, typename... Args>
graph::unweighted_graph<typename std::iterator_traits<It1>::value_type, true, Args...>
  three_dimensional_transitive_reduction(It1 first1, It1 last1, It2 first2, It2 last2, It3 first3,
                                         It3 last3) {
    if (std::distance(first1, last1) != std::distance(first2, last2) ||
        std::distance(first2, last2) != std::distance(first3, last3))
        throw std::invalid_argument("Incompatible/incomplete generator lengths");

    typedef typename std::iterator_traits<It1>::value_type Vertex;
    std::unordered_map<Vertex, std::size_t, Args...> base_order;
    std::vector<Vertex> elements(first3, last3);
    std::vector<std::size_t> isomorphism1(elements.size()), isomorphism2(elements.size());

    // Generate permutation lists by mapping lists 1 and 2 to values 1 - n in order
    std::size_t i = 0;
    for (; first1 != last1; ++first1) {
        base_order[*first1] = i;
        ++i;
    }

    Vertex temp;
    try {
        std::transform(first3, last3, isomorphism1.begin(), [&base_order, &temp](const Vertex& v) {
            temp = v;
            return base_order.at(v);
        });
    } catch (...) {
        std::cout << "Iso1" << std::endl;
        std::cout << temp << std::endl;
        throw;
    }

    i = 0;
    for (; first2 != last2; ++first2) {
        base_order[*first2] = i;
        ++i;
    }
    try {
        std::transform(first3, last3, isomorphism2.begin(), [&base_order, &temp](const Vertex& v) {
            temp = v;
            return base_order.at(v);
        });
    } catch (...) {
        std::cout << "Iso2" << std::endl;
        std::cout << temp << std::endl;
        throw;
    }

    graph::unweighted_graph<Vertex, true, Args...> result;
    for (const Vertex& v : elements)
        result.add_vertex(v);

    for (std::size_t i = 0; i < elements.size(); ++i) {
        van_Emde_Boas_map<std::size_t> map_tree(elements.size());
        for (std::size_t j = i + 1; j < elements.size(); ++j) {
            // Verify that edge would occur in the transitive closure
            if (isomorphism1[i] < isomorphism1[j] && isomorphism2[i] < isomorphism2[j]) {
                auto it = map_tree.insert({isomorphism1[j], isomorphism2[j]}).first;

                // If there are any values where both isomorphisms give smaller numbers, there is a
                // longer path around to current
                auto pred = it;
                if (it != map_tree.begin())
                    --pred;
                if (it != map_tree.begin() && isomorphism2[j] > pred->second) {
                    map_tree.erase(it);
                } else {
                    try {
                        result.force_add(elements[i], elements[j]);
                    } catch (...) {
                        std::cout << elements[i] << " " << elements[j] << std::endl;
                        throw;
                    }

                    // Need to maintain reverse order of stored iso2 vals so above check works
                    ++it;
                    while (it != map_tree.end() && it->second > isomorphism2[j])
                        it = map_tree.erase(it);
                }
            }
        }
    }
    return result;
}
} // namespace graph_alg

#endif // GRAPH_ORDER_DIMENSION_H
