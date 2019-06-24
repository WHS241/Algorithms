#ifndef TREE_ITERATOR_IMPL_H
#define TREE_ITERATOR_IMPL_H

#include <list>
#include "BinaryTree.h"

template<typename T> class TreeIterator;

// The implementation for the Binary Tree iterator
template<typename T>
class TreeIteratorImpl {
public:
    TreeIteratorImpl() = delete;
    TreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current);

    virtual bool operator==(const TreeIteratorImpl<T>&) const noexcept;
    virtual bool operator!=(const TreeIteratorImpl<T>&) const noexcept;

    virtual const T& operator*() const noexcept;
    virtual T& operator*() noexcept;
    virtual const T* operator->() const noexcept;
    virtual T* operator->() noexcept;

    virtual TreeIteratorImpl& operator++() noexcept = 0;

protected:
    typename BinaryTree<T>::BinaryNode* current;
};

template<typename T>
class PreOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    PreOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current);

    PreOrderTreeIteratorImpl& operator++() noexcept;

private:
    static typename BinaryTree<T>::BinaryNode* preOrderStart(typename BinaryTree<T>::BinaryNode* root) noexcept;
};

template<typename T>
class InOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    InOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current);

    InOrderTreeIteratorImpl& operator++() noexcept;

private:
    static typename BinaryTree<T>::BinaryNode* leftmostDescendant(typename BinaryTree<T>::BinaryNode* root) noexcept;
};

template<typename T>
class PostOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    PostOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current);

    PostOrderTreeIteratorImpl& operator++() noexcept;
};

template<typename T>
class LevelOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    LevelOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current);

    LevelOrderTreeIteratorImpl& operator++() noexcept;

private:
    std::list<typename BinaryTree<T>::BinaryNode*> buffer;
    typename std::list<typename BinaryTree<T>::BinaryNode*>::iterator pos;

    friend class TreeIterator<T>;
};

#include "src/BinaryTreeIteratorImpl.cpp"

#endif //TREE_ITERATOR_IMPL_H