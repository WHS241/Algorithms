#ifndef TREE_ITERATOR_IMPL_CPP
#define TREE_ITERATOR_IMPL_CPP

#include <structures/binary_tree_base.h>
#include <include/structures/binary_tree_base.h>

namespace tree {
    template<typename T>
    bool tree_iterator_impl<T>::operator==(const tree_iterator_impl <T> &src) const noexcept {
        return _current == src._current;
    }

    template<typename T>
    bool tree_iterator_impl<T>::operator!=(const tree_iterator_impl <T> &src) const noexcept {
        return !operator==(src);
    }

    template<typename T>
    const T &tree_iterator_impl<T>::operator*() const noexcept {
        return _current->_item;
    }

    template<typename T>
    T &tree_iterator_impl<T>::operator*() noexcept {
        return const_cast<T &>(static_cast<const tree_iterator_impl<T> *>(this)->operator*());
    }

    template<typename T>
    const T *tree_iterator_impl<T>::operator->() const noexcept {
        return &(_current->_item);
    }

    template<typename T>
    T *tree_iterator_impl<T>::operator->() noexcept {
        return const_cast<T *>(static_cast<const tree_iterator_impl<T> *>(this)->operator->());
    }

    template<typename T>
    typename binary_tree<T>::node *pre_order_iterator_impl<T>::s_pre_order_end(
            typename binary_tree<T>::node *root) noexcept {
        if (!root)
            return root;

        for (typename binary_tree<T>::node *left(root->_left), *right(root->_right); left || right;
             left = root->_left, right = root->_right)
            root = right ? right : left;

        return root;
    }

    template<typename T>
    typename binary_tree<T>::node *in_order_iterator_impl<T>::s_leftmost_descendant(
            typename binary_tree<T>::node *root) noexcept {
        if (!root)
            return root;

        for (typename binary_tree<T>::node *left(root->_left); left; left = root->_left)
            root = left;

        return root;
    }

    template<typename T>
    typename binary_tree<T>::node *in_order_iterator_impl<T>::s_rightmost_descendant(
            typename binary_tree<T>::node *root) noexcept {
        if (!root)
            return root;

        for (typename binary_tree<T>::node *right(root->_right); right; right = root->_right)
            root = right;

        return root;
    }

    template<typename T>
    typename binary_tree<T>::node *post_order_iterator_impl<T>::s_post_order_start(
            typename binary_tree<T>::node *root) noexcept {
        if (!root)
            return root;

        for (typename binary_tree<T>::node *left(root->_left), *right(root->_right); left || right;
             left = root->_left, right = root->_right)
            root = left ? left : right;

        return root;
    }

    template<typename T>
    tree_iterator_impl<T>::tree_iterator_impl(typename binary_tree<T>::node *current)
            : _current(current) {
    }

    template<typename T>
    pre_order_iterator_impl<T>::pre_order_iterator_impl(typename binary_tree<T>::node *ptr)
            : tree_iterator_impl<T>(ptr) {
    }

    template<typename T>
    pre_order_iterator_impl <T> &pre_order_iterator_impl<T>::operator++() noexcept {
        if (this->_current->_left) {
            this->_current = this->_current->_left;
        } else if (this->_current->_right) {
            this->_current = this->_current->_right;
        } else {
            typename binary_tree<T>::node *parent = this->_current->_parent;
            while (parent && ((!parent->_right && parent->_left) || this->_current == parent->_right)) {
                this->_current = parent;
                parent = this->_current->_parent;
            }
            if (!parent) {
                this->_current = parent;
            } else {
                this->_current = parent->_right;
            }
        }
        return *this;
    }

    template<typename T>
    pre_order_iterator_impl <T> &pre_order_iterator_impl<T>::operator--() noexcept {
        typename binary_tree<T>::node *parent = this->_current->_parent;
        this->_current = (parent && (parent->_right == this->_current) && parent->_left)
                        ? s_pre_order_end(parent->_left)
                        : parent;

        return *this;
    }

    template<typename T>
    in_order_iterator_impl<T>::in_order_iterator_impl(typename binary_tree<T>::node *ptr)
            : tree_iterator_impl<T>(s_leftmost_descendant(ptr)) {
    }

    template<typename T>
    in_order_iterator_impl <T> &in_order_iterator_impl<T>::operator++() noexcept {
        if (this->_current->_right) {
            this->_current = s_leftmost_descendant(this->_current->_right);
        } else {
            typename binary_tree<T>::node *parent = this->_current->_parent;
            while (parent && parent->_right == this->_current) {
                this->_current = parent;
                parent = this->_current->_parent;
            }
            this->_current = parent;
        }
        return *this;
    }

    template<typename T>
    in_order_iterator_impl <T> &in_order_iterator_impl<T>::operator--() noexcept {
        if (this->_current->_left) {
            this->_current = s_rightmost_descendant(this->_current->_left);
        } else {
            typename binary_tree<T>::node *parent = this->_current->_parent;
            while (parent && parent->_left == this->_current) {
                this->_current = parent;
                parent = this->_current->_parent;
            }
            this->_current = parent;
        }
        return *this;
    }

    template<typename T>
    post_order_iterator_impl<T>::post_order_iterator_impl(typename binary_tree<T>::node *ptr)
            : tree_iterator_impl<T>(s_post_order_start(ptr)) {
    }

    template<typename T>
    post_order_iterator_impl <T> &post_order_iterator_impl<T>::operator++() noexcept {
        typename binary_tree<T>::node *parent = this->_current->_parent;
        this->_current = (parent && (parent->_left == this->_current) && parent->_right)
                        ? s_post_order_start(parent->_right)
                        : parent;

        return *this;
    }

    template<typename T>
    post_order_iterator_impl <T> &post_order_iterator_impl<T>::operator--() noexcept {
        if (this->_current->_right) {
            this->_current = this->_current->_right;
        } else if (this->_current->_left) {
            this->_current = this->_current->_left;
        } else {
            typename binary_tree<T>::node *parent = this->_current->_parent;
            while (parent && ((!parent->_left && parent->_right) || this->_current == parent->_left)) {
                this->_current = parent;
                parent = this->_current->_parent;
            }
            if (!parent) {
                this->_current = parent;
            } else {
                this->_current = parent->_left;
            }
        }
        return *this;
    }

    template<typename T>
    level_order_iterator_impl<T>::level_order_iterator_impl(typename binary_tree<T>::node *ptr)
            : tree_iterator_impl<T>(ptr), _buffer() {
        _buffer.push_back(ptr);
        for (auto node : _buffer) {
            if (node->_left)
                _buffer.push_back(node->_left);
            if (node->_right)
                _buffer.push_back(node->_right);
        }

        t_pos = _buffer.begin();
    }

    template<typename T>
    level_order_iterator_impl <T> &level_order_iterator_impl<T>::operator++() noexcept {
        ++t_pos;
        this->_current = (t_pos == _buffer.end()) ? nullptr : *t_pos;

        return *this;
    }

    template<typename T>
    level_order_iterator_impl <T> &level_order_iterator_impl<T>::operator--() noexcept {
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