#ifndef NODE_BINARY_HEAP_CPP
#define NODE_BINARY_HEAP_CPP
#include <algorithm>

template <typename T, typename Compare>
NodeBinaryHeap<T, Compare>::NodeBinaryHeap(Compare comp)
    : NodeHeap<T, Compare>(comp)
{
}

template <typename T, typename Compare>
NodeBinaryHeap<T, Compare>::NodeBinaryHeap(const NodeBinaryHeap<T, Compare>& src)
    : NodeHeap<T, Compare>(src)
    , root()
{
    if (!src.empty())
        root.reset(src.root->deepClone());
}

template <typename T, typename Compare>
NodeBinaryHeap<T, Compare>& NodeBinaryHeap<T, Compare>::operator=(
    const NodeBinaryHeap<T, Compare>& rhs)
{
    if (this != &rhs) {
        NodeBinaryHeap<T, Compare> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T, typename Compare>
NodeBinaryHeap<T, Compare>::NodeBinaryHeap(NodeBinaryHeap<T, Compare>&& src)
    : NodeBinaryHeap<T, Compare>(src.compare)
{
    *this = std::move(src);
}

template <typename T, typename Compare>
NodeBinaryHeap<T, Compare>& NodeBinaryHeap<T, Compare>::operator=(NodeBinaryHeap<T, Compare>&& rhs)
{
    if (this != &rhs) {
        root.reset();
        this->_size = 0;
        root.swap(rhs.root);
        std::swap(this->_size, rhs._size);
        std::swap(this->compare, rhs.compare);
    }
    return *this;
}

template <typename T, typename Compare>
typename NodeBinaryHeap<T, Compare>::Node* NodeBinaryHeap<T, Compare>::add(const T& item)
{
    std::unique_ptr<Node> newNode(new Node(item));

    if (this->_size == 0) {
        root = std::move(newNode);
        ++this->_size;
        return root.get();
    }
    // use a stack of bools to determine the direction to go to parent
    std::stack<bool> goLeft;
    for (uint32_t i = (this->_size + 1) / 2; i != 1; i /= 2) {
        goLeft.push(i % 2 == 0);
    }
    Node* parent = root.get();

    while (!goLeft.empty()) {
        parent = goLeft.top() ? parent->children.front() : parent->children.back();
        goLeft.pop();
    }

    Node* ptr = newNode.get();
    parent->children.push_back(ptr);
    ptr->parent = parent;
    newNode.release();
    ++this->_size;

    // bubble up to correct position
    while (ptr != root.get()) {
        parent = ptr->parent;
        bool leftChild = (parent->children.front() == ptr);
        if (this->compare(**ptr, **parent))
            bubbleDown(parent, leftChild);
        else
            break;
    }

    return ptr;
}

template <typename T, typename Compare>
void NodeBinaryHeap<T, Compare>::merge(NodeBinaryHeap<T, Compare>& src)
{
    if (this == &src)
        return;

    std::list<T> dataset = data(this->root.get());
    dataset.splice(dataset.end(), data(src.root.get()));
    *this = NodeBinaryHeap(dataset.begin(), dataset.end(), this->compare);
    src.root.reset();
    src._size = 0;
}

template <typename T, typename Compare> T NodeBinaryHeap<T, Compare>::getRoot() const
{
    if (this->empty())
        throw std::underflow_error("Empty heap");
    return **root;
}

template <typename T, typename Compare> T NodeBinaryHeap<T, Compare>::removeRoot()
{
    if (this->empty())
        throw std::underflow_error("Empty heap");

    T retVal = std::move(**root);

    // edge case
    if (this->_size == 1) {
        this->root.reset();
        this->_size = 0;
        return retVal;
    }

    // find last node, the same way as in add
    std::stack<bool> goLeft;
    for (uint32_t i = this->_size; i != 1; i /= 2)
        goLeft.push(i % 2 == 0);

    Node* final = this->root.get();
    while (!goLeft.empty()) {
        final = goLeft.top() ? final->children.front() : final->children.back();
        goLeft.pop();
    }

    // swap node positions
    Node* toRemove = this->root.release();
    final->children.swap(toRemove->children);
    for (Node* child : final->children) {
        child->parent = final;
    }
    Node* oldParent = final->parent;
    final->parent = nullptr;
    auto it = std::find(oldParent->children.begin(), oldParent->children.end(), final);
    auto temp = it++;
    oldParent->children.erase(temp, it);
    this->root.reset(final);

    delete toRemove;
    toRemove = nullptr;
    --this->_size;

    // now the bubble-down
    while (!final->children.empty()) {
        Node* leftChild = final->children.front();
        Node* rightChild = final->children.back();
        bool swapLeft = this->compare(**leftChild, **rightChild);
        Node* comparePtr = swapLeft ? leftChild : rightChild;
        if (!this->compare(**final, **comparePtr))
            bubbleDown(final, swapLeft);
        else
            break;
    }

    return retVal;
}

template <typename T, typename Compare>
void NodeBinaryHeap<T, Compare>::decrease(
    typename NodeBinaryHeap<T, Compare>::Node* target, const T& newVal)
{
    if (this->compare(**target, newVal))
        throw std::invalid_argument("Increasing key");

    **target = newVal;
    // bubble up
    while (target != root.get()) {
        Node* parent = target->parent;
        bool leftChild = (parent->children.front() == target);
        if (this->compare(**target, **parent))
            bubbleDown(parent, leftChild);
        else
            break;
    }
}

template <typename T, typename Compare>
void NodeBinaryHeap<T, Compare>::bubbleDown(
    typename NodeBinaryHeap<T, Compare>::Node* parent, bool useLeftChild) noexcept
{
    // move all pointers in/out of parent and bubbling child,
    // except grandparent pointer (handled below)
    Node* toSwap = useLeftChild ? parent->children.front() : parent->children.back();
    toSwap->parent = parent->parent;
    parent->parent = toSwap;
    parent->children.swap(toSwap->children);
    if (useLeftChild) {
        toSwap->children.front() = parent;
        toSwap->children.back()->parent = toSwap;
    } else {
        toSwap->children.back() = parent;
        toSwap->children.front()->parent = toSwap;
    }
    for (Node* child : parent->children)
        child->parent = parent;

    // re-point grandparent or root pointer
    if (parent == this->root.get()) {
        this->root.release();
        this->root.reset(toSwap);
    } else {
        Node* grandparent = toSwap->parent;
        auto it = std::find(grandparent->children.begin(), grandparent->children.end(), parent);
        *it = toSwap;
    }
}

template <typename T, typename Compare>
std::list<T> NodeBinaryHeap<T, Compare>::data(const Node* root)
{
    std::list<T> result;
    if (root != nullptr) {
        result.push_back(**root);
        for (auto child : root->children)
            result.splice(result.end(), data(child));
    }

    return result;
}

#endif // !NODE_BINARY_HEAP_CPP
