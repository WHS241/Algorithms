#ifndef RED_BLACK_TREE_CPP
#define RED_BLACK_TREE_CPP

#include <include/structures/RedBlackTree.h>

template <typename T, typename Compare> void RedBlackTree<T, Compare>::insert(const T& item)
{
    auto* ptr = dynamic_cast<RedBlackNode*>(this->root.get());
    if (ptr == nullptr) {
        auto newRoot = new RedBlackNode(item);
        newRoot->isBlack = true;
        this->root.reset(newRoot);
        this->_size = 1;
        return;
    }

    bool goLeft = this->compare(item, ptr->_item);
    auto* next = goLeft ? dynamic_cast<RedBlackNode*>(ptr->_left)
                        : dynamic_cast<RedBlackNode*>(ptr->_right);
    while (next != nullptr) {
        ptr = next;
        goLeft = this->compare(item, ptr->_item);
        next = goLeft ? dynamic_cast<RedBlackNode*>(ptr->_left)
                      : dynamic_cast<RedBlackNode*>(ptr->_right);
    }

    auto add = new RedBlackNode(item, ptr);
    add->isBlack = false;
    if (goLeft) {
        ptr->replaceLeft(add);
    } else {
        ptr->replaceRight(add);
    }
    ++this->_size;

    // now to balance the tree
    while (add != this->root.get()) {
        auto parent = dynamic_cast<RedBlackNode*>(add->_parent);
        // Parent is black: no adjustments needed
        if (parent->isBlack)
            return;

        auto grandparent = dynamic_cast<RedBlackNode*>(parent->_parent);
        auto uncle = dynamic_cast<RedBlackNode*>(
            (grandparent->_left == parent) ? grandparent->_right : grandparent->_left);

        if (uncle != nullptr && !uncle->isBlack) {
            // red parent and uncle
            // we can simply repaint the parent, uncle, and grandparent, then continue
            // balancing
            parent->isBlack = true;
            uncle->isBlack = true;
            grandparent->isBlack = false;
            add = grandparent;
        } else {
            // red parent, black uncle
            if (parent == grandparent->_left) {
                // rotate red node outside
                if (add == parent->_right)
                    BinarySearchTree<T, Compare>::rotate(parent, false);

                // rotate grandparent over
                BinarySearchTree<T, Compare>::rotate(grandparent, true);

            } else {
                // mirror version
                if (add == parent->_left)
                    BinarySearchTree<T, Compare>::rotate(parent, true);
                BinarySearchTree<T, Compare>::rotate(grandparent, false);
            }

            // recolor, reassign if necessary
            grandparent->isBlack = false;
            add = dynamic_cast<RedBlackNode*>(grandparent->_parent);
            add->isBlack = true;
            return;
        }
    }

    // if we reach here, add is the new root: set to black
    add->isBlack = true;
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove(typename BinaryTree<T>::iterator it)
{
    BinarySearchTree<T, Compare>::verify(it);
    auto node = dynamic_cast<RedBlackNode*>(BinaryTree<T>::getNode(it));
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
        node = dynamic_cast<RedBlackNode*>(successor);
    }

    auto child = dynamic_cast<RedBlackNode*>(node->_left ? node->_left : node->_right);
    bool goLeft = (child == node->_left);
    if (node == this->root.get()) {
        // child must be red, can make that the new root
        if (goLeft) {
            node->_left = nullptr;
        } else {
            node->_right = nullptr;
        }
        this->root.reset(child);
        child->_parent = nullptr;
        child->isBlack = true;
        --this->_size;
        return;
    }

    auto parent = dynamic_cast<RedBlackNode*>(node->_parent);
    bool isLeft = (node == parent->_left);

    auto swapOut = [](RedBlackNode* newParent, RedBlackNode* newChild, bool changeLeft) {
        if (newChild && newChild->_parent) {
            if (newChild == newChild->_parent->_left)
                newChild->_parent->changeLeft(nullptr);
            else
                newChild->_parent->changeRight(nullptr);
        }

        if (changeLeft)
            newParent->replaceLeft(newChild);
        else
            newParent->replaceRight(newChild);
    };

    // start balancing
    if (!node->isBlack) {
        // removing won't change black-distances
        swapOut(parent, child, isLeft);
        --this->_size;
    } else if (child != nullptr) { // must be red, otherwise unbalanced
        // repaint child before deletion to preserve black-distances
        child->isBlack = true;
        swapOut(parent, child, isLeft);
        --this->_size;
    } else {
        swapOut(parent, child, isLeft);
        --this->_size;
        node = child;

        // need to rebalance
        // since parent-node-child has black-height 2, sibling subtree needs at
        // least one level sibling will never be nullptr
        auto sibling = dynamic_cast<RedBlackNode*>(isLeft ? parent->_right : parent->_left);
        auto siblingOuter = dynamic_cast<RedBlackNode*>(sibling->_left);
        auto siblingInner = dynamic_cast<RedBlackNode*>(sibling->_right);
        if (isLeft)
            std::swap(siblingInner, siblingOuter);

        // need to keep looping until at least one of
        // parent/sibling/sibling-children is red
        while (node != this->root.get()) {
            if (!parent->isBlack || !sibling->isBlack)
                break;
            if ((!siblingInner || siblingInner->isBlack)
                && (!siblingOuter || siblingOuter->isBlack)) {
                sibling->isBlack = false;

                // balanced up to parent, but parent black-height is shorter than its sibling's
                // need to rebalance starting at parent
                node = parent;
                parent = dynamic_cast<RedBlackNode*>(node->_parent);
                if (parent) {
                    isLeft = (node == parent->_left);
                    sibling = dynamic_cast<RedBlackNode*>(isLeft ? parent->_right : parent->_left);
                    siblingOuter = dynamic_cast<RedBlackNode*>(sibling->_left);
                    siblingInner = dynamic_cast<RedBlackNode*>(sibling->_right);
                    if (isLeft) {
                        std::swap(siblingInner, siblingOuter);
                    }
                }
            } else {
                break;
            }
        }
        if (node == this->root.get())
            return;

        // red sibling: parent must have been black; rotate and swap
        if (!sibling->isBlack) {
            BinarySearchTree<T, Compare>::rotate(parent, !isLeft);
            parent->isBlack = false;
            sibling->isBlack = true;

            // not fully balanced yet: relabel pointers, then move on to red parent
            // scenarios
            sibling = siblingInner;
            siblingOuter = dynamic_cast<RedBlackNode*>(sibling->_left);
            siblingInner = dynamic_cast<RedBlackNode*>(sibling->_right);
            if (isLeft) {
                std::swap(siblingInner, siblingOuter);
            }
        }

        // parent must be red by here
        if ((!siblingInner || siblingInner->isBlack) && (!siblingOuter || siblingOuter->isBlack)) {
            // recoloring parent and sibling restores original black-lengths
            parent->isBlack = true;
            sibling->isBlack = false;
        } else {
            // we need red on the outside, regardless of the inside color
            if ((!siblingOuter || siblingOuter->isBlack)) {
                // inner must be red if we reach here
                BinarySearchTree<T, Compare>::rotate(sibling, isLeft);
                sibling->isBlack = false;
                siblingInner->isBlack = true;

                // reassign to new nodes
                siblingOuter = sibling;
                sibling = siblingInner;
                siblingInner = dynamic_cast<RedBlackNode*>(
                    isLeft ? siblingInner->_left : siblingInner->_right);
            }

            // siblingOuter now must be red, sibling is black
            BinarySearchTree<T, Compare>::rotate(parent, !isLeft);
            sibling->isBlack = parent->isBlack;
            parent->isBlack = true;
            siblingOuter->isBlack = true;
        }
    }
}

template <typename T, typename Compare>
RedBlackTree<T, Compare>::RedBlackNode::RedBlackNode(
    const T& item, RedBlackNode* parent, RedBlackNode* left, RedBlackNode* right)
    : BinaryTree<T>::BinaryNode(item, parent, left, right)
{
}

template <typename T, typename Compare>
typename RedBlackTree<T, Compare>::RedBlackNode* RedBlackTree<T, Compare>::RedBlackNode::changeLeft(
    RedBlackTree<T, Compare>::RedBlackNode* toAdd) noexcept
{
    return dynamic_cast<RedBlackNode*>(BinaryTree<T>::BinaryNode::changeLeft(toAdd));
}

template <typename T, typename Compare>
typename RedBlackTree<T, Compare>::RedBlackNode*
RedBlackTree<T, Compare>::RedBlackNode::changeRight(
    RedBlackTree<T, Compare>::RedBlackNode* toAdd) noexcept
{
    return dynamic_cast<RedBlackNode*>(BinaryTree<T>::BinaryNode::changeRight(toAdd));
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::RedBlackNode::replaceLeft(
    RedBlackTree<T, Compare>::RedBlackNode* toAdd) noexcept
{
    auto prev = changeLeft(toAdd);
    delete prev;
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::RedBlackNode::replaceRight(
    RedBlackTree<T, Compare>::RedBlackNode* toAdd) noexcept
{
    auto prev = changeRight(toAdd);
    delete prev;
}

#endif // !RED_BLACK_TREE_CPP
