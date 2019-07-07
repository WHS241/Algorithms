#ifndef GRAPH_MISC_CPP
#define GRAPH_MISC_CPP

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Graph.h>

template<typename T>
Graph<T> GraphMisc::minDegreeInducedSubgraph(const Graph<T>& src, uint32_t limit) {
    if (src.directed())
        throw std::invalid_argument("Directed graph");
    Graph<T> temp(src);
    
    std::unordered_map<T, uint32_t> degree;
    std::unordered_set<T> toRemove;
    for (const T& vertex : temp.vertices()) {
        degree[vertex] = temp.degree(vertex);
        if (degree[vertex] < limit)
            toRemove.insert(vertex);
    }

    while (!toRemove.empty()) {
        T target = *toRemove.begin();
        toRemove.erase(toRemove.begin());
        for (const T& neighbor : temp.neighbors(target)) {
            --degree[neighbor];
            if (degree[neighbor] < limit)
                toRemove.insert(neighbor);
        }
        temp.remove(target);
    }

    return temp;
}

#endif //GRAPH_MISC_CPP