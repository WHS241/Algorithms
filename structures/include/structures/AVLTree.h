#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "BinarySearchTree.h"

// AVL tree implementation
// For any node, difference in height between subtrees <= 1
template<typename T, typename Compare = std::less<>>
class AVLSearchTree : virtual public BinarySearchTree<T, Compare> {
public:
    AVLSearchTree() = default;

    template<typename It>
    AVLSearchTree(It first, It last, Compare comp = Compare()) : BinarySearchTree<T, Compare>(first, last, comp) {
        std::vector<T> elements(first, last);
        std::sort(elements.begin(), elements.end(), comp);
        this->root.reset(generate(elements, 0, elements.size(), nullptr));
        this->_size = elements.size();
    }

    // жи(log n)
    void insert(const T&) override;

    // жи(log n)
    virtual void remove(typename BinaryTree<T>::iterator it);

protected:
    struct AVLNode : virtual public BinaryTree<T>::BinaryNode {
        AVLNode(const T& item = T(), AVLNode* parent = nullptr, AVLNode* left = nullptr, AVLNode* right = nullptr);

        virtual AVLNode* changeLeft(AVLNode* toAdd) noexcept;
        virtual AVLNode* changeRight(AVLNode* toAdd) noexcept;
        virtual void replaceLeft(AVLNode* toAdd) noexcept;
        virtual void replaceRight(AVLNode* toAdd) noexcept;

        uint32_t leftHeight;
        uint32_t rightHeight;
    };

    static AVLNode* generate(const std::vector<T>&, uint32_t, uint32_t, AVLNode*);

    void balanceTree(AVLNode* start) noexcept;
};

#include "src/structures/AVLTree.cpp"

#endif // !AVL_TREE_H
