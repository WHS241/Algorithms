#ifndef NODE_HEAP_CPP
#define NODE_HEAP_CPP

namespace heap {
template <typename T, typename Compare> const T& node_base<T, Compare>::node::operator*() const
{
    return _value;
}

template <typename T, typename Compare> node_base<T, Compare>::node::~node() noexcept
{
    for (node* child : _children)
        delete child;
}

template <typename T, typename Compare> T& node_base<T, Compare>::node::operator*()
{
    return const_cast<T&>(static_cast<const node*>(this)->operator*());
}

template <typename T, typename Compare> const T* node_base<T, Compare>::node::operator->() const
{
    return &_value;
}

template <typename T, typename Compare> T* node_base<T, Compare>::node::operator->()
{
    return const_cast<T*>(static_cast<const node*>(this)->operator->());
}

template <typename T, typename Compare>
typename node_base<T, Compare>::node* node_base<T, Compare>::node::_deep_clone() const
{
    std::unique_ptr<node> clone(new node(_value));
    for (auto child : _children) {
        std::unique_ptr<node> child_node = std::unique_ptr<node>(child->_deep_clone());
        clone->_children.push_back(child_node.get());
        child_node->_parent = clone.get();
        child_node.release();
    }
    return clone.release();
}

template <typename T, typename Compare> void node_base<T, Compare>::insert(const T& item)
{
    this->add(item);
}

template <typename T, typename Compare> uint32_t node_base<T, Compare>::size() const noexcept
{
    return _size;
}

template <typename T, typename Compare>
typename node_base<T, Compare>::node* node_base<T, Compare>::s_make_node(const T& item)
{
    return new node(item);
}
}

#endif // NODE_HEAP_CPP
