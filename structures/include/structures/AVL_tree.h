#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "binary_search_tree.h"

namespace tree {
/**
 *  AVL tree implementation
 * For any node, difference in height between subtrees <= 1
 *
 * Georgy Adelson-Velsky, Evgenii Landis
 * An algorithm for the organization of information
 * (1962) Soviet Doklady
 */
template <typename T, typename Compare = std::less<T>>
class AVL_tree : public binary_search_tree<T, Compare> {
public:
    using typename binary_search_tree<T, Compare>::value_type,
        typename binary_search_tree<T, Compare>::value_compare,
        typename binary_search_tree<T, Compare>::iterator,
        typename binary_search_tree<T, Compare>::const_iterator,
        typename binary_search_tree<T, Compare>::reverse_iterator,
        typename binary_search_tree<T, Compare>::const_reverse_iterator,
        binary_search_tree<T, Compare>::binary_search_tree,

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
    struct t_node : virtual public binary_tree<T>::node {
        t_node(const T& item = T(), t_node* parent = nullptr, t_node* left = nullptr,
            t_node* right = nullptr);

        virtual t_node* change_left(t_node* toAdd) noexcept;
        virtual t_node* change_right(t_node* toAdd) noexcept;

        virtual void replace_left(t_node* toAdd) noexcept;
        virtual void replace_right(t_node* toAdd) noexcept;

        uint32_t left_height;
        uint32_t right_height;
    };

    void _balance_tree(t_node* start) noexcept;
};
}

#include "../../src/structures/AVL_tree.tpp"

#endif // !AVL_TREE_H
