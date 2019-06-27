#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "BinarySearchTree.h"

template<typename T, typename Compare = std::less<>>
class AVLSearchTree : virtual public BinarySearchTree<T, Compare> {
public:
    AVLSearchTree() = default;

    template<typename It>
    AVLSearchTree(It first, It last, Compare comp = Compare()) : BinarySearchTree<T, Compare>(first, last, comp) {
        std::vector<T> elements(first, last);
        std::sort(elements.begin(), elements.end(), comp);
        this->balancedTreeRoot.reset(generate(elements, 0, elements.size(), nullptr));
        this->_size = elements.size();
    }

    void insert(const T&) override;
    virtual void remove(typename BinaryTree<T>::iterator it);

protected:
    struct AVLNode {
        AVLNode(const T& item = T(), AVLNode* parent = nullptr, AVLNode* left = nullptr, AVLNode* right = nullptr)
            : _item(item)
            , _parent(parent)
            , _left(left)
            , _right(right)
            , leftHeight(0)
            , rightHeight(0) {
            if (this->_left) {
                leftHeight = std::max(this->_left->leftHeight, this->_left->rightHeight) + 1;
            }
            if (this->_right) {
                rightHeight = std::max(this->_right->leftHeight, this->_right->rightHeight) + 1;
            }
        }

        virtual AVLNode* changeLeft(AVLNode* toAdd) noexcept;
        virtual AVLNode* changeRight(AVLNode* toAdd) noexcept;
        virtual void replaceLeft(AVLNode* toAdd) noexcept;
        virtual void replaceRight(AVLNode* toAdd) noexcept;

        T _item;
        AVLNode* _parent;
        AVLNode* _left;
        AVLNode* _right;
        uint32_t leftHeight;
        uint32_t rightHeight;
    };
    static AVLNode* generate(const std::vector<T>&, uint32_t, uint32_t, AVLNode*);
    void balanceTree(AVLNode* start) noexcept;
    void rotate(AVLNode* upper, bool useLeftChild);
    std::unique_ptr<AVLNode> balancedTreeRoot;
};

#include "src/AVLTree.cpp"

#endif // !AVL_TREE_H
