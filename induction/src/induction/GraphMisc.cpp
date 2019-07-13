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
        throw std::domain_error("Directed graph");
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

template<typename T>
T GraphMisc::celebrity(const Graph<T>& src) {
    if (!src.directed())
        throw std::domain_error("Undirected graph");
    if (src.order() == 0)
        throw std::invalid_argument("Empty");

    auto vertices = src.vertices();
    std::list<T> vertList(vertices.begin(), vertices.end());
    
    // eliminate all but one candidate
    while (vertList.size() > 1)
        if (src.hasEdge(vertList.front(), vertList.back()))
            vertList.pop_front();
        else
            vertList.pop_back();
    
    // verify
    bool isCelebrity = src.degree(vertList.front()) == 0;
    for (auto it = vertices.begin(); isCelebrity && it != vertices.end(); ++it)
        if (*it != vertList.front() && !src.hasEdge(*it, vertList.front()))
            isCelebrity = false;
    
    if (isCelebrity)
        return vertList.front();
    throw std::invalid_argument("No celebrity");
}

#endif //GRAPH_MISC_CPP