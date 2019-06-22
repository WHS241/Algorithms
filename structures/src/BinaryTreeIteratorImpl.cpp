#ifndef TREE_ITERATOR_IMPL_CPP
#define TREE_ITERATOR_IMPL_CPP

#include <structures/BinaryTreeNode.h>

template<typename T>
bool TreeIteratorImpl<T>::operator==(const TreeIteratorImpl<T>& src) const {
    return current == src.current;
}

template<typename T>
bool TreeIteratorImpl<T>::operator!=(const TreeIteratorImpl<T>& src) const {
    return !operator==(src);
}

template<typename T>
const T& TreeIteratorImpl<T>::operator*() const {
    return current->_item;
}

template<typename T>
T& TreeIteratorImpl<T>::operator*() {
    return const_cast<T&>(static_cast<const TreeIteratorImpl<T>*>(this)->operator*());
}

template<typename T>
const T* TreeIteratorImpl<T>::operator->() const {
    return &(current->_item);
}

template<typename T>
T* TreeIteratorImpl<T>::operator->() {
    return const_cast<T&>(static_cast<const TreeIteratorImpl<T>*>(this)->operator->());
}

template<typename T>
BinaryNode<T>* PreOrderTreeIteratorImpl<T>::preOrderStart(BinaryNode<T>* root) {
    if (!root)
        return root;

    while (root->left || root->right)
        root = root->left ? root->left.get() : root->right.get();
    
    return root;
}

template<typename T>
BinaryNode<T>* InOrderTreeIteratorImpl<T>::leftmostDescendant(BinaryNode<T>* root) {
    if (!root)
        return root;

    while (root->left)
        root = root->left.get();
    
    return root;
}

template<typename T>
TreeIteratorImpl<T>::TreeIteratorImpl(BinaryNode<T>* current) : current(current) {}

template<typename T>
PreOrderTreeIteratorImpl<T>::PreOrderTreeIteratorImpl(BinaryNode<T>* ptr)
    : TreeIteratorImpl<T>(preOrderStart(ptr)) {}

template<typename T>
PreOrderTreeIteratorImpl<T>& PreOrderTreeIteratorImpl<T>::operator++() {
    BinaryNode<T>* parent = current->parent;
    current = (parent && (parent->left.get() == current) && parent->right) ? preOrderStart(parent->right.get()) : parent;

    return *this;
}

template<typename T>
InOrderTreeIteratorImpl<T>::InOrderTreeIteratorImpl(BinaryNode<T>* ptr)
    : TreeIteratorImpl<T>(leftmostDescendant(ptr)) {}

template<typename T>
InOrderTreeIteratorImpl<T>& InOrderTreeIteratorImpl<T>::operator++() {
    if (current->right) {
        current = leftmostDescendant(current->right.get());
    }
    else {
        BinaryNode<T>* parent = current->parent;
        while (parent && parent->right.get() == current) {
            current = parent;
            parent = current->parent;
        }
        current = parent;
    }
    return *this;
}

template<typename T>
PostOrderTreeIteratorImpl<T>::PostOrderTreeIteratorImpl(BinaryNode<T>* ptr)
    : TreeIteratorImpl<T>(ptr) {}

template<typename T>
PostOrderTreeIteratorImpl<T>& PostOrderTreeIteratorImpl<T>::operator++() {
    if (current->left) {
        current = current->left.get();
    }
    else if (current->right) {
        current = current->right.get();
    }
    else {
        BinaryNode<T>* parent = current->parent;
        while (parent && ((!parent->right && parent->left) || current == parent->right.get())) {
            current = parent;
            parent = current->parent;
        }
        if (!parent) {
            current = parent;
        }
        else {
            current = parent->right.get();
        }
    }
    return *this;
}

template<typename T>
LevelOrderTreeIteratorImpl<T>::LevelOrderTreeIteratorImpl(BinaryNode<T>* ptr)
    : TreeIteratorImpl<T>(ptr)
    , buffer() {
    buffer.push_back(ptr);
    for (auto node : buffer) {
        if (node->left)
            buffer.push_back(node->left.get());
        if (node->right)
            buffer.push_back(node->right.get());
    }

    pos = buffer.begin();
}

template<typename T>
LevelOrderTreeIteratorImpl<T>& LevelOrderTreeIteratorImpl<T>::operator++() {
    ++pos;
    current = (pos == buffer.end()) ? nullptr : *pos;

    return *this;
}



#endif // TREE_ITERATOR_IMPL_CPP