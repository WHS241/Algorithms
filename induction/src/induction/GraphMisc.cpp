#ifndef GRAPH_MISC_CPP
#define GRAPH_MISC_CPP

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <structures/graph.h>

template <typename T, bool Weighted>
graph::graph<T, false, Weighted> induction::minDegreeInducedSubgraph(
    const graph::graph<T, false, Weighted>& src, uint32_t limit)
{
    graph::graph<T, false, Weighted> temp(src);

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

template <typename T, bool Weighted>
T induction::celebrity(const graph::graph<T, true, Weighted>& src)
{
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