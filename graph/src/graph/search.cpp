#ifndef GRAPH_SEARCH_CPP
#define GRAPH_SEARCH_CPP
#include <list>
#include <queue>
#include <stdexcept>
#include <unordered_map>

// Recursive helper for DFS
template <typename T, typename F1, typename F2>
static void depthFirstHelper(const Graph<T>& src, const T& current, F1& preFunc, F2& postFunc,
    std::unordered_map<T, bool>& visited)
{
    visited[current] = true;
    preFunc(current);
    for (const T& neighbor : src.neighbors(current)) {
        if (!visited[neighbor]) {
            depthFirstHelper(src, neighbor, preFunc, postFunc, visited);
            postFunc(current, neighbor);
        }
    }
}

template <typename T, typename F1, typename F2>
void GraphAlg::depthFirst(const Graph<T>& src, const T& startVertex, F1 preFunc, F2 postFunc)
{
    std::unordered_map<T, bool> visited;
    auto vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(startVertex) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    depthFirstHelper(src, startVertex, preFunc, postFunc, visited);
}

template <typename T, typename F1, typename F2, typename F3>
void GraphAlg::depthFirstForest(
    const Graph<T>& src, const T& startVertex, F1 preFunc, F2 postFunc, F3 postRoot)
{
    std::unordered_map<T, bool> visited;
    auto vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(startVertex) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    depthFirstHelper(src, startVertex, preFunc, postFunc, visited);
    postRoot(startVertex);

    auto findUnvisited = [](const std::pair<T, bool>& x) { return !x.second; };

    for (auto it = std::find_if(visited.begin(), visited.end(), findUnvisited); it != visited.end();
         it = std::find_if(visited.begin(), visited.end(), findUnvisited)) {
        depthFirstHelper(src, it->first, preFunc, postFunc, visited);
        postRoot(it->first);
    }
}

template <typename T, typename F>
void GraphAlg::breadthFirst(const Graph<T>& src, const T& startVertex, F function)
{
    std::unordered_map<T, bool> visited;
    auto vertices = src.vertices();
    if (vertices.empty())
        return;

    for (const T& vertex : vertices)
        visited[vertex] = false;

    if (visited.find(startVertex) == visited.end())
        throw std::out_of_range("Vertex does not exist");

    std::queue<T, std::list<T>> toVisit;
    toVisit.push(startVertex);
    while (!toVisit.empty()) {
        T current = toVisit.front();
        toVisit.pop();
        if (!visited[current]) {
            function(current);
            visited[current] = true;

            for (const T& neighbor : src.neighbors(current)) {
                if (!visited[neighbor]) {
                    toVisit.push(neighbor);
                }
            }
        }
    }
}

#endif // GRAPH_SEARCH_CPP