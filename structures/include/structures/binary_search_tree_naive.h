#ifndef PRIMITIVE_BINARY_SEARCH_TREE_H
#define PRIMITIVE_BINARY_SEARCH_TREE_H

#include "binary_search_tree.h"

namespace tree {
// A basic Binary Search Tree, with no self-balancing techniques
// Operations may be Ω(n)
template <typename T, typename Compare = std::less<T>>
class basic_binary_search_tree : public binary_search_tree<T, Compare> {
public:
    basic_binary_search_tree() = default;

    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    basic_binary_search_tree(It first, It last);
    template <typename It> basic_binary_search_tree(It first, It last, Compare comp);

    void insert(const T&) override;
    virtual void remove(typename binary_tree<T>::iterator it);
};
}

#include "src/structures/binary_search_tree_naive.tpp"

#endif // PRIMITIVE_BINARY_SEARCH_TREE_H