#ifndef TREE_ITERATOR_IMPL_CPP
#define TREE_ITERATOR_IMPL_CPP

#include <include/structures/binary_tree_base.h>
#include <structures/binary_tree_base.h>

namespace tree {
template <typename T>
bool tree_iterator_impl<T>::operator==(const tree_iterator_impl<T>& src) const noexcept
{
    return _current == src._current;
}

template <typename T>
bool tree_iterator_impl<T>::operator!=(const tree_iterator_impl<T>& src) const noexcept
{
    return !operator==(src);
}

template <typename T> const T& tree_iterator_impl<T>::operator*() const noexcept
{
    return _current->item;
}

template <typename T> T& tree_iterator_impl<T>::operator*() noexcept
{
    return const_cast<T&>(static_cast<const tree_iterator_impl<T>*>(this)->operator*());
}

template <typename T> const T* tree_iterator_impl<T>::operator->() const noexcept
{
    return &(_current->item);
}

template <typename T> T* tree_iterator_impl<T>::operator->() noexcept
{
    return const_cast<T*>(static_cast<const tree_iterator_impl<T>*>(this)->operator->());
}

template <typename T>
typename binary_tree<T>::node* pre_order_iterator_impl<T>::s_pre_order_end(
    typename binary_tree<T>::node* root) noexcept
{
    if (!root)
        return root;

    for (typename binary_tree<T>::node *left(root->left), *right(root->right); left || right;
         left = root->left, right = root->right)
        root = right ? right : left;

    return root;
}

template <typename T>
typename binary_tree<T>::node* in_order_iterator_impl<T>::s_leftmost_descendant(
    typename binary_tree<T>::node* root) noexcept
{
    if (!root)
        return root;

    for (typename binary_tree<T>::node* left(root->left); left; left = root->left)
        root = left;

    return root;
}

template <typename T>
typename binary_tree<T>::node* in_order_iterator_impl<T>::s_rightmost_descendant(
    typename binary_tree<T>::node* root) noexcept
{
    if (!root)
        return root;

    for (typename binary_tree<T>::node* right(root->right); right; right = root->right)
        root = right;

    return root;
}

template <typename T>
typename binary_tree<T>::node* post_order_iterator_impl<T>::s_post_order_start(
    typename binary_tree<T>::node* root) noexcept
{
    if (!root)
        return root;

    for (typename binary_tree<T>::node *left(root->left), *right(root->right); left || right;
         left = root->left, right = root->right)
        root = left ? left : right;

    return root;
}

template <typename T>
tree_iterator_impl<T>::tree_iterator_impl(typename binary_tree<T>::node* current)
    : _current(current)
{
}

template <typename T>
pre_order_iterator_impl<T>::pre_order_iterator_impl(typename binary_tree<T>::node* ptr)
    : tree_iterator_impl<T>(ptr)
{
}

template <typename T> pre_order_iterator_impl<T>& pre_order_iterator_impl<T>::operator++() noexcept
{
    if (this->_current->left) {
        this->_current = this->_current->left;
    } else if (this->_current->right) {
        this->_current = this->_current->right;
    } else {
        typename binary_tree<T>::node* parent = this->_current->parent;
        while (parent && ((!parent->right && parent->left) || this->_current == parent->right)) {
            this->_current = parent;
            parent = this->_current->parent;
        }
        if (!parent) {
            this->_current = parent;
        } else {
            this->_current = parent->right;
        }
    }
    return *this;
}

template <typename T> pre_order_iterator_impl<T>& pre_order_iterator_impl<T>::operator--() noexcept
{
    typename binary_tree<T>::node* parent = this->_current->parent;
    this->_current = (parent && (parent->right == this->_current) && parent->left)
        ? s_pre_order_end(parent->left)
        : parent;

    return *this;
}

template <typename T>
in_order_iterator_impl<T>::in_order_iterator_impl(typename binary_tree<T>::node* ptr)
    : tree_iterator_impl<T>(s_leftmost_descendant(ptr))
{
}

template <typename T> in_order_iterator_impl<T>& in_order_iterator_impl<T>::operator++() noexcept
{
    if (this->_current->right) {
        this->_current = s_leftmost_descendant(this->_current->right);
    } else {
        typename binary_tree<T>::node* parent = this->_current->parent;
        while (parent && parent->right == this->_current) {
            this->_current = parent;
            parent = this->_current->parent;
        }
        this->_current = parent;
    }
    return *this;
}

template <typename T> in_order_iterator_impl<T>& in_order_iterator_impl<T>::operator--() noexcept
{
    if (this->_current->left) {
        this->_current = s_rightmost_descendant(this->_current->left);
    } else {
        typename binary_tree<T>::node* parent = this->_current->parent;
        while (parent && parent->left == this->_current) {
            this->_current = parent;
            parent = this->_current->parent;
        }
        this->_current = parent;
    }
    return *this;
}

template <typename T>
post_order_iterator_impl<T>::post_order_iterator_impl(typename binary_tree<T>::node* ptr)
    : tree_iterator_impl<T>(s_post_order_start(ptr))
{
}

template <typename T>
post_order_iterator_impl<T>& post_order_iterator_impl<T>::operator++() noexcept
{
    typename binary_tree<T>::node* parent = this->_current->parent;
    this->_current = (parent && (parent->left == this->_current) && parent->right)
        ? s_post_order_start(parent->right)
        : parent;

    return *this;
}

template <typename T>
post_order_iterator_impl<T>& post_order_iterator_impl<T>::operator--() noexcept
{
    if (this->_current->right) {
        this->_current = this->_current->right;
    } else if (this->_current->left) {
        this->_current = this->_current->left;
    } else {
        typename binary_tree<T>::node* parent = this->_current->parent;
        while (parent && ((!parent->left && parent->right) || this->_current == parent->left)) {
            this->_current = parent;
            parent = this->_current->parent;
        }
        if (!parent) {
            this->_current = parent;
        } else {
            this->_current = parent->left;
        }
    }
    return *this;
}

template <typename T>
level_order_iterator_impl<T>::level_order_iterator_impl(typename binary_tree<T>::node* ptr)
    : tree_iterator_impl<T>(ptr)
    , _buffer()
{
    _buffer.push_back(ptr);
    for (auto node : _buffer) {
        if (node->left)
            _buffer.push_back(node->left);
        if (node->right)
            _buffer.push_back(node->right);
    }

    t_pos = _buffer.begin();
}

template <typename T>
level_order_iterator_impl<T>& level_order_iterator_impl<T>::operator++() noexcept
{
    ++t_pos;
    this->_current = (t_pos == _buffer.end()) ? nullptr : *t_pos;

    return *this;
}

template <typename T>
level_order_iterator_impl<T>& level_order_iterator_impl<T>::operator--() noexcept
{
    if (t_pos == _buffer.begin()) {
        t_pos = _buffer.end();
        this->_current = nullptr;
    } else {
        --t_pos;
        this->_current = *t_pos;
    }

    return *this;
}
}

#endif // TREE_ITERATOR_IMPL_CPP