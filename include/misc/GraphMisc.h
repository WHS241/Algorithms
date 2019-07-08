#ifndef GRAPH_MISC_H
#define GRAPH_MISC_H

#include <Graph.h>

namespace GraphMisc {
    // find maximal induced subgraph with all degrees >= limit
    // O(V)
    template<typename T>
    Graph<T> minDegreeInducedSubgraph(const Graph<T>& src, uint32_t limit);

    // Celebrity problem
    // Find a vertex (person) with in-degree V-1 and out-degree 0
    // throw std::invalid_argument if none exists
    template<typename T>
    T celebrity(const Graph<T>& src);
}

#include "../../src/misc/GraphMisc.cpp"

#endif // !GRAPH_MISC_H