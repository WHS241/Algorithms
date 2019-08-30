#ifndef NODE_HEAP_CPP
#define NODE_HEAP_CPP

template <typename T, typename Compare> const T& NodeHeap<T, Compare>::Node::operator*() const
{
    return value;
}

template <typename T, typename Compare> NodeHeap<T, Compare>::Node::~Node() noexcept
{
    for (Node* child : children)
        delete child;
}

template <typename T, typename Compare> T& NodeHeap<T, Compare>::Node::operator*()
{
    return const_cast<T&>(static_cast<const Node*>(this)->operator*());
}

template <typename T, typename Compare> const T* NodeHeap<T, Compare>::Node::operator->() const
{
    return &value;
}

template <typename T, typename Compare> T* NodeHeap<T, Compare>::Node::operator->()
{
    return const_cast<T*>(static_cast<const Node*>(this)->operator->());
}

template <typename T, typename Compare>
typename NodeHeap<T, Compare>::Node* NodeHeap<T, Compare>::Node::deepClone() const
{
    std::unique_ptr<Node> clone(new Node(value));
    for (auto child : children) {
        std::unique_ptr<Node> childNode = std::unique_ptr<Node>(child->deepClone());
        clone->children.push_back(childNode.get());
        childNode->parent = clone.get();
        childNode.release();
    }
    return clone.release();
}

template <typename T, typename Compare> void NodeHeap<T, Compare>::insert(const T& item)
{
    this->add(item);
}

template <typename T, typename Compare> uint32_t NodeHeap<T, Compare>::size() const noexcept
{
    return _size;
}

template <typename T, typename Compare>
typename NodeHeap<T, Compare>::Node* NodeHeap<T, Compare>::makeNode(const T& item)
{
    return new Node(item);
}

#endif // NODE_HEAP_CPP
