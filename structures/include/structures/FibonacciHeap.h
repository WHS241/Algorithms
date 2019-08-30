#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include "NodeHeap.h"

#include <unordered_set>

template <typename T, typename Compare = std::less<>>
class FibonacciHeap : virtual public NodeHeap<T, Compare> {
public:
    typedef typename NodeHeap<T, Compare>::Node Node;

    FibonacciHeap(Compare comp = Compare());

    template <typename It>
    FibonacciHeap(It first, It last, Compare comp = Compare())
            : NodeHeap<T, Compare>(comp)
            , trees()
            , min(nullptr)
    {
        try {
            for (; first != last; ++first)
                add(*first);
        } catch (...) {
            for (Node* root : trees)
                delete root;
            throw;
        }
    }

    virtual ~FibonacciHeap() noexcept;
    FibonacciHeap(const FibonacciHeap<T, Compare>&);
    FibonacciHeap& operator=(const FibonacciHeap<T, Compare>&);
    FibonacciHeap(FibonacciHeap<T, Compare>&&);
    FibonacciHeap& operator=(FibonacciHeap<T, Compare>&&);

    // Θ(1)
    virtual Node* add(const T&);

    // Θ(1)
    virtual T getRoot() const;

    // amortized Θ(log n)
    virtual T removeRoot();

    // amortized Θ(1)
    void decrease(Node* target, const T& newVal);

    // Θ(1)
    void merge(FibonacciHeap<T, Compare>&);

private:
    std::list<Node*> trees;

    Node* min;
};

#include <src/structures/FibonacciHeap.cpp>

#endif //!FIBONACCI_HEAP_H
