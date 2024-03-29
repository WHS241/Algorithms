#ifndef NODE_HEAP_H
#define NODE_HEAP_H

#include "heap_base.h"

namespace heap {
template<typename T, typename Compare> class binary_heap;
template<typename T, typename Compare> class binomial;
template<typename T, typename Compare> class Fibonacci;

// node_base:
// adding an element returns a pointer to the added node
// pointer is valid until element is removed from heap
template<typename T, typename Compare = std::less<T>> class node_base : public base<T, Compare> {
    public:
    node_base() = default;
    explicit node_base(Compare comp) : base<T, Compare>(comp) {}

    class node {
        public:
        virtual ~node() noexcept;
        node(const node&) = delete;
        node& operator=(const node&) = delete;
        const T& operator*() const;
        const T* operator->() const;
        T& operator*();
        T* operator->();

        private:
        node() = default;
        node(const T& value) : _value(value), _parent(nullptr), _children(), _flag(false){};
        node* _deep_clone() const;

        T _value;
        node* _parent;
        std::list<node*> _children;
        bool _flag;

        friend class node_base<T, Compare>;
        friend class Fibonacci<T, Compare>;
    };

    class node_wrapper {
        public:
        node_wrapper(node* = nullptr) noexcept;
        const node* get() const noexcept;

        private:
        node* _node;
        friend class node_base<T, Compare>;
    };

    virtual ~node_base() noexcept = default;

    // Inserting with add returns a pointer to the corresponding value
    void insert(const T& item) override;
    virtual node_wrapper add(const T& item) = 0;

    // Decrease the value stored in the target pointer
    // Should throw if new_val is greater
    virtual void decrease(node_wrapper target, const T& new_val) = 0;

    uint32_t size() const noexcept override;

    // Though not included due to inheritance issues, children should also implement
    // merge([class]&& src) which moves all the values of src into this, emptying src.
    // Node pointers should not be invalidated by this merge.

    protected:
    // Copies _size over
    // Protected because this is meant to only be a helper function for the copy ctor of concrete
    // children
    node_base(const node_base<T, Compare>& src) = default;

    static node* _s_extract_node(node_wrapper&) noexcept;
    static node* _s_make_node(const T& item);
    static bool _s_get_flag(const node&);
    static void _s_set_flag(node&, bool);

    uint32_t _size = 0;
};
} // namespace heap

#include "../../src/structures/heap_node_base.tpp"

#endif // !NODE_HEAP_H
