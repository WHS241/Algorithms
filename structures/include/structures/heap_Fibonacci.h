#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include "heap_node_base.h"

#include <unordered_set>

namespace heap {
template <typename T, typename Compare = std::less<>>
class Fibonacci : public node_base<T, Compare> {
public:
    typedef typename node_base<T, Compare>::node node;

    Fibonacci() = default;
    Fibonacci(Compare comp);

    template <typename It, typename _Compare = Compare,
        typename _Requires = typename std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    Fibonacci(It first, It last);
    template <typename It> Fibonacci(It first, It last, Compare comp);

    virtual ~Fibonacci() noexcept;
    Fibonacci(const Fibonacci<T, Compare>&);
    Fibonacci& operator=(const Fibonacci<T, Compare>&);
    Fibonacci(Fibonacci<T, Compare>&&) noexcept;
    Fibonacci& operator=(Fibonacci<T, Compare>&& rhs) noexcept;

    // Θ(1)
    virtual node* add(const T&);

    // Θ(1)
    virtual T get_root() const;

    // amortized Θ(log n)
    virtual T remove_root();

    // amortized Θ(1)
    void decrease(node* target, const T& new_val);

    // Θ(1)
    void merge(Fibonacci<T, Compare>&);

private:
    std::list<node*> _trees;

    node* _min = nullptr;
};
}

#include <src/structures/heap_Fibonacci.cpp>

#endif // FIBONACCI_HEAP_H
