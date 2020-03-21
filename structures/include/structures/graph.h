#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "graph_impl.h"

namespace graph {
enum graph_type { adj_matrix, adj_list };

template <typename VertexType, bool Directed, bool Weighted, typename EdgeType = double, typename Hash = std::hash<VertexType>, typename KeyEqual = std::equal_to<VertexType>> class graph {
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
    virtual bool has_vertex(const VertexType&) const noexcept;
    // returns true if start and dest have edge between them
    virtual bool has_edge(const VertexType& start, const VertexType& dest) const;
    // returns cost of edge between start and dest if exists, or NaN if has_edge(start, dest) is
    // false
    virtual EdgeType edge_cost(const VertexType& start, const VertexType& dest) const;
    // returns degree of vertex
    virtual uint32_t degree(const VertexType&) const;
    // returns list of neighbors (with edge costs for edges())
    virtual std::list<VertexType> neighbors(const VertexType& start) const;
    virtual std::list<std::pair<VertexType, EdgeType>> edges(const VertexType&) const;

    // return a new graph with the same data in a different representation
    virtual graph<VertexType, Directed, Weighted, EdgeType, Hash, KeyEqual> convert(graph_type target) const;
    // return vertices in graph
    virtual std::vector<VertexType> vertices() const;
    // generate a subgraph induced by a set of vertices
    template <typename InputIterator,
        typename _Requires = std::enable_if_t<
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::value_type, VertexType>>>
    graph generate_induced_subgraph(
        InputIterator first, InputIterator last) const;

    // set edge between start and dest to cost
    // adds edge if currently nonexistent
    virtual void set_edge(const VertexType& start, const VertexType& dest, const EdgeType& cost = EdgeType());
    // force add an edge, even if edge already exists
    virtual void force_add(const VertexType& start, const VertexType& dest, const EdgeType& cost = EdgeType());
    // add a new vertex with degree 0 and the given name
    // returns new size
    virtual uint32_t add_vertex(const VertexType& name);
    // remove edge between start and dest
    virtual void remove_edge(const VertexType& start, const VertexType& dest);
    // remove all edges out of start
    virtual void isolate(const VertexType& start);
    // remove vertex
    // no-op if not in graph
    virtual void remove(const VertexType& to_remove);
    // clear graph
    virtual void clear() noexcept;

private:
    void _check_self_loop(const VertexType& u, const VertexType& v);
    void _set_type(graph_type type, const impl<Directed, Weighted, EdgeType>* src = nullptr);
    void _set_empty(graph_type type);

    graph_type _type;
    std::unique_ptr<impl<Directed, Weighted, EdgeType>> _impl;
    std::unordered_map<VertexType, uint32_t, Hash, KeyEqual> _translation;
    std::vector<VertexType> _reverse_translation;
};

template <typename T, bool Directed, bool Weighted, typename EdgeType, typename Hash, typename KeyEqual>
std::ostream& operator<<(std::ostream& os, const graph<T, Directed, Weighted, EdgeType, Hash, KeyEqual>& rhs)
{
    for (const T& vertex : rhs.vertices()) {
        os << vertex << ": ";
        auto edges = rhs.edges(vertex);
        std::transform(edges.begin(), edges.end(), std::ostream_iterator<std::string>(os, ", "),
            [](const std::pair<T, EdgeType>& edge) {
                std::ostringstream builder;
                builder << '[' << edge.first << "] (" << edge.second << ')';
                return builder.str();
            });
        os << std::endl;
    }
    return os;
}
}

#include "src/structures/graph.tpp"

#endif // !GRAPH_H
