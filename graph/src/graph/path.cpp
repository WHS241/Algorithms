#ifndef GRAPH_PATH_CPP
#define GRAPH_PATH_CPP
#include <stdexcept>

#include <graph/search.h>

#include <structures/Graph.h>

template <typename T>
std::list<T> GraphAlg::leastEdgesPath(const Graph<T>& src, const T& start, const T& dest)
{
    std::list<T> result;
    std::unordered_map<T, uint32_t> searchNumber;
    uint32_t currentNum = 0;
    bool found = false;

    std::unordered_map<T, T> parent;
    breadthFirst(
        src, start, [&dest, &searchNumber, &currentNum, &found, &parent, &src](const T& curr) {
            searchNumber[curr] = currentNum++;
            if (curr == dest) {
                found = true;
            } else if (!found) {
                std::list<T> neighbors = src.neighbors(curr);
                for (const T& vertex : neighbors) {
                    if (parent.find(vertex) == parent.end())
                        parent[vertex] = curr;
                }
            }
        });

    if (!found)
        throw std::domain_error("No path");

    for (T current = dest; current != start; current = parent[current])
        result.push_front(current);

    return result;
}

template <typename T, typename Compare>
std::pair<double, std::list<T>> GraphAlg::shortestPathDAG(
    const Graph<T>& src, const T& start, const T& dest, Compare comp)
{
    std::unordered_map<T, std::pair<double, T>> allRes = allVertShortestPathDAG(src, start, comp);
    std::pair<double, std::list<T>> result;
    if (allRes.find(dest) == allRes.end())
        throw std::domain_error("No path");

    for (T current = dest; current != start; current = allRes[current].second) {
        result.second.push_back(current);
        result.first += allRes[current].first;
    }
    return result;
}

template <typename T, typename Compare>
std::unordered_map<T, std::pair<double, T>> GraphAlg::allVertShortestPathDAG(
    const Graph<T>& src, const T& start, Compare compare)
{
    if (!src.directed())
        throw std::invalid_argument("Undirected graph");
    std::unordered_map<T, uint32_t> inDegree;
    std::list<T> toAdd; // vertices with in-degree 0
    std::vector<T> order; // stores topological sort

    // populate map
    for (const T& vert : src.vertices()) {
        inDegree[vert] = 0;
    }

    // correct map values
    for (const T& source : src.vertices())
        for (const T& terminal : src.neighbors(source))
            ++inDegree[terminal];

    for (const std::pair<T, uint32_t>& value : inDegree)
        if (value.second == 0)
            toAdd.push_back(value.first);

    // topological sort
    while (!toAdd.empty()) {
        T current = toAdd.front();
        toAdd.pop_front();
        order.push_back(current);
        for (const T& terminal : src.neighbors(current)) {
            --inDegree[terminal];
            if (inDegree[terminal] == 0)
                toAdd.push_back(terminal);
        }
    }

    if (order.size() != src.order())
        throw std::invalid_argument("Not DAG");

    // start processing
    std::unordered_map<T, std::pair<double, T>> result;
    auto it = std::find(order.begin(), order.end(), start);
    if (it == order.end())
        throw std::invalid_argument("Start vertex does not exist");
    result[start].first = 0;
    result[start].second = start;

    for (; it != order.end(); ++it) {
        if (result.find(*it) != result.end()) {
            for (const std::pair<T, double>& edge : src.edges(*it)) {
                if (result.find(edge.first) == result.end()
                    || compare(result[*it].first + edge.second, result[edge.first].first)) {
                    result[edge.first].first = result[*it].first + edge.second;
                    result[edge.first].second = *it;
                }
            }
        }
    }
}
#endif // GRAPH_PATH_CPP
