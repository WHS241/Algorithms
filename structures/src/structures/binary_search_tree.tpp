#ifndef BINARY_SEARCH_TREE_CPP
#define BINARY_SEARCH_TREE_CPP
#include <memory>
#include <stdexcept>
#include <vector>

namespace tree {

template<typename T, typename Compare>
template<typename, typename>
binary_search_tree<T, Compare>::binary_search_tree(bool duplicates) :
    binary_search_tree<T, Compare>(Compare(), duplicates) {}

template<typename T, typename Compare>
binary_search_tree<T, Compare>::binary_search_tree(const Compare& comp, bool duplicates) :
    binary_tree<T>(), _compare(comp), _allow_duplicates(duplicates){};

template<typename T, typename Compare>
template<typename It, typename _Compare, typename _Requires>
binary_search_tree<T, Compare>::binary_search_tree(It first, It last, bool duplicates) :
    binary_search_tree(first, last, Compare(), duplicates){};

template<typename T, typename Compare>
template<typename It>
binary_search_tree<T, Compare>::binary_search_tree(It first, It last, const Compare& comp,
                                                   bool duplicates) :
    binary_tree<T>(), _compare(comp), _allow_duplicates(duplicates) {
    for (; first != last; ++first) {
        this->insert(*first);
    }
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::iterator binary_search_tree<T, Compare>::begin() {
    return binary_tree<T>::begin(traversal::in_order);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::iterator binary_search_tree<T, Compare>::end() {
    return binary_tree<T>::end(traversal::in_order);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::const_iterator
  binary_search_tree<T, Compare>::cbegin() const {
    return binary_tree<T>::cbegin(traversal::in_order);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::const_iterator
  binary_search_tree<T, Compare>::cend() const {
    return binary_tree<T>::cend(traversal::in_order);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::reverse_iterator binary_search_tree<T, Compare>::rbegin() {
    return std::make_reverse_iterator(end());
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::reverse_iterator binary_search_tree<T, Compare>::rend() {
    return std::make_reverse_iterator(begin());
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::const_reverse_iterator
  binary_search_tree<T, Compare>::crbegin() const {
    return std::make_reverse_iterator(cend());
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::const_reverse_iterator
  binary_search_tree<T, Compare>::crend() const {
    return std::make_reverse_iterator(cbegin());
}

template<typename T, typename Compare>
bool binary_search_tree<T, Compare>::contains(const T& item) const noexcept {
    return _find(item);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::iterator
  binary_search_tree<T, Compare>::find(const T& item) {
    return this->_make_iterator(_find(item), traversal::in_order, false);
}

template<typename T, typename Compare>
typename binary_search_tree<T, Compare>::const_iterator
  binary_search_tree<T, Compare>::find(const T& item) const {
    return this->_make_const_iterator(_find(item), traversal::in_order, false);
}

template<typename T, typename Compare>
typename binary_tree<T>::node* binary_search_tree<T, Compare>::_find(const T& item) const noexcept {
    typename binary_tree<T>::node* current = this->_root.get();
    while ((current != nullptr) && (_compare(current->item, item) || _compare(item, current->item)))
        current = _compare(item, current->item) ? current->left : current->right;

    return current;
}

template<typename T, typename Compare>
void binary_search_tree<T, Compare>::_rotate(typename binary_tree<T>::node* upper,
                                             bool use_left_child) {
    typename binary_tree<T>::node* parent = upper->parent;
    bool is_root = (parent == nullptr);
    bool is_left_child = !is_root && (upper == parent->left);

    if ((use_left_child && upper->left == nullptr) ||
        (!use_left_child && upper->right == nullptr)) {
        throw std::invalid_argument("No such child");
    }

    typename binary_tree<T>::node *new_root, *to_swap;

    if (use_left_child) {
        new_root = upper->left;
        to_swap = new_root->right;
        new_root->change_right(upper);
        upper->change_left(to_swap);
    } else {
        new_root = upper->right;
        to_swap = new_root->left;
        new_root->change_left(upper);
        upper->change_right(to_swap);
    }

    if (is_root) {
        this->_root.release();
        this->_root.reset(new_root);
    } else if (is_left_child) {
        parent->change_left(new_root);
    } else {
        parent->change_right(new_root);
    }
}
} // namespace tree

#endif // BINARY_SEARCH_TREE_CPP
