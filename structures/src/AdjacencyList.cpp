#include <AdjacencyList.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>

AdjacencyList::AdjacencyList(bool directed, bool weighted)
    : GraphImpl(directed, weighted)
    , graph()
{}

const GraphImpl& AdjacencyList::copyFrom(const GraphImpl& src) {
    GRAPH_REP temp(src.order());
    for (uint32_t i = 0; i < src.order(); ++i) {
        temp[i] = src.edges(i);
    }

    dir = src.directed();
    weight = src.weighted();
    graph = std::move(temp);

    return *this;
}

uint32_t AdjacencyList::order() const noexcept {
    return graph.size();
}

bool AdjacencyList::hasEdge(const uint32_t& start, const uint32_t& dest) const noexcept {
    if (start >= graph.size() || dest >= graph.size())
        return false;
    return std::find_if(graph[start].begin(), graph[start].end(), [&dest](auto edge) {
        return edge.first == dest;
        }) != graph[start].end();
}

double AdjacencyList::edgeCost(const uint32_t& start, const uint32_t& dest) const noexcept {
    if (start >= graph.size() || dest >= graph.size())
        return std::numeric_limits<double>::quiet_NaN();
    auto it = std::find_if(graph[start].begin(), graph[start].end(), [&dest](auto edge) {
        return edge.first == dest;
        });
    return (it == graph[start].end()) ? std::numeric_limits<double>::quiet_NaN() : it->second;
}

uint32_t AdjacencyList::degree(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");
    return graph[start].size();
}

std::list<uint32_t> AdjacencyList::neighbors(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");
    std::list<uint32_t> result;
    std::transform(graph[start].begin(), graph[start].end(), std::back_inserter(result), [](auto edge) {
        return edge.first;
        });
    return result;
}

std::list<std::pair<uint32_t, double>> AdjacencyList::edges(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");
    return graph[start];
}

void AdjacencyList::setEdge(const uint32_t& start, const uint32_t& dest, double cost) {
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    if (start >= graph.size() || dest >= graph.size())
        throw std::out_of_range("Degree number");

    if (dir) {
        auto it = std::find_if(graph[start].begin(), graph[start].end(), [&dest](auto edge) {
            return edge.first == dest;
            });
        if (it == graph[start].end())
            graph[start].push_back(std::make_pair(dest, cost));
        else
            it->second = cost;
    }
    else {
        //exception safety
        auto temp1(graph[start]), temp2(graph[dest]);
        auto it = std::find_if(temp1.begin(), temp1.end(), [&dest](auto edge) {
            return edge.first == dest;
            });
        if (it == temp1.end()) {
            temp1.push_back(std::make_pair(dest, cost));
            temp2.push_back(std::make_pair(start, cost));
        }
        else {
            it->second = cost;
            it = std::find_if(temp2.begin(), temp2.end(), [&start](auto edge) {
                return edge.first == start;
                });
            it->second = cost;
        }

        graph[start] = std::move(temp1);
        graph[dest] = std::move(temp2);
    }
}

uint32_t AdjacencyList::addVertex() {
    graph.push_back(std::list<EDGE>());
    return graph.size();
}

void AdjacencyList::removeEdge(const uint32_t& start, const uint32_t& dest) {
    if (start >= graph.size() || dest >= graph.size())
        throw std::out_of_range("Degree number");


    auto it = std::find_if(graph[start].begin(), graph[start].end(), [&dest](auto edge) {
        return edge.first == dest;
        });
    if (it != graph[start].end()) {
        graph[start].erase(it);
        if (!dir) {
            it = std::find_if(graph[dest].begin(), graph[dest].end(), [&start](auto edge) {
                return edge.first == start;
                });
            graph[dest].erase(it);
        }
    }
}

void AdjacencyList::isolate(const uint32_t& target) {
    if (target >= graph.size())
        throw std::out_of_range("Degree number");

    if (!dir) {
        std::list<EDGE> edges = graph[target];
        for (auto& e : edges)
            graph[e.first].erase(std::find_if(graph[e.first].begin(), graph[e.first].end(), [&target](auto complement) {
                return complement.first == target;
                }));
    }

    graph[target].clear();
}

void AdjacencyList::remove(const uint32_t& toRemove) {
    if (toRemove >= graph.size())
        throw std::out_of_range("Degree number");

    std::swap(graph[toRemove], graph.back());
    for (uint32_t i = 0; i < graph.size() - 1; ++i) {
        auto it = graph[i].begin();
        while (it != graph[i].end()) {
            if (it->first == toRemove) {
                it = graph[i].erase(it);
            }
            else {
                if (it->first == graph.size() - 1)
                    it->first = toRemove;
                ++it;
            }
        }
    }
    graph.pop_back();
}

void AdjacencyList::clear() noexcept {
    graph.clear();
}