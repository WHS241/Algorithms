#ifndef ADJACENCY_MATRIX_CPP
#define ADJACENCY_MATRIX_CPP
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>

namespace graph {
template<bool Directed, bool Weighted, typename EdgeWeight>
const impl<Directed, Weighted, EdgeWeight>&
  adjacency_matrix<Directed, Weighted, EdgeWeight>::copy_from(
    const impl<Directed, Weighted, EdgeWeight>& src) {
    auto cast = dynamic_cast<const adjacency_matrix*>(&src);
    if (cast) {
        return (*this = *cast);
    }

    _t_graph_rep temp(
      src.order(), std::vector<_t_matrix_entry>(src.order(), std::make_pair(false, EdgeWeight())));
    for (uint32_t i = 0; i < src.order(); ++i) {
        for (std::pair<uint32_t, EdgeWeight>& n : src.edges(i)) {
            temp[i][n.first] = {true, n.second};
        }
    }

    _graph = std::move(temp);

    return *this;
}

template<bool Directed, bool Weighted, typename EdgeWeight>
uint32_t adjacency_matrix<Directed, Weighted, EdgeWeight>::order() const noexcept {
    return _graph.size();
}

template<bool Directed, bool Weighted, typename EdgeWeight>
bool adjacency_matrix<Directed, Weighted, EdgeWeight>::has_edge(
  const uint32_t& start, const uint32_t& dest) const noexcept {
    return _graph[start][dest].first;
}

template<bool Directed, bool Weighted, typename EdgeWeight>
EdgeWeight adjacency_matrix<Directed, Weighted, EdgeWeight>::edge_cost(const uint32_t& start,
                                                                       const uint32_t& dest) const {
    if (!has_edge(start, dest))
        throw std::domain_error("No edge");
    return _graph[start][dest].second;
}

template<bool Directed, bool Weighted, typename EdgeWeight>
uint32_t adjacency_matrix<Directed, Weighted, EdgeWeight>::degree(const uint32_t& start) const {
    return std::count_if(_graph[start].begin(), _graph[start].end(),
                         [](const _t_matrix_entry& entry) { return !entry.first; });
}

template<bool Directed, bool Weighted, typename EdgeWeight>
std::list<uint32_t>
  adjacency_matrix<Directed, Weighted, EdgeWeight>::neighbors(const uint32_t& start) const {
    std::list<uint32_t> result;
    for (uint32_t i = 0; i < _graph.size(); ++i)
        if (_graph[start][i].first)
            result.push_back(i);

    return result;
}

template<bool Directed, bool Weighted, typename EdgeType>
std::list<std::pair<uint32_t, EdgeType>>
  adjacency_matrix<Directed, Weighted, EdgeType>::edges(const uint32_t& start) const {
    std::list<std::pair<uint32_t, EdgeType>> result;
    for (uint32_t i = 0; i < _graph.size(); ++i)
        if (_graph[start][i].first)
            result.emplace_back(i, _graph[start][i].second);

    return result;
}

template<bool Directed, bool Weighted, typename EdgeType>
std::pair<impl<Directed, Weighted, EdgeType>*, std::vector<uint32_t>>
  adjacency_matrix<Directed, Weighted, EdgeType>::induced_subgraph(
    const std::list<uint32_t>& subset) const {
    std::vector<bool> selected(_graph.size(), false);
    for (uint32_t vertex : subset) {
        selected.at(vertex) = true;
    }

    // determine what vertex in subgraph corresponds to what vertex
    std::unique_ptr<adjacency_matrix<Directed, Weighted, EdgeType>> subgraph =
      std::make_unique<adjacency_matrix<Directed, Weighted, EdgeType>>();
    std::vector<uint32_t> translate_to_sub(_graph.size());
    for (uint32_t i = 0; i < _graph.size(); ++i) {
        if (selected[i]) {
            translate_to_sub[i] = subgraph->order();
            subgraph->add_vertex();
        }
    }

    for (uint32_t i = 0; i < _graph.size(); ++i)
        if (selected[i])
            for (uint32_t j = 0; j < _graph.size(); ++j)
                if (selected[j])
                    subgraph->_graph[translate_to_sub[i]][translate_to_sub[j]] = _graph[i][j];
    return std::make_pair<impl<Directed, Weighted, EdgeType>*, std::vector<uint32_t>>(
      subgraph.release(), std::move(translate_to_sub));
}

template<bool Directed, bool Weighted, typename EdgeWeight>
void adjacency_matrix<Directed, Weighted, EdgeWeight>::set_edge(const uint32_t& start,
                                                                const uint32_t& dest,
                                                                const EdgeWeight& cost) {
    _graph[start][dest] = {true, cost};
    if constexpr (!Directed)
        _graph[dest][start] = {true, cost};
}

template<bool Directed, bool Weighted, typename EdgeWeight>
uint32_t adjacency_matrix<Directed, Weighted, EdgeWeight>::add_vertex() {
    uint32_t old_size = _graph.size();
    try {
        for (std::vector<_t_matrix_entry>& vertex : _graph)
            vertex.emplace_back(false, EdgeWeight());
        _graph.emplace_back(_graph.size() + 1, std::make_pair(false, EdgeWeight()));
    } catch (...) {
        for (std::vector<_t_matrix_entry>& vertex : _graph)
            if (vertex.size() > old_size)
                vertex.pop_back();
        throw;
    }
    return _graph.size();
}

template<bool Directed, bool Weighted, typename EdgeWeight>
void adjacency_matrix<Directed, Weighted, EdgeWeight>::remove_edge(const uint32_t& start,
                                                                   const uint32_t& dest) {
    if (start != dest)
        _graph[start][dest].first = false;
}

template<bool Directed, bool Weighted, typename EdgeWeight>
void adjacency_matrix<Directed, Weighted, EdgeWeight>::isolate(const uint32_t& target) {
    if (target >= _graph.size())
        throw std::out_of_range("Degree number");
    _graph[target] =
      std::vector<_t_matrix_entry>(_graph.size(), _t_matrix_entry(false, EdgeWeight()));

    if constexpr (!Directed)
        for (std::vector<_t_matrix_entry>& edge_list : _graph)
            edge_list[target].first = false;
}

template<bool Directed, bool Weighted, typename EdgeWeight>
void adjacency_matrix<Directed, Weighted, EdgeWeight>::remove(const uint32_t& target) {
    std::swap(_graph[target], _graph.back());
    _graph.pop_back();
    for (std::vector<_t_matrix_entry>& vertex : _graph) {
        std::swap(vertex[target], vertex.back());
        vertex.pop_back();
    }
}

template<bool Directed, bool Weighted, typename EdgeWeight>
void adjacency_matrix<Directed, Weighted, EdgeWeight>::clear() noexcept {
    _graph.clear();
}
} // namespace graph
#endif // ADJACENCY_MATRIX_CPP