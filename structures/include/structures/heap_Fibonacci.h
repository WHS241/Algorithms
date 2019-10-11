#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include "heap_node_base.h"

#include <unordered_set>

namespace heap {
    template<typename T, typename Compare = std::less<>>
    class Fibonacci : public node_base<T, Compare> {
    public:
        typedef typename node_base<T, Compare>::node node;

        Fibonacci() = default;
        Fibonacci(Compare comp);

        template<typename It, typename _Compare = Compare, typename _Requires = typename
        std::enable_if_t<std::is_default_constructible_v<_Compare>>>
        Fibonacci(It first, It last) : Fibonacci(first, last, Compare()){};
        template<typename It>
        Fibonacci(It first, It last, Compare comp)
                : node_base<T, Compare>(comp), _trees(), _min(nullptr) {
            try {
                for (; first != last; ++first)
                    add(*first);
            } catch (...) {
                for (node *root : _trees)
                    delete root;
                throw;
            }
        }

        virtual ~Fibonacci() noexcept;
        Fibonacci(const Fibonacci<T, Compare> &);
        Fibonacci &operator=(const Fibonacci<T, Compare> &);
        Fibonacci(Fibonacci<T, Compare> &&) noexcept;

        template<typename _Compare = Compare, typename _Requires = typename
        std::enable_if_t<std::is_move_assignable_v<_Compare>>>
        Fibonacci &operator=(Fibonacci<T, Compare> && rhs) noexcept {
            if (this != &rhs) {
                for (node *root : _trees)
                    delete root;
                _min = nullptr;
                this->_size = 0;

                std::swap(_trees, rhs._trees);
                std::swap(_min, rhs._min);
                std::swap(this->_size, rhs._size);
                std::swap(this->_compare, rhs._compare);
            }
            return *this;
        }

        // Θ(1)
        virtual node *add(const T &);

        // Θ(1)
        virtual T get_root() const;

        // amortized Θ(log n)
        virtual T remove_root();

        // amortized Θ(1)
        void decrease(node *target, const T &new_val);

        // Θ(1)
        void merge(Fibonacci<T, Compare> &);

    private:
        std::list<node *> _trees;

        node *_min = nullptr;
    };
}

#include <src/structures/heap_Fibonacci.cpp>

#endif //FIBONACCI_HEAP_H
