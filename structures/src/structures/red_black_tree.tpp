#ifndef RED_BLACK_TREE_CPP
#define RED_BLACK_TREE_CPP

namespace tree {

template<typename T, typename Compare>
std::pair<typename red_black_tree<T, Compare>::iterator, bool>
  red_black_tree<T, Compare>::insert(const T& item) {
    auto* ptr = dynamic_cast<t_node*>(this->_root.get());
    if (ptr == nullptr) {
        auto new_root = new t_node(item, true);
        this->_root.reset(new_root);
        this->_size = 1;
        return {begin(), true};
    }

    bool go_left = this->_compare(item, ptr->item);
    auto* next = dynamic_cast<t_node*>(go_left ? ptr->left : ptr->right);
    while (next != nullptr) {
        ptr = next;
        go_left = this->_compare(item, ptr->item);

        if (!this->_allow_duplicates && !go_left && !this->_compare(ptr->item, item))
            return {this->_make_iterator(ptr, traversal::in_order, false), false};

        next = dynamic_cast<t_node*>(go_left ? ptr->left : ptr->right);
    }

    // add a new node as red
    auto add = new t_node(item, false, ptr);
    if (go_left) {
        ptr->replace_left(add);
    } else {
        ptr->replace_right(add);
    }
    ++this->_size;

    std::pair<iterator, bool> result = {this->_make_iterator(add, traversal::in_order, false),
                                        true};

    // now to balance the tree
    while (add != this->_root.get()) {
        auto parent = dynamic_cast<t_node*>(add->parent);
        // Parent is black: no adjustments needed
        if (parent->is_black)
            return result;

        // since root is black, a red parent means there must be a black (non-null) grandparent
        auto grandparent = dynamic_cast<t_node*>(parent->parent);
        auto uncle = dynamic_cast<t_node*>((grandparent->left == parent) ? grandparent->right
                                                                         : grandparent->left);

        if (uncle != nullptr && !uncle->is_black) {
            // red parent and uncle
            // we repaint the parent, uncle, and grandparent
            // black-length has increased, so continue balancing with grandparent
            parent->is_black = true;
            uncle->is_black = true;
            grandparent->is_black = false;
            add = grandparent;
        } else {
            // red parent, black uncle
            if (parent == grandparent->left) {
                // ensure a red node is one outside of subtree (left-left child of grandparent)
                if (add == parent->right)
                    binary_search_tree<T, Compare>::_rotate(parent, false);

                // rotate grandparent to uncle-side
                binary_search_tree<T, Compare>::_rotate(grandparent, true);

            } else {
                // mirror version
                if (add == parent->left)
                    binary_search_tree<T, Compare>::_rotate(parent, true);
                binary_search_tree<T, Compare>::_rotate(grandparent, false);
            }

            // recolor old grandparent and new subtree node, no further rebalancing
            grandparent->is_black = false;
            add = dynamic_cast<t_node*>(grandparent->parent);
            add->is_black = true;
            return result;
        }
    }

    // if we reach here, add is the new root: set to black
    add->is_black = true;
    return result;
}

template<typename T, typename Compare>
typename red_black_tree<T, Compare>::iterator
  red_black_tree<T, Compare>::erase(typename red_black_tree<T, Compare>::iterator it) {
    binary_search_tree<T, Compare>::_verify(it);
    auto node = dynamic_cast<t_node*>(binary_tree<T>::_s_get_node(it));
    if (node == nullptr)
        return end();
    if (this->_size == 1) {
        this->_root.reset();
        this->_size = 0;
        return end();
    }

    ++it; // return value

    // if two children, find successor and swap values
    if (node->left && node->right) {
        typename binary_tree<T>::node* successor = node->left;
        while (successor->right)
            successor = successor->right;

        // swap values directly, then set node to process as successor
        std::swap(node->item, successor->item);
        node = dynamic_cast<t_node*>(successor);
    }

    // node now has at most one non-null child
    auto child = dynamic_cast<t_node*>(node->left ? node->left : node->right);
    bool go_left = (child == node->left);
    if (node == this->_root.get()) {
        // child must be red, can make that the new root
        if (go_left) {
            node->left = nullptr;
        } else {
            node->right = nullptr;
        }
        this->_root.reset(child); // deletes old root (variable node)
        child->parent = nullptr;
        child->is_black = true;
        --this->_size;
        return it;
    }

    auto parent = dynamic_cast<t_node*>(node->parent);
    bool is_left_child = (node == parent->left);

    auto swap_out = [](t_node* new_parent, t_node* new_child, bool change_left) {
        // detach new_child from its old parent (variable node)
        if (new_child && new_child->parent) {
            if (new_child == new_child->parent->left)
                new_child->parent->change_left(nullptr);
            else
                new_child->parent->change_right(nullptr);
        }

        // Attach to new parent (deletes variable node)
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
    } else if (child != nullptr) { // child must be red, otherwise unbalanced
        // repaint child before deletion to preserve black-distances
        child->is_black = true;
        swap_out(parent, child, is_left_child);
        --this->_size;
    } else {
        swap_out(parent, child, is_left_child);
        --this->_size;
        node = child;

        // need to rebalance
        // since parent-node-child had black-height 2, sibling subtree needs at
        // least one level, so sibling will never be nullptr
        auto sibling = dynamic_cast<t_node*>(is_left_child ? parent->right : parent->left);
        auto outer_sibling = dynamic_cast<t_node*>(sibling->left);
        auto inner_sibling = dynamic_cast<t_node*>(sibling->right);
        if (is_left_child)
            std::swap(inner_sibling, outer_sibling);

        // need to keep looping until at least one of
        // parent/sibling/sibling-children is red
        while (node != this->_root.get()) {
            if (!parent->is_black || !sibling->is_black)
                break;
            if ((!inner_sibling || inner_sibling->is_black) &&
                (!outer_sibling || outer_sibling->is_black)) {
                sibling->is_black = false;

                // balanced up to parent, but parent black-height is shorter than its sibling's
                // need to rebalance starting at parent
                node = parent;
                parent = dynamic_cast<t_node*>(node->parent);
                if (parent) {
                    is_left_child = (node == parent->left);
                    sibling = dynamic_cast<t_node*>(is_left_child ? parent->right : parent->left);
                    outer_sibling = dynamic_cast<t_node*>(sibling->left);
                    inner_sibling = dynamic_cast<t_node*>(sibling->right);
                    if (is_left_child) {
                        std::swap(inner_sibling, outer_sibling);
                    }
                }
            } else {
                break;
            }
        }
        if (node == this->_root.get())
            return it;

        // red sibling: parent must have been black; rotate and swap
        if (!sibling->is_black) {
            binary_search_tree<T, Compare>::_rotate(parent, !is_left_child);
            parent->is_black = false;
            sibling->is_black = true;

            // not fully balanced yet: relabel pointers, then move on to red parent
            // scenarios
            sibling = inner_sibling;
            outer_sibling = dynamic_cast<t_node*>(sibling->left);
            inner_sibling = dynamic_cast<t_node*>(sibling->right);
            if (is_left_child) {
                std::swap(inner_sibling, outer_sibling);
            }
        }

        if ((!inner_sibling || inner_sibling->is_black) &&
            (!outer_sibling || outer_sibling->is_black)) {
            // parent must be red by here
            // recoloring parent and sibling restores original black-lengths
            parent->is_black = true;
            sibling->is_black = false;
        } else {
            // we need outer to be red, inside color doesn't matter
            if ((!outer_sibling || outer_sibling->is_black)) {
                // must have red inner, black sibling if we reach here
                // rotate and swap colors
                binary_search_tree<T, Compare>::_rotate(sibling, is_left_child);
                sibling->is_black = false;
                inner_sibling->is_black = true;

                // reassign based on new structure
                outer_sibling = sibling;
                sibling = inner_sibling;
                inner_sibling =
                  dynamic_cast<t_node*>(is_left_child ? inner_sibling->left : inner_sibling->right);
            }

            // must now have red outer, black sibling
            binary_search_tree<T, Compare>::_rotate(parent, !is_left_child);
            sibling->is_black = parent->is_black;
            parent->is_black = true;
            outer_sibling->is_black = true;
        }
    }

    return it;
}

template<typename T, typename Compare>
red_black_tree<T, Compare>::t_node::t_node(const T& item, bool black, t_node* parent, t_node* left,
                                           t_node* right) :
    binary_tree<T>::node(item, parent, left, right), is_black(black) {}

template<typename T, typename Compare>
typename red_black_tree<T, Compare>::t_node* red_black_tree<T, Compare>::t_node::change_left(
  red_black_tree<T, Compare>::t_node* to_add) noexcept {
    return dynamic_cast<t_node*>(binary_tree<T>::node::change_left(to_add));
}

template<typename T, typename Compare>
typename red_black_tree<T, Compare>::t_node* red_black_tree<T, Compare>::t_node::change_right(
  red_black_tree<T, Compare>::t_node* to_add) noexcept {
    return dynamic_cast<t_node*>(binary_tree<T>::node::change_right(to_add));
}

template<typename T, typename Compare>
void red_black_tree<T, Compare>::t_node::replace_left(
  red_black_tree<T, Compare>::t_node* to_add) noexcept {
    t_node* prev = change_left(to_add);
    delete prev;
}

template<typename T, typename Compare>
void red_black_tree<T, Compare>::t_node::replace_right(
  red_black_tree<T, Compare>::t_node* to_add) noexcept {
    t_node* prev = change_right(to_add);
    delete prev;
}
} // namespace tree

#endif // !RED_BLACK_TREE_CPP
