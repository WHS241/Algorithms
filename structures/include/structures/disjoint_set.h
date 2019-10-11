#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <algorithm>
#include <functional>
#include <unordered_map>

template <typename T> class disjoint_set {
public:
    disjoint_set();

    template <typename It>
    disjoint_set(It first, It last)
        : _sets()
    {
        std::for_each(first, last, [this](const T& item) { insert(item); });
    };

    // O(1)
    void insert(const T& item);

    // O(α(n))
    void union_(const T& first, const T& second);
    T find(const T& item);

    void remove_set(const T& member);
    void clear() noexcept;

    uint32_t size() const noexcept;

private:
    struct t_data {
        T parent;
        uint32_t size;
    };
    std::unordered_map<T, t_data> _sets;
};

#include "src/structures/disjoint_set.cpp"

#endif // !UNION_FIND_H
