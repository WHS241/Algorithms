#ifndef PRIMITIVE_BINARY_SEARCH_TREE_H
#define PRIMITIVE_BINARY_SEARCH_TREE_H

#include "BinarySearchTree.h"

// A basic Binary Search Tree, with no self-balancing techniques
// Operations may be O(n)
template<typename T, typename Compare = std::less<>>
class PrimitiveBinarySearchTree : virtual public BinarySearchTree<T, Compare> {
public :
    PrimitiveBinarySearchTree() = default;

    template<typename It>
    PrimitiveBinarySearchTree(It first, It last, Compare comp = Compare()) : BinarySearchTree<T, Compare>(first, last, comp) {}

    void insert(const T&) override;
    virtual void remove(typename BinaryTree<T>::iterator it);
};

#include "src/PrimitiveBinarySearchTree.cpp"

#endif // PRIMITIVE_BINARY_SEARCH_TREE_H