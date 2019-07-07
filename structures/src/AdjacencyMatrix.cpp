#include <AdjacencyMatrix.h>

#include <algorithm>
#include <functional>
#include <limits>

AdjacencyMatrix::AdjacencyMatrix(bool directed, bool weighted)
    : GraphImpl(directed, weighted)
    , graph()
{}

const GraphImpl& AdjacencyMatrix::copyFrom(const GraphImpl& src) {
    if (this != &src) {
        GRAPH_REP temp(src.order(), std::vector<double>(src.order(), std::numeric_limits<double>::quiet_NaN()));
        for (uint32_t i = 0; i < src.order(); ++i) {
            for (auto& n : src.edges(i)) {
                temp[i][n.first] = n.second;
            }
        }

        graph = std::move(temp);
        dir = src.directed();
        weight = src.weighted();
    }

    return *this;
}

uint32_t AdjacencyMatrix::order() const noexcept {
    return graph.size();
}

bool AdjacencyMatrix::hasEdge(const uint32_t& start, const uint32_t& dest) const noexcept {
    return !std::isnan(edgeCost(start, dest));
}

double AdjacencyMatrix::edgeCost(const uint32_t& start, const uint32_t& dest) const noexcept {
    if (start >= graph.size() || dest >= graph.size())
        return std::numeric_limits<double>::quiet_NaN();
    return graph[start][dest];
}

uint32_t AdjacencyMatrix::degree(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");
    return std::count_if(graph[start].begin(), graph[start].end(), [this](auto edge) {
        return !std::isnan(edge);
        });
}

std::list<uint32_t> AdjacencyMatrix::neighbors(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");

    std::list<uint32_t> result;
    for (uint32_t i = 0; i < graph.size(); ++i)
        if (!std::isnan(graph[start][i]))
            result.push_back(i);

    return result;
}

std::list<std::pair<uint32_t, double>> AdjacencyMatrix::edges(const uint32_t& start) const {
    if (start >= graph.size())
        throw std::out_of_range("Degree number");

    std::list<std::pair<uint32_t, double>> result;
    for (uint32_t i = 0; i < graph.size(); ++i)
        if (!std::isnan(graph[start][i]))
            result.push_back(std::make_pair(i, graph[start][i]));

    return result;
}

void AdjacencyMatrix::setEdge(const uint32_t& start, const uint32_t& dest, double cost) {
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    if (start >= graph.size() || dest >= graph.size())
        throw std::out_of_range("Degree number");

    graph[start][dest] = cost;
    if (!dir)
        graph[dest][start] = cost;
}

uint32_t AdjacencyMatrix::addVertex() {
    auto temp(graph);
    for (auto& vertex : temp)
        vertex.push_back(std::numeric_limits<double>::quiet_NaN());
    temp.push_back(std::vector<double>(graph.size() + 1, std::numeric_limits<double>::quiet_NaN()));

    graph = std::move(temp);
    return graph.size();
}

void AdjacencyMatrix::removeEdge(const uint32_t& start, const uint32_t& dest) {
    if (start != dest)
        setEdge(start, dest, std::numeric_limits<double>::quiet_NaN());
}

void AdjacencyMatrix::isolate(const uint32_t& target) {
    if(target >= graph.size())
        throw std::out_of_range("Degree number");
    graph[target] = std::vector<double>(graph.size(), std::numeric_limits<double>::quiet_NaN());

    if (!dir)
        for (auto& edgeList : graph)
            edgeList[target] = std::numeric_limits<double>::quiet_NaN();
}

void AdjacencyMatrix::remove(const uint32_t& toRemove) {
    if (toRemove >= graph.size())
        throw std::out_of_range("Degree number");

    std::swap(graph[toRemove], graph.back());
    for (auto& vertex : graph)
        std::swap(vertex[toRemove], vertex.back());
}

void AdjacencyMatrix::clear() noexcept {
    graph.clear();
}