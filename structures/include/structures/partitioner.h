#ifndef PARTITIONER_H
#define PARTITIONER_H

#include <util/it_hash.h>

#include "graph.h"

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
class partitioner {
    public:
    typedef typename std::list<std::list<Vertex>>::iterator set_ptr;

    // Initialize for a given graph
    partitioner(const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>&);

    // Get an iterator/pointer to the partition group of a particular vertex
    typename std::list<std::list<Vertex>>::iterator get_set(const Vertex&) const;

    // Perform a partition on a vertex; returns a set of the iterators that have been split
    // Parameter 1: pivot vertex
    // Parameter 2: if pivot shares set with neighbors (and/or non-neighbors), should the pivot go with the neighbors (true) or non-neighbors (false)
    std::unordered_set<set_ptr, util::it_hash<set_ptr>> partition(const Vertex&, bool include_with_neighbors);

    // Merge two sets, given a vertex in each. The second set is always merged into the first.
    void merge_sets(const Vertex&, const Vertex&);

    // Clear empty sets to free up memory
    // WARNING: this may invalidate pointers previously returned from partition()
    void clean() noexcept;

    // Remove a vertex from the sets
    void remove_vertex(const Vertex&) noexcept;

    // Add a previously removed vertex to its own set
    void add_back(const Vertex&);

    private:
    const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& graph;
    std::list<std::list<Vertex>> sets;
    std::unordered_map<Vertex, std::pair<typename std::list<std::list<Vertex>>::iterator, typename std::list<Vertex>::iterator>, Args...> set_ptrs;
};

#include "../../src/structures/partitioner.tpp"

#endif // PARTITIONER_H