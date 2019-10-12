#ifndef PRIMITIVE_BINARY_SEARCH_TREE_CPP
#define PRIMITIVE_BINARY_SEARCH_TREE_CPP

namespace tree {
template <typename T, typename Compare>
void basic_binary_search_tree<T, Compare>::insert(const T& item)
{
    typename binary_tree<T>::node* ptr = this->_root.get();
    if (ptr == nullptr) {
        this->_root.reset(new typename binary_tree<T>::node(item));
        this->_size = 1;
        return;
    }

    bool go_left = this->_compare(item, ptr->item);
    typename binary_tree<T>::node* next = go_left ? ptr->left : ptr->right;
    while (next != nullptr) {
        ptr = next;
        go_left = this->_compare(item, ptr->item);
        next = go_left ? ptr->left : ptr->right;
    }

    if (go_left) {
        ptr->replace_left(new typename binary_tree<T>::node(item, ptr));
    } else {
        ptr->replace_right(new typename binary_tree<T>::node(item, ptr));
    }
    ++this->_size;
}

template <typename T, typename Compare>
void basic_binary_search_tree<T, Compare>::remove(typename binary_tree<T>::iterator it)
{
    binary_search_tree<T, Compare>::_verify(it);
    auto node = binary_tree<T>::_get_node(it);
    if (node == nullptr)
        return;

    auto parent = node->parent;
    bool is_root = parent == nullptr;
    bool is_left_child = !is_root && (node == parent->left);

    // Found node to switch with: pass to lambda to actually perform the switch
    // and delete
    auto swap_out
        = [&parent, &node, this, is_left_child, is_root](typename binary_tree<T>::node* ptr) {
              if (ptr != nullptr) {
                  if (ptr->parent == node) {

                      if (ptr == node->right) {
                          // must be single child: no further action
                          node->right = nullptr;
                      } else {
                          if (!ptr->right) {
                              // may be successor (root has two children): move other branch over
                              ptr->replace_right(node->right);
                              node->right = nullptr;
                          }
                          node->left = nullptr;
                      }
                  } else {
                      // must be successor to double-child root
                      ptr->parent->change_right(ptr->left);
                      ptr->change_left(node->left);
                      ptr->replace_right(node->right);
                      node->left = nullptr;
                      node->right = nullptr;
                  }
              }

              if (is_root) {
                  this->_root.reset(ptr);
                  if (ptr)
                      ptr->parent = nullptr;
              } else if (is_left_child) {
                  parent->replace_left(ptr);
              } else {
                  parent->replace_right(ptr);
              }
              --this->_size;
          };

    // Leaf: only need to delete
    // Single child: move up
    if (!node->left) {
        swap_out(node->right);
        return;
    }
    if (!node->right) {
        swap_out(node->left);
        return;
    }

    // Two children: must determine successor
    auto successor = node->left;
    while (successor->right)
        successor = successor->right;

    swap_out(successor);
}
}

#endif // PRIMITIVE_BINARY_SEARCH_TREE_CPP