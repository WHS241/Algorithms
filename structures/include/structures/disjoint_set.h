#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>

/**
 * Also known as UNION-FIND
 *
 * Design:
 * Bernard Galler, Michael Fischer
 * An improved equivalence algorithm
 * (1964) doi:10.1145/364099.364331
 *
 * Analysis:
 * Robert Tarjan
 * Efficiency of a good but not linear set union algorithm
 * (1975) doi:10.1145/321879.321884
 */
template<typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
class disjoint_set {
    public:
    disjoint_set() = default;

    template<typename It> disjoint_set(It first, It last);

    // Inserts a new element under its own set
    // O(1)
    void insert(const T& item);

    // Union the sets that the parameter elements belong to
    // O(α(n))
    void union_sets(const T& first, const T& second);

    // Return the root of the set/tree
    // O(α(n))
    T find(const T& item);

    // Remove all elements in the same set as the parameter
    void remove_set(const T& member);

    // Empty the structure
    void clear() noexcept;

    uint32_t size() const noexcept;
    uint32_t num_sets() const noexcept;

    private:
    struct _t_data {
        T parent;
        uint32_t size;
    };
    std::unordered_map<T, _t_data, Hash, KeyEqual> _sets;
    std::unordered_set<T, Hash, KeyEqual> _roots;
};

#include "../../src/structures/disjoint_set.tpp"

#endif // UNION_FIND_H
