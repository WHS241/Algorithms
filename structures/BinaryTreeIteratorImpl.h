#ifndef TREE_ITERATOR_IMPL_H
#define TREE_ITERATOR_IMPL_H

#include <list>

template<typename T> class BinaryNode;
template<typename T> class TreeIterator;

template<typename T>
class TreeIteratorImpl {
public:
    TreeIteratorImpl() = delete;
    TreeIteratorImpl(BinaryNode<T>* current);

    virtual bool operator==(const TreeIteratorImpl<T>&) const;
    virtual bool operator!=(const TreeIteratorImpl<T>&) const;

    virtual const T& operator*() const;
    virtual T& operator*();
    virtual const T* operator->() const;
    virtual T* operator->();

    virtual TreeIteratorImpl& operator++() = 0;

protected:
    BinaryNode<T>* current;
};

template<typename T>
class PreOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    PreOrderTreeIteratorImpl(BinaryNode<T>* current);

    PreOrderTreeIteratorImpl& operator++();

private:
    static BinaryNode<T>* preOrderStart(BinaryNode<T>* root);
};

template<typename T>
class InOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    InOrderTreeIteratorImpl(BinaryNode<T>* current);

    InOrderTreeIteratorImpl& operator++();

private:
    static BinaryNode<T>* leftmostDescendant(BinaryNode<T>* root);
};

template<typename T>
class PostOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    PostOrderTreeIteratorImpl(BinaryNode<T>* current);

    PostOrderTreeIteratorImpl& operator++();
};

template<typename T>
class LevelOrderTreeIteratorImpl : public TreeIteratorImpl<T> {
public:
    LevelOrderTreeIteratorImpl(BinaryNode<T>* current);

    LevelOrderTreeIteratorImpl& operator++();

private:
    std::list<BinaryNode<T>*> buffer;
    typename std::list<BinaryNode<T>*>::iterator pos;

    friend class TreeIterator<T>;
};

#include "src/BinaryTreeIteratorImpl.cpp"

#endif //TREE_ITERATOR_IMPL_H