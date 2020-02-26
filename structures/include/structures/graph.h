#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
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

template <typename T, bool Directed, bool Weighted> class graph {
public:
    graph(graph_type type = adj_list);

    virtual ~graph() = default;
    graph(const graph<T, Directed, Weighted>& src);
    const graph& operator=(const graph<T, Directed, Weighted>& rhs);
    graph(graph<T, Directed, Weighted>&& src) noexcept;
    const graph& operator=(graph<T, Directed, Weighted>&& rhs) noexcept;

    // order of graph
    virtual uint32_t order() const noexcept;
    // returns true if start and dest have edge between them
    virtual bool has_edge(const T& start, const T& dest) const noexcept;
    // returns cost of edge between start and dest if exists, or NaN if has_edge(start, dest) is
    // false
    virtual double edge_cost(const T& start, const T& dest) const;
    // returns degree of vertex
    virtual uint32_t degree(const T&) const;
    // returns list of neighbors (with edge costs for edges())
    virtual std::list<T> neighbors(const T& start) const;
    virtual std::list<std::pair<T, double>> edges(const T&) const;

    // return a new graph with the same data in a different representation
    virtual graph<T, Directed, Weighted> convert(graph_type target) const;
    // return vertices in graph
    virtual std::vector<T> vertices() const;
    // generate a subgraph induced by a set of vertices
    template <typename InputIterator,
        typename _Requires = std::enable_if_t<
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::value_type, T>>>
    graph<T, Directed, Weighted> generate_induced_subgraph(
        InputIterator first, InputIterator last) const;

    // set edge between start and dest to cost
    // adds edge if currently nonexistent
    virtual void set_edge(const T& start, const T& dest, double cost = 0.0);
    // force add an edge, even if edge already exists
    virtual void force_add(const T& start, const T& dest, double cost = 0.0);
    // add a new vertex with degree 0 and the given name
    // returns new size
    virtual uint32_t add_vertex(const T& name);
    // remove edge between start and dest
    virtual void remove_edge(const T& start, const T& dest);
    // remove all edges out of start
    virtual void isolate(const T& start);
    // remove vertex
    // no-op if not in graph
    virtual void remove(const T& to_remove);
    // clear graph
    virtual void clear() noexcept;

private:
    void _set_type(graph_type type, const impl<Directed, Weighted>* src = nullptr);
    void _set_empty(graph_type type);

    graph_type _type;
    std::unique_ptr<impl<Directed, Weighted>> _impl;
    std::unordered_map<T, uint32_t> _translation;
    std::vector<T> _reverse_translation;
};

template <typename T, bool Directed>
std::ostream& operator<<(std::ostream& os, const graph<T, Directed, true>& rhs)
{
    for (const T& vertex : rhs.vertices()) {
        os << vertex << ": ";
        auto edges = rhs.edges(vertex);
        std::transform(edges.begin(), edges.end(), std::ostream_iterator<std::string>(os, ", "),
            [](const std::pair<T, double>& edge) {
                std::ostringstream builder;
                builder << '[' << edge.first << "] (" << edge.second << ')';
                return builder.str();
            });
        os << std::endl;
    }
    return os;
}
}

#include "src/structures/graph.cpp"

#endif // !GRAPH_H
