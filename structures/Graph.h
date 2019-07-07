#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "GraphImpl.h"

enum GraphType {AdjMatrix, AdjList};


template<typename T>
class Graph {
public:
    Graph(bool directed = false, bool weighted = false, GraphType type = AdjList);
    virtual ~Graph() = default;
    Graph(const Graph<T>& src);
    const Graph& operator=(const Graph<T>& rhs);
    Graph(Graph<T>&& src);
    const Graph& operator=(Graph<T>&& rhs);

    // order of graph
    virtual uint32_t order() const noexcept;
    // returns true if start and dest have edge between them
    virtual bool hasEdge(const T& start, const T& dest) const noexcept;
    // returns cost of edge between start and dest
    virtual double edgeCost(const T& start, const T& dest) const;
    // returns degree of vertex
    virtual uint32_t degree(const T&) const;
    // returns list of neighbors (with edge costs for edges())
    virtual std::list<T> neighbors(const T& start) const;
    virtual std::list<std::pair<T, double>> edges(const T&) const;
    // returns whether graph is directed/weighted
    virtual bool directed() const noexcept;
    virtual bool weighted() const noexcept;
    // return a new graph with the same data in a different representation
    virtual Graph<T> convert(GraphType target) const;
    // return vertices in graph
    virtual std::vector<T> vertices() const;

    // set edge between start and dest to cost
    // adds edge if currently nonexistent
    virtual void setEdge(const T& start, const T& dest, double cost = 0.0);
    // add a new vertex with degree 0 and the given name
    // returns new size
    virtual uint32_t addVertex(const T& name);
    // remove edge between start and dest
    virtual void removeEdge(const T& start, const T& dest);
    // remove all edges out of start
    virtual void isolate(const T& start);
    // remove vertex
    // no-op if not in graph
    virtual void remove(const T& toRemove);
    // clear graph
    virtual void clear() noexcept;

private:
    void setType(GraphType type, const GraphImpl* const src = nullptr);
    void setEmpty(GraphType type, bool directed, bool weighted);

    GraphType type;
    std::unique_ptr<GraphImpl> impl;
    std::unordered_map<T, uint32_t> translation;
    std::vector<T> reverse_translation;
};

#include "src/Graph.cpp"

#endif // !GRAPH_H
