#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "binary_tree_iterator.h"

namespace tree {
/*
Abstract base class for any binary tree structure
*/
template<typename T> class binary_tree {
    public:
    typedef tree_iterator<T> iterator;
    typedef tree_const_iterator<T> const_iterator;

    // Ctor; creates an empty Binary Tree
    binary_tree() noexcept;

    virtual ~binary_tree() noexcept = default;
    binary_tree(const binary_tree<T>&);
    binary_tree& operator=(const binary_tree<T>&);
    binary_tree(binary_tree<T>&&) = default;
    binary_tree& operator=(binary_tree<T>&&) = default;

    // Iterator classes. Parameter on begin() iterators determines traversal type
    iterator begin(traversal);
    iterator end(traversal);

    const_iterator cbegin(traversal) const;
    const_iterator cend(traversal) const;

    // Checks whether the tree is empty
    virtual bool empty() const noexcept;

    // Returns the number of elements in the tree
    virtual uint32_t size() const noexcept;

    // Clear the tree
    virtual void clear() noexcept;

    // Insertion function
    // Success: returns iterator to item and true
    // Fail: returns iterator to blocking item and false
    virtual std::pair<iterator, bool> insert(const T&) = 0;

    // Removal function: removes item if it exists in tree
    // Returns number of elements removed (0 or 1)
    virtual uint32_t erase(const T&);

    // Removal function: removes item at given iterator
    // Returns the iterator to the element immediately afterwards
    // Undefined behavior if iterator points outside the current structure
    virtual iterator erase(iterator it) = 0;
    virtual iterator erase(const_iterator it);

    protected:
    struct node {
        node(const T& item = T(), node* parent = nullptr, node* left = nullptr,
             node* right = nullptr);

        virtual ~node() noexcept;
        node(const node&) = delete;
        virtual const node& operator=(const node&) = delete;
        node(node&&) noexcept = default;
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

    static node* _s_get_node(iterator it);
    static const node* _s_get_node(const_iterator it);

    iterator _make_iterator(node*, traversal, bool);
    const_iterator _make_const_iterator(const node*, traversal, bool) const;

    // Returns a pointer to the node containing an element; nullptr if not found
    virtual const node* _find(const T&) const = 0;

    std::unique_ptr<node> _root;
    uint32_t _size;

    friend class tree_iterator<T>;
    friend class tree_const_iterator<T>;
    friend class tree_iterator_impl<T>;
    friend class pre_order_iterator_impl<T>;
    friend class in_order_iterator_impl<T>;
    friend class post_order_iterator_impl<T>;
    friend class level_order_iterator_impl<T>;

    void _verify(iterator check) const;
    void _verify(const_iterator check) const;
};
} // namespace tree

#include "../../src/structures/binary_tree_base.tpp"

#endif // BINARY_TREE_H
