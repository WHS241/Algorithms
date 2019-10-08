#ifndef NODE_BINARY_HEAP_H
#define NODE_BINARY_HEAP_H

#include <list>
#include <memory>
#include <queue>
#include <stack>

#include "NodeHeap.h"

template <typename T, typename Compare = std::less<>>
class NodeBinaryHeap : virtual public NodeHeap<T, Compare> {
public:
    typedef typename NodeHeap<T, Compare>::Node Node;

    NodeBinaryHeap() = default;
    explicit NodeBinaryHeap(Compare comp);

    // Θ(n)
    template <typename It>
    NodeBinaryHeap(It first, It last, Compare comp = Compare())
        : NodeHeap<T, Compare>(comp)
    {
        this->_size = std::distance(first, last);
        if (first != last) {
            root.reset(this->makeNode(*first));
            ++first;
            std::queue<Node*> nodeQueue; // for adding elements
            std::stack<Node*> nodeStack; // for the bubble down
            nodeQueue.push(root.get());
            nodeStack.push(root.get());

            // create all the nodes, attach to heap
            for (; first != last; ++first) {
                Node* parent = nodeQueue.front();
                if (!parent->children.empty())
                    nodeQueue.pop();

                Node* newNode = this->makeNode(*first);
                newNode->parent = parent;
                parent->children.push_back(newNode);
                nodeQueue.push(newNode);
                nodeStack.push(newNode);
            }

            while (!nodeStack.empty()) {
                Node* startPtr = nodeStack.top();
                nodeStack.pop();
                while (!startPtr->children.empty()) {
                    Node* leftChild = startPtr->children.front();
                    Node* rightChild = startPtr->children.back();
                    bool swapLeft = this->compare(**leftChild, **rightChild);
                    Node* comparePtr = swapLeft ? leftChild : rightChild;
                    if (!this->compare(**startPtr, **comparePtr))
                        bubbleDown(startPtr, swapLeft);
                    else
                        break;
                }
            }
        }
    };

    virtual ~NodeBinaryHeap() noexcept = default;
    NodeBinaryHeap(const NodeBinaryHeap<T, Compare>&);
    NodeBinaryHeap& operator=(const NodeBinaryHeap<T, Compare>&);
    NodeBinaryHeap(NodeBinaryHeap<T, Compare>&&);
    NodeBinaryHeap& operator=(NodeBinaryHeap<T, Compare>&&);

    // Θ(log n)
    virtual Node* add(const T&);

    // Θ(1)
    virtual T getRoot() const;

    // Θ(log n)
    virtual T removeRoot();

    // Θ(log n)
    void decrease(Node* target, const T& newVal);

    // Θ(n)
    // Binary heap merging currently will invalidate all previous Node*
    void merge(NodeBinaryHeap<T, Compare>&);

private:
    void bubbleDown(Node* parent, bool useLeftChild) noexcept;
    static std::list<T> data(const Node*);

    std::unique_ptr<Node> root;
};

#include <src/structures/NodeBinaryHeap.cpp>

#endif // !NODE_BINARY_HEAP_H
