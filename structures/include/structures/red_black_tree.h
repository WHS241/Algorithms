#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "binary_search_tree.h"

namespace tree {
/**
 * Red black trees
 * Leonidas Guibas, Robert Sedgewick
 * A Dichromatic Framework for Balanced Trees
 * (1978) doi:10.1109/SFCS.1978.3
 */
template <typename T, typename Compare = std::less<T>>
class red_black_tree : public binary_search_tree<T, Compare> {
public:
    red_black_tree() = default;
    explicit red_black_tree(Compare comp);

    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    red_black_tree(It first, It last);
    template <typename It> red_black_tree(It first, It last, Compare comp);

    // Θ(log n)
    void insert(const T&) override;

    // Θ(log n)
    virtual void remove(typename binary_tree<T>::iterator it);

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
