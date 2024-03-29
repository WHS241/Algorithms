#ifndef TREE_ITERATOR_CPP
#define TREE_ITERATOR_CPP

#include <structures/binary_tree_base.h>
#include <structures/binary_tree_iterator_impl.h>

namespace tree {
template<typename T>
tree_iterator<T>::tree_iterator(typename binary_tree<T>::node* root, traversal order,
                                bool entire_subtree, binary_tree<T>* tree) :
    _trav(order), _tree(tree) {
    switch (order) {
    case pre_order:
        _impl.reset(new pre_order_iterator_impl<T>(root, tree));
        break;

    case in_order:
        _impl.reset(new in_order_iterator_impl<T>(root, tree, entire_subtree));
        break;

    case post_order:
        _impl.reset(new post_order_iterator_impl<T>(root, tree, entire_subtree));
        break;

    case level_order:
        _impl.reset(new level_order_iterator_impl<T>(root, tree));

    default:
        break;
    }
}

template<typename T> bool tree_iterator<T>::operator==(const tree_iterator<T>& rhs) const {
    return *_impl == *rhs._impl;
}

template<typename T> bool tree_iterator<T>::operator!=(const tree_iterator<T>& rhs) const {
    return !operator==(rhs);
}

template<typename T>
tree_iterator<T>::tree_iterator(const tree_iterator<T>& src) : _trav(src._trav), _tree(src._tree) {
    tree_iterator_impl<T>* ptr = src._impl.get();
    auto prePtr = dynamic_cast<pre_order_iterator_impl<T>*>(ptr);
    if (prePtr) {
        _impl.reset(new pre_order_iterator_impl<T>(*prePtr));
        return;
    }
    auto inPtr = dynamic_cast<in_order_iterator_impl<T>*>(ptr);
    if (inPtr) {
        _impl.reset(new in_order_iterator_impl<T>(*inPtr));
        return;
    }
    auto postPtr = dynamic_cast<post_order_iterator_impl<T>*>(ptr);
    if (postPtr) {
        _impl.reset(new post_order_iterator_impl<T>(*postPtr));
        return;
    }
    auto levelPtr = dynamic_cast<level_order_iterator_impl<T>*>(ptr);
    _impl.reset(new level_order_iterator_impl<T>(*levelPtr));
}

template<typename T> tree_iterator<T>& tree_iterator<T>::operator=(const tree_iterator<T>& rhs) {
    if (this != &rhs) {
        tree_iterator<T> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template<typename T> const T& tree_iterator<T>::operator*() const {
    return **_impl;
}

template<typename T> T& tree_iterator<T>::operator*() {
    return **_impl;
}

template<typename T> const T* tree_iterator<T>::operator->() const {
    return &operator*();
}

template<typename T> T* tree_iterator<T>::operator->() {
    return &operator*();
}

template<typename T> tree_iterator<T>& tree_iterator<T>::operator++() {
    ++(*_impl);
    return *this;
}

template<typename T> tree_iterator<T> tree_iterator<T>::operator++(int) {
    tree_iterator<T> temp(*this);
    ++(*this);
    return temp;
}

template<typename T> tree_iterator<T>& tree_iterator<T>::operator--() {
    --(*_impl);
    return *this;
}

template<typename T> tree_iterator<T> tree_iterator<T>::operator--(int) {
    tree_iterator<T> temp(*this);
    --(*this);
    return temp;
}

template<typename T> typename binary_tree<T>::node* tree_iterator<T>::_get_node() {
    return _impl->_current;
}

template<typename T>
tree_const_iterator<T>::tree_const_iterator(const typename binary_tree<T>::node* root,
                                            traversal order, bool entire_subtree,
                                            const binary_tree<T>* tree) :
    _trav(order), _tree(tree) {
    auto stripped_root = const_cast<typename binary_tree<T>::node*>(root);
    auto stripped_tree = const_cast<binary_tree<T>*>(tree);
    switch (order) {
    case pre_order:
        _impl.reset(new pre_order_iterator_impl<T>(stripped_root, stripped_tree));
        break;

    case in_order:
        _impl.reset(new in_order_iterator_impl<T>(stripped_root, stripped_tree, entire_subtree));
        break;

    case post_order:
        _impl.reset(new post_order_iterator_impl<T>(stripped_root, stripped_tree, entire_subtree));
        break;

    case level_order:
        _impl.reset(new level_order_iterator_impl<T>(stripped_root, stripped_tree));

    default:
        break;
    }
}

template<typename T>
tree_const_iterator<T>::tree_const_iterator(const tree_const_iterator<T>& src) {
    tree_iterator_impl<T>* ptr = src._impl.get();
    auto prePtr = dynamic_cast<pre_order_iterator_impl<T>*>(ptr);
    if (prePtr) {
        _impl.reset(new pre_order_iterator_impl<T>(*prePtr));
        return;
    }
    auto inPtr = dynamic_cast<in_order_iterator_impl<T>*>(ptr);
    if (inPtr) {
        _impl.reset(new in_order_iterator_impl<T>(*inPtr));
        return;
    }
    auto postPtr = dynamic_cast<post_order_iterator_impl<T>*>(ptr);
    if (postPtr) {
        _impl.reset(new post_order_iterator_impl<T>(*postPtr));
        return;
    }
    auto levelPtr = dynamic_cast<level_order_iterator_impl<T>*>(ptr);
    _impl.reset(new level_order_iterator_impl<T>(*levelPtr));
}

template<typename T>
tree_const_iterator<T>& tree_const_iterator<T>::operator=(const tree_const_iterator<T>& rhs) {
    if (this != &rhs) {
        tree_const_iterator<T> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template<typename T>
bool tree_const_iterator<T>::operator==(const tree_const_iterator<T>& rhs) const {
    return *_impl == *rhs._impl;
}

template<typename T>
bool tree_const_iterator<T>::operator!=(const tree_const_iterator<T>& rhs) const {
    return !operator==(rhs);
}

template<typename T> const T& tree_const_iterator<T>::operator*() const {
    return **_impl;
}

template<typename T> const T* tree_const_iterator<T>::operator->() const {
    return &operator*();
}

template<typename T> tree_const_iterator<T>& tree_const_iterator<T>::operator++() {
    ++(*_impl);
    return *this;
}

template<typename T> tree_const_iterator<T> tree_const_iterator<T>::operator++(int) {
    tree_iterator<T> temp(*this);
    ++(*this);
    return temp;
}

template<typename T> tree_const_iterator<T>& tree_const_iterator<T>::operator--() {
    --(*_impl);
    return *this;
}

template<typename T> tree_const_iterator<T> tree_const_iterator<T>::operator--(int) {
    tree_iterator<T> temp(*this);
    --(*this);
    return temp;
}

template<typename T> const typename binary_tree<T>::node* tree_const_iterator<T>::_get_node() {
    return _impl->_current;
}
} // namespace tree

#endif // TREE_ITERATOR_CPP