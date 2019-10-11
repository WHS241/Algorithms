#ifndef HEAP_H
#define HEAP_H

#include <functional>
#include <vector>

#include "binary_tree_base.h"

namespace heap {
// Base class for heap structures
    template<typename T, typename Compare = std::less<>>
    class base {
    public:
        base() = default;
        explicit base(Compare comp);

        virtual void insert(const T &) = 0;
        virtual T get_root() const = 0;
        virtual T remove_root() = 0;
        virtual uint32_t size() const noexcept = 0;
        virtual bool empty() const noexcept;

    protected :
        Compare _compare;
    };

// Array-based binary heap
    template<typename T, typename Container = std::vector<T>, typename Compare = std::less<>>
    class priority_queue : virtual public base<T, Compare> {
    public:
        static_assert(std::is_same_v<T, typename Container::value_type>, "value_type must be same as underlying container");
        priority_queue() = default;
        explicit priority_queue(Compare comp);

        // Θ(n)
        template<typename It, typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
        priority_queue(It first, It last) : priority_queue(first, last, Compare()) {};
        template<typename It>
        priority_queue(It first, It last, Compare comp)
                : base<T, Compare>(comp), _heap(first, last) {
            for (uint32_t position = _heap.size() - 1; position + 1 > 0; --position) {
                uint32_t current(position);

                // bubble down
                while (2 * current + 1 < _heap.size()) {
                    uint32_t child(2 * current + 1);
                    if (2 * current + 2 < _heap.size() && !this->_compare(_heap[child], _heap[child + 1]))
                        ++child;

                    if (!this->_compare(_heap[current], _heap[child])) {
                        std::swap(_heap[current], _heap[child]);
                        current = child;
                    } else {
                        break;
                    }
                }
            }
        };

        // Θ(log n)
        virtual void insert(const T &);

        // Θ(n)
        virtual void merge(priority_queue<T, Compare> &&);

        // Θ(log n)
        virtual T remove_root();

        // Θ(1)
        virtual T get_root() const;
        virtual uint32_t size() const noexcept;

    private:
        Container _heap;
    };
}

#include "src/structures/heap_base.cpp"

#endif // HEAP_H
