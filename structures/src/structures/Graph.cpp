#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <stdexcept>

#include <structures/AdjacencyList.h>
#include <structures/AdjacencyMatrix.h>

template <typename T>
Graph<T>::Graph(bool directed, bool weighted, GraphType type)
    : type(type)
    , translation()
    , reverse_translation()
{
    setEmpty(type, directed, weighted);
}

template <typename T>
Graph<T>::Graph(const Graph<T>& src)
    : type(src.type)
    , translation(src.translation)
    , reverse_translation(src.reverse_translation)
{
    setType(type, src.impl.get());
}

template <typename T> const Graph<T>& Graph<T>::operator=(const Graph<T>& rhs)
{
    if (this != &rhs) {
        Graph<T> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T>
Graph<T>::Graph(Graph<T>&& src)
    : type()
    , impl()
    , translation()
    , reverse_translation()
{
    *this = std::move(src);
}

template <typename T> const Graph<T>& Graph<T>::operator=(Graph<T>&& rhs)
{
    if (this != &rhs) {
        impl.reset();
        std::swap(type, rhs.type);
        std::swap(impl, rhs.impl);
        translation = std::move(rhs.translation);
        reverse_translation = std::move(rhs.reverse_translation);
    }
    return *this;
}

template <typename T> uint32_t Graph<T>::order() const noexcept { return impl->order(); }

template <typename T> bool Graph<T>::hasEdge(const T& start, const T& dest) const noexcept
{
    if (translation.find(start) == translation.end() || translation.find(dest) == translation.end())
        return false;

    return impl->hasEdge(translation.at(start), translation.at(dest));
}

template <typename T> double Graph<T>::edgeCost(const T& start, const T& dest) const
{
    return impl->edgeCost(translation.at(start), translation.at(dest));
}

template <typename T> uint32_t Graph<T>::degree(const T& vertex) const
{
    return impl->degree(translation.at(vertex));
}

template <typename T> std::list<T> Graph<T>::neighbors(const T& start) const
{
    auto representation = impl->neighbors(translation.at(start));
    std::list<T> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
        [this](uint32_t& x) { return reverse_translation[x]; });
    return result;
}

template <typename T> std::list<std::pair<T, double>> Graph<T>::edges(const T& src) const
{
    auto representation = impl->edges(src);
    std::list<std::pair<T, double>> result;
    std::transform(representation.begin(), representation.end(), std::back_inserter(result),
        [this](std::pair<uint32_t, double>& edge) {
            return std::make_pair(reverse_translation[edge.first], edge.second);
        });
    return result;
}

template <typename T> bool Graph<T>::directed() const noexcept { return impl->directed(); }

template <typename T> bool Graph<T>::weighted() const noexcept { return impl->weighted(); }

template <typename T> Graph<T> Graph<T>::convert(GraphType type) const
{
    Graph<T> result(type);
    result.translation = this->translation;
    result.reverse_translation = this->reverse_translation;
    result.setType(type, this->impl.get());

    return result;
}

template <typename T> std::vector<T> Graph<T>::vertices() const { return reverse_translation; }

template <typename T> void Graph<T>::setEdge(const T& start, const T& dest, double cost)
{
    if (start == dest)
        throw std::invalid_argument("Self-loops not allowed");
    impl->setEdge(translation.at(start), translation.at(dest), cost);
}

template <typename T> uint32_t Graph<T>::addVertex(const T& name)
{
    if (translation.find(name) != translation.end())
        throw std::invalid_argument("Already exists in graph");

    auto tempTranslate(translation);
    auto tempReverse(reverse_translation);
    tempTranslate[name] = tempReverse.size();
    tempReverse.push_back(name);
    uint32_t result = impl->addVertex();
    translation = std::move(tempTranslate);
    reverse_translation = std::move(tempReverse);
    return result;
}

template <typename T> void Graph<T>::removeEdge(const T& start, const T& dest)
{
    impl->removeEdge(translation.at(start), translation.at(dest));
}

template <typename T> void Graph<T>::isolate(const T& start)
{
    impl->isolate(translation.at(start));
}

template <typename T> void Graph<T>::remove(const T& toRemove)
{
    if (translation.find(toRemove) == translation.end())
        return;

    auto tempTranslate(translation);
    auto tempReverse(reverse_translation);
    tempTranslate[reverse_translation.back()] = translation[toRemove];
    tempReverse[translation[toRemove]] = tempReverse.back();
    tempTranslate.erase(toRemove);
    tempReverse.pop_back();
    impl->remove(translation[toRemove]);
    translation = std::move(tempTranslate);
    reverse_translation = std::move(tempReverse);
}

template <typename T> void Graph<T>::clear() noexcept
{
    impl->clear();
    translation.clear();
    reverse_translation.clear();
}

template <typename T> void Graph<T>::setType(GraphType type, const GraphImpl* const src)
{
    if (src == this->impl.get()) {
        if (type != this->type) {
            *this = convert(type);
        }
    } else {
        setEmpty(type, src->directed(), src->weighted());
        this->impl->copyFrom(*src);
    }
}

template <typename T> void Graph<T>::setEmpty(GraphType type, bool directed, bool weighted)
{
    switch (type) {
    case AdjMatrix:
        impl.reset(new AdjacencyMatrix(directed, weighted));
        break;

    case AdjList:
        impl.reset(new AdjacencyList(directed, weighted));
        break;

    default:
        break;
    }
}

#endif // GRAPH_CPP
