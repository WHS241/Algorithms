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
    return const_iterator(nullptr, PreOrder);
}

template<typename T>
void BinaryTree<T>::remove(const T& item) {
    iterator toRemove(find(item), PreOrder);
    if (toRemove == end())
        throw std::invalid_argument("Not found in tree");

    this->remove(toRemove);
}

template<typename T>
uint32_t BinaryTree<T>::size() const noexcept {
    return _size;
}

template<typename T>
BinaryTree<T>::BinaryNode::BinaryNode(const T& item, BinaryNode* parent, BinaryNode* left, BinaryNode* right)
    : _item(item)
    , _parent(parent)
    , _left(left)
    , _right(right)
{
    if (left)
        left->_parent = this;
    if (right)
        right->_parent = this;
}

template<typename T>
BinaryTree<T>::BinaryNode::~BinaryNode() noexcept {
    delete _left;
    delete _right;
}

template<typename T>
BinaryTree<T>::BinaryNode::BinaryNode(BinaryNode&& src) {
    *this = std::move(src);
}

template<typename T>
const typename BinaryTree<T>::BinaryNode& BinaryTree<T>::BinaryNode::operator=(BinaryNode&& rhs) {
    if (this != &rhs) {
        delete _left;
        delete _right;
        _left = nullptr;
        _right = nullptr;
        _item = std::move(_item);
        std::swap(_left, rhs._left);
        std::swap(_right, rhs._right);
        
        if (_parent != nullptr) {
            if (_parent->_left == this) {
                _parent->_left = nullptr;
            }
            else {
                _parent->_right = nullptr;
            }
        }

        _parent = rhs._parent;
        if (_parent != nullptr) {
            if (_parent->_left == &rhs) {
                _parent->_left = this;
            }
            else {
                _parent->_right = this;
            }
        }
    }

    return *this;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* BinaryTree<T>::BinaryNode::changeLeft(typename BinaryTree<T>::BinaryNode* add) noexcept {
    auto ptr = _left;
    _left = add;

    if (add != nullptr)
        add->_parent = this;

    // Middle of complex move?
    if(ptr && (ptr->_parent == this))
        ptr->_parent = nullptr;
    return ptr;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* BinaryTree<T>::BinaryNode::changeRight(typename BinaryTree<T>::BinaryNode* add) noexcept {
    auto ptr = _right;
    _right = add;

    if (add != nullptr)
        add->_parent = this;

    // Middle of complex move?
    if (ptr && (ptr->_parent == this))
        ptr->_parent = nullptr;
    return ptr;
}

template<typename T>
void BinaryTree<T>::BinaryNode::replaceLeft(typename BinaryTree<T>::BinaryNode* add) noexcept {
    auto prev = changeLeft(add);
    delete prev;
}

template<typename T>
void BinaryTree<T>::BinaryNode::replaceRight(typename BinaryTree<T>::BinaryNode* add) noexcept {
    auto prev = changeRight(add);
    delete prev;
}

template<typename T>
typename BinaryTree<T>::BinaryNode* BinaryTree<T>::BinaryNode::clone() const {
    return new BinaryNode(this->_item, nullptr, (this->_left) ? this->_left->clone() : nullptr, (this->_right) ? this->_right->clone() : nullptr);
}

template<typename T>
typename BinaryTree<T>::BinaryNode* BinaryTree<T>::getNode(typename BinaryTree<T>::iterator it) {
    return it.getNode();
}

template<typename T>
typename BinaryTree<T>::BinaryNode* BinaryTree<T>::getNode(typename BinaryTree<T>::const_iterator it) {
    return it.getNode();
}

#endif // BINARY_TREE_CPP