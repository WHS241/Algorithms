#ifndef PRIMITIVE_BINARY_SEARCH_TREE_CPP
#define PRIMITIVE_BINARY_SEARCH_TREE_CPP

template<typename T, typename Compare>
void PrimitiveBinarySearchTree<T, Compare>::insert(const T& item) {
    BinaryNode* ptr = root.get();
    if (ptr == nullptr) {
        root.reset(new BinaryNode(item));
        return;
    }

    bool goLeft = compare(item, *ptr);
    BinaryNode* next = goLeft ? ptr->_left.get() : ptr->_right.get();
    while (next != nullptr) {
        ptr = next;
        goLeft = compare(item, *ptr);
        BinaryNode* next = goLeft ? ptr->_left.get() : ptr->_right.get();
    }

    if (goLeft) {
        ptr.addLeft(new BinaryNode(item, ptr));
    }
    else {
        ptr.addRight(new BinaryNode(item, ptr));
    }
}

#endif //PRIMITIVE_BINARY_SEARCH_TREE_CPP