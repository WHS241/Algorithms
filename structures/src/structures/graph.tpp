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
template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::graph(graph_type type) :
    _type(type), _translation(), _reverse_translation() {
    _set_empty(type);
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::graph(
  const graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>& src) :
    _type(src._type),
    _translation(src._translation),
    _reverse_translation(src._reverse_translation) {
    _set_type(_type, src._impl.get());
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
const graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>&
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::operator=(
    const graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>& rhs) {
    if (this != &rhs) {
        graph temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::graph(
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>&& src) noexcept :
    _type(), _impl(), _translation(), _reverse_translation() {
    *this = std::move(src);
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
const graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>&
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::operator=(
    graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>&& rhs) noexcept {
    if (this != &rhs) {
        _impl.reset();
        std::swap(_type, rhs._type);
        std::swap(_impl, rhs._impl);
        _translation = std::move(rhs._translation);
        _reverse_translation = std::move(rhs._reverse_translation);
    }
    return *this;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
uint32_t graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::order() const noexcept {
    return _impl->order();
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
bool graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::has_vertex(
  const Vertex& v) const noexcept {
    return _translation.find(v) != _translation.end();
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
bool graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::has_edge(
  const Vertex& start, const Vertex& dest) const {
    return _impl->has_edge(_translation.at(start), _translation.at(dest));
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
EdgeWeight graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::edge_cost(
  const Vertex& start, const Vertex& dest) const {
    return _impl->edge_cost(_translation.at(start), _translation.at(dest));
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
uint32_t graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::degree(
  const Vertex& vertex) const {
    return _impl->degree(_translation.at(vertex));
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
std::list<Vertex> graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::neighbors(
  const Vertex& start) const {
    std::list<uint32_t> representation = _impl->neighbors(_translation.at(start));
    std::list<Vertex> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
                   [this](uint32_t& x) { return _reverse_translation[x]; });
    return result;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
std::list<std::pair<Vertex, EdgeWeight>>
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::edges(const Vertex& src) const {
    std::list<std::pair<uint32_t, EdgeWeight>> representation = _impl->edges(_translation.at(src));
    std::list<std::pair<Vertex, EdgeWeight>> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
                   [this](std::pair<uint32_t, EdgeWeight>& edge) {
                       return std::make_pair(_reverse_translation[edge.first], edge.second);
                   });
    return result;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
graph_type graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::get_type() const {
    if (dynamic_cast<const adjacency_matrix<Directed, Weighted, EdgeWeight>*>(_impl.get()))
        return adj_matrix;
    return adj_list;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::convert(graph_type type) const {
    graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual> result(type);
    result._translation = this->_translation;
    result._reverse_translation = this->_reverse_translation;
    result._set_type(type, this->_impl.get());

    return result;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
std::vector<Vertex>
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::vertices() const {
    return _reverse_translation;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
template<typename InputIterator, typename _Requires>
graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>
  graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::generate_induced_subgraph(
    InputIterator first, InputIterator last) const {
    // get this in sorted order for impl
    std::vector<bool> selected(_reverse_translation.size(), false);
    std::for_each(first, last, [this, &selected](const Vertex& vertex) {
        selected[_translation.at(vertex)] = true;
    });

    std::list<uint32_t> vertex_subset;
    for (uint32_t i = 0; i < _reverse_translation.size(); ++i)
        if (selected[i])
            vertex_subset.push_back(i);
    graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual> result(_type);
    std::pair<impl<Directed, Weighted, EdgeWeight>*, std::vector<uint32_t>> output =
      _impl->induced_subgraph(vertex_subset);

    // make sure the vertices line up with original in the translations
    result._impl.reset(output.first);
    result._reverse_translation.resize(vertex_subset.size());
    for (uint32_t i = 0; i < _reverse_translation.size(); ++i)
        if (selected[i]) {
            result._translation[_reverse_translation[i]] = output.second[i];
            result._reverse_translation[output.second[i]] = _reverse_translation[i];
        }
    return result;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::set_edge(
  const Vertex& start, const Vertex& dest, const EdgeWeight& cost) {
    _check_self_loop(start, dest);

    _impl->set_edge(_translation.at(start), _translation.at(dest), Weighted ? cost : EdgeWeight());
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::force_add(
  const Vertex& start, const Vertex& dest, const EdgeWeight& cost) {
    _check_self_loop(start, dest);

    switch (_type) {
    case adj_matrix:
        _impl->set_edge(_translation.at(start), _translation.at(dest),
                        Weighted ? cost : EdgeWeight());
        break;

    case adj_list:
        dynamic_cast<adjacency_list<Directed, Weighted, EdgeWeight>*>(_impl.get())
          ->force_add(_translation.at(start), _translation.at(dest),
                      Weighted ? cost : EdgeWeight());
        break;

    default:
        break;
    }
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::sanitize() {
    if (_type == adj_list)
        dynamic_cast<adjacency_list<Directed, Weighted, EdgeWeight>*>(_impl.get())->sanitize();
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
uint32_t
  graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::add_vertex(const Vertex& name) {
    if (_translation.find(name) != _translation.end())
        throw std::invalid_argument("Already exists in graph");

    std::unordered_map<Vertex, uint32_t, Hash, KeyEqual> tempTranslate(_translation);
    std::vector<Vertex> tempReverse(_reverse_translation);
    tempTranslate[name] = tempReverse.size();
    tempReverse.push_back(name);
    uint32_t result = _impl->add_vertex();
    _translation = std::move(tempTranslate);
    _reverse_translation = std::move(tempReverse);
    return result;
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::remove_edge(const Vertex& start,
                                                                              const Vertex& dest) {
    _impl->remove_edge(_translation.at(start), _translation.at(dest));
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::isolate(const Vertex& start) {
    _impl->isolate(_translation.at(start));
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::remove(const Vertex& to_remove) {
    if (!has_vertex(to_remove))
        return;

    std::unordered_map<Vertex, uint32_t, Hash, KeyEqual> temp_translate(_translation);
    std::vector<Vertex> temp_reverse(_reverse_translation);

    // impl removes vertex by swapping it with the back; we need to update our maps
    temp_translate[_reverse_translation.back()] = std::move(_translation[to_remove]);
    temp_reverse[_translation[to_remove]] = std::move(temp_reverse.back());
    temp_translate.erase(to_remove);
    temp_reverse.pop_back();

    _impl->remove(_translation[to_remove]);

    _translation = std::move(temp_translate);
    _reverse_translation = std::move(temp_reverse);
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeWeight, Hash, KeyEqual>::clear() noexcept {
    _impl->clear();
    _translation.clear();
    _reverse_translation.clear();
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::_set_type(
  graph_type type, const impl<Directed, Weighted, EdgeType>* src) {
    if (src == this->_impl.get()) {
        if (type != this->_type) {
            *this = convert(type);
        }
    } else {
        _set_empty(type);
        this->_impl->copy_from(*src);
    }
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::_set_empty(graph_type type) {
    switch (type) {
    case adj_matrix:
        _impl.reset(new adjacency_matrix<Directed, Weighted, EdgeType>());
        break;

    case adj_list:
        _impl.reset(new adjacency_list<Directed, Weighted, EdgeType>());
        break;

    default:
        break;
    }
}

template<typename Vertex, bool Directed, bool Weighted, typename EdgeType, typename Hash,
         typename KeyEqual>
void graph<Vertex, Directed, Weighted, EdgeType, Hash, KeyEqual>::_check_self_loop(
  const Vertex& u, const Vertex& v) {
    if (_translation.key_eq()(u, v))
        throw std::invalid_argument("Self-loops not allowed");
}
} // namespace graph

#endif // GRAPH_CPP
