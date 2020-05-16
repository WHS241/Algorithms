#ifndef PRIMITIVE_BINARY_SEARCH_TREE_CPP
#define PRIMITIVE_BINARY_SEARCH_TREE_CPP

namespace tree {

template <typename T, typename Compare>
template <typename, typename>
basic_binary_search_tree<T, Compare>::basic_binary_search_tree(bool duplicates)
    : binary_search_tree<T, Compare>(duplicates)
{
}

template <typename T, typename Compare>
basic_binary_search_tree<T, Compare>::basic_binary_search_tree(const Compare& comp, bool duplicates)
    : basic_binary_search_tree(comp, duplicates) {};

template <typename T, typename Compare>
template <typename It, typename, typename>
basic_binary_search_tree<T, Compare>::basic_binary_search_tree(It first, It last, bool duplicates)
    : binary_search_tree<T, Compare>(first, last, duplicates)
{
}

template <typename T, typename Compare>
template <typename It>
basic_binary_search_tree<T, Compare>::basic_binary_search_tree(
    It first, It last, const Compare& comp, bool duplicates)
    : binary_search_tree<T, Compare>(first, last, comp, duplicates)
{
}

template <typename T, typename Compare>
std::pair<typename basic_binary_search_tree<T, Compare>::iterator, bool>
basic_binary_search_tree<T, Compare>::insert(const T& item)
{
    typename binary_tree<T>::node* ptr = this->_root.get();
    if (ptr == nullptr) {
        this->_root.reset(new typename binary_tree<T>::node(item));
        this->_size = 1;
        return { this->begin(), true };
    }

    bool go_left = this->_compare(item, ptr->item);
    typename binary_tree<T>::node* next = go_left ? ptr->left : ptr->right;
    while (next != nullptr) {
        ptr = next;
        if (!this->_allow_duplicates && !this->_compare(ptr->item, item)
            && !this->_compare(item, ptr->item))
            return { this->_make_iterator(ptr, traversal::in_order, false), false };
        go_left = this->_compare(item, ptr->item);
        next = go_left ? ptr->left : ptr->right;
    }

    auto created = new typename binary_tree<T>::node(item, ptr);
    if (go_left) {
        ptr->replace_left(created);
    } else {
        ptr->replace_right(created);
    }
    ++this->_size;
    return { this->_make_iterator(created, traversal::in_order), true };
}

template <typename T, typename Compare>
typename basic_binary_search_tree<T, Compare>::iterator basic_binary_search_tree<T, Compare>::erase(
    typename binary_tree<T>::iterator it)
{
    binary_search_tree<T, Compare>::_verify(it);
    typename binary_tree<T>::node* node = binary_tree<T>::_s_get_node(it);
    if (node == nullptr)
        return this->end();

    ++it; // final return value

    typename binary_tree<T>::node* parent = node->parent;
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
        return it;
    }
    if (!node->right) {
        swap_out(node->left);
        return it;
    }

    // Two children: must determine successor
    typename binary_tree<T>::node* successor = node->left;
    while (successor->right)
        successor = successor->right;

    swap_out(successor);
    return it;
}
}

#endif // PRIMITIVE_BINARY_SEARCH_TREE_CPP