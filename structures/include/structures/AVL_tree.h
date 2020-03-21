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
    AVL_tree() = default;
    explicit AVL_tree(Compare comp);

    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    AVL_tree(It first, It last);
    template <typename It> AVL_tree(It first, It last, Compare comp);

    // Θ(log n)
    void insert(const T&) override;

    // Θ(log n)
    virtual void remove(typename binary_tree<T>::iterator it);

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

    static t_node* generate(const std::vector<T>&, uint32_t, uint32_t, t_node*);

    void balance_tree(t_node* start) noexcept;
};
}

#include "src/structures/AVL_tree.cpp"

#endif // !AVL_TREE_H
