#ifndef NPC_REDUCTION_KARP_H
#define NPC_REDUCTION_KARP_H

#include <algorithm>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <structures/graph.h>

#include <util/pair_hash.h>
#include <util/key_eq_wrapper.h>

namespace NP_complete {

/*
 * CNF-SAT -> 0-1 Integer Programming
 *
 * Richard M. Karp
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T>
std::pair<std::vector<std::vector<int>>, std::vector<int>> SAT_to_Integer_Programming(
    const std::list<std::list<std::pair<T, bool>>>& expr)
{
    std::pair<std::vector<std::vector<int>>, std::vector<int>> ret_val;
    ret_val.first.resize(expr.size());
    ret_val.second.resize(expr.size());

    if constexpr (std::is_default_constructible_v<std::hash<T>>) {
        std::unordered_map<T, std::size_t> variables;
        for (const std::list<std::pair<T, bool>>& clause : expr)
            for (const std::pair<T, bool>& literal : clause)
                variables.insert(literal.first, variables.size());

        auto it_1 = expr.begin();
        auto it_2 = ret_val.first.begin();
        auto it_3 = ret_val.second.begin();
        while (it_1 != expr.end()) {
            it_2->resize(variables.size(), 0);
            *it_3 = 1;

            for (const std::pair<T, bool>& literal : *it_1) {
                (*it_2)[variables.at(literal.first)] = literal.second ? 1 : -1;
                if (!literal.second)
                    --*it_3;
            }

            ++it_1;
            ++it_2;
            ++it_3;
        }
    } else {
        std::vector<T> variables;
        for (const std::list<std::pair<T, bool>>& clause : expr)
            for (const std::pair<T, bool>& literal : clause)
                if (std::find(variables.begin(), variables.end(), literal.first) == variables.end())
                    variables.push_back(literal.first);

        auto it_1 = expr.begin();
        auto it_2 = ret_val.first.begin();
        auto it_3 = ret_val.second.begin();
        while (it_1 != expr.end()) {
            it_2->resize(variables.size(), 0);
            *it_3 = 1;

            for (const std::pair<T, bool>& literal : *it_1) {
                (*it_2)[std::find(variables.begin(), variables.end(), literal.first)
                    - variables.begin()]
                    = literal.second ? 1 : -1;
                if (!literal.second)
                    --*it_3;
            }

            ++it_1;
            ++it_2;
            ++it_3;
        }
    }

    return ret_val;
}

/*
 * CNF-SAT -> Clique 
 *
 * Richard M. Karp (credited to Stephen A. Cook and Raymond Reiter)
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
std::pair<graph::unweighted_graph<std::pair<T, std::size_t>, false, util::pair_hash<T, std::size_t, Hash>, util::key_eq_wrapper<T, std::size_t, KeyEqual>>, std::size_t> SAT_to_Clique(
    std::list<std::list<std::pair<T, bool>>> expr, Hash = Hash(), KeyEqual = KeyEqual()) {
    std::pair<graph::unweighted_graph<std::pair<T, std::size_t>, false, util::pair_hash<T, std::size_t, Hash>, util::key_eq_wrapper<T, std::size_t, KeyEqual>>, std::size_t> ret_val;

    // sanitize: remove duplicate literals and always-true clauses
    expr.remove_if([](std::list<std::pair<T, bool>>& clause) {
            std::unordered_map<T, bool, Hash, KeyEqual> values;
            for (auto it = clause.begin(); it != clause.end();) {
                if (values.find(it->first) == values.end()) {
                    values.insert(*it);
                    ++it;
                } else if (values.at(it->first) == it->second) {
                    it = clause.erase(it);
                } else {
                    return true;
                }
            }
            return false;
            });
    
    ret_val.second = expr.size();

    std::size_t i = 0;
    for (const std::list<std::pair<T, bool>>& clause : expr) {
        for (const std::pair<T, bool>& literal : clause)
            ret_val.first.add_vertex({literal.first, i});
        ++i;
    }

    i = 0;
    for (auto it = expr.begin(); it != expr.end(); ++it, ++i) {
        for (const std::pair<T, bool>& literal1 : *it) {
            auto it2 = it;
            std::size_t j = i + 1;
            for (++it2; it2 != expr.end(); ++it2, ++j)
                for (const std::pair<T, bool>& literal2 : *it2)
                    if (literal1.first != literal2.first || literal1.second == literal2.second)
                        ret_val.first.force_add({literal1.first, i}, {literal2.first, j});
        }
    }

    return ret_val;
}

/*
 * Clique -> Independent Set
 */
template <typename T, bool Weighted,
    typename... Args>
std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t> Clique_to_Independent_Set(const std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t>& instance) {
    std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t> result(instance);

    std::vector<T> vertices = result.first.vertices();
    for (const T& v : vertices)
        result.first.isolate(v);

    for (std::size_t i = 0; i < vertices.size(); ++i)
        for (std::size_t j = i + 1; j < vertices.size(); ++j)
            if (!instance.first.has_edge(vertices[i], vertices[j]))
                result.first.force_add(vertices[i], vertices[j]);

    return result;
}

/*
 * Independent Set -> Vertex Cover
 * Adapted from Karp
 */
template <typename T, bool Weighted, typename... Args>
std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t> Independent_Set_to_Vertex_Cover(const std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t>& instance) {
    return std::make_pair(instance.first, instance.first.order() - instance.second);
}

/*
 * Vertex Cover -> Set Cover
 *
 * Richard M. Karp
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, bool Weighted, typename Edge, typename Hash, typename KeyEqual>
    std::pair<std::vector<std::unordered_set<std::pair<T, T>, util::pair_hash_unordered<T, Hash>, util::key_eq_unordered<T, KeyEqual>>>, std::size_t> Vertex_Cover_to_Set_Cover(const std::pair<graph::graph<T, false, Weighted, Edge, Hash, KeyEqual>, std::size_t>& input) {
        std::pair<std::vector<std::unordered_set<std::pair<T, T>, util::pair_hash_unordered<T, Hash>, util::key_eq_unordered<T, KeyEqual>>>, std::size_t> result;

        result.second = input.second;

        for (const T& u : input.first.vertices()) {
            std::unordered_set<std::pair<T, T>, util::pair_hash_unordered<T, Hash>, util::key_eq_unordered<T, KeyEqual>> current_set;
            for (const T& v : input.first.neighbors(u))
                current_set.insert({u, v});
            result.first.push_back(std::move(current_set));
        }

        return result;
    }

/*
 * Vertex Cover -> Feedback Vertex Set 
 *
 * Richard M. Karp
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, bool Weighted, typename... Args>
std::pair<graph::graph<T, true, Weighted, Args...>, std::size_t> Vertex_Cover_to_Feedback_Vertex(const std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t>& input) {
    std::pair<graph::graph<T, true, Weighted, Args...>, std::size_t> result;
    result.second = input.second;

    for(const T& v : input.first.vertices())
        result.first.add_vertex(v);

    for (const T& u : input.first.vertices())
        for (const T& v : input.first.neighbors(u))
            result.first.force_add(u, v);
    
    return result;
}

/*
 * Vertex Cover -> Feedback Edge Set 
 *
 * Richard M. Karp (co-credited to Eugene L. Lawler)
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, bool Weighted, typename Edge, typename Hash, typename KeyEqual>
std::pair<graph::graph<std::pair<T, bool>, true, Weighted, Edge, util::pair_hash<T, bool, Hash>, util::key_eq_wrapper<T, bool, KeyEqual>>, std::size_t> Vertex_Cover_to_Feedback_Edge(const std::pair<graph::graph<T, false, Weighted, Edge, Hash, KeyEqual>, std::size_t>& input) {
    std::pair<graph::graph<std::pair<T, bool>, true, Weighted, Edge, util::pair_hash<T, bool, Hash>, util::key_eq_wrapper<T, bool, KeyEqual>>, std::size_t> result;

    result.second = input.second;
    
    // false -> in, true -> out

    for(const T& v : input.first.vertices()) {
        result.first.add_vertex(std::make_pair(v, false));
        result.first.add_vertex(std::make_pair(v, true));
        result.first.force_add(std::make_pair(v, false), std::make_pair(v, true));
    }

    for (const T& u : input.first.vertices())
        for (const T& v : input.first.neighbors(u))
            result.first.force_add(std::make_pair(u, true), std::make_pair(v, false));
    return result;
}

template <typename T, typename Hash = std::hash<T>>
struct VC_DHC_hash {
    std::hash<std::size_t> hash_1;
    Hash hash_2; 
    std::size_t operator()(const std::tuple<std::size_t, std::size_t, T, T>& vertex) const {
        return util::asym_combine_hash(util::asym_combine_hash(hash_1(std::get<0>(vertex)), hash_1(std::get<1>(vertex))), util::asym_combine_hash(hash_2(std::get<2>(vertex)), hash_2(std::get<3>(vertex))));
    }
};

template <typename T, typename KeyEqual = std::equal_to<T>>
struct VC_DHC_eq {
    KeyEqual key_eq;
    bool operator()(const std::tuple<std::size_t, std::size_t, T, T>& lhs, const std::tuple<std::size_t, std::size_t, T, T>& rhs) const {
        return (std::get<0>(lhs) == std::get<0>(rhs)) &&
               (std::get<1>(lhs) == std::get<1>(rhs)) &&
               key_eq(std::get<2>(lhs), std::get<2>(rhs)) &&
               key_eq(std::get<3>(lhs), std::get<3>(rhs));
    }
};

/*
 * Vertex Cover -> Directed Hamiltonian Cycle
 *
 * Richard M. Karp
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, bool Weighted, typename EdgeType, typename Hash, typename KeyEqual>
graph::graph<std::tuple<std::size_t, std::size_t, T, T>, true, Weighted, EdgeType, VC_DHC_hash<T, Hash>, VC_DHC_eq<T, KeyEqual>> Vertex_Cover_to_DHC(const std::pair<graph::graph<T, false, Weighted, EdgeType, Hash, KeyEqual>, std::size_t>& input) {
    typedef std::tuple<std::size_t, std::size_t, T, T> DHC_vertex;
    graph::graph<DHC_vertex, true, Weighted, EdgeType, VC_DHC_hash<T, Hash>, VC_DHC_eq<T, KeyEqual>> result;
    
    for(std::size_t i = 0; i < input.second; ++i)
        result.add_vertex(DHC_vertex(i, 0, T(), T()));

    if (input.second > 1U)
        for (std::size_t i = 0; i < input.second; ++i)
            result.force_add(DHC_vertex(i, 0, T(), T()), DHC_vertex((i + 1) % input.second, 0, T(), T()));

    std::vector<T> vertices = input.first.vertices();
    for (const T& u : vertices) {
        std::list<T> neighbors = input.first.neighbors(u);
        for (const T& v : neighbors) {
            result.add_vertex(DHC_vertex(-1, 1, u, v));
            result.add_vertex(DHC_vertex(-1, 2, u, v));
            result.force_add(DHC_vertex(-1, 1, u, v), DHC_vertex(-1, 2, u, v));
        }

        for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
            auto it2 = it;
            ++it2;
            if (it == neighbors.begin()) {
                for (std::size_t i = 0; i < input.second; ++i)
                    result.force_add(DHC_vertex(i, 0, T(), T()), DHC_vertex(-1, 1, u, *it));
            }

            if (it2 == neighbors.end()) {
                for (std::size_t i = 0; i < input.second; ++i)
                    result.force_add(DHC_vertex(-1, 2, u, *it), DHC_vertex(i, 0, T(), T()));
            } else {
                result.force_add(DHC_vertex(-1, 2, u, *it), DHC_vertex(-1, 1, u, *it2));
            }
        }
    }

    for (const T& u : vertices) {
        for (const T& v : input.first.neighbors(u)) {
            result.force_add(DHC_vertex(-1, 1, u, v), DHC_vertex(-1, 1, v, u));
            result.force_add(DHC_vertex(-1, 2, u, v), DHC_vertex(-1, 2, v, u));
        }
    }

    return result;
}

/*
 * Directed Hamiltonian Cycle -> Undirected Hamiltonian Cycle
 *
 * Richard M. Karp (credited to Robert E. Tarjan)
 * Reducibility Among Combinatorial Problems
 * doi:10.1007/978-1-4684-2001-2_9
 * (1972)
 */
template <typename T, bool Weighted, typename EdgeWeight, typename Hash, typename KeyEqual>
graph::graph<std::pair<T, short>, false, Weighted, EdgeWeight, util::pair_hash<T, short, Hash>, util::key_eq_wrapper<T, short, KeyEqual>> DHC_to_UHC(const graph::graph<T, true, Weighted, EdgeWeight, Hash, KeyEqual>& input) {
    graph::graph<std::pair<T, short>, false, Weighted, EdgeWeight, util::pair_hash<T, short, Hash>, util::key_eq_wrapper<T, short, KeyEqual>> result;

    for (const T& v : input.vertices()) {
        for (short i = 0; i < 3; ++i)
            result.add_vertex(std::make_pair(v, i));

        for (short i = 0; i < 2; ++i)
            result.force_add(std::make_pair(v, i), std::make_pair(v, i + 1));
    }

    for (const T& u : input.vertices())
        for (const T& v : input.neighbors(u))
            result.force_add(std::pair<T, short>(u, 2), std::pair<T, short>(v, 0));

    return result;
}

}

#endif // NPC_REDUCTION_KARP_H
