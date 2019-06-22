#ifndef BINARY_TREE_CPP
#define BINARY_TREE_CPP

#include <structures/BinaryTreeIterator.h>

template<typename T>
BinaryTree<T>::BinaryTree()
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
    return iterator(root.get(), traversal);
}

template<typename T>
typename BinaryTree<T>::const_iterator BinaryTree<T>::end() const {
    return iterator(nullptr, PreOrder);
}

template<typename T>
uint32_t BinaryTree<T>::size() const {
    return _size;
}

#endif // BINARY_TREE_CPP