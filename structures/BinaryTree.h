#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <functional>

#include "BinaryTreeNode.h"

template<typename T> class TreeIterator;
template<typename T> class TreeConstIterator;
enum Traversal;

template<typename T>
class BinaryTree {
public:
    typedef TreeIterator<T> iterator;
    typedef TreeConstIterator<T> const_iterator;

    BinaryTree();

    virtual ~BinaryTree() = default;

    iterator begin(Traversal);
    iterator end();
    const_iterator begin(Traversal) const;
    const_iterator end() const;

    uint32_t size() const;

    virtual void insert(const T&) = 0;
    virtual void remove(const T&) = 0;
    virtual void remove(iterator it) = 0;

private:
    std::unique_ptr<BinaryNode<T>> root;
    uint32_t _size;
};

#include "src/BinaryTree.cpp"

#endif // BINARY_TREE_H
