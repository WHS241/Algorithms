#ifndef B_TREE_H
#define B_TREE_H

#include <array>
#include <functional>
#include <memory>

namespace tree {
/*
 * B-tree
 * 1. Each node contains between k - 1 and 2k - 1 values, inclusive (except root)
 * 2. If a non-leaf node contains m values, it must have m + 1 children
 * 3. All leaf nodes are at the same depth
 *
 * Rudolf Bayer, Edward M. McCreight
 * Organization and maintenance of large random-access files
 * (1970) doi:10.1145/1734663.1734671
 */
template<typename T, std::size_t K, typename Compare = std::less<T>> class B_tree {
    static_assert(K >= 2, "Need minimum degree of at least 2");

    private:
    struct node {
        std::array<T, 2 * K - 1> _vals;
        std::array<node*, 2 * K> _children;
        std::size_t _size;

        node();
        ~node() noexcept;
        node(const node&) = delete;
        node& operator=(const node&) = delete;
        node(node&&) = default;
        node& operator=(node&&);
        node* clone() const;
    };

    std::size_t _size;
    std::unique_ptr<node> _root;
    Compare _comp;
    bool _dups;

    T& find_successor(node* top, bool left);

    public:
    typedef T value_type;
    typedef Compare value_compare;
    typedef std::size_t size_type;

    // TODO: Implement actual iterators and integrate into insert/erase methods
    typedef const T* iterator;
    typedef const T* const_iterator;

    // initialize empty B-tree
    template<typename _C = Compare,
             typename = std::enable_if_t<std::is_default_constructible_v<_C>>>
    B_tree(bool duplicates = false);
    B_tree(const Compare& comp, bool duplicates = false);

    // populate with initial data
    template<typename It, typename _Compare = Compare,
             typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    B_tree(It first, It last, bool duplicates = false);
    template<typename It> B_tree(It first, It last, const Compare& comp, bool duplicates = false);

    B_tree(const B_tree&);
    B_tree& operator=(const B_tree&);
    B_tree(B_tree&&) = default;
    B_tree& operator=(B_tree&&) = default;

    bool empty() const noexcept;
    size_type size() const noexcept;
    void clear() noexcept;

    std::pair<iterator, bool> insert(const T&);
    std::pair<iterator, bool> insert(T&&);
    iterator erase(const T&);
    iterator find(const T&) const noexcept;
    bool contains(const T&) const noexcept;
};
} // namespace tree

#include "../../src/structures/B_tree.tpp"
#endif // B_TREE_H
