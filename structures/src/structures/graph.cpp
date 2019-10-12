#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <stdexcept>

#include <structures/graph_adjacency_list.h>
#include <structures/graph_adjacency_matrix.h>

namespace graph {
template <typename T, bool Directed, bool Weighted>
graph<T, Directed, Weighted>::graph(graph_type type)
    : _type(type)
    , _translation()
    , _reverse_translation()
{
    _set_empty(type);
}

template <typename T, bool Directed, bool Weighted>
graph<T, Directed, Weighted>::graph(const graph<T, Directed, Weighted>& src)
    : _type(src._type)
    , _translation(src._translation)
    , _reverse_translation(src._reverse_translation)
{
    _set_type(_type, src._impl.get());
}

template <typename T, bool Directed, bool Weighted>
const graph<T, Directed, Weighted>& graph<T, Directed, Weighted>::operator=(
    const graph<T, Directed, Weighted>& rhs)
{
    if (this != &rhs) {
        graph<T, Directed, Weighted> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T, bool Directed, bool Weighted>
graph<T, Directed, Weighted>::graph(graph<T, Directed, Weighted>&& src) noexcept
    : _type()
    , _impl()
    , _translation()
    , _reverse_translation()
{
    *this = std::move(src);
}

template <typename T, bool Directed, bool Weighted>
const graph<T, Directed, Weighted>& graph<T, Directed, Weighted>::operator=(
    graph<T, Directed, Weighted>&& rhs) noexcept
{
    if (this != &rhs) {
        _impl.reset();
        std::swap(_type, rhs._type);
        std::swap(_impl, rhs._impl);
        _translation = std::move(rhs._translation);
        _reverse_translation = std::move(rhs._reverse_translation);
    }
    return *this;
}

template <typename T, bool Directed, bool Weighted>
uint32_t graph<T, Directed, Weighted>::order() const noexcept
{
    return _impl->order();
}

template <typename T, bool Directed, bool Weighted>
bool graph<T, Directed, Weighted>::has_edge(const T& start, const T& dest) const noexcept
{
    if (_translation.find(start) == _translation.end()
        || _translation.find(dest) == _translation.end())
        return false;

    return _impl->has_edge(_translation.at(start), _translation.at(dest));
}

template <typename T, bool Directed, bool Weighted>
double graph<T, Directed, Weighted>::edge_cost(const T& start, const T& dest) const
{
    return _impl->edge_cost(_translation.at(start), _translation.at(dest));
}

template <typename T, bool Directed, bool Weighted>
uint32_t graph<T, Directed, Weighted>::degree(const T& vertex) const
{
    return _impl->degree(_translation.at(vertex));
}

template <typename T, bool Directed, bool Weighted>
std::list<T> graph<T, Directed, Weighted>::neighbors(const T& start) const
{
    auto representation = _impl->neighbors(_translation.at(start));
    std::list<T> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
        [this](uint32_t& x) { return _reverse_translation[x]; });
    return result;
}

template <typename T, bool Directed, bool Weighted>
std::list<std::pair<T, double>> graph<T, Directed, Weighted>::edges(const T& src) const
{
    auto representation = _impl->edges(_translation.at(src));
    std::list<std::pair<T, double>> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
        [this](std::pair<uint32_t, double>& edge) {
            return std::make_pair(_reverse_translation[edge.first], edge.second);
        });
    return result;
}

template <typename T, bool Directed, bool Weighted>
graph<T, Directed, Weighted> graph<T, Directed, Weighted>::convert(graph_type type) const
{
    graph<T, Directed, Weighted> result(type);
    result._translation = this->_translation;
    result._reverse_translation = this->_reverse_translation;
    result._set_type(type, this->_impl.get());

    return result;
}

template <typename T, bool Directed, bool Weighted>
std::vector<T> graph<T, Directed, Weighted>::vertices() const
{
    return _reverse_translation;
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::set_edge(const T& start, const T& dest, double cost)
{
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    _impl->set_edge(_translation.at(start), _translation.at(dest), cost);
}

template <typename T, bool Directed, bool Weighted>
uint32_t graph<T, Directed, Weighted>::add_vertex(const T& name)
{
    if (_translation.find(name) != _translation.end())
        throw std::invalid_argument("Already exists in graph");

    auto tempTranslate(_translation);
    auto tempReverse(_reverse_translation);
    tempTranslate[name] = tempReverse.size();
    tempReverse.push_back(name);
    uint32_t result = _impl->add_vertex();
    _translation = std::move(tempTranslate);
    _reverse_translation = std::move(tempReverse);
    return result;
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::remove_edge(const T& start, const T& dest)
{
    _impl->remove_edge(_translation.at(start), _translation.at(dest));
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::isolate(const T& start)
{
    _impl->isolate(_translation.at(start));
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::remove(const T& to_remove)
{
    if (_translation.find(to_remove) == _translation.end())
        return;

    auto temp_translate(_translation);
    auto temp_reverse(_reverse_translation);
    temp_translate[_reverse_translation.back()] = _translation[to_remove];
    temp_reverse[_translation[to_remove]] = temp_reverse.back();
    temp_translate.erase(to_remove);
    temp_reverse.pop_back();
    _impl->remove(_translation[to_remove]);
    _translation = std::move(temp_translate);
    _reverse_translation = std::move(temp_reverse);
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::clear() noexcept
{
    _impl->clear();
    _translation.clear();
    _reverse_translation.clear();
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::_set_type(graph_type type, const impl<Directed, Weighted>* src)
{
    if (src == this->_impl.get()) {
        if (type != this->_type) {
            *this = convert(type);
        }
    } else {
        _set_empty(type);
        this->_impl->copy_from(*src);
    }
}

template <typename T, bool Directed, bool Weighted>
void graph<T, Directed, Weighted>::_set_empty(graph_type type)
{
    switch (type) {
    case adj_matrix:
        _impl.reset(new graph_adjacency_matrix<Directed, Weighted>());
        break;

    case adj_list:
        _impl.reset(new adjacency_list<Directed, Weighted>());
        break;

    default:
        break;
    }
}
}

#endif // GRAPH_CPP
