#ifndef GRAPH_MISC_CPP
#define GRAPH_MISC_CPP

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <structures/graph.h>

template<typename T, bool Weighted>
T induction::celebrity(const graph::graph<T, true, Weighted>& src) {
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

#endif // GRAPH_MISC_CPP