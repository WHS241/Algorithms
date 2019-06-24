#ifndef BINARY_TREE_CPP
#define BINARY_TREE_CPP

#include <stdexcept>

#include <structures/BinaryTreeIterator.h>

template<typename T>
BinaryTree<T>::BinaryTree() noexcept
    : root()
    , _size(0)
{}

template<typename T>
typename BinaryTree<T>::iterator BinaryTree<T>::begin(Traversal traversal) {
    return iterator(root.get(), traversal);
}

template<typename T>
typename BinaryTree<T>::iterator BinaryTree<T>::end() {
    return iterator(nullptr, PreOrder);
}

template<typename T>
typename BinaryTree<T>::const_iterator BinaryTree<T>::begin(Traversal traversal) const {
    return const_iterator(root.get(), traversal);
}

template<typename T>
typename BinaryTree<T>::const_iterator BinaryTree<T>::end() const {
    return const_iterator(nullptr, PostOrder);
}

template<typename T>
void BinaryTree<T>::remove(const T& item) {
    iterator toRemove(find(item), PostOrder);
    if (toRemove == end())
        throw std::invalid_argument("Not found in tree");

    remove(toRemove);
}

template<typename T>
uint32_t BinaryTree<T>::size() const noexcept {
    return _size;
}

template<typename T>
void BinaryTree<T>::BinaryNode::addLeft(typename BinaryTree<T>::BinaryNode* add) noexcept {
    _left.reset(add);
    add->_parent = this;
}

template<typename T>
void BinaryTree<T>::BinaryNode::addRight(typename BinaryTree<T>::BinaryNode* add) noexcept {
    _right.reset(add);
    add->_parent = this;
}

#endif // BINARY_TREE_CPP