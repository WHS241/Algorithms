#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "BinarySearchTree.h"

// Red-black tree
template <typename T, typename Compare = std::less<>>
class RedBlackTree : virtual public BinarySearchTree<T, Compare> {
public:
    RedBlackTree(Compare comp = Compare())
        : BinarySearchTree<T, Compare>(comp) {};

    template <typename It>
    RedBlackTree(It first, It last, Compare comp = Compare())
        : BinarySearchTree<T, Compare>()
    {
        this->_size = 0;
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    // Θ(log n)
    void insert(const T&) override;

    // Θ(log n)
    virtual void remove(typename BinaryTree<T>::iterator it);

protected:
    struct RedBlackNode : virtual public BinaryTree<T>::BinaryNode {
        RedBlackNode(const T& item = T(), RedBlackNode* parent = nullptr,
            RedBlackNode* left = nullptr, RedBlackNode* right = nullptr);

        virtual RedBlackNode* changeLeft(RedBlackNode* toAdd) noexcept;
        virtual RedBlackNode* changeRight(RedBlackNode* toAdd) noexcept;
        virtual void replaceLeft(RedBlackNode* toAdd) noexcept;
        virtual void replaceRight(RedBlackNode* toAdd) noexcept;

        bool isBlack;
    };
};

#include <src/structures/RedBlackTree.cpp>

#endif // !RED_BLACK_TREE_H
