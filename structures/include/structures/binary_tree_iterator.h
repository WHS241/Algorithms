#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include "binary_tree_base.h"
#include <iterator>
#include <memory>

namespace tree {
template<typename T> class binary_tree;

enum traversal { pre_order, in_order, post_order, level_order };

template<typename T> class tree_iterator_impl;

// The iterator pattern for the binary_tree
template<typename T> class tree_iterator {
    public:
    using difference_type = int;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::bidirectional_iterator_tag;

    using const_pointer = const T*;
    using const_reference = const T&;

    tree_iterator() = default;
    tree_iterator(const tree_iterator<T>&);
    tree_iterator& operator=(const tree_iterator<T>&);
    tree_iterator(tree_iterator<T>&&) = default;
    tree_iterator& operator=(tree_iterator<T>&&) = default;

    bool operator==(const tree_iterator<T>&) const;
    bool operator!=(const tree_iterator<T>&) const;
    const_reference operator*() const;
    reference operator*();
    const_pointer operator->() const;
    pointer operator->();

    tree_iterator& operator++();
    tree_iterator operator++(int);
    tree_iterator& operator--();
    tree_iterator operator--(int);

    private:
    tree_iterator(typename binary_tree<T>::node* root, traversal order, bool entire_subtree,
                  binary_tree<T>* tree);
    typename binary_tree<T>::node* _get_node();

    std::unique_ptr<tree_iterator_impl<T>> _impl;
    traversal _trav;
    binary_tree<T>* _tree;
    friend class binary_tree<T>;
};

template<typename T> class tree_const_iterator {
    public:
    using difference_type = int;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    tree_const_iterator() = default;
    tree_const_iterator(const tree_const_iterator<T>&);
    tree_const_iterator& operator=(const tree_const_iterator<T>&);
    tree_const_iterator(tree_const_iterator<T>&&) = default;
    tree_const_iterator& operator=(tree_const_iterator<T>&&) = default;

    bool operator==(const tree_const_iterator<T>&) const;
    bool operator!=(const tree_const_iterator<T>&) const;
    reference operator*() const;
    pointer operator->() const;

    tree_const_iterator& operator++();
    tree_const_iterator operator++(int);
    tree_const_iterator& operator--();
    tree_const_iterator operator--(int);

    private:
    tree_const_iterator(const typename binary_tree<T>::node* root, traversal order,
                        bool entire_subtree, const binary_tree<T>* tree);
    const typename binary_tree<T>::node* _get_node();

    std::unique_ptr<tree_iterator_impl<T>> _impl;
    traversal _trav;
    const binary_tree<T>* _tree;
    friend class binary_tree<T>;
};
} // namespace tree

#include "../../src/structures/binary_tree_iterator.tpp"

#endif // TREE_ITERATOR_H
