#ifndef GRAPH_MISC_H
#define GRAPH_MISC_H

#include <Graph.h>

namespace GraphMisc {
    // find maximal induced subgraph with all degrees >= limit
    template<typename T>
    Graph<T> minDegreeInducedSubgraph(const Graph<T>& src, uint32_t limit);
}

#include "../../src/misc/GraphMisc.cpp"

#endif // !GRAPH_MISC_H