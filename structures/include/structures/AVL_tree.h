#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "binary_search_tree.h"

namespace tree {
// AVL tree implementation
// For any node, difference in height between subtrees <= 1
template <typename T, typename Compare = std::less<>>
class AVL_tree : public binary_search_tree<T, Compare> {
public:
    AVL_tree() = default;
    explicit AVL_tree(Compare comp)
        : binary_search_tree<T, Compare>(comp) {};

    template <typename It, typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
    AVL_tree(It first, It last)
        : AVL_tree(first, last, Compare()) {};
    template <typename It>
    AVL_tree(It first, It last, Compare comp = Compare())
        : binary_search_tree<T, Compare>(first, last, comp)
    {
        std::vector<T> elements(first, last);
        std::sort(elements.begin(), elements.end(), comp);
        this->_root.reset(generate(elements, 0, elements.size(), nullptr));
        this->_size = elements.size();
    }

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
