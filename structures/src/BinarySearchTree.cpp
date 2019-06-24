#ifndef BINARY_SEARCH_TREE_CPP
#define BINARY_SEARCH_TREE_CPP

#include <algorithm>
#include <iterator>
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
    root->addLeft(generate(elements, first, middle, root.get()));
    root->addRight(generate(elements, middle + 1, last, root.get()));
    return root.release();
}

template<typename T, typename Compare>
typename BinaryTree<T>::BinaryNode* BinarySearchTree<T, Compare>::find(const T& item) const noexcept {
    Compare compare = Compare();

    typename BinaryTree<T>::BinaryNode* current = this->root.get();
    while ((current != nullptr) && (current->_item != item))
        current = compare(item, current->_item) ? current->_left.get() : current->_right.get();
    
    return current;
}

#endif // BINARY_SEARCH_TREE_CPP
