#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "BinaryTreeIterator.h"

/*
Abstract base class for any binary tree structure
*/
template<typename T>
class BinaryTree {
public:
    typedef TreeIterator<T> iterator;
    typedef TreeConstIterator<T> const_iterator;

    // Ctor; creates an empty Binary Tree
    BinaryTree() noexcept;

    virtual ~BinaryTree() noexcept = default;

    // Iterator classes. Parameter on begin() iterators determines traversal type
    iterator begin(Traversal);
    iterator end();
    const_iterator begin(Traversal) const;
    const_iterator end() const;

    // Returns the number of elements in the tree
    uint32_t size() const noexcept;

    // Insertion/deletion functions. remove(const T&) is transformed into remove(iterator) in implementation
    virtual void insert(const T&) = 0;
    virtual void remove(const T&);
    virtual void remove(iterator it) = 0;

protected:

    struct BinaryNode {
        BinaryNode(const T& item = T(), BinaryNode* parent = nullptr, BinaryNode* left = nullptr, BinaryNode* right = nullptr)
            : _item(item)
            , _parent(parent)
            , _left(left)
            , _right(right)
        {
            if (left)
                left->_parent = this;
            if (right)
                right->_parent = this;
        };

        void addLeft(BinaryNode* toAdd) noexcept;
        void addRight(BinaryNode* toAdd) noexcept;

        T _item;

        BinaryNode* _parent;
        std::unique_ptr<BinaryNode> _left;
        std::unique_ptr<BinaryNode> _right;
    };

    // Returns a pointer to the node containing an element; nullptr if not found
    virtual BinaryNode* find(const T&) const = 0;

    std::unique_ptr<BinaryNode> root;
    uint32_t _size;

    friend class TreeIterator<T>;
    friend class TreeConstIterator<T>;
    friend class TreeIteratorImpl<T>;
    friend class PreOrderTreeIteratorImpl<T>;
    friend class InOrderTreeIteratorImpl<T>;
    friend class PostOrderTreeIteratorImpl<T>;
    friend class LevelOrderTreeIteratorImpl<T>;
};

#include "src/BinaryTree.cpp"

#endif // BINARY_TREE_H
