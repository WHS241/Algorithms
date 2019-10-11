#ifndef ADJACENCY_MATRIX_CPP
#define ADJACENCY_MATRIX_CPP
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

namespace graph {
    template<bool Directed, bool Weighted>
    const impl <Directed, Weighted> &
    graph_adjacency_matrix<Directed, Weighted>::copy_from(const impl <Directed, Weighted> &src) {
        auto cast = dynamic_cast<const graph_adjacency_matrix *>(&src);
        if (cast) {
            return (*this = *cast);
        }

        _t_graph_rep temp(src.order(),
                          std::vector<double>(src.order(), std::numeric_limits<double>::quiet_NaN()));
        for (uint32_t i = 0; i < src.order(); ++i) {
            for (auto &n : src.edges(i)) {
                temp[i][n.first] = n.second;
            }
        }

        _graph = std::move(temp);

        return *this;
    }

    template<bool Directed, bool Weighted>
    uint32_t graph_adjacency_matrix<Directed, Weighted>::order() const noexcept { return _graph.size(); }

    template<bool Directed, bool Weighted>
    bool graph_adjacency_matrix<Directed, Weighted>::has_edge(const uint32_t &start, const uint32_t &dest) const noexcept {
        return !std::isnan(edge_cost(start, dest));
    }

    template<bool Directed, bool Weighted>
    double graph_adjacency_matrix<Directed, Weighted>::edge_cost(const uint32_t &start, const uint32_t &dest) const noexcept {
        if (start >= _graph.size() || dest >= _graph.size())
            return std::numeric_limits<double>::quiet_NaN();
        return _graph[start][dest];
    }

    template<bool Directed, bool Weighted>
    uint32_t graph_adjacency_matrix<Directed, Weighted>::degree(const uint32_t &start) const {
        if (start >= _graph.size())
            throw std::out_of_range("Degree number");
        return std::count_if(
                _graph[start].begin(), _graph[start].end(), [](auto edge) { return !std::isnan(edge); });
    }

    template<bool Directed, bool Weighted>
    std::list<uint32_t> graph_adjacency_matrix<Directed, Weighted>::neighbors(const uint32_t &start) const {
        if (start >= _graph.size())
            throw std::out_of_range("Degree number");

        std::list<uint32_t> result;
        for (uint32_t i = 0; i < _graph.size(); ++i)
            if (!std::isnan(_graph[start][i]))
                result.push_back(i);

        return result;
    }

    template<bool Directed, bool Weighted>
    std::list<std::pair<uint32_t, double>> graph_adjacency_matrix<Directed, Weighted>::edges(const uint32_t &start) const {
        if (start >= _graph.size())
            throw std::out_of_range("Degree number");

        std::list<std::pair<uint32_t, double>> result;
        for (uint32_t i = 0; i < _graph.size(); ++i)
            if (!std::isnan(_graph[start][i]))
                result.emplace_back(i, _graph[start][i]);

        return result;
    }

    template<bool Directed, bool Weighted>
    void graph_adjacency_matrix<Directed, Weighted>::set_edge(const uint32_t &start, const uint32_t &dest, double cost) {
        if (start == dest)
            throw std::invalid_argument("Self-loops not allowed");
        if (start >= _graph.size() || dest >= _graph.size())
            throw std::out_of_range("Degree number");

        _graph[start][dest] = cost;
        if constexpr (!Directed)
            _graph[dest][start] = cost;
    }

    template<bool Directed, bool Weighted>
    uint32_t graph_adjacency_matrix<Directed, Weighted>::add_vertex() {
        auto temp(_graph);
        for (auto &vertex : temp)
            vertex.push_back(std::numeric_limits<double>::quiet_NaN());
        temp.emplace_back(_graph.size() + 1, std::numeric_limits<double>::quiet_NaN());

        _graph = std::move(temp);
        return _graph.size();
    }

    template<bool Directed, bool Weighted>
    void graph_adjacency_matrix<Directed, Weighted>::remove_edge(const uint32_t &start, const uint32_t &dest) {
        if (start != dest)
            set_edge(start, dest, std::numeric_limits<double>::quiet_NaN());
    }

    template<bool Directed, bool Weighted>
    void graph_adjacency_matrix<Directed, Weighted>::isolate(const uint32_t &target) {
        if (target >= _graph.size())
            throw std::out_of_range("Degree number");
        _graph[target] = std::vector<double>(_graph.size(), std::numeric_limits<double>::quiet_NaN());

        if (!Directed)
            for (auto &edgeList : _graph)
                edgeList[target] = std::numeric_limits<double>::quiet_NaN();
    }

    template<bool Directed, bool Weighted>
    void graph_adjacency_matrix<Directed, Weighted>::remove(const uint32_t &target) {
        if (target >= _graph.size())
            throw std::out_of_range("Degree number");

        std::swap(_graph[target], _graph.back());
        for (auto &vertex : _graph)
            std::swap(vertex[target], vertex.back());
    }

    template<bool Directed, bool Weighted>
    void graph_adjacency_matrix<Directed, Weighted>::clear() noexcept { _graph.clear(); }
}
#endif // ADJACENCY_MATRIX_CPP