#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "NodeHeap.h"

template <typename T, typename Compare = std::less<>>
class BinomialHeap : virtual public NodeHeap<T, Compare> {
public:
    typedef typename NodeHeap<T, Compare>::Node Node;

    BinomialHeap(Compare comp = Compare());

    template <typename It>
    BinomialHeap(It first, It last, Compare comp = Compare())
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

    virtual ~BinomialHeap() noexcept;
    BinomialHeap(const BinomialHeap<T, Compare>&);
    BinomialHeap& operator=(const BinomialHeap<T, Compare>&);
    BinomialHeap(BinomialHeap<T, Compare>&&);
    BinomialHeap& operator=(BinomialHeap<T, Compare>&&);

    // Θ(log n)
    virtual Node* add(const T&);

    // Θ(1)
    virtual T getRoot() const;

    // Θ(log n)
    virtual T removeRoot();

    // Θ(log n)
    void decrease(Node* target, const T& newVal);

    // Θ(log n)
    void merge(BinomialHeap<T, Compare>&);

private:
    std::vector<Node*> trees;

    Node* min;
};

#include <src/structures/BinomialHeap.cpp>

#endif // BINOMIAL_HEAP_H