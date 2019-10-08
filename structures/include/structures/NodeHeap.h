#ifndef NODE_HEAP_H
#define NODE_HEAP_H

#include "Heap.h"

template <typename T, typename Compare> class NodeBinaryHeap;
template <typename T, typename Compare> class BinomialHeap;
template <typename T, typename Compare> class FibonacciHeap;

// NodeHeap:
// adding an element returns a pointer to the added node
// pointer is valid until element is removed from heap
template <typename T, typename Compare = std::less<>>
class NodeHeap : public Heap<T, Compare> {
public:
    NodeHeap() = default;
    explicit NodeHeap(Compare comp)
        : Heap<T, Compare>(comp)
    {
    }

    class Node {
    public:
        virtual ~Node() noexcept;
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        const T& operator*() const;
        const T* operator->() const;

    private:
        Node() = default;
        Node(const T& value)
            : value(value)
            , parent(nullptr)
            , children()
            , flag(false) {};
        Node* deepClone() const;

        T& operator*();
        T* operator->();

        T value;
        Node* parent;
        std::list<Node*> children;
        bool flag;

        friend class NodeHeap<T, Compare>;
        friend class NodeBinaryHeap<T, Compare>;
        friend class BinomialHeap<T, Compare>;
        friend class FibonacciHeap<T, Compare>;
    };

    virtual ~NodeHeap() noexcept = default;

    // Inserting with add returns a pointer to the corresponding value
    void insert(const T& item) override;
    virtual Node* add(const T& item) = 0;

    virtual void decrease(Node* target, const T& newVal) = 0;

    uint32_t size() const noexcept override;

    // Though not included due to inheritance issues, children should also implement merge(const [class]& src)
    // which moves all the values of src into this, emptying src.
    // Node pointers should not be invalidated by this merge.

protected:
    // Copies _size over
    // Protected because this is meant to only be a helper function for the copy ctor of concrete children
    NodeHeap(const NodeHeap<T, Compare>& src) = default;
    
    static Node* makeNode(const T& item);

    uint32_t _size = 0;
};

#include <src/structures/NodeHeap.cpp>

#endif // !NODE_HEAP_H
