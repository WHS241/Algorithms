#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <algorithm>
#include <functional>
#include <unordered_map>

template<typename T>
class UnionFind {
public :
    UnionFind();

    template<typename It>
    UnionFind(It first, It last)
        : sets() {
        std::for_each(first, last, [this](const T& item) { insert(item); });
    };

    void insert(const T& item);
    void setUnion(const T& first, const T& second);
    T find(const T& item);
    void removeSet(const T& member);
    void clear() noexcept;

    uint32_t size() const noexcept;

private :
    struct Data {
        T parent;
        uint32_t size;
    };
    std::unordered_map<T, Data> sets;
};

#include "src/UnionFind.cpp"

#endif // !UNION_FIND_H

