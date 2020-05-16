#ifndef PRIMITIVE_BINARY_SEARCH_TREE_H
#define PRIMITIVE_BINARY_SEARCH_TREE_H

#include "binary_search_tree.h"

namespace tree {
// A basic Binary Search Tree, with no self-balancing techniques
// Operations may be Ω(n)
template <typename T, typename Compare = std::less<T>>
class basic_binary_search_tree : public binary_search_tree<T, Compare> {
public:
    typedef T value_type;
    typedef Compare value_compare;
    typedef typename binary_tree<T>::iterator iterator;
    typedef typename binary_tree<T>::const_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    // initialize empty BST
    template <typename _C = Compare,
        typename = std::enable_if_t<std::is_default_constructible_v<_C>>>
    explicit basic_binary_search_tree(bool duplicates = false);
    explicit basic_binary_search_tree(const Compare& comp, bool duplicates = false);

    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    basic_binary_search_tree(It first, It last, bool duplicates = false);
    template <typename It>
    basic_binary_search_tree(It first, It last, const Compare& comp, bool duplicates = false);

    std::pair<iterator, bool> insert(const T&) override;
    iterator erase(iterator it) override;
};
}

#include "src/structures/binary_search_tree_naive.tpp"

#endif // PRIMITIVE_BINARY_SEARCH_TREE_H