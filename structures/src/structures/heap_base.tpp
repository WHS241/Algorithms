#ifndef HEAP_CPP
#define HEAP_CPP

#include <algorithm>
#include <iterator>
#include <list>

namespace heap {
template <typename T, typename Compare>
base<T, Compare>::base(Compare comp)
    : _compare(comp)
{
}

template <typename T, typename Compare> bool base<T, Compare>::empty() const noexcept
{
    return this->size() == 0;
}

template <typename T, typename Compare, typename Container>
priority_queue<T, Compare, Container>::priority_queue(Compare comp)
    : base<T, Compare>(comp)
    , _heap()
{
}

template <typename T, typename Compare, typename Container>
template <typename It, typename _Compare, typename _Requires>
priority_queue<T, Compare, Container>::priority_queue(It first, It last)
    : priority_queue(first, last, Compare()) {};

template <typename T, typename Compare, typename Container>
template <typename It>
priority_queue<T, Compare, Container>::priority_queue(It first, It last, Compare comp)
    : base<T, Compare>(comp)
    , _heap(first, last)
{
    typedef typename std::iterator_traits<typename Container::iterator>::difference_type diff_type;
    auto begin_it = _heap.begin();
    diff_type size = last - first;
    for (diff_type position = size - 1; position + 1 > 0; --position) {
        diff_type current(position);

        // bubble down
        while (2 * current + 1 < size) {
            diff_type child(2 * current + 1);
            if (child + 1 != size && !this->_compare(begin_it[child], begin_it[child + 1])) {
                ++child;
            }

            if (!this->_compare(begin_it[current], begin_it[child])) {
                std::iter_swap(begin_it + current, begin_it + child);
                current = child;
            } else {
                break;
            }
        }
    }
};

template <typename T, typename Compare, typename Container>
void priority_queue<T, Compare, Container>::merge(priority_queue<T, Compare, Container>&& src)
{
    std::list<T> temp(_heap.begin(), _heap.end());
    std::copy(src._heap.begin(), src._heap.end(), std::back_inserter(temp));
    priority_queue<T, Compare, Container> result(temp.begin(), temp.end(), this->_compare);
    std::swap(_heap, result._heap);
}

template <typename T, typename Compare, typename Container>
void priority_queue<T, Compare, Container>::insert(const T& item)
{
    uint32_t index(_heap.size());
    _heap.push_back(item);

    while (index > 0) {
        uint32_t parent((index - 1) / 2);
        if (this->_compare(_heap[index], _heap[parent])) {
            std::swap(_heap[index], _heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

template <typename T, typename Compare, typename Container>
T priority_queue<T, Compare, Container>::remove_root()
{
    T prevRoot = _heap.front();
    std::swap(_heap.front(), _heap.back());

    uint32_t current = 0;
    while (2 * current + 1 < _heap.size() - 1) {
        uint32_t child(2 * current + 1);
        if (2 * current + 2 < _heap.size() - 1 && !this->_compare(_heap[child], _heap[child + 1]))
            ++child;

        if (!this->_compare(_heap[current], _heap[child])) {
            std::swap(_heap[current], _heap[child]);
            current = child;
        } else {
            break;
        }
    }

    _heap.pop_back();
    return prevRoot;
}

template <typename T, typename Compare, typename Container>
T priority_queue<T, Compare, Container>::get_root() const
{
    return _heap.front();
}

template <typename T, typename Compare, typename Container>
uint32_t priority_queue<T, Compare, Container>::size() const noexcept
{
    return _heap.size();
}
}

#endif // HEAP_CPP
