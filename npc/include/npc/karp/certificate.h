#ifndef NPC_CERTIFICATE_KARP_H
#define NPC_CERTIFICATE_KARP_H

#include <algorithm>
#include <list>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <graph/search.h>

#include <structures/graph.h>

/*
 * Certificates for NP-complete problems
 * Each function accepts an instance in the first parameter and certificate in the second
 * Returns true if the certificate was successfully validated
 */
namespace NP_complete {
/*
 * CNF-Satisfiability
 *
 * Given a Boolean statement in conjunctive normal form, is there an assignment of the variables
 * such that the statement is true?
 *
 * Stephen A. Cook
 * The complexity of theorem proving procedures
 * doi:10.1145/800157.805047
 *
 * Leonid Levin
 * Universal search problems
 *
 * Instance format:
 * A list of clauses
 * Each clause is a list of literals
 * Each literal is a label-value pair
 * Ex. (x1 + x2')(x3' + x4) is inputted as {{{x1, true}, {x2, false}},{{x3, false}, {x4, true}}}
 *
 * Certificate format: unordered_map from variable to assignment
 */
template <typename T, typename... Args>
bool cert_CNF_SAT(const std::list<std::list<std::pair<T, bool>>>& expr,
    const std::unordered_map<T, bool, Args...>& cert)
{
    return std::all_of(
        expr.begin(), expr.end(), [&cert](const std::list<std::pair<T, bool>>& clause) {
            return std::any_of(
                clause.begin(), clause.end(), [&cert](const std::pair<T, bool>& literal) {
                    return cert.at(literal.first) == literal.second;
                });
        });
}

/*
 * 0-1 Integer Programming
 *
 * Given an m * n matrix A and a m-dimensional vector b, is there a n-dim. vector x with values only
 * from the set {0, 1} such that sum{1 <= j <= n}(A_ij * x_j) >= b_i for all 1 <= i <= m?
 *
 * Instance format: A matrix-vector pair, the matrix is a vector of vectors
 *
 * Certificate format: An n-dimensional vector
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool cert_0_1_programming(const std::pair<std::vector<std::vector<T>>, std::vector<T>>& instance,
    const std::vector<T>& cert)
{
    for (const T& x : cert)
        if (x != 0 || x != 1)
            return false;

    const std::vector<std::vector<T>>& m = instance.first;
    const std::vector<T>& n = instance.second;

    if (m.size() != n.size())
        return false;

    for (std::size_t i = 0; i < m.size(); ++i) {
        if (m[i].size() != cert.size())
            return false;
        T sum = std::inner_product(m[i].begin(), m[i].end(), cert.begin(), T());
        if (sum < n[i])
            return false;
    }
    return true;
}

/*
 * Clique
 *
 * Given an undirected graph G and integer k, are there at least k distinct vertices {v_1, ... v_k}
 * such that (v_i, v_j) is an edge in G for all 1 <= i, j <= k, i != j?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format: A pair of iterators (first->begin, second->end) to a collection of vertices
 */
template <typename T, typename It, bool Weighted,
    typename = std::enable_if_t<std::is_same_v<T, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_clique(const std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t>& instance,
    const std::pair<It, It>& cert)
{
    if (std::distance(cert.first, cert.second) < instance.second)
        return false;
    const graph::graph<T, false, Weighted, Args...>& main_graph = instance.first;
    graph::graph<T, false, Weighted, Args...> subgraph
        = main_graph.generate_induced_subgraph(cert.first, cert.second);
    subgraph.sanitize();
    std::vector<T> vertices = subgraph.vertices();

    return (subgraph.order() >= instance.second)
        && std::all_of(vertices.begin(), vertices.end(),
            [&subgraph](const T& v) { return subgraph.degree(v) == subgraph.order() - 1; });
}

/*
 * Independent Set
 *
 * Given an undirected graph G and integer k, are there at least k distinct vertices {v_1, ... v_k}
 * such that (v_i, v_j) is NOT an edge in G for all 1 <= i, j <= k, i != j?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format: A pair of iterators (first->begin, second->end) to a collection of vertices
 */
template <typename T, typename It, bool Weighted,
    typename = std::enable_if_t<std::is_same_v<T, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_independent_set(
    const std::pair<graph::graph<T, false, Weighted, Args...>, std::size_t>& instance,
    const std::pair<It, It>& cert)
{
    if (std::distance(cert.first, cert.second) < instance.second)
        return false;
    const graph::graph<T, false, Weighted, Args...>& main_graph = instance.first;
    graph::graph<T, false, Weighted, Args...> subgraph
        = main_graph.generate_induced_subgraph(cert.first, cert.second);
    std::vector<T> vertices = subgraph.vertices();

    return (subgraph.order() >= instance.second)
        && std::all_of(vertices.begin(), vertices.end(),
            [&subgraph](const T& v) { return subgraph.degree(v) == 0; });
}

/*
 * Vertex Cover
 *
 * Given an undirected graph G and integer k, is there a set of vertices S such that |S| <= k and
 * for all edges (i, j) in G, either i or j (or both) is in S?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format: A pair of iterators (first->begin, second->end) to a collection of vertices
 */
template <typename T, typename It, bool Weighted, typename EdgeType,
    typename = std::enable_if_t<std::is_same_v<T, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_vertex_cover(
    const std::pair<graph::graph<T, false, Weighted, EdgeType, Args...>, std::size_t>& instance,
    const std::pair<It, It>& cert)
{
    std::unordered_set<T, Args...> cover_set(cert.first, cert.second);
    const graph::graph<T, false, Weighted, EdgeType, Args...>& input_graph = instance.first;
    std::vector<T> vertices = input_graph.vertices();

    return (cover_set.size() <= instance.second)
        && std::all_of(vertices.begin(), vertices.end(), [&cover_set, &input_graph](const T& u) {
               std::list<T> neighbors = input_graph.neighbors(u);

               return cover_set.find(u) != cover_set.end()
                   || std::all_of(neighbors.begin(), neighbors.end(),
                       [&cover_set](const T& v) { return cover_set.find(v) != cover_set.end(); });
           });
}

/*
 * Set Cover
 *
 * Given a collection of sets {S_1,..., S_n} and integer k, is there a collection of sets T_j,
 * 1<=j<=k such that {T_j} is a subset of {S_i} and union(T_j) = union(S_i)?
 *
 * Instance format:
 * the {S_i} sets and integer
 *
 * Certificate format:
 * {T_j} sets
 */
template <typename T, typename Hash, typename KeyEqual>
bool cert_set_cover(
    const std::pair<std::vector<std::unordered_set<T, Hash, KeyEqual>>, std::size_t>& instance,
    std::vector<std::unordered_set<T, Hash, KeyEqual>> cert)
{
    std::vector<bool> found(instance.first.size(), false);
    std::remove_if(cert.begin(), cert.end(),
        [&sets = instance.first, &found](const std::unordered_set<T, Hash, KeyEqual>& t) {
            for (std::size_t i = 0; i < sets.size(); ++i) {
                if (!found[i] && sets[i] == t) {
                    found[i] = true;
                    return false;
                }
            }
            return true;
        });
    auto union_sets = [](std::vector<std::unordered_set<T, Hash, KeyEqual>> sets) {
        return std::reduce(sets.begin(), sets.end(), std::unordered_set<T, Hash, KeyEqual>(),
            [](std::unordered_set<T, Hash, KeyEqual>& x, std::unordered_set<T, Hash, KeyEqual>& y) {
                x.merge(y);
                return x;
            });
    };

    auto equal_check = [](const std::unordered_set<T, Hash, KeyEqual>& x, std::unordered_set<T, Hash, KeyEqual> y) -> bool {
        if (x.size() != y.size())
            return false;

        for (const T& z : x) {
            if (y.find(z) == y.end())
                return false;
            y.erase(z);
        }
        return true;
    };


    return cert.size() <= instance.second && equal_check(union_sets(cert), union_sets(instance.first));
}

/*
 * Feedback Vertex Set
 *
 * Given a directed graph G and integer k, is there a set of vertices S such that |S| <= k and for
 * any cycle in G, there is a vertex from the cycle that is in S?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format:
 * A begin-end iterator pair to a collection of vertices
 */
template <typename T, typename It, bool Weighted, typename EdgeType,
    typename = std::enable_if_t<std::is_same_v<T, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_feedback_vertex(
    const std::pair<graph::graph<T, true, Weighted, EdgeType, Args...>, std::size_t>& instance,
    const std::pair<It, It>& cert)
{
    if (std::distance(cert.first, cert.second) > instance.second)
        return false;

    graph::graph<T, true, Weighted, EdgeType, Args...> input_graph = instance.first;
    std::for_each(cert.first, cert.second, [&input_graph](const T& v) { input_graph.remove(v); });
    try {
        graph_alg::topological_sort(input_graph);
        return true;
    } catch (std::invalid_argument&) {
        return false;
    }
}

/*
 * Feedback Edge Set
 *
 * Given a directed graph G and integer k, is there a set of edges S such that |S| <= k and for any
 * cycle in G, there is an edge from the cycle that is in S?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format:
 * A begin-end iterator pair to a collection of edges
 * Each edge is a pair of vertices (x, y), where x->y is the edge represented
 */
template <typename T, typename It, bool Weighted, typename EdgeType,
    typename
    = std::enable_if_t<std::is_same_v<std::pair<T, T>, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_feedback_edge(
    const std::pair<graph::graph<T, true, Weighted, EdgeType, Args...>, std::size_t>& instance,
    const std::pair<It, It>& cert)
{
    if (std::distance(cert.first, cert.second) > instance.second)
        return false;

    graph::graph<T, true, Weighted, EdgeType, Args...> input_graph = instance.first;
    std::for_each(cert.first, cert.second,
        [&input_graph](const std::pair<T, T>& e) { input_graph.remove_edge(e.first, e.second); });
    try {
        graph_alg::topological_sort(input_graph);
        return true;
    } catch (std::invalid_argument&) {
        return false;
    }
}

/*
 * Hamiltonian cycle
 *
 * Given a graph G (either directed or undirected), is there a simple cycle (does not repeat
 * vertices) that goes through every vertex in the graph?
 *
 * Instance format:
 * A graph
 *
 * Certificate format:
 * A begin-end iterator pair to a collection of vertices
 * The vertices are ordered such that (v[i], v[i+1]) is an edge in the Hamiltonian cycle
 */
template <typename T, typename It, bool Directed, bool Weighted, typename EdgeType,
    typename = std::enable_if_t<std::is_same_v<T, typename std::iterator_traits<It>::value_type>>,
    typename... Args>
bool cert_Hamiltonian_cycle(const graph::graph<T, Directed, Weighted, EdgeType, Args...>& instance,
    const std::pair<It, It>& cert)
{
    if (std::distance(cert.first, cert.second) != instance.order())
        return false;

    // Check cycle is simple
    std::vector<bool> found(instance.order(), false);
    if (std::any_of(cert.first, cert.second, [&found, &instance](const T& v) {
            try {
                std::size_t index = instance.get_translation().at(v);
                bool duplicate = found[index];
                found[index] = true;
                return duplicate;
            } catch (std::out_of_range&) {
                return true;
            }
        }))
        return false;

    for (It temp = cert.first; temp != cert.second; ++temp) {
        It temp2 = temp;
        ++temp2;
        if (temp2 == cert.second)
            temp2 = cert.first;

        if (!instance.has_edge(*temp, *temp2))
            return false;
    }
    return true;
}

/*
 * Steiner Tree
 *
 * Given a weighted graph G, a subset of vertices R, and value k, is there a subgraph T of G such
 * that all three conditions below hold?
 * 1. T has only one connected component
 * 2. Every vertex in R is in T
 * 3. The total weight of the edges in T <= k
 *
 * Instance format:
 * {graph G, begin-end iterator pair for R, value k}
 *
 * Certificate format:
 * graph
 */

/*
 * Graph coloring
 *
 * Given a graph G and value k, is there a mapping f: vertices of G -> set of <= k values such that
 * f(x) != f(y) if (x, y) is an edge in G?
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format:
 * An unordered_map from the vertices of G to "color" values
 */

/*
 * Clique cover
 *
 * Given a graph G and value k, are there <= k sets {S_1, S_2, ...} such that the following hold?
 * 1. union(S_i) = vertices of G
 * 2. For each S_i, its members form a clique in G
 *
 * Instance format:
 * A graph-integer pair
 *
 * Certificate format:
 * begin-end iterator pair to collections of vertices
 */

/*
 * Exact cover
 *
 * Given sets of elements {S_i} and integer k, are there <= k sets {T_j} such that the following
 * hold?
 * 1. {T_j} is a subset of {S_i}
 * 2. For all a != b, T_a intersect T_b = empty set
 * 3. union({T_j}) = union({S_i})
 *
 * Instance format:
 * begin-end iterator pair to sets of elements, integer k
 *
 * Certificate format:
 * A list of subsets
 */

/*
 * Hitting set
 *
 * Given sets of elements {S_i}, is there some set T such that for all i, | T intersect S_i | = 1?
 *
 */

/*
 * 3-dimensional matching
 *
 * Given a set of 3 dimensional vectors {v_i}, let S_0 = {v_i[0] for all i}, S_1 = {v_i[1] for all
 * i}, similar for S_2. Is there a set of vectors {u_j} such that the following hold?
 * 1. {u_j} is a subset of {v_i}
 * 2. For all a != b and all x, u_a[x] != u_b[x]
 * 3. For each S_i, for all x in S_i, there is a vector w in {u_j} such that w[i] = x
 */

/*
 * Knapsack
 *
 * Given a collection C of (not necessarily distinct) values, and target value k, is there a
 * subcollection of C such that the sum of all its values equal k?
 */

/*
 * Job sequencing
 *
 * Given a set of jobs {J_i}, each with a duration, deadline, and penalty, and value k, is there a
 * scheduling of the jobs such that the following hold?
 * 1. Each job is scheduled for a contiguous block of time equal to its duration
 * 2. No jobs have overlapping times
 * 3. No job is scheduled for time t < 0
 * 4. The sum of the penalties for the jobs that are not finished by their given deadlines <= k
 */

/*
 * Partition
 *
 * Given a collection C of values, can C be partitioned into two subcollections such that the sums
 * of values in each subcollection are equal?
 */

/*
 * Max-cut
 *
 * Given a weighted graph G with strictly positive weights and value k, can the vertices of G be
 * partitioned into two sets such that the sum of the weights of edges going between vertices in
 * different sets >= k?
 */
}

#endif // NPC_CERTIFICATE_H
