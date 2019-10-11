#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include <iterator>
#include <memory>
#include "binary_tree_base.h"

namespace tree {
    template<typename T>
    class binary_tree;

    enum traversal {
        PreOrder, InOrder, PostOrder, LevelOrder
    };

    template<typename T>
    class tree_iterator_impl;

// The iterator pattern for the binary_tree
    template<typename T>
    class tree_iterator {
    public:
        using difference_type = int;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        tree_iterator() = delete;
        tree_iterator(const tree_iterator<T> &);
        bool operator==(const tree_iterator<T> &) const;
        bool operator!=(const tree_iterator<T> &) const;
        const T &operator*() const;
        T &operator*();
        const T *operator->() const;
        T *operator->();

        tree_iterator &operator++();
        tree_iterator operator++(int);
        tree_iterator &operator--();
        tree_iterator operator--(int);

    private:
        tree_iterator(typename binary_tree<T>::node *root, traversal order);
        typename binary_tree<T>::node *_get_node();
        
        std::unique_ptr<tree_iterator_impl<T>> _impl;
        friend class binary_tree<T>;
    };

    template<typename T>
    class tree_const_iterator {
    public:
        using difference_type = int;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::bidirectional_iterator_tag;

        tree_const_iterator() = delete;
        tree_const_iterator(const tree_const_iterator<T> &);

        bool operator==(const tree_const_iterator<T> &) const;
        bool operator!=(const tree_const_iterator<T> &) const;
        const T &operator*() const;
        const T *operator->() const;
        
        tree_const_iterator &operator++();
        tree_const_iterator operator++(int);
        tree_const_iterator &operator--();
        tree_const_iterator operator--(int);

    private:
        tree_const_iterator(typename binary_tree<T>::node *root, traversal order);
        typename binary_tree<T>::node *_get_node();

        std::shared_ptr<tree_iterator_impl<T>> _impl;
        friend class binary_tree<T>;
    };
}

#include "src/structures/binary_tree_iterator.cpp"

#endif // TREE_ITERATOR_H
