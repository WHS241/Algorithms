#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include <memory>

enum Traversal {PreOrder, InOrder, PostOrder, LevelOrder};

template<typename T> class BinaryNode;
template<typename T> class TreeIteratorImpl;
template<typename T> class BinaryTree;

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

private :
    TreeIterator(BinaryNode<T>* root, Traversal order);

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

private :
    TreeConstIterator(BinaryNode<T>* root, Traversal order);

    std::shared_ptr<TreeIteratorImpl<T>> impl;

    friend class BinaryTree<T>;
};

#include "src/BinaryTreeIterator.cpp"

#endif // TREE_ITERATOR_H
