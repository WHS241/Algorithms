#ifndef BINARY_SEARCH_TREE_CPP
#define BINARY_SEARCH_TREE_CPP
#include <memory>
#include <stdexcept>
#include <vector>

namespace tree {
template <typename T, typename Compare>
bool binary_search_tree<T, Compare>::contains(const T& item) const noexcept
{
    return _find(item);
}

template <typename T, typename Compare>
typename binary_tree<T>::node* binary_search_tree<T, Compare>::_generate(
    const std::vector<T>& elements, uint32_t first, uint32_t last,
    typename binary_tree<T>::node* parent)
{
    if (first == last) {
        return nullptr;
    }

    uint32_t middle = (first + last) / 2;
    std::unique_ptr<typename binary_tree<T>::node> root(
        new typename binary_tree<T>::node(elements[middle], parent));

    if (last - first > 1) {
        root->replace_left(_generate(elements, first, middle, root.get()));
        root->replace_right(_generate(elements, middle + 1, last, root.get()));
    }
    return root.release();
}

template <typename T, typename Compare>
typename binary_tree<T>::node* binary_search_tree<T, Compare>::_find(const T& item) const noexcept
{

    typename binary_tree<T>::node* current = this->_root.get();
    while ((current != nullptr) && (current->item != item))
        current = _compare(item, current->item) ? current->left : current->right;

    return current;
}

template <typename T, typename Compare>
void binary_search_tree<T, Compare>::_rotate(
    typename binary_tree<T>::node* upper, bool use_left_child)
{
    auto parent = upper->parent;
    bool is_root = (parent == nullptr);
    bool is_left_child = !is_root && (upper == parent->left);

    if ((use_left_child && upper->left == nullptr)
        || (!use_left_child && upper->right == nullptr)) {
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

template <typename T, typename Compare>
void binary_search_tree<T, Compare>::_verify(typename binary_tree<T>::iterator check)
{
    auto found = _find(*check);
    if (found != binary_tree<T>::_get_node(check))
        throw std::invalid_argument("Not in current tree");
}
}

#endif // BINARY_SEARCH_TREE_CPP
