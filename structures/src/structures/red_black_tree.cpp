#ifndef RED_BLACK_TREE_CPP
#define RED_BLACK_TREE_CPP

namespace tree {
    template<typename T, typename Compare>
    void red_black_tree<T, Compare>::insert(const T &item) {
        auto *ptr = dynamic_cast<t_node *>(this->root.get());
        if (ptr == nullptr) {
            auto new_root = new t_node(item, true);
            this->root.reset(new_root);
            this->_size = 1;
            return;
        }

        bool go_left = this->compare(item, ptr->_item);
        auto *next = go_left ? dynamic_cast<t_node *>(ptr->_left)
                             : dynamic_cast<t_node *>(ptr->_right);
        while (next != nullptr) {
            ptr = next;
            go_left = this->compare(item, ptr->_item);
            next = go_left ? dynamic_cast<t_node *>(ptr->_left)
                           : dynamic_cast<t_node *>(ptr->_right);
        }

        auto add = new t_node(item, false, ptr);
        if (go_left) {
            ptr->replace_left(add);
        } else {
            ptr->replace_right(add);
        }
        ++this->_size;

        // now to balance the tree
        while (add != this->root.get()) {
            auto parent = dynamic_cast<t_node *>(add->_parent);
            // Parent is black: no adjustments needed
            if (parent->is_black)
                return;

            auto grandparent = dynamic_cast<t_node *>(parent->_parent);
            auto uncle = dynamic_cast<t_node *>(
                    (grandparent->_left == parent) ? grandparent->_right : grandparent->_left);

            if (uncle != nullptr && !uncle->is_black) {
                // red parent and uncle
                // we can simply repaint the parent, uncle, and grandparent, then continue
                // balancing
                parent->is_black = true;
                uncle->is_black = true;
                grandparent->is_black = false;
                add = grandparent;
            } else {
                // red parent, black uncle
                if (parent == grandparent->_left) {
                    // rotate red node outside
                    if (add == parent->_right)
                        binary_search_tree<T, Compare>::rotate(parent, false);

                    // rotate grandparent over
                    binary_search_tree<T, Compare>::rotate(grandparent, true);

                } else {
                    // mirror version
                    if (add == parent->_left)
                        binary_search_tree<T, Compare>::rotate(parent, true);
                    binary_search_tree<T, Compare>::rotate(grandparent, false);
                }

                // recolor, reassign if necessary
                grandparent->is_black = false;
                add = dynamic_cast<t_node *>(grandparent->_parent);
                add->is_black = true;
                return;
            }
        }

        // if we reach here, add is the new root: set to black
        add->is_black = true;
    }

    template<typename T, typename Compare>
    void red_black_tree<T, Compare>::remove(typename binary_tree<T>::iterator it) {
        binary_search_tree<T, Compare>::verify(it);
        auto node = dynamic_cast<t_node *>(binary_tree<T>::getNode(it));
        if (node == nullptr)
            return;
        if (this->_size == 1) {
            this->root.reset();
            this->_size = 0;
            return;
        }

        // if two children, find successor and swap values
        if (node->_left && node->_right) {
            auto successor = node->_left;
            while (successor->_right)
                successor = successor->_right;

            // swap values directly, then set node to process as successor
            std::swap(node->_item, successor->_item);
            node = dynamic_cast<t_node *>(successor);
        }

        auto child = dynamic_cast<t_node *>(node->_left ? node->_left : node->_right);
        bool go_left = (child == node->_left);
        if (node == this->root.get()) {
            // child must be red, can make that the new root
            if (go_left) {
                node->_left = nullptr;
            } else {
                node->_right = nullptr;
            }
            this->root.reset(child);
            child->_parent = nullptr;
            child->is_black = true;
            --this->_size;
            return;
        }

        auto parent = dynamic_cast<t_node *>(node->_parent);
        bool is_left_child = (node == parent->_left);

        auto swap_out = [](t_node *new_parent, t_node *new_child, bool change_left) {
            if (new_child && new_child->_parent) {
                if (new_child == new_child->_parent->_left)
                    new_child->_parent->changeLeft(nullptr);
                else
                    new_child->_parent->changeRight(nullptr);
            }

            if (change_left)
                new_parent->replace_left(new_child);
            else
                new_parent->replace_right(new_child);
        };

        // start balancing
        if (!node->is_black) {
            // removing won't change black-distances
            swap_out(parent, child, is_left_child);
            --this->_size;
        } else if (child != nullptr) { // must be red, otherwise unbalanced
            // repaint child before deletion to preserve black-distances
            child->is_black = true;
            swap_out(parent, child, is_left_child);
            --this->_size;
        } else {
            swap_out(parent, child, is_left_child);
            --this->_size;
            node = child;

            // need to rebalance
            // since parent-node-child has black-height 2, sibling subtree needs at
            // least one level sibling will never be nullptr
            auto sibling = dynamic_cast<t_node *>(is_left_child ? parent->_right : parent->_left);
            auto outer_sibling = dynamic_cast<t_node *>(sibling->_left);
            auto inner_sibling = dynamic_cast<t_node *>(sibling->_right);
            if (is_left_child)
                std::swap(inner_sibling, outer_sibling);

            // need to keep looping until at least one of
            // parent/sibling/sibling-children is red
            while (node != this->root.get()) {
                if (!parent->is_black || !sibling->is_black)
                    break;
                if ((!inner_sibling || inner_sibling->is_black)
                    && (!outer_sibling || outer_sibling->is_black)) {
                    sibling->is_black = false;

                    // balanced up to parent, but parent black-height is shorter than its sibling's
                    // need to rebalance starting at parent
                    node = parent;
                    parent = dynamic_cast<t_node *>(node->_parent);
                    if (parent) {
                        is_left_child = (node == parent->_left);
                        sibling = dynamic_cast<t_node *>(is_left_child ? parent->_right : parent->_left);
                        outer_sibling = dynamic_cast<t_node *>(sibling->_left);
                        inner_sibling = dynamic_cast<t_node *>(sibling->_right);
                        if (is_left_child) {
                            std::swap(inner_sibling, outer_sibling);
                        }
                    }
                } else {
                    break;
                }
            }
            if (node == this->root.get())
                return;

            // red sibling: parent must have been black; rotate and swap
            if (!sibling->is_black) {
                binary_search_tree<T, Compare>::rotate(parent, !is_left_child);
                parent->is_black = false;
                sibling->is_black = true;

                // not fully balanced yet: relabel pointers, then move on to red parent
                // scenarios
                sibling = inner_sibling;
                outer_sibling = dynamic_cast<t_node *>(sibling->_left);
                inner_sibling = dynamic_cast<t_node *>(sibling->_right);
                if (is_left_child) {
                    std::swap(inner_sibling, outer_sibling);
                }
            }

            // parent must be red by here
            if ((!inner_sibling || inner_sibling->is_black) && (!outer_sibling || outer_sibling->is_black)) {
                // recoloring parent and sibling restores original black-lengths
                parent->is_black = true;
                sibling->is_black = false;
            } else {
                // we need red on the outside, regardless of the inside color
                if ((!outer_sibling || outer_sibling->is_black)) {
                    // inner must be red if we reach here
                    binary_search_tree<T, Compare>::rotate(sibling, is_left_child);
                    sibling->is_black = false;
                    inner_sibling->is_black = true;

                    // reassign to new nodes
                    outer_sibling = sibling;
                    sibling = inner_sibling;
                    inner_sibling = dynamic_cast<t_node *>(
                            is_left_child ? inner_sibling->_left : inner_sibling->_right);
                }

                // outer_sibling now must be red, sibling is black
                binary_search_tree<T, Compare>::rotate(parent, !is_left_child);
                sibling->is_black = parent->is_black;
                parent->is_black = true;
                outer_sibling->is_black = true;
            }
        }
    }

    template<typename T, typename Compare>
    red_black_tree<T, Compare>::t_node::t_node(
            const T &item, bool black, t_node *parent, t_node *left, t_node *right)
            : binary_tree<T>::node(item, parent, left, right), is_black(black) {
    }

    template<typename T, typename Compare>
    typename red_black_tree<T, Compare>::t_node *red_black_tree<T, Compare>::t_node::change_left(
            red_black_tree<T, Compare>::t_node *to_add) noexcept {
        return dynamic_cast<t_node *>(binary_tree<T>::node::changeLeft(to_add));
    }

    template<typename T, typename Compare>
    typename red_black_tree<T, Compare>::t_node *
    red_black_tree<T, Compare>::t_node::change_right(
            red_black_tree<T, Compare>::t_node *to_add) noexcept {
        return dynamic_cast<t_node *>(binary_tree<T>::node::changeRight(to_add));
    }

    template<typename T, typename Compare>
    void red_black_tree<T, Compare>::t_node::replace_left(
            red_black_tree<T, Compare>::t_node *to_add) noexcept {
        auto prev = change_left(to_add);
        delete prev;
    }

    template<typename T, typename Compare>
    void red_black_tree<T, Compare>::t_node::replace_right(
            red_black_tree<T, Compare>::t_node *to_add) noexcept {
        auto prev = change_right(to_add);
        delete prev;
    }
}

#endif // !RED_BLACK_TREE_CPP
