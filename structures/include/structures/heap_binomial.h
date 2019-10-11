#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "heap_node_base.h"

#include <vector>

namespace heap {
    template<typename T, typename Compare = std::less<>>
    class binomial : public node_base<T, Compare> {
    public:
        typedef typename node_base<T, Compare>::node node;

        binomial() = default;
        explicit binomial(Compare comp);

        template<typename It, typename _Compare = Compare, typename _Requires = typename
        std::enable_if_t<std::is_default_constructible_v<_Compare>>>
        binomial(It first, It last) : binomial(first, last, Compare()) {};
        template<typename It>
        binomial(It first, It last, Compare comp)
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

        virtual ~binomial() noexcept;
        binomial(const binomial<T, Compare> &);
        binomial &operator=(const binomial<T, Compare> &);
        binomial(binomial<T, Compare> &&) noexcept;

        template<typename _Compare = Compare, typename _Requires = typename
        std::enable_if_t<std::is_move_assignable_v<_Compare>>>
        binomial &operator=(binomial<T, Compare> && rhs) noexcept{
            if (this != &rhs) {
                for (node *root : _trees) {
                    delete root;
                }
                _trees.clear();
                _min = nullptr;
                this->_size = 0;
                std::swap(_trees, rhs._trees);
                std::swap(_min, rhs._min);
                std::swap(this->_size, rhs._size);
                this->_compare =  std::move(rhs._compare);
            }
            return *this;
        }

        // Θ(log n)
        virtual node *add(const T &);

        // Θ(1)
        virtual T get_root() const;

        // Θ(log n)
        virtual T remove_root();

        // Θ(log n)
        void decrease(node *target, const T &new_value);

        // Θ(log n)
        void merge(binomial<T, Compare> &);

    private:
        std::vector<node *> _trees;

        node *_min;
    };
}

#include <src/structures/heap_binomial.cpp>

#endif // BINOMIAL_HEAP_H