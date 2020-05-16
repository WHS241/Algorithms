#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "binary_search_tree.h"

namespace tree {
/**
 * Red-black tree
 * 1. Each node is marked as red or black
 * 2. nullptr is black
 * 3. Red nodes can only have black children
 * 4. A path from root to any nullptr contains the same number of black nodes
 *
 * Leonidas Guibas, Robert Sedgewick
 * A Dichromatic Framework for Balanced Trees
 * (1978) doi:10.1109/SFCS.1978.3
 */
template <typename T, typename Compare = std::less<T>>
class red_black_tree : public binary_search_tree<T, Compare> {
public:
    using typename binary_search_tree<T, Compare>::value_type,
        typename binary_search_tree<T, Compare>::value_compare,
        typename binary_search_tree<T, Compare>::iterator,
        typename binary_search_tree<T, Compare>::const_iterator,
        typename binary_search_tree<T, Compare>::reverse_iterator,
        typename binary_search_tree<T, Compare>::const_reverse_iterator;
    using binary_search_tree<T, Compare>::binary_search_tree,

        binary_search_tree<T, Compare>::begin, binary_search_tree<T, Compare>::end,
        binary_search_tree<T, Compare>::cbegin, binary_search_tree<T, Compare>::cend,
        binary_search_tree<T, Compare>::rbegin, binary_search_tree<T, Compare>::rend,
        binary_search_tree<T, Compare>::crbegin, binary_search_tree<T, Compare>::crend,

        binary_search_tree<T, Compare>::empty, binary_search_tree<T, Compare>::size,
        binary_search_tree<T, Compare>::contains, binary_search_tree<T, Compare>::find,
        binary_search_tree<T, Compare>::erase;

    // Θ(log n)
    std::pair<iterator, bool> insert(const T&) override;

    // Θ(log n)
    iterator erase(iterator it) override;

protected:
    struct t_node : public binary_tree<T>::node {
        t_node(const T& item = T(), bool black = false, t_node* parent = nullptr,
            t_node* left = nullptr, t_node* right = nullptr);

        virtual t_node* change_left(t_node* to_add) noexcept;
        virtual t_node* change_right(t_node* to_add) noexcept;
        virtual void replace_left(t_node* to_add) noexcept;
        virtual void replace_right(t_node* to_add) noexcept;

        bool is_black;
    };
};
}

#include <src/structures/red_black_tree.tpp>

#endif // RED_BLACK_TREE_H
