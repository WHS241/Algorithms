#ifndef TREE_NODE_CPP
#define TREE_NODE_CPP

template<typename T>
BinaryNode<T>::BinaryNode(T item, BinaryNode<T>* left, BinaryNode<T>* right, BinaryNode<T>* parent)
    : _item(item)
    , left(left)
    , right(right)
    , parent(parent)
{
    if (left)
        left->parent = this;
    if (right)
        right->parent = this;
}

template<typename T>
T BinaryNode<T>::item() const {
    return _item;
}

template<typename T>
void BinaryNode<T>::addLeft(std::unique_ptr<BinaryNode<T>>&& child) {
    if(child)
        child->parent = this;
    left = std::move(child);
}

template<typename T>
void BinaryNode<T>::addRight(std::unique_ptr<BinaryNode<T>>&& child) {
    if(child)
        child->parent = this;
    right = std::move(child);
}

#endif //TREE_NODE_CPP