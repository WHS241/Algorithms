#ifndef GRAPH_PATH_CPP
#define GRAPH_PATH_CPP
#include <stdexcept>

#include <graph/search.h>

#include <structures/Graph.h>
#include <structures/heap>

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

template <typename T>
std::pair<double, std::list<T>> GraphAlg::pathDijkstra(
    const Graph<T>& src, const T& start, const T& dest)
{
    std::unordered_map<T, std::pair<double, T>> allPairResult = pathDijkstra(src, start);
    if (allPairResult.at(dest).second == dest)
        throw std::invalid_argument("No path");

    std::list<T> path;

    for (T current = dest; current != start; current = allPairResult[current].second) {
        path.push_front(current);
    }

    return std::make_pair(allPairResult[dest].first, path);
}
template <typename T>
std::unordered_map<T, std::pair<double, T>> GraphAlg::pathDijkstra(
    const Graph<T>& src, const T& start)
{
    if (!src.weighted())
        throw std::invalid_argument("Unweighted graph");

    struct Data {
        T current;
        T from;
        double cost;
        bool operator<(const Data& rhs) const { return cost < rhs.cost; };
    };

    typedef typename NodeHeap<Data>::Node Node;

    std::vector<T> vertices = src.vertices();
    std::vector<Data> data(vertices.size());
    std::transform(vertices.begin(), vertices.end(), data.begin(), [&start](const T& value) {
        Data result;
        result.current = value;
        result.from = value;
        result.cost = (value == start) ? 0 : std::numeric_limits<double>::max();
        return result;
    });

    // the heap structure used here determines the runtime of the algorithm
    // must be a NodeHeap
    FibonacciHeap<Data> heap;
    std::unordered_map<T, Node*> tracker;

    auto it1 = vertices.begin();
    for (auto& node : data) {
        tracker[*it1] = heap.add(node);
        ++it1;
    }

    auto heapPtr = &heap;
    std::unordered_map<T, std::pair<double, T>> result;

    while (!heapPtr->empty()) {
        Data toAdd = heap.removeRoot();
        tracker.erase(toAdd.current);
        result[toAdd.current] = std::make_pair(toAdd.cost, toAdd.from);
        // if we are trying to optimize single-destination algorithm, can add break here
        // once dest is found

        // don't want to deal with potential overflow case if no path is found
        if (toAdd.current == start || toAdd.from != toAdd.current) {
            // update heap values
            for (const T& neighbor : src.neighbors(toAdd.current)) {
                auto it = tracker.find(neighbor);
                if (it != tracker.end()) {
                    Node* ptr = it->second;
                    const Node* readPtr = ptr;
                    double edge = src.edgeCost(toAdd.current, neighbor);
                    if (edge < 0)
                        throw std::invalid_argument("Negative weight");

                    double toComp = edge + toAdd.cost;
                    if (toComp < (*readPtr)->cost) {
                        Data replace(**readPtr);
                        replace.from = toAdd.current;
                        replace.cost = toComp;
                        heap.decrease(ptr, replace);
                    }
                }
            }
        }
    }

    return result;
}

template <typename T>
std::unordered_map<T, std::pair<double, T>> GraphAlg::pathBellmanFord(
    const Graph<T>& src, const T& start)
{
    if (!src.weighted())
        throw std::invalid_argument("Unweighted graph");

    // If there are any negative weights in an undirected graph, there is a trivial negative
    // cycle Can use the more efficient Dijkstra algorithm in this case
    if (!src.directed())
        return pathDijkstra(src, start);

    std::unordered_map<T, std::pair<double, T>> result;
    auto vertices = src.vertices();
    for (const T& v : vertices)
        result[v] = std::make_pair(v == start ? 0 : std::numeric_limits<double>::max(), v);

    // relax continuously
    // a loop can have at most V edges, can put off the final edge until negative cycle check
    for (uint32_t i = 1; i < vertices.size(); ++i) {
        bool relaxed = false;

        // go through all the edges
        for (const T& v : vertices) {
            // updating won't help if we haven't visited yet
            if (v == start || result[v].second != v) {
                for (auto& edge : src.edges(v)) {
                    if ((edge.first != start && result[edge.first].second == edge.first)
                        || (result[v].first + edge.second < result[edge.first].first)) {
                        result[edge.first] = std::make_pair(result[v].first + edge.second, v);
                        relaxed = true;
                    }
                }
            }
        }

        // if no edges were updated, another iteration won't help
        // we also know there cannot be a negative cycle accessible to start,
        // because then we would go through all V-1 iterations, continuously relaxing through the 
        // cycle
        if (!relaxed)
            return result;
    }

    // After V-1 iterations, any further updates must be the result of a negative cycle
    for (const T& v : vertices)
        if (v == start || result[v].second != v)
            for (auto& edge : src.edges(v))
                if (result[v].first + edge.second < result[edge.first].first)
                    throw std::domain_error("Negative cycle");

    return result;
}

#endif // GRAPH_PATH_CPP
