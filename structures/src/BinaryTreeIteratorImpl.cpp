#ifndef TREE_ITERATOR_IMPL_CPP
#define TREE_ITERATOR_IMPL_CPP

#include <BinaryTree.h>

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
typename BinaryTree<T>::BinaryNode* PreOrderTreeIteratorImpl<T>::preOrderEnd(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode *left(root->_left), *right(root->_right); left || right; left = root->_left, right = root->_right)
        root = right ? right : left;
    
    return root;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* InOrderTreeIteratorImpl<T>::leftmostDescendant(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode* left(root->_left); left; left = root->_left)
        root = left;
    
    return root;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* InOrderTreeIteratorImpl<T>::rightmostDescendant(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode* right(root->_right); right; right = root->_right)
        root = right;

    return root;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* PostOrderTreeIteratorImpl<T>::postOrderStart(typename BinaryTree<T>::BinaryNode* root) noexcept {
    if (!root)
        return root;

    for (typename BinaryTree<T>::BinaryNode* left(root->_left), *right(root->_right); left || right; left = root->_left, right = root->_right)
        root = left ? left : right;

    return root;
}

template<typename T>
TreeIteratorImpl<T>::TreeIteratorImpl(typename BinaryTree<T>::BinaryNode* current) : current(current) {}

template<typename T>
PreOrderTreeIteratorImpl<T>::PreOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(ptr) {}

template<typename T>
PreOrderTreeIteratorImpl<T>& PreOrderTreeIteratorImpl<T>::operator++() noexcept {
    if (this->current->_left) {
        this->current = this->current->_left;
    }
    else if (this->current->_right) {
        this->current = this->current->_right;
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && ((!parent->_right && parent->_left) || this->current == parent->_right)) {
            this->current = parent;
            parent = this->current->_parent;
        }
        if (!parent) {
            this->current = parent;
        }
        else {
            this->current = parent->_right;
        }
    }
    return *this;
}

template<typename T>
PreOrderTreeIteratorImpl<T>& PreOrderTreeIteratorImpl<T>::operator--() noexcept {
    typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
    this->current = (parent && (parent->_right == this->current) && parent->_left) ? preOrderEnd(parent->_left) : parent;

    return *this;
}

template<typename T>
InOrderTreeIteratorImpl<T>::InOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(leftmostDescendant(ptr)) {}

template<typename T>
InOrderTreeIteratorImpl<T>& InOrderTreeIteratorImpl<T>::operator++() noexcept {
    if (this->current->_right) {
        this->current = leftmostDescendant(this->current->_right);
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && parent->_right == this->current) {
            this->current = parent;
            parent = this->current->_parent;
        }
        this->current = parent;
    }
    return *this;
}

template<typename T>
InOrderTreeIteratorImpl<T>& InOrderTreeIteratorImpl<T>::operator--() noexcept {
    if (this->current->_left) {
        this->current = rightmostDescendant(this->current->_left);
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && parent->_left == this->current) {
            this->current = parent;
            parent = this->current->_parent;
        }
        this->current = parent;
    }
    return *this;
}

template<typename T>
PostOrderTreeIteratorImpl<T>::PostOrderTreeIteratorImpl(typename BinaryTree<T>::BinaryNode* ptr)
    : TreeIteratorImpl<T>(postOrderStart(ptr)) {}

template<typename T>
PostOrderTreeIteratorImpl<T>& PostOrderTreeIteratorImpl<T>::operator++() noexcept {
    typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
    this->current = (parent && (parent->_left == this->current) && parent->_right) ? postOrderStart(parent->_right) : parent;

    return *this;
}

template<typename T>
PostOrderTreeIteratorImpl<T>& PostOrderTreeIteratorImpl<T>::operator--() noexcept {
    if (this->current->_right) {
        this->current = this->current->_right;
    }
    else if (this->current->_left) {
        this->current = this->current->_left;
    }
    else {
        typename BinaryTree<T>::BinaryNode* parent = this->current->_parent;
        while (parent && ((!parent->_left && parent->_right) || this->current == parent->_left)) {
            this->current = parent;
            parent = this->current->_parent;
        }
        if (!parent) {
            this->current = parent;
        }
        else {
            this->current = parent->_left;
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
            buffer.push_back(node->_left);
        if (node->_right)
            buffer.push_back(node->_right);
    }

    pos = buffer.begin();
}

template<typename T>
LevelOrderTreeIteratorImpl<T>& LevelOrderTreeIteratorImpl<T>::operator++() noexcept {
    ++pos;
    this->current = (pos == buffer.end()) ? nullptr : *pos;

    return *this;
}

template<typename T>
LevelOrderTreeIteratorImpl<T>& LevelOrderTreeIteratorImpl<T>::operator--() noexcept {
    if (pos == buffer.begin()) {
        pos = buffer.end();
        this->current = nullptr;
    }
    else {
        --pos;
        this->current = *pos;
    }

    return *this;
}



#endif // TREE_ITERATOR_IMPL_CPP