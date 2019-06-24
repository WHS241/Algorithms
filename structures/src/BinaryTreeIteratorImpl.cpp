#ifndef TREE_ITERATOR_IMPL_CPP
#define TREE_ITERATOR_IMPL_CPP

#include <structures/BinaryTree.h>

template<typename T>
bool TreeIteratorImpl<T>::operator==(const TreeIteratorImpl<T>& src) const noexcept {
    return current == src.current;
}

template<typename T>
bool TreeIteratorImpl<T>::operator!=(const TreeIteratorImpl<T>& src) const noexcept {
    return !operator==(src);
}

template<typename T>
const T& TreeIteratorImpl<T>::operator*() const noexcept {
    return current->_item;
}

template<typename T>
T& TreeIteratorImpl<T>::operator*() noexcept {
    return const_cast<T&>(static_cast<const TreeIteratorImpl<T>*>(this)->operator*());
}

template<typename T>
const T* TreeIteratorImpl<T>::operator->() const noexcept {
    return &(current->_item);
}

template<typename T>
T* TreeIteratorImpl<T>::operator->() noexcept {
    return const_cast<T*>(static_cast<const TreeIteratorImpl<T>*>(this)->operator->());
}

template<typename T>
typename BinaryTree<T>::BinaryNode* PreOrderTreeIteratorImpl<T>::preOrderStart(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode*left(root->_left.get()), *right(root->_right.get()); left || right; left = root->_left.get(), right = root->_right.get())
        root = left ? left : right;
    
    return root;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* InOrderTreeIteratorImpl<T>::leftmostDescendant(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode* left(root->_left.get()); left; left = root->_left.get())
        root = left;
    
    return root;
}

template<typename T>
TreeIteratorImpl<T>::TreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current) : current(current) {}

template<typename T>
PreOrderTreeIteratorImpl<T>::PreOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(preOrderStart(ptr)) {}

template<typename T>
PreOrderTreeIteratorImpl<T>& PreOrderTreeIteratorImpl<T>::operator++() noexcept {
    typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
    this->current = (parent && (parent->_left.get() == this->current) && parent->_right) ? preOrderStart(parent->_right.get()) : parent;

    return *this;
}

template<typename T>
InOrderTreeIteratorImpl<T>::InOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(leftmostDescendant(ptr)) {}

template<typename T>
InOrderTreeIteratorImpl<T>& InOrderTreeIteratorImpl<T>::operator++() noexcept {
    if (this->current->_right) {
        this->current = leftmostDescendant(this->current->_right.get());
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && parent->_right.get() == this->current) {
            this->current = parent;
            parent = this->current->_parent;
        }
        this->current = parent;
    }
    return *this;
}

template<typename T>
PostOrderTreeIteratorImpl<T>::PostOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(ptr) {}

template<typename T>
PostOrderTreeIteratorImpl<T>& PostOrderTreeIteratorImpl<T>::operator++() noexcept {
    if (this->current->_left) {
        this->current = this->current->_left.get();
    }
    else if (this->current->_right) {
        this->current = this->current->_right.get();
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && ((!parent->_right && parent->_left) || this->current == parent->_right.get())) {
            this->current = parent;
            parent = this->current->_parent;
        }
        if (!parent) {
            this->current = parent;
        }
        else {
            this->current = parent->_right.get();
        }
    }
    return *this;
}

template<typename T>
LevelOrderTreeIteratorImpl<T>::LevelOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(ptr)
    , buffer() {
    buffer.push_back(ptr);
    for (auto node : buffer) {
        if (node->_left)
            buffer.push_back(node->_left.get());
        if (node->_right)
            buffer.push_back(node->_right.get());
    }

    pos = buffer.begin();
}

template<typename T>
LevelOrderTreeIteratorImpl<T>& LevelOrderTreeIteratorImpl<T>::operator++() noexcept {
    ++pos;
    this->current = (pos == buffer.end()) ? nullptr : *pos;

    return *this;
}



#endif // TREE_ITERATOR_IMPL_CPP