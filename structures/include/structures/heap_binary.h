#ifndef NODE_BINARY_HEAP_H
#define NODE_BINARY_HEAP_H

#include <list>
#include <memory>
#include <queue>
#include <stack>

#include "heap_node_base.h"

namespace heap {
    template<typename T, typename Compare = std::less<>>
    class binary_heap : public node_base<T, Compare> {
    public:
        typedef typename node_base<T, Compare>::node node;

        binary_heap() = default;
        explicit binary_heap(Compare comp);

        // Θ(n)
        template<typename It, typename _Compare = Compare, typename _Requires = typename
        std::enable_if_t<std::is_default_constructible_v<_Compare>>>
        binary_heap(It first, It last) : binary_heap(first, last, Compare()) {};
        template<typename It>
        binary_heap(It first, It last, Compare comp)
                : node_base<T, Compare>(comp) {
            this->_size = std::distance(first, last);
            if (first != last) {
                _root.reset(this->s_make_node(*first));
                ++first;
                std::queue<node *> node_queue; // for adding elements
                std::stack<node *> node_stack; // for the bubble down
                node_queue.push(_root.get());
                node_stack.push(_root.get());

                // create all the nodes, attach to heap
                for (; first != last; ++first) {
                    node *parent = node_queue.front();
                    if (!parent->_children.empty())
                        node_queue.pop();

                    node *new_node = this->s_make_node(*first);
                    new_node->_parent = parent;
                    parent->_children.push_back(new_node);
                    node_queue.push(new_node);
                    node_stack.push(new_node);
                }

                while (!node_stack.empty()) {
                    node *start_ptr = node_stack.top();
                    node_stack.pop();
                    while (!start_ptr->_children.empty()) {
                        node *left_child = start_ptr->_children.front();
                        node *right_child = start_ptr->_children.back();
                        bool swap_left = this->_compare(**left_child, **right_child);
                        node *compare_ptr = swap_left ? left_child : right_child;
                        if (!this->_compare(**start_ptr, **compare_ptr))
                            _bubble_down(start_ptr, swap_left);
                        else
                            break;
                    }
                }
            }
        };

        virtual ~binary_heap() noexcept = default;
        binary_heap(const binary_heap<T, Compare> &);
        binary_heap &operator=(const binary_heap<T, Compare> &);
        binary_heap(binary_heap<T, Compare> &&) noexcept;
        binary_heap &operator=(binary_heap<T, Compare> &&) noexcept;

        // Θ(log n)
        virtual node *add(const T &);

        // Θ(1)
        virtual T get_root() const;

        // Θ(log n)
        virtual T remove_root();

        // Θ(log n)
        void decrease(node *target, const T &new_val);

        // Θ(n)
        // Binary heap merging currently will invalidate all previous Node*
        void merge(binary_heap<T, Compare> &);

    private:
        void _bubble_down(node *parent, bool use_left_child) noexcept;
        static std::list<T> s_data(const node *);

        std::unique_ptr<node> _root;
    };
}

#include <src/structures/heap_binary.cpp>

#endif // !NODE_BINARY_HEAP_H
