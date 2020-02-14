#ifndef ADJACENCY_LIST_CPP
#define ADJACENCY_LIST_CPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>

namespace graph {
template <bool Directed, bool Weighted>
const impl<Directed, Weighted>& adjacency_list<Directed, Weighted>::copy_from(
    const impl<Directed, Weighted>& src)
{
    auto cast = dynamic_cast<const adjacency_list*>(&src);
    if (cast) {
        return (*this = *cast);
    }

    _t_graph_rep temp(src.order());
    for (uint32_t i = 0; i < src.order(); ++i) {
        temp[i] = src.edges(i);
    }

    _graph = std::move(temp);

    return *this;
}

template <bool Directed, bool Weighted>
uint32_t adjacency_list<Directed, Weighted>::order() const noexcept
{
    return _graph.size();
}

template <bool Directed, bool Weighted>
bool adjacency_list<Directed, Weighted>::has_edge(const uint32_t& start, const uint32_t& dest) const
    noexcept
{
    if (start >= _graph.size() || dest >= _graph.size())
        return false;
    return std::find_if(_graph[start].cbegin(), _graph[start].cend(), [&dest](const _t_edge& edge) {
        return edge.first == dest;
    }) != _graph[start].cend();
}

template <bool Directed, bool Weighted>
double adjacency_list<Directed, Weighted>::edge_cost(
    const uint32_t& start, const uint32_t& dest) const noexcept
{
    if (start >= _graph.size() || dest >= _graph.size())
        return std::numeric_limits<double>::quiet_NaN();
    auto it = std::find_if(_graph[start].cbegin(), _graph[start].cend(),
        [&dest](const _t_edge& edge) { return edge.first == dest; });
    return (it == _graph[start].cend()) ? std::numeric_limits<double>::quiet_NaN() : it->second;
}

template <bool Directed, bool Weighted>
uint32_t adjacency_list<Directed, Weighted>::degree(const uint32_t& start) const
{
    if (start >= _graph.size())
        throw std::out_of_range("Degree number");
    return _graph[start].size();
}

template <bool Directed, bool Weighted>
std::list<uint32_t> adjacency_list<Directed, Weighted>::neighbors(const uint32_t& start) const
{
    if (start >= _graph.size())
        throw std::out_of_range("Degree number");
    std::list<uint32_t> result;
    std::transform(_graph[start].cbegin(), _graph[start].cend(), std::back_inserter(result),
        [](const _t_edge& edge) { return edge.first; });
    return result;
}

template <bool Directed, bool Weighted>
std::list<std::pair<uint32_t, double>> adjacency_list<Directed, Weighted>::edges(
    const uint32_t& start) const
{
    if (start >= _graph.size())
        throw std::out_of_range("Degree number");
    return _graph[start];
}

template<bool Directed, bool Weighted>
std::pair<impl<Directed, Weighted>*, std::vector<uint32_t>> adjacency_list<Directed, Weighted>::induced_subgraph(const std::list<uint32_t>& subset) const {
    std::vector<bool> selected(_graph.size(), false);
    for(uint32_t vertex : subset) {
        if (vertex >= _graph.size())
            throw std::out_of_range("Degree number");
        selected[vertex] = true;
    }

    // determine what vertex in subgraph corresponds to what vertex
    std::unique_ptr<adjacency_list<Directed, Weighted>> subgraph = std::make_unique<adjacency_list<Directed, Weighted>>();
    std::vector<uint32_t> translate_to_sub(_graph.size());
    for(uint32_t i = 0; i < _graph.size(); ++i) {
        if (selected[i]) {
            translate_to_sub[i] = subgraph->order();
            subgraph->add_vertex();
        }
    }

    for(uint32_t i = 0; i < _graph.size(); ++i)
        if (selected[i])
            for (const _t_edge& edge : _graph[i])
                if (selected[edge.first])
                    if constexpr(Directed)
                        subgraph->force_add(translate_to_sub[i], translate_to_sub[edge.first], edge.second);
                    else if (translate_to_sub[i] < translate_to_sub[edge.first])
                        subgraph->force_add(translate_to_sub[i], translate_to_sub[edge.first], edge.second);
    
    return std::make_pair<impl<Directed, Weighted>*, std::vector<uint32_t>>(subgraph.release(), std::move(translate_to_sub));
}

template <bool Directed, bool Weighted>
void adjacency_list<Directed, Weighted>::set_edge(
    const uint32_t& start, const uint32_t& dest, double cost)
{
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    if (start >= _graph.size() || dest >= _graph.size())
        throw std::out_of_range("Degree number");

    if constexpr (Directed) {
        auto it = std::find_if(_graph[start].begin(), _graph[start].end(),
            [&dest](const _t_edge& edge) { return edge.first == dest; });
        if (it == _graph[start].end())
            _graph[start].emplace_back(dest, cost);
        else
            it->second = cost;
    } else {
        // exception safety
        std::list<_t_edge> temp1(_graph[start]), temp2(_graph[dest]);
        auto it = std::find_if(temp1.begin(), temp1.end(),
            [&dest](const _t_edge& edge) { return edge.first == dest; });
        if (it == temp1.end()) {
            temp1.emplace_back(dest, cost);
            temp2.emplace_back(start, cost);
        } else {
            it->second = cost;
            it = std::find_if(temp2.begin(), temp2.end(),
                [&start](const _t_edge& edge) { return edge.first == start; });
            it->second = cost;
        }

        _graph[start] = std::move(temp1);
        _graph[dest] = std::move(temp2);
    }
}

template <bool Directed, bool Weighted>
void adjacency_list<Directed, Weighted>::force_add(
    const uint32_t& start, const uint32_t& dest, double cost)
{
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    if (start >= _graph.size() || dest >= _graph.size())
        throw std::out_of_range("Degree number");

    if constexpr (Directed) {
        _graph[start].emplace_back(dest, cost);
    } else {
        // exception safety
        std::list<_t_edge> temp1(_graph[start]), temp2(_graph[dest]);
        temp1.emplace_back(dest, cost);
        temp2.emplace_back(start, cost);
        _graph[start] = std::move(temp1);
        _graph[dest] = std::move(temp2);
    }
}

template <bool Directed, bool Weighted> uint32_t adjacency_list<Directed, Weighted>::add_vertex()
{
    _graph.emplace_back();
    return _graph.size();
}

template <bool Directed, bool Weighted>
void adjacency_list<Directed, Weighted>::remove_edge(const uint32_t& start, const uint32_t& dest)
{
    if (start >= _graph.size() || dest >= _graph.size())
        throw std::out_of_range("Degree number");

    auto it = std::find_if(_graph[start].begin(), _graph[start].end(),
        [&dest](const _t_edge& edge) { return edge.first == dest; });
    if (it != _graph[start].end()) {
        _graph[start].erase(it);
        if constexpr (!Directed) {
            it = std::find_if(_graph[dest].begin(), _graph[dest].end(),
                [&start](const _t_edge& edge) { return edge.first == start; });
            _graph[dest].erase(it);
        }
    }
}

template <bool Directed, bool Weighted>
void adjacency_list<Directed, Weighted>::isolate(const uint32_t& target)
{
    if (target >= _graph.size())
        throw std::out_of_range("Degree number");

    if constexpr (!Directed) {
        std::list<_t_edge> edges = _graph[target];
        for (const _t_edge& e : edges)
            _graph[e.first].erase(std::find_if(_graph[e.first].begin(), _graph[e.first].end(),
                [&target](const _t_edge& complement) { return complement.first == target; }));
    }

    _graph[target].clear();
}

template <bool Directed, bool Weighted>
void adjacency_list<Directed, Weighted>::remove(const uint32_t& to_remove)
{
    if (to_remove >= _graph.size())
        throw std::out_of_range("Degree number");

    std::swap(_graph[to_remove], _graph.back());
    for (uint32_t i = 0; i < _graph.size() - 1; ++i) {
        auto it = _graph[i].begin();
        while (it != _graph[i].end()) {
            if (it->first == to_remove) {
                it = _graph[i].erase(it);
            } else {
                if (it->first == _graph.size() - 1)
                    it->first = to_remove;
                ++it;
            }
        }
    }
    _graph.pop_back();
}

template <bool Directed, bool Weighted> void adjacency_list<Directed, Weighted>::clear() noexcept
{
    _graph.clear();
}
}

#endif // ADJACENCY_LIST_CPP