#ifndef BINARY_SEARCH_TREE_CPP
#define BINARY_SEARCH_TREE_CPP
#include <stdexcept>
#include <memory>
#include <vector>

template<typename T, typename Compare>
bool BinarySearchTree<T, Compare>::contains(const T& item) const noexcept {
    return find(item);
}

template<typename T, typename Compare>
typename BinaryTree<T>::BinaryNode* BinarySearchTree<T, Compare>::generate(const std::vector<T>& elements, uint32_t first, uint32_t last, typename BinaryTree<T>::BinaryNode* parent) {
    if (first == last) {
        return nullptr;
    }

    uint32_t middle = (first + last) / 2;
    std::unique_ptr<typename BinaryTree<T>::BinaryNode> root(new typename BinaryTree<T>::BinaryNode(elements[middle], parent));

    if (last - first > 1) {
        root->replaceLeft(generate(elements, first, middle, root.get()));
        root->replaceRight(generate(elements, middle + 1, last, root.get()));
    }
    return root.release();
}

template<typename T, typename Compare>
typename BinaryTree<T>::BinaryNode* BinarySearchTree<T, Compare>::find(const T& item) const noexcept {
    Compare compare = Compare();

    typename BinaryTree<T>::BinaryNode* current = this->root.get();
    while ((current != nullptr) && (current->_item != item))
        current = compare(item, current->_item) ? current->_left : current->_right;
    
    return current;
}

template<typename T, typename Compare>
void BinarySearchTree<T, Compare>::rotate(typename BinaryTree<T>::BinaryNode* upper, bool useLeftChild) {
    auto parent = upper->_parent;
    bool isRoot = (parent == nullptr);
    bool isLeftChild = !isRoot && (upper == parent->_left);

    if ((useLeftChild && upper->_left == nullptr) || (!useLeftChild && upper->_right == nullptr)) {
        throw std::invalid_argument("No such child");
    }

    typename BinaryTree<T>::BinaryNode *newRoot, *toSwap;

    if (useLeftChild) {
        newRoot = upper->_left;
        toSwap = newRoot->_right;
        newRoot->changeRight(upper);
        upper->changeLeft(toSwap);
    }
    else {
        newRoot = upper->_right;
        toSwap = newRoot->_left;
        newRoot->changeLeft(upper);
        upper->changeRight(toSwap);
    }

    if (isRoot) {
        this->root.release();
        this->root.reset(newRoot);
    }
    else if (isLeftChild) {
        parent->changeLeft(newRoot);
    }
    else {
        parent->changeRight(newRoot);
    }
}

#endif // BINARY_SEARCH_TREE_CPP
