#ifndef NODE_BINARY_HEAP_H
#define NODE_BINARY_HEAP_H

#include <list>
#include <memory>
#include <queue>
#include <stack>

#include "heap_node_base.h"

namespace heap {
template <typename T, typename Compare = std::less<T>>
class binary_heap : public node_base<T, Compare> {
public:
    using typename node_base<T, Compare>::node, typename node_base<T, Compare>::node_wrapper;

    binary_heap() = default;
    explicit binary_heap(Compare comp);

    // Θ(n)
    template <typename It, typename _Compare = Compare,
        typename _Requires = typename std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    binary_heap(It first, It last);
    template <typename It> binary_heap(It first, It last, Compare comp);

    virtual ~binary_heap() noexcept = default;
    binary_heap(const binary_heap<T, Compare>&);
    binary_heap& operator=(const binary_heap<T, Compare>&);
    binary_heap(binary_heap<T, Compare>&&) noexcept;
    binary_heap& operator=(binary_heap<T, Compare>&&) noexcept;

    // Θ(log n)
    virtual node_wrapper add(const T&);

    // Θ(1)
    virtual T get_root() const;

    // Θ(log n)
    virtual T remove_root();

    // Θ(log n)
    void decrease(node_wrapper target, const T& new_val);

    // Θ(n)
    // Binary heap merging currently will invalidate all previous Node*
    void merge(binary_heap<T, Compare>&);

private:
    void _bubble_down(node* parent, bool use_left_child) noexcept;
    static std::list<T> _s_data(const node*);

    std::unique_ptr<node> _root;
};
}

#include "../../src/structures/heap_binary.tpp"

#endif // !NODE_BINARY_HEAP_H
