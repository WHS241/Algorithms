#ifndef BINARY_TREE_NODE_H
#define BINARY_TREE_NODE_H

#include <memory>

template<typename T> class TreeIteratorImpl;
template<typename T> class PreOrderTreeIteratorImpl;
template<typename T> class InOrderTreeIteratorImpl;
template<typename T> class PostOrderTreeIteratorImpl;
template<typename T> class LevelOrderTreeIteratorImpl; 
template<typename T> class BinaryTree;

template<typename T>
class BinaryNode {
public:
    BinaryNode(T item = T(), BinaryNode<T>* left = nullptr, BinaryNode<T>* right = nullptr, BinaryNode<T>* parent = nullptr);
    
    T item() const;
    
private:
    void addLeft(std::unique_ptr<BinaryNode<T>>&&);
    void addRight(std::unique_ptr<BinaryNode<T>>&&);

    T _item;
    
    BinaryNode* parent;
    std::unique_ptr<BinaryNode<T>> left;
    std::unique_ptr<BinaryNode<T>> right;

    friend class TreeIteratorImpl<T>;
    friend class PreOrderTreeIteratorImpl<T>;
    friend class InOrderTreeIteratorImpl<T>;
    friend class PostOrderTreeIteratorImpl<T>;
    friend class LevelOrderTreeIteratorImpl<T>;
    friend class BinaryTree<T>;
};

#include "src/BinaryTreeNode.cpp"

#endif // BINARY_TREE_NODE_H