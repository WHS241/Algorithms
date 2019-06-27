#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

template<typename T> class BinaryTree;

#include <memory>

enum Traversal {PreOrder, InOrder, PostOrder, LevelOrder};

template<typename T> class TreeIteratorImpl;

// The iterator pattern for the BinaryTree
template<typename T>
class TreeIterator {
public :
    TreeIterator() = delete;
    TreeIterator(const TreeIterator<T>&);

    bool operator==(const TreeIterator<T>&) const;
    bool operator!=(const TreeIterator<T>&) const;

    const T& operator*() const;
    T& operator*();
    const T* operator->() const;
    T* operator->();
    TreeIterator& operator++();
    TreeIterator operator++(int);
    TreeIterator& operator--();
    TreeIterator operator--(int);

private :
    TreeIterator(typename BinaryTree<T>::BinaryNode* root, Traversal order);

    typename BinaryTree<T>::BinaryNode* getNode();

    std::unique_ptr<TreeIteratorImpl<T>> impl;

    friend class BinaryTree<T>;
};

template<typename T>
class TreeConstIterator {
public :
    TreeConstIterator() = delete;
    TreeConstIterator(const TreeConstIterator<T>&);

    bool operator==(const TreeConstIterator<T>&) const;
    bool operator!=(const TreeConstIterator<T>&) const;

    const T& operator*() const;
    const T* operator->() const;
    TreeConstIterator& operator++();
    TreeConstIterator operator++(int);
    TreeConstIterator& operator--();
    TreeConstIterator operator--(int);

private :
    TreeConstIterator(typename BinaryTree<T>::BinaryNode* root, Traversal order);

    typename BinaryTree<T>::BinaryNode* getNode();

    std::shared_ptr<TreeIteratorImpl<T>> impl;

    friend class BinaryTree<T>;
};

#include "src/BinaryTreeIterator.cpp"

#endif // TREE_ITERATOR_H
