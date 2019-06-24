#ifndef TREE_ITERATOR_CPP
#define TREE_ITERATOR_CPP

#include <structures/BinaryTreeIteratorImpl.h>

template<typename T>
TreeIterator<T>::TreeIterator(typename BinaryTree<T>::BinaryNode* root, Traversal order)
{
    switch (order) {
    case PreOrder:
        impl.reset(new PreOrderTreeIteratorImpl<T>(root));
        break;

    case InOrder:
        impl.reset(new InOrderTreeIteratorImpl<T>(root));
        break;

    case PostOrder:
        impl.reset(new PostOrderTreeIteratorImpl<T>(root));
        break;

    case LevelOrder:
        impl.reset(new LevelOrderTreeIteratorImpl<T>(root));

    default:
        break;
    }
}

template<typename T>
bool TreeIterator<T>::operator==(const TreeIterator<T>& rhs) const {
    return *impl == *rhs.impl;
}

template<typename T>
bool TreeIterator<T>::operator!=(const TreeIterator<T>& rhs) const {
    return !operator==(rhs);
}

template<typename T>
TreeIterator<T>::TreeIterator(const TreeIterator<T>& src)
{
    TreeIteratorImpl<T>* ptr = src.impl.get();
    auto prePtr = dynamic_cast<PreOrderTreeIteratorImpl<T>*>(ptr);
    if (prePtr) {
        impl.reset(new PreOrderTreeIteratorImpl<T>(*prePtr));
        return;
    }
    auto inPtr = dynamic_cast<InOrderTreeIteratorImpl<T>*>(ptr);
    if (inPtr) {
        impl.reset(new InOrderTreeIteratorImpl<T>(*inPtr));
        return;
    }
    auto postPtr = dynamic_cast<PostOrderTreeIteratorImpl<T>*>(ptr);
    if (postPtr) {
        impl.reset(new PostOrderTreeIteratorImpl<T>(*postPtr));
        return;
    }
    auto levelPtr = dynamic_cast<LevelOrderTreeIteratorImpl<T>*>(ptr);
    impl.reset(new LevelOrderTreeIteratorImpl<T>(*levelPtr));
}

template<typename T>
const T& TreeIterator<T>::operator*() const {
    return **impl;
}

template<typename T>
T& TreeIterator<T>::operator*() {
    return **impl;
}

template<typename T>
const T* TreeIterator<T>::operator->() const {
    return &operator*();
}

template<typename T>
T* TreeIterator<T>::operator->() {
    return &operator*();
}

template<typename T>
TreeIterator<T>& TreeIterator<T>::operator++() {
    ++(*impl);
    return *this;
}

template<typename T>
TreeIterator<T> TreeIterator<T>::operator++(int) {
    TreeIterator<T> temp(*this);
    ++(*this);
    return temp;
}

template<typename T>
TreeConstIterator<T>::TreeConstIterator(typename BinaryTree<T>::BinaryNode* root, Traversal order) {
    switch (order) {
    case PreOrder:
        impl.reset(new PreOrderTreeIteratorImpl<T>(root));
        break;

    case InOrder:
        impl.reset(new PreOrderTreeIteratorImpl<T>(root));
        break;

    case PostOrder:
        impl.reset(new PreOrderTreeIteratorImpl<T>(root));
        break;

    case LevelOrder:
        impl.reset(new PreOrderTreeIteratorImpl<T>(root));

    default:
        break;
    }
}

template<typename T>
TreeConstIterator<T>::TreeConstIterator(const TreeConstIterator<T>& src)
{
    TreeIteratorImpl<T>* ptr = src.impl.get();
    auto prePtr = dynamic_cast<PreOrderTreeIteratorImpl<T>*>(ptr);
    if (prePtr) {
        impl.reset(new PreOrderTreeIteratorImpl<T>(*prePtr));
        return;
    }
    auto inPtr = dynamic_cast<InOrderTreeIteratorImpl<T>*>(ptr);
    if (inPtr) {
        impl.reset(new InOrderTreeIteratorImpl<T>(*inPtr));
        return;
    }
    auto postPtr = dynamic_cast<PostOrderTreeIteratorImpl<T>*>(ptr);
    if (postPtr) {
        impl.reset(new PostOrderTreeIteratorImpl<T>(*postPtr));
        return;
    }
    auto levelPtr = dynamic_cast<LevelOrderTreeIteratorImpl<T>*>(ptr);
    impl.reset(new LevelOrderTreeIteratorImpl<T>(*levelPtr));
}

template<typename T>
bool TreeConstIterator<T>::operator==(const TreeConstIterator<T>& rhs) const {
    return *impl == *rhs.impl;
}

template<typename T>
bool TreeConstIterator<T>::operator!=(const TreeConstIterator<T>& rhs) const {
    return !operator==(rhs);
}

template<typename T>
const T& TreeConstIterator<T>::operator*() const {
    return **impl;
}

template<typename T>
const T* TreeConstIterator<T>::operator->() const {
    return &operator*();
}

template<typename T>
TreeConstIterator<T>& TreeConstIterator<T>::operator++() {
    ++(*impl);
    return *this;
}

template<typename T>
TreeConstIterator<T> TreeConstIterator<T>::operator++(int) {
    TreeIterator<T> temp(*this);
    ++(*this);
    return temp;
}

#endif //TREE_ITERATOR_CPP