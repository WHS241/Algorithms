#ifndef HEAP_H
#define HEAP_H

#include <functional>
#include <vector>

#include "binary_tree_base.h"

namespace heap {
// Base class for heap structures
template <typename T, typename Compare = std::less<T>> class base {
public:
    base() = default;
    explicit base(Compare comp);

    virtual void insert(const T&) = 0;
    virtual T get_root() const = 0;
    virtual T remove_root() = 0;
    virtual uint32_t size() const noexcept = 0;
    virtual bool empty() const noexcept;

protected:
    Compare _compare;
};

// Array-based binary heap
template <typename T, typename Compare = std::less<T>, typename Container = std::vector<T>>
class priority_queue : virtual public base<T, Compare> {
public:
    static_assert(
        std::is_invocable_r_v<bool, Compare, T, T>, "comparator incompatible with data types");
    static_assert(std::is_same_v<T, typename Container::value_type>,
        "value_type must be same as underlying container");
    static_assert(std::is_same_v<std::random_access_iterator_tag,
                      typename Container::iterator::iterator_category>,
        "random-access container required");
    
    
    priority_queue() = default;
    explicit priority_queue(Compare comp);

    // Θ(n)
    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<Compare>>>
    priority_queue(It first, It last);
    template <typename It> priority_queue(It first, It last, Compare comp);

    // Θ(log n)
    virtual void insert(const T&);

    // Θ(n)
    virtual void merge(priority_queue<T, Compare, Container>&&);

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
