#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "graph_impl.h"

namespace graph {
enum graph_type { adj_matrix, adj_list };

/*
Generic Graph representation
Uses a hash map to translate to vertices labeled 1 - n behind the scenes

Preconditions: Hash and KeyEqual are default-constructible and copy-assignable
*/
template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight = double,
    typename Hash = std::hash<Vertex>, typename KeyEqual = std::equal_to<Vertex>>
class graph {
public:
    graph(graph_type type = adj_list);

    virtual ~graph() = default;
    graph(const graph& src);
    const graph& operator=(const graph& rhs);
    graph(graph&& src) noexcept;
    const graph& operator=(graph&& rhs) noexcept;

    // order of graph
    virtual uint32_t order() const noexcept;
    // returns true if vertex exists in graph
    virtual bool has_vertex(const Vertex&) const noexcept;
    // returns true if start and dest have edge between them
    virtual bool has_edge(const Vertex& start, const Vertex& dest) const;
    // returns cost of edge between start and dest if exists, or NaN if has_edge(start, dest) is
    // false
    virtual EdgeWeight edge_cost(const Vertex& start, const Vertex& dest) const;
    // returns degree of vertex
    virtual uint32_t degree(const Vertex&) const;
    // returns list of neighbors (with edge costs for edges())
    virtual std::list<Vertex> neighbors(const Vertex& start) const;
    virtual std::list<std::pair<Vertex, EdgeWeight>> edges(const Vertex&) const;

    // return a new graph with the same data in a different representation
    virtual graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual> convert(
        graph_type target) const;
    // return vertices in graph
    virtual std::vector<Vertex> vertices() const;
    // generate a subgraph induced by a set of vertices
    template <typename InputIterator,
        typename _Requires = std::enable_if_t<std::is_convertible_v<
            typename std::iterator_traits<InputIterator>::value_type, Vertex>>>
    graph generate_induced_subgraph(InputIterator first, InputIterator last) const;

    // set edge between start and dest to cost
    // adds edge if currently nonexistent
    virtual void set_edge(
        const Vertex& start, const Vertex& dest, const EdgeWeight& cost = EdgeWeight());
    // force add an edge, even if edge already exists
    virtual void force_add(
        const Vertex& start, const Vertex& dest, const EdgeWeight& cost = EdgeWeight());
    // add a new vertex with degree 0 and the given name
    // returns new size
    virtual uint32_t add_vertex(const Vertex& name);
    // remove edge between start and dest
    virtual void remove_edge(const Vertex& start, const Vertex& dest);
    // remove all edges out of start
    virtual void isolate(const Vertex& start);
    // remove vertex
    // no-op if not in graph
    virtual void remove(const Vertex& to_remove);
    // clear graph
    virtual void clear() noexcept;

    // Get information about the translation map
    // This field member may be helpful in some cases
    virtual const std::unordered_map<Vertex, uint32_t, Hash, KeyEqual>&
    get_translation() const noexcept
    {
        return _translation;
    }

private:
    void _check_self_loop(const Vertex& u, const Vertex& v);
    void _set_type(graph_type type, const impl<Directed, Weighted, EdgeWeight>* src = nullptr);
    void _set_empty(graph_type type);

    graph_type _type;
    std::unique_ptr<impl<Directed, Weighted, EdgeWeight>> _impl;
    std::unordered_map<Vertex, uint32_t, Hash, KeyEqual> _translation;
    std::vector<Vertex> _reverse_translation;
};

template <typename T, bool Directed, bool Weighted, typename EdgeWeight, typename... MapArgs>
std::ostream& operator<<(
    std::ostream& os, const graph<T, Directed, Weighted, EdgeWeight, MapArgs...>& rhs)
{
    for (const T& vertex : rhs.vertices()) {
        os << vertex << ": ";
        auto edges = rhs.edges(vertex);
        std::transform(edges.begin(), edges.end(), std::ostream_iterator<std::string>(os, ", "),
            [](const std::pair<T, EdgeWeight>& edge) {
                std::ostringstream builder;
                builder << '[' << edge.first << ']';
                if constexpr (Weighted)
                    builder << " (" << edge.second << ')';
                return builder.str();
            });
        os << std::endl;
    }
    return os;
}

// Make parameter juggling easier
template <typename Vertex, bool Directed, typename... Args>
using unweighted_graph = graph<Vertex, Directed, false, bool, Args...>;

}

#include "../../src/structures/graph.tpp"

#endif // !GRAPH_H
