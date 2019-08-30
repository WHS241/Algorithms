#ifndef HEAP_H
#define HEAP_H

#include <functional>
#include <vector>

#include "BinaryTree.h"

// Base class for heap structures
template <typename T, typename Compare = std::less<>> class Heap {
public:
    Heap(Compare comp = Compare());
    

    virtual void insert(const T&) = 0;
    virtual T getRoot() const = 0;
    virtual T removeRoot() = 0;
    virtual uint32_t size() const noexcept = 0;
    virtual bool empty() const noexcept;
        
protected : 
    Compare compare;
};

// Array-based binary heap
template <typename T, typename Compare = std::less<>>
class BinaryHeap : virtual public Heap<T, Compare> {
public:
    BinaryHeap(Compare comp = Compare());

    // Θ(n)
    template <typename It>
    BinaryHeap(It first, It last, Compare comp = Compare())
        : Heap<T, Compare>(comp)
        , heap(first, last)
    {
        for (uint32_t position = heap.size() - 1; position + 1 > 0; --position) {
            uint32_t current(position);

            // bubble down
            while (2 * current + 1 < heap.size()) {
                uint32_t child(2 * current + 1);
                if (2 * current + 2 < heap.size() && !this->compare(heap[child], heap[child + 1]))
                    ++child;

                if (!this->compare(heap[current], heap[child])) {
                    std::swap(heap[current], heap[child]);
                    current = child;
                } else {
                    break;
                }
            }
        }
    };

    // Θ(log n)
    virtual void insert(const T&);

    // Θ(n)
    virtual void merge(BinaryHeap<T, Compare>&&);

    // Θ(log n)
    virtual T removeRoot();

    // Θ(1)
    virtual T getRoot() const;
    virtual uint32_t size() const noexcept;

private:
    std::vector<T> heap;
};

#include "src/structures/Heap.cpp"

#endif // HEAP_H
