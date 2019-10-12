#ifndef AVL_TREE_CPP
#define AVL_TREE_CPP

#include <algorithm>

namespace tree {
template <typename T, typename Compare>
typename AVL_tree<T, Compare>::t_node* AVL_tree<T, Compare>::generate(
    const std::vector<T>& elements, uint32_t first, uint32_t last,
    typename AVL_tree<T, Compare>::t_node* parent)
{
    if (first == last) {
        return nullptr;
    }

    uint32_t middle = (first + last) / 2;
    std::unique_ptr<t_node> root(new t_node(elements[middle], parent));

    if (last - first > 1) {
        root->replace_left(generate(elements, first, middle, root.get()));
        root->replace_right(generate(elements, middle + 1, last, root.get()));
    }

    return root.release();
}

template <typename T, typename Compare> void AVL_tree<T, Compare>::insert(const T& item)
{
    t_node* ptr = dynamic_cast<t_node*>(this->_root.get());
    if (ptr == nullptr) {
        this->_root.reset(new t_node(item));
        this->_size = 1;
        return;
    }

    bool go_left = this->_compare(item, ptr->item);
    t_node* next = go_left ? dynamic_cast<t_node*>(ptr->left) : dynamic_cast<t_node*>(ptr->right);
    while (next != nullptr) {
        ptr = next;
        go_left = this->_compare(item, ptr->item);
        next = go_left ? dynamic_cast<t_node*>(ptr->left) : dynamic_cast<t_node*>(ptr->right);
    }

    auto add = new t_node(item, ptr);
    if (go_left) {
        ptr->replace_left(add);
    } else {
        ptr->replace_right(add);
    }
    ++this->_size;

    balance_tree(add);
}

template <typename T, typename Compare>
void AVL_tree<T, Compare>::balance_tree(typename AVL_tree<T, Compare>::t_node* start) noexcept
{
    enum rotate { left, right, none };

    while (start != this->_root.get() && start != nullptr) {
        auto left_child = dynamic_cast<t_node*>(start->left);
        auto right_child = dynamic_cast<t_node*>(start->right);

        start->left_height
            = left_child ? std::max(left_child->left_height, left_child->right_height) + 1 : 0;
        start->right_height
            = right_child ? std::max(right_child->left_height, right_child->right_height) + 1 : 0;

        rotate rotated = none;
        bool double_rotate = false;
        if (start->left_height + 1 < start->right_height) {
            if (right_child->left_height > right_child->right_height) {
                this->_rotate(right_child, true);
                double_rotate = true;
            }

            this->_rotate(start, false);
            rotated = right;
        } else if (start->left_height > start->right_height + 1) {
            if (left_child->left_height < left_child->right_height) {
                this->_rotate(left_child, false);
                double_rotate = true;
            }
            this->_rotate(start, true);
            rotated = left;
        }

        if (rotated == left) {
            if (double_rotate) {
                right_child = dynamic_cast<t_node*>(left_child->right);
                left_child->right_height = right_child
                    ? std::max(right_child->left_height, right_child->right_height) + 1
                    : 0;
            }
            left_child = dynamic_cast<t_node*>(start->left);
            start->left_height
                = left_child ? std::max(left_child->left_height, left_child->right_height) + 1 : 0;
        } else if (rotated == right) {
            if (double_rotate) {
                left_child = dynamic_cast<t_node*>(right_child->left);
                right_child->left_height = left_child
                    ? std::max(left_child->left_height, left_child->right_height) + 1
                    : 0;
            }
            right_child = dynamic_cast<t_node*>(start->right);
            start->right_height = right_child
                ? std::max(right_child->left_height, right_child->right_height) + 1
                : 0;
        }

        start = dynamic_cast<t_node*>(start->parent);
    }
}

template <typename T, typename C>
typename AVL_tree<T, C>::t_node* AVL_tree<T, C>::t_node::change_left(
    typename AVL_tree<T, C>::t_node* add) noexcept
{
    auto ptr = binary_tree<T>::node::change_left(add);
    left_height = (add != nullptr) ? std::max(add->left_height, add->right_height) + 1 : 0;
    return dynamic_cast<t_node*>(ptr);
}

template <typename T, typename C>
typename AVL_tree<T, C>::t_node* AVL_tree<T, C>::t_node::change_right(
    typename AVL_tree<T, C>::t_node* add) noexcept
{
    auto ptr = binary_tree<T>::node::change_right(add);
    right_height = (add != nullptr) ? std::max(add->left_height, add->right_height) + 1 : 0;
    return dynamic_cast<t_node*>(ptr);
}

template <typename T, typename C>
void AVL_tree<T, C>::t_node::replace_left(typename AVL_tree<T, C>::t_node* add) noexcept
{
    auto prev = change_left(add);
    delete prev;
}

template <typename T, typename C>
void AVL_tree<T, C>::t_node::replace_right(typename AVL_tree<T, C>::t_node* add) noexcept
{
    auto prev = change_right(add);
    delete prev;
}

template <typename T, typename C> void AVL_tree<T, C>::remove(typename binary_tree<T>::iterator it)
{
    binary_search_tree<T, C>::_verify(it);
    auto node = binary_tree<T>::_get_node(it);
    if (node == nullptr)
        return;
    if (this->_size == 1) {
        this->_root.reset();
        this->_size = 0;
        return;
    }

    auto parent = dynamic_cast<t_node*>(node->parent);
    bool is_root = parent == nullptr;
    bool is_left_child = !is_root && (node == parent->left);

    // Found node to switch with: pass to lambda to actually perform the switch
    // and delete
    auto swap_out = [&parent, &node, this, is_left_child, is_root](t_node* ptr) {
        if (ptr != nullptr) {
            auto node_parent(dynamic_cast<t_node*>(ptr->parent));
            if (node_parent == node) {

                if (ptr == node->right) {
                    // must be single child: no further action
                    node->right = nullptr;
                } else {
                    if (!ptr->right) {
                        // may be successor (root has two children): move other branch over
                        ptr->replace_right(dynamic_cast<t_node*>(node->right));
                        node->right = nullptr;
                    }
                    node->left = nullptr;
                }
            } else {
                // must be successor to double-child root
                node_parent->change_right(dynamic_cast<t_node*>(ptr->left));
                ptr->change_left(dynamic_cast<t_node*>(node->left));
                ptr->replace_right(dynamic_cast<t_node*>(node->right));
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
        swap_out(dynamic_cast<t_node*>(node->right));
        balance_tree(parent);
        return;
    }
    if (!node->right) {
        swap_out(dynamic_cast<t_node*>(node->left));
        balance_tree(parent);
        return;
    }

    // Two children: must determine successor
    auto successor = dynamic_cast<t_node*>(node->left);
    while (successor->right)
        successor = dynamic_cast<t_node*>(successor->right);

    auto successor_parent = dynamic_cast<t_node*>(successor->parent);
    bool direct_shift(node == successor_parent);
    swap_out(successor);
    if (!direct_shift) {
        balance_tree(successor_parent);
    } else {
        balance_tree(successor);
    }
}

template <typename T, typename C>
AVL_tree<T, C>::t_node::t_node(const T& item, t_node* parent, t_node* left, t_node* right)
    : binary_tree<T>::node(item, parent, left, right)
    , left_height(0)
    , right_height(0)
{
    if (left != nullptr) {
        left_height = std::max(left->left_height, left->right_height) + 1;
    }
    if (right != nullptr) {
        right_height = std::max(right->left_height, right->right_height) + 1;
    }
}
}

#endif // AVL_TREE_CPP