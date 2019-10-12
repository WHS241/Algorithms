#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "binary_tree_iterator.h"

namespace tree {
/*
Abstract base class for any binary tree structure
*/
template <typename T> class binary_tree {
public:
    struct node {
        node(const T& item = T(), node* parent = nullptr, node* left = nullptr,
            node* right = nullptr);

        virtual ~node() noexcept;
        node(const node&) = delete;
        virtual const node& operator=(const node&) = delete;
        node(node&&) noexcept;
        virtual const node& operator=(node&&) noexcept;

        virtual node* change_left(node* toAdd) noexcept;
        virtual node* change_right(node* toAdd) noexcept;
        virtual void replace_left(node* toAdd) noexcept;
        virtual void replace_right(node* toAdd) noexcept;
        virtual node* clone() const;

        T item;
        node* parent;
        node* left;
        node* right;
    };

    typedef tree_iterator<T> iterator;
    typedef tree_const_iterator<T> const_iterator;

    // Ctor; creates an empty Binary Tree
    binary_tree() noexcept;

    virtual ~binary_tree() noexcept = default;

    // Iterator classes. Parameter on begin() iterators determines traversal type
    iterator begin(traversal);
    iterator end();

    const_iterator begin(traversal) const;
    const_iterator end() const;

    // Returns the number of elements in the tree
    uint32_t size() const noexcept;

    // Insertion/deletion functions. remove(const T&) is transformed into
    // remove(iterator) in implementation
    virtual void insert(const T&) = 0;
    virtual void remove(const T&);
    virtual void remove(iterator it) = 0;

protected:
    static node* _get_node(iterator it);
    static node* _get_node(const_iterator it);

    // Returns a pointer to the node containing an element; nullptr if not found
    virtual node* _find(const T&) const = 0;

    std::unique_ptr<node> _root;
    uint32_t _size;

    friend class tree_iterator<T>;
    friend class tree_const_iterator<T>;
    friend class tree_iterator_impl<T>;
    friend class pre_order_iterator_impl<T>;
    friend class in_order_iterator_impl<T>;
    friend class post_order_iterator_impl<T>;
    friend class level_order_iterator_impl<T>;
};
}

#include "src/structures/binary_tree_base.cpp"
#include "src/structures/binary_tree_iterator.cpp"

#endif // BINARY_TREE_H
