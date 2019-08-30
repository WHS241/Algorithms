#ifndef HEAP_CPP
#define HEAP_CPP

#include <algorithm>
#include <iterator>
#include <list>

template <typename T, typename Compare>
Heap<T, Compare>::Heap(Compare comp)
    : compare(comp)
{
}

template <typename T, typename Compare> bool Heap<T, Compare>::empty() const noexcept
{
    return this->size() == 0;
}

template <typename T, typename Compare>
BinaryHeap<T, Compare>::BinaryHeap(Compare comp)
    : Heap<T, Compare>(comp)
    , heap()
{
}

template <typename T, typename Compare>
void BinaryHeap<T, Compare>::merge(BinaryHeap<T, Compare>&& src)
{
    std::list<T> temp(heap.begin(), heap.end());
    std::copy(src.heap.begin(), src.heap.end(), std::back_inserter(temp));
    BinaryHeap<T, Compare> result(temp.begin(), temp.end(), this->compare);
    std::swap(heap, result.heap);
}

template <typename T, typename Compare> void BinaryHeap<T, Compare>::insert(const T& item)
{
    uint32_t index(heap.size());
    heap.push_back(item);

    while (index > 0) {
        uint32_t parent((index - 1) / 2);
        if (this->compare(heap[index], heap[parent])) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

template <typename T, typename Compare> T BinaryHeap<T, Compare>::removeRoot()
{
    T prevRoot = heap.front();
    std::swap(heap.front(), heap.back());

    uint32_t current = 0;
    while (2 * current + 1 < heap.size() - 1) {
        uint32_t child(2 * current + 1);
        if (2 * current + 2 < heap.size() - 1 && !this->compare(heap[child], heap[child + 1]))
            ++child;

        if (!this->compare(heap[current], heap[child])) {
            std::swap(heap[current], heap[child]);
            current = child;
        } else {
            break;
        }
    }

    heap.pop_back();
    return prevRoot;
}

template <typename T, typename Compare> T BinaryHeap<T, Compare>::getRoot() const
{
    return heap.front();
}

template <typename T, typename Compare> uint32_t BinaryHeap<T, Compare>::size() const noexcept
{
    return heap.size();
}

#endif // HEAP_CPP
