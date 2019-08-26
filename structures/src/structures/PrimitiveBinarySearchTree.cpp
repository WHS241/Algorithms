#ifndef PRIMITIVE_BINARY_SEARCH_TREE_CPP
#define PRIMITIVE_BINARY_SEARCH_TREE_CPP

template <typename T, typename Compare>
void PrimitiveBinarySearchTree<T, Compare>::insert(const T& item)
{
    typename BinaryTree<T>::BinaryNode* ptr = this->root.get();
    if (ptr == nullptr) {
        this->root.reset(new typename BinaryTree<T>::BinaryNode(item));
        this->_size = 1;
        return;
    }

    bool goLeft = this->compare(item, ptr->_item);
    typename BinaryTree<T>::BinaryNode* next = goLeft ? ptr->_left : ptr->_right;
    while (next != nullptr) {
        ptr = next;
        goLeft = this->compare(item, ptr->_item);
        next = goLeft ? ptr->_left : ptr->_right;
    }

    if (goLeft) {
        ptr->replaceLeft(new typename BinaryTree<T>::BinaryNode(item, ptr));
    } else {
        ptr->replaceRight(new typename BinaryTree<T>::BinaryNode(item, ptr));
    }
    ++this->_size;
}

template <typename T, typename Compare>
void PrimitiveBinarySearchTree<T, Compare>::remove(typename BinaryTree<T>::iterator it)
{
    BinarySearchTree<T, Compare>::verify(it);
    auto node = BinaryTree<T>::getNode(it);
    if (node == nullptr)
        return;

    auto parent = node->_parent;
    bool isRoot = parent == nullptr;
    bool leftChild = !isRoot && (node == parent->_left);

    // Found node to switch with: pass to lambda to actually perform the switch
    // and delete
    auto swapOut
        = [&parent, &node, this, leftChild, isRoot](typename BinaryTree<T>::BinaryNode* ptr) {
              if (ptr != nullptr) {
                  if (ptr->_parent == node) {

                      if (ptr == node->_right) {
                          // must be single child: no further action
                          node->_right = nullptr;
                      } else {
                          if (!ptr->_right) {
                              // may be successor (root has two children): move other branch over
                              ptr->replaceRight(node->_right);
                              node->_right = nullptr;
                          }
                          node->_left = nullptr;
                      }
                  } else {
                      // must be successor to double-child root
                      ptr->_parent->changeRight(ptr->_left);
                      ptr->changeLeft(node->_left);
                      ptr->replaceRight(node->_right);
                      node->_left = nullptr;
                      node->_right = nullptr;
                  }
              }

              if (isRoot) {
                  this->root.reset(ptr);
                  if (ptr)
                      ptr->_parent = nullptr;
              } else if (leftChild) {
                  parent->replaceLeft(ptr);
              } else {
                  parent->replaceRight(ptr);
              }
              --this->_size;
          };

    // Leaf: only need to delete
    // Single child: move up
    if (!node->_left) {
        swapOut(node->_right);
        return;
    }
    if (!node->_right) {
        swapOut(node->_left);
        return;
    }

    // Two children: must determine successor
    auto successor = node->_left;
    while (successor->_right)
        successor = successor->_right;

    swapOut(successor);
}

#endif // PRIMITIVE_BINARY_SEARCH_TREE_CPP