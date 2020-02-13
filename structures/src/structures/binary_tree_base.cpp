#ifndef BINARY_TREE_CPP
#define BINARY_TREE_CPP

#include <stdexcept>

#include <include/structures/binary_tree_iterator.h>

namespace tree {
template <typename T>
binary_tree<T>::binary_tree() noexcept
    : _root()
    , _size(0)
{
}

template <typename T> typename binary_tree<T>::iterator binary_tree<T>::begin(traversal traversal)
{
    return iterator(_root.get(), traversal);
}

template <typename T> typename binary_tree<T>::iterator binary_tree<T>::end()
{
    return iterator(nullptr, pre_order);
}

template <typename T>
typename binary_tree<T>::const_iterator binary_tree<T>::begin(traversal traversal) const
{
    return const_iterator(_root.get(), traversal);
}

template <typename T> typename binary_tree<T>::const_iterator binary_tree<T>::end() const
{
    return const_iterator(nullptr, pre_order);
}

template <typename T> void binary_tree<T>::remove(const T& item)
{
    iterator toRemove(_find(item), pre_order);
    if (toRemove == end())
        throw std::invalid_argument("Not found in tree");

    this->remove(toRemove);
}

template <typename T> uint32_t binary_tree<T>::size() const noexcept { return _size; }

template <typename T>
binary_tree<T>::node::node(const T& item, node* parent, node* left, node* right)
    : item(item)
    , parent(parent)
    , left(left)
    , right(right)
{
    if (left)
        left->parent = this;
    if (right)
        right->parent = this;
}

template <typename T> binary_tree<T>::node::~node() noexcept
{
    delete left;
    delete right;
}

template <typename T> binary_tree<T>::node::node(node&& src) noexcept { *this = std::move(src); }

template <typename T>
const typename binary_tree<T>::node& binary_tree<T>::node::operator=(node&& rhs) noexcept
{
    if (this != &rhs) {
        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
        item = std::move(rhs.item);
        std::swap(left, rhs.left);
        std::swap(right, rhs.right);

        if (parent != nullptr) {
            if (parent->left == this) {
                parent->left = nullptr;
            } else {
                parent->right = nullptr;
            }
        }

        parent = rhs.parent;
        if (parent != nullptr) {
            if (parent->left == &rhs) {
                parent->left = this;
            } else {
                parent->right = this;
            }
        }
    }

    return *this;
}

template <typename T>
typename binary_tree<T>::node* binary_tree<T>::node::change_left(
    typename binary_tree<T>::node* add) noexcept
{
    typename binary_tree<T>::node* ptr = left;
    left = add;

    if (add != nullptr)
        add->parent = this;

    // Middle of complex move?
    if (ptr && (ptr->parent == this))
        ptr->parent = nullptr;
    return ptr;
}

template <typename T>
typename binary_tree<T>::node* binary_tree<T>::node::change_right(
    typename binary_tree<T>::node* add) noexcept
{
    typename binary_tree<T>::node* ptr = right;
    right = add;

    if (add != nullptr)
        add->parent = this;

    // Middle of complex move?
    if (ptr && (ptr->parent == this))
        ptr->parent = nullptr;
    return ptr;
}

template <typename T>
void binary_tree<T>::node::replace_left(typename binary_tree<T>::node* add) noexcept
{
    typename binary_tree<T>::node* prev = change_left(add);
    delete prev;
}

template <typename T>
void binary_tree<T>::node::replace_right(typename binary_tree<T>::node* add) noexcept
{
    typename binary_tree<T>::node* prev = change_right(add);
    delete prev;
}

template <typename T> typename binary_tree<T>::node* binary_tree<T>::node::clone() const
{
    return new node(this->item, nullptr, (this->left) ? this->left->clone() : nullptr,
        (this->right) ? this->right->clone() : nullptr);
}

template <typename T>
typename binary_tree<T>::node* binary_tree<T>::_get_node(typename binary_tree<T>::iterator it)
{
    return it._get_node();
}

template <typename T>
typename binary_tree<T>::node* binary_tree<T>::_get_node(typename binary_tree<T>::const_iterator it)
{
    return it._get_node();
}
}

#endif // BINARY_TREE_CPP