#ifndef GRAPH_COMPONENTS_CPP
#define GRAPH_COMPONENTS_CPP
#include <iterator>
#include <stdexcept>
#include <unordered_map>

#include <graph/search.h>

template <typename T>
std::list<std::unordered_set<T>> GraphAlg::connectedComponents(const Graph<T>& src)
{
    if (src.directed())
        throw std::invalid_argument("Directed graph");

    std::list<std::unordered_set<T>> result;
    if (src.order() == 0)
        return result;

    std::unordered_set<T> current;
    depthFirstForest(
        src, src.vertices().front(), [&current](const T& item) { current.insert(item); },
        [](const T&, const T&) {},
        [&result, &current](const T&) {
            result.push_back(current);
            current.clear();
        });

    return result;
}

template <typename T> std::unordered_set<T> GraphAlg::articulationPoints(const Graph<T>& src)
{
    if (src.directed())
        throw std::invalid_argument("Directed graph");

    std::unordered_map<T, bool> visited;
    std::unordered_map<T, uint32_t> searchNumber, low;
    uint32_t currentNum = 0;
    std::unordered_set<T> result;

    if (src.order() == 0)
        return result;

    auto vertices = src.vertices();
    for (T& v : vertices)
        visited[v] = false;

    for (auto it = visited.begin(); it != visited.end();
         it = std::find_if(it, visited.end(), [](auto val) { return !val.second; })) {
        const T& start = it->first;
        uint32_t numRootChildren = 0;

        depthFirst(
            src, start,
            [&currentNum, &searchNumber, &low, &visited](const T& curr) {
                searchNumber[curr] = low[curr] = currentNum++;
                visited[curr] = true;
            },
            [&src, &searchNumber, &low, &result, &numRootChildren, &start](
                const T& parent, const T& child) {
                if (parent == start) {
                    ++numRootChildren;
                } else {
                    // calculate low value of DFS for child (min low of children
                    // currently stored)
                    std::list<T> neighbors = src.neighbors(child);
                    for (const T& v : neighbors) {
                        if (low[child] > searchNumber[v])
                            low[child] = searchNumber[v];
                    }

                    // condition for biconnectivity
                    if (low[child] == searchNumber[parent]) {
                        result.insert(parent);
                    }

                    // update low value of parent
                    if (low[child] < low[parent])
                        low[parent] = low[child];
                }
            });

        if (numRootChildren > 1) {
            result.insert(start);
        }
    }

    return result;
}

template <typename T>
std::list<std::unordered_set<T>> GraphAlg::stronglyConnectedComponents(const Graph<T>& src)
{
    if (!src.directed())
        throw std::invalid_argument("Undirected graph");

    std::unordered_map<T, bool> finished;
    std::unordered_map<T, uint32_t> searchNumber, low;
    uint32_t currentNum = 0;
    std::list<std::unordered_set<T>> result;
    std::unordered_map<T, std::unordered_set<T>> component;

    if (src.order() == 0)
        return result;

    auto vertices = src.vertices();
    for (T& v : vertices)
        finished[v] = false;

    uint32_t numRootChildren = 0;

    depthFirstForest(
        src, src.vertices().front(),
        [&currentNum, &searchNumber, &low, &component](const T& curr) {
            searchNumber[curr] = low[curr] = currentNum++;
            std::unordered_set<T> currComp;
            currComp.insert(curr);
            component[curr] = std::move(currComp);
        },
        [&component, &finished, &low, &result, &searchNumber, &src](
            const T& parent, const T& child) {
            // calculate low value for child (stored min low value of children)
            std::list<T> neighbors = src.neighbors(child);
            for (const T& v : neighbors) {
                if (!finished[v] && (low[child] > searchNumber[v]))
                    low[child] = searchNumber[v];
            }

            // break off SCC if found
            if (low[child] == searchNumber[child]) {
                auto& toAdd = component[child];
                for (const T& member : toAdd)
                    finished[member] = true;
                result.push_back(std::move(toAdd));
            }

            // if not SCC, parent must be part of true SCC
            if (!finished[child]) {
                component[parent].merge(std::move(component[child]));
            }

            // update low value of parent
            if (low[child] < low[parent])
                low[parent] = low[child];
        },
        [&component, &result, &finished](const T& root) {
            // Handle root
            auto& toAdd = component[root];
            for (const T& member : toAdd)
                finished[member] = true;
            result.push_back(toAdd);
        });

    return result;
}

#endif // GRAPH_COMPONENTS_CPP