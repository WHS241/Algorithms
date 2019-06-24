#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <structures/BinaryTree.h>
#include <functional>
#include <vector>

// A Binary Search Tree (BST) abstract class
template<typename T, typename Compare = std::less<>>
class BinarySearchTree : virtual public BinaryTree<T> {
public:
    // initialize empty BST
    BinarySearchTree() = default;

    // populate with data; [first, last) can be dereferenced to reach data. Need not be sorted.
    BinarySearchTree(std::vector<T>& elements, Compare comp = Compare())
        : BinaryTree<T>()
        , compare(comp)
    {
        std::sort(elements.begin(), elements.end(), comp);
        this->root.reset(generate(elements, 0, elements.size(), nullptr));
    }

    // searches the BST for an element. Returns true if element exists
    virtual bool contains(const T&) const noexcept;

protected:

    Compare compare;
    static typename BinaryTree<T>::BinaryNode* generate(const std::vector<T>&, uint32_t, uint32_t, typename BinaryTree<T>::BinaryNode*);
    virtual typename BinaryTree<T>::BinaryNode* find(const T&) const noexcept override;
};

#include "src/BinarySearchTree.cpp"

#endif //BINARY_SEARCH_TREE_H