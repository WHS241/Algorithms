#ifndef BINARY_TREE_CPP
#define BINARY_TREE_CPP

#include <stdexcept>

#include <structures/binary_tree_iterator.h>

namespace tree {

template <typename T> binary_tree<T>::node::~node() noexcept
{
    delete left;
    delete right;
}

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
binary_tree<T>::binary_tree() noexcept
    : _root()
    , _size(0)
{
}

template <typename T>
binary_tree<T>::binary_tree(const binary_tree<T>& src)
    : _root(src._root->clone())
    , _size(src._size)
{
}

template <typename T> binary_tree<T>& binary_tree<T>::operator=(const binary_tree<T>& src)
{
    if (this != &src) {
        _root.reset(src._root->clone());
        _size = src._size;
    }
    return *this;
}

template <typename T> typename binary_tree<T>::iterator binary_tree<T>::begin(traversal tr)
{
    return _make_iterator(_root.get(), tr, true);
}

template <typename T> typename binary_tree<T>::iterator binary_tree<T>::end(traversal tr)
{
    return _make_iterator(nullptr, tr, false);
}

template <typename T>
typename binary_tree<T>::const_iterator binary_tree<T>::cbegin(traversal tr) const
{
    return _make_const_iterator(_root.get(), tr, true);
}

template <typename T>
typename binary_tree<T>::const_iterator binary_tree<T>::cend(traversal tr) const
{
    return _make_const_iterator(nullptr, tr, false);
}

template <typename T> uint32_t binary_tree<T>::erase(const T& item)
{
    iterator toRemove = _make_iterator(const_cast<node*>(_find(item)), pre_order, false);
    if (toRemove == end(traversal::pre_order))
        return 0;
    uint32_t prev_size = this->size();
    this->erase(toRemove);
    return prev_size - this->size();
}

template <typename T>
typename binary_tree<T>::iterator binary_tree<T>::erase(typename binary_tree<T>::const_iterator it)
{
    iterator copy = _make_iterator(const_cast<node*>(it._get_node()), it._trav, false);
    return this->erase(copy);
}

template <typename T> bool binary_tree<T>::empty() const noexcept { return _size == 0; }

template <typename T> uint32_t binary_tree<T>::size() const noexcept { return _size; }

template <typename T> void binary_tree<T>::clear() noexcept
{
    _root.reset();
    _size = 0;
}

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
    std::unique_ptr<node> left_child((this->left) ? this->left->clone() : nullptr);
    std::unique_ptr<node> right_child((this->right) ? this->right->clone() : nullptr);

    auto ret = new node(this->item, nullptr, left_child.get(), right_child.get());
    left_child.release();
    right_child.release();
    return ret;
}

template <typename T>
typename binary_tree<T>::node* binary_tree<T>::_s_get_node(typename binary_tree<T>::iterator it)
{
    return it._get_node();
}

template <typename T>
const typename binary_tree<T>::node* binary_tree<T>::_s_get_node(
    typename binary_tree<T>::const_iterator it)
{
    return it._get_node();
}

template <typename T>
typename binary_tree<T>::iterator binary_tree<T>::_make_iterator(
    typename binary_tree<T>::node* n, traversal t, bool entire_subtree)
{
    return iterator(n, t, entire_subtree, this);
}

template <typename T>
typename binary_tree<T>::const_iterator binary_tree<T>::_make_const_iterator(
    const typename binary_tree<T>::node* n, traversal t, bool entire_subtree) const
{
    return const_iterator(n, t, entire_subtree, this);
}

template <typename T> void binary_tree<T>::_verify(typename binary_tree<T>::iterator check) const
{
    if (check._tree != this)
        throw std::invalid_argument("Not in current tree");
}

template <typename T>
void binary_tree<T>::_verify(typename binary_tree<T>::const_iterator check) const
{
    if (check._tree != this)
        throw std::invalid_argument("Not in current tree");
}

}

#endif // BINARY_TREE_CPP
