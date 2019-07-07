#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <functional>
#include <vector>

#include "BinaryTree.h"

// A Binary Search Tree (BST) abstract class
template<typename T, typename Compare = std::less<>>
class BinarySearchTree : public BinaryTree<T> {
public:
    // initialize empty BST
    BinarySearchTree() = default;

    // populate with data; [first, last) can be dereferenced to reach data. Need not be sorted.
    template<typename It>
    BinarySearchTree(It first, It last, Compare comp = Compare())
        : BinaryTree<T>()
        , compare(comp)
    {
        std::vector<T> elements(first, last);
        std::sort(elements.begin(), elements.end(), comp);
        this->root.reset(this->generate(elements, 0, elements.size(), nullptr));
        this->_size = elements.size();
    }

    // searches the BST for an element. Returns true if element exists
    virtual bool contains(const T&) const noexcept;

protected:
    Compare compare;
    static typename BinaryTree<T>::BinaryNode* generate(const std::vector<T>&, uint32_t, uint32_t, typename BinaryTree<T>::BinaryNode*);
    virtual typename BinaryTree<T>::BinaryNode* find(const T&) const noexcept override;
    void rotate(typename BinaryTree<T>::BinaryNode* upper, bool useLeftChild);
};

#include "src/BinarySearchTree.cpp"

#endif //BINARY_SEARCH_TREE_H