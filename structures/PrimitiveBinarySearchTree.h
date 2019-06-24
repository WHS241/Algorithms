#ifndef PRIMITIVE_BINARY_SEARCH_TREE_H
#define PRIMITIVE_BINARY_SEARCH_TREE_H

#include "BinarySearchTree.h"

// A basic Binary Search Tree, with no self-balancing techniques
// Operations may be ��(n)
template<typename T, typename Compare = std::less<>>
class PrimitiveBinarySearchTree : virtual public BinarySearchTree<T, Compare> {
public :
    PrimitiveBinarySearchTree() = default;

    PrimitiveBinarySearchTree(std::vector<T> elements, Compare comp = Compare()) : BinarySearchTree<T, Compare>(elements, comp) {}

    void insert(const T&) override;
    void remove(typename BinaryTree<T>::iterator it) override;
};

#include "src/PrimitiveBinarySearchTree.cpp"

#endif // PRIMITIVE_BINARY_SEARCH_TREE_H