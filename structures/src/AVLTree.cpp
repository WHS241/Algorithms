#ifndef AVL_TREE_CPP
#define AVL_TREE_CPP

#include <algorithm>

template<typename T, typename Compare>
typename AVLSearchTree<T, Compare>::AVLNode* AVLSearchTree<T, Compare>::generate(const std::vector<T>& elements, uint32_t first, uint32_t last, typename AVLSearchTree<T, Compare>::AVLNode* parent) {
    if (first == last) {
        return nullptr;
    }

    uint32_t middle = (first + last) / 2;
    std::unique_ptr<AVLNode> root(new AVLNode(elements[middle], parent));

    if (last - first > 1) {
        root->replaceLeft(generate(elements, first, middle, root.get()));
        root->replaceRight(generate(elements, middle + 1, last, root.get()));
    }

    return root.release();
}

template<typename T, typename Compare>
void AVLSearchTree<T, Compare>::insert(const T& item) {
    AVLNode* ptr = dynamic_cast<AVLNode*>(this->root.get());
    if (ptr == nullptr) {
        this->root.reset(new AVLNode(item));
        
        this->_size = 1;
        return;
    }

    bool goLeft = this->compare(item, ptr->_item);
    AVLNode* next = goLeft ? dynamic_cast<AVLNode*>(ptr->_left) : dynamic_cast<AVLNode*>(ptr->_right);
    while (next != nullptr) {
        ptr = next;
        goLeft = this->compare(item, ptr->_item);
        next = goLeft ? dynamic_cast<AVLNode*>(ptr->_left) : dynamic_cast<AVLNode*>(ptr->_right);
    }

    auto add = new AVLNode(item, ptr);
    if (goLeft) {
        ptr->replaceLeft(add);
    }
    else {
        ptr->replaceRight(add);
    }
    ++this->_size;

    balanceTree(add);
}

template<typename T, typename Compare>
void AVLSearchTree<T, Compare>::balanceTree(typename AVLSearchTree<T, Compare>::AVLNode* start) noexcept {
    enum Rotate {Left, Right, None};

    while (start != this->root.get() && start != nullptr) {
        auto leftChild = dynamic_cast<AVLNode*>(start->_left);
        auto rightChild = dynamic_cast<AVLNode*>(start->_right);

        start->leftHeight = leftChild ? std::max(leftChild->leftHeight, leftChild->rightHeight) + 1 : 0;
        start->rightHeight = rightChild ? std::max(rightChild->leftHeight, rightChild->rightHeight) + 1 : 0;

        Rotate rotated = None;
        bool doubleRotate = false;
        if (start->leftHeight + 1 < start->rightHeight) {
            if (rightChild->leftHeight > rightChild->rightHeight) {
                this->rotate(rightChild, true);
                doubleRotate = true;
            }

            this->rotate(start, false);
            rotated = Right;
        }
        else if (start->leftHeight > start->rightHeight + 1) {
            if (leftChild->leftHeight < leftChild->rightHeight) {
                this->rotate(leftChild, false);
                doubleRotate = true;
            }
            this->rotate(start, true);
            rotated = Left;
        }

        if (rotated == Left) {
            if (doubleRotate) {
                rightChild = dynamic_cast<AVLNode*>(leftChild->_right);
                leftChild->rightHeight = rightChild ? std::max(rightChild->leftHeight, rightChild->rightHeight) + 1 : 0;
            }
            leftChild = dynamic_cast<AVLNode*>(start->_left);
            start->leftHeight = leftChild ? std::max(leftChild->leftHeight, leftChild->rightHeight) + 1 : 0;
        }
        else if (rotated == Right) {
            if (doubleRotate) {
                leftChild = dynamic_cast<AVLNode*>(rightChild->_left);
                rightChild->leftHeight = leftChild ? std::max(leftChild->leftHeight, leftChild->rightHeight) + 1 : 0;
            }
            rightChild = dynamic_cast<AVLNode*>(start->_right);
            start->rightHeight = rightChild ? std::max(rightChild->leftHeight, rightChild->rightHeight) + 1 : 0;
        }

        start = dynamic_cast<AVLNode*>(start->_parent);
    }
}

template<typename T, typename C>
typename AVLSearchTree<T, C>::AVLNode* AVLSearchTree<T, C>::AVLNode::changeLeft(typename AVLSearchTree<T, C>::AVLNode* add) noexcept {
    auto ptr = BinaryTree<T>::BinaryNode::changeLeft(add);
    leftHeight = (add != nullptr) ? std::max(add->leftHeight, add->rightHeight) + 1 : 0;
    return dynamic_cast<AVLNode*>(ptr);
}

template<typename T, typename C>
typename AVLSearchTree<T, C>::AVLNode* AVLSearchTree<T, C>::AVLNode::changeRight(typename AVLSearchTree<T, C>::AVLNode* add) noexcept {
    auto ptr = BinaryTree<T>::BinaryNode::changeRight(add);
    rightHeight = (add != nullptr) ? std::max(add->leftHeight, add->rightHeight) + 1 : 0;
    return dynamic_cast<AVLNode*>(ptr);
}

template<typename T, typename C>
void AVLSearchTree<T, C>::AVLNode::replaceLeft(typename AVLSearchTree<T, C>::AVLNode* add) noexcept {
    auto prev = changeLeft(add);
    delete prev;
}

template<typename T, typename C>
void AVLSearchTree<T, C>::AVLNode::replaceRight(typename AVLSearchTree<T, C>::AVLNode* add) noexcept {
    auto prev = changeRight(add);
    delete prev;
}

template<typename T, typename C>
void AVLSearchTree<T, C>::remove(typename BinaryTree<T>::iterator it) {
    auto node = BinaryTree<T>::getNode(it);
    if (node == nullptr)
        return;
    if (this->_size == 1) {
        this->root.reset();
        this->_size = 0;
        return;
    }

    auto parent = dynamic_cast<AVLNode*>(node->_parent);
    bool isRoot = parent == nullptr;
    bool leftChild = !isRoot && (node == parent->_left);

    // Found node to switch with: pass to lambda to actually perform the switch and delete
    auto swapOut = [&parent, &node, this, leftChild, isRoot](AVLNode* ptr) {
        if (ptr != nullptr) {
            auto nodeParent(dynamic_cast<AVLNode*>(ptr->_parent));
            if (nodeParent == node) {

                if (ptr == node->_right) {
                    // must be single child: no further action
                    node->_right = nullptr;
                }
                else {
                    if (!ptr->_right) {
                        // may be successor (root has two children): move other branch over
                        ptr->replaceRight(dynamic_cast<AVLNode*>(node->_right));
                        node->_right = nullptr;
                    }
                    node->_left = nullptr;
                }
            }
            else {
                // must be successor to double-child root
                nodeParent->changeRight(dynamic_cast<AVLNode*>(ptr->_left));
                ptr->changeLeft(dynamic_cast<AVLNode*>(node->_left));
                ptr->replaceRight(dynamic_cast<AVLNode*>(node->_right));
                node->_left = nullptr;
                node->_right = nullptr;
            }
        }

        if (isRoot) {
            this->root.reset(ptr);
            if (ptr)
                ptr->_parent = nullptr;
        }
        else if (leftChild) {
            parent->replaceLeft(ptr);
        }
        else {
            parent->replaceRight(ptr);
        }
        --this->_size;
    };

    // Leaf: only need to delete
    // Single child: move up
    if (!node->_left) {
        swapOut(dynamic_cast<AVLNode*>(node->_right));
        balanceTree(parent);
        return;
    }
    if (!node->_right) {
        swapOut(dynamic_cast<AVLNode*>(node->_left));
        balanceTree(parent);
        return;
    }

    // Two children: must determine successor
    auto successor = dynamic_cast<AVLNode*>(node->_left);
    while (successor->_right)
        successor = dynamic_cast<AVLNode*>(successor->_right);

    auto successorParent = dynamic_cast<AVLNode*>(successor->_parent);
    bool directShift(node == successorParent);
    swapOut(successor);
    if (!directShift) {
        balanceTree(successorParent);
    }
    else {
        balanceTree(successor);
    }
}

template<typename T, typename C>
AVLSearchTree<T, C>::AVLNode::AVLNode(const T& item, AVLNode* parent, AVLNode* left, AVLNode* right)
    : BinaryTree<T>::BinaryNode(item, parent, left, right)
    , leftHeight(0)
    , rightHeight(0)
{
    if (left != nullptr) {
        leftHeight = std::max(left->leftHeight, left->rightHeight) + 1;
    }
    if (right != nullptr) {
        rightHeight = std::max(right->leftHeight, right->rightHeight) + 1;
    }
}

#endif //AVL_TREE_CPP