#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "heap_node_base.h"

#include <vector>

namespace heap {

/**
 * Jean Vuillemin
 * A data structure for manipulating priority queues
 * (1978) doi:10.1145/359460.359478
 */
template<typename T, typename Compare = std::less<T>>
class binomial : public node_base<T, Compare> {
    public:
    using typename node_base<T, Compare>::node, typename node_base<T, Compare>::node_wrapper;

    binomial() = default;
    explicit binomial(Compare comp);

    template<
      typename It, typename _Compare = Compare,
      typename _Requires = typename std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    binomial(It first, It last);
    template<typename It> binomial(It first, It last, Compare comp);

    virtual ~binomial() noexcept;
    binomial(const binomial<T, Compare>&);
    binomial& operator=(const binomial<T, Compare>&);
    binomial(binomial<T, Compare>&&) noexcept;
    binomial& operator=(binomial<T, Compare>&& rhs) noexcept;

    // Θ(log n)
    virtual node_wrapper add(const T&);

    // Θ(1)
    virtual T get_root() const;

    // Θ(log n)
    virtual T remove_root();

    // Θ(log n)
    void decrease(node_wrapper target, const T& new_value);

    // Θ(log n)
    void merge(binomial<T, Compare>&);

    private:
    std::vector<node*> _trees;

    node* _min;
};
} // namespace heap

#include "../../src/structures/heap_binomial.tpp"

#endif // BINOMIAL_HEAP_H
