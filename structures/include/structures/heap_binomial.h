#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "heap_node_base.h"

namespace heap {
    template<typename T, typename Compare = std::less<>>
    class binomial : public node_base<T, Compare> {
    public:
        typedef typename node_base<T, Compare>::node node;

        binomial() = default;
        explicit binomial(Compare comp);

        template<typename It, typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
        binomial(It first, It last) : binomial(first, last, Compare()) {};
        template<typename It>
        binomial(It first, It last, Compare comp)
                : node_base<T, Compare>(comp), trees(), min(nullptr) {
            try {
                for (; first != last; ++first)
                    add(*first);
            } catch (...) {
                for (node *root : trees)
                    delete root;
                throw;
            }
        }

        virtual ~binomial() noexcept;
        binomial(const binomial<T, Compare> &);
        binomial &operator=(const binomial<T, Compare> &);
        binomial(binomial<T, Compare> &&) noexcept;
        binomial &operator=(binomial<T, Compare> &&) noexcept;

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
        std::vector<node *> trees;

        node *min;
    };
}

#include <src/structures/heap_binomial.cpp>

#endif // BINOMIAL_HEAP_H