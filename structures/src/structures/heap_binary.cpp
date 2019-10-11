#ifndef NODE_BINARY_HEAP_CPP
#define NODE_BINARY_HEAP_CPP
#include <algorithm>

namespace heap {
    template<typename T, typename Compare>
    binary_heap<T, Compare>::binary_heap(Compare comp)
            : node_base<T, Compare>(comp) {
    }

    template<typename T, typename Compare>
    binary_heap<T, Compare>::binary_heap(const binary_heap <T, Compare> &src)
            : node_base<T, Compare>(src), _root() {
        if (!src.empty())
            _root.reset(src._root->_deep_clone());
    }

    template<typename T, typename Compare>
    binary_heap <T, Compare> &binary_heap<T, Compare>::operator=(
            const binary_heap <T, Compare> &rhs) {
        if (this != &rhs) {
            binary_heap<T, Compare> temp(rhs);
            *this = std::move(temp);
        }
        return *this;
    }

    template<typename T, typename Compare>
    binary_heap<T, Compare>::binary_heap(binary_heap <T, Compare> &&src) noexcept
            : binary_heap<T, Compare>(src._compare) {
        *this = std::move(src);
    }

    template<typename T, typename Compare>
    binary_heap <T, Compare> &binary_heap<T, Compare>::operator=(binary_heap <T, Compare> &&rhs) noexcept {
        if (this != &rhs) {
            _root.reset();
            this->_size = 0;
            _root.swap(rhs._root);
            std::swap(this->_size, rhs._size);
            std::swap(this->_compare, rhs._compare);
        }
        return *this;
    }

    template<typename T, typename Compare>
    typename binary_heap<T, Compare>::node *binary_heap<T, Compare>::add(const T &item) {
        std::unique_ptr<node> new_node(new node(item));

        if (this->_size == 0) {
            _root = std::move(new_node);
            ++this->_size;
            return _root.get();
        }
        // use a stack of bools to determine the direction to go to parent
        std::stack<bool> go_left;
        for (uint32_t i = (this->_size + 1) / 2; i != 1; i /= 2) {
            go_left.push(i % 2 == 0);
        }
        node *parent = _root.get();

        while (!go_left.empty()) {
            parent = go_left.top() ? parent->_children.front() : parent->_children.back();
            go_left.pop();
        }

        node *ptr = new_node.get();
        parent->_children.push_back(ptr);
        ptr->_parent = parent;
        new_node.release();
        ++this->_size;

        // bubble up to correct position
        while (ptr != _root.get()) {
            parent = ptr->_parent;
            bool is_left_child = (parent->_children.front() == ptr);
            if (this->_compare(**ptr, **parent))
                _bubble_down(parent, is_left_child);
            else
                break;
        }

        return ptr;
    }

    template<typename T, typename Compare>
    void binary_heap<T, Compare>::merge(binary_heap <T, Compare> &src) {
        if (this == &src)
            return;

        std::list<T> dataset = s_data(this->_root.get());
        dataset.splice(dataset.end(), s_data(src._root.get()));
        *this = binary_heap(dataset.begin(), dataset.end(), this->_compare);
        src._root.reset();
        src._size = 0;
    }

    template<typename T, typename Compare>
    T binary_heap<T, Compare>::get_root() const {
        if (this->empty())
            throw std::underflow_error("Empty heap");
        return **_root;
    }

    template<typename T, typename Compare>
    T binary_heap<T, Compare>::remove_root() {
        if (this->empty())
            throw std::underflow_error("Empty heap");

        T to_return = std::move(**_root);

        // edge case
        if (this->_size == 1) {
            this->_root.reset();
            this->_size = 0;
            return to_return;
        }

        // find last node, the same way as in add
        std::stack<bool> go_left;
        for (uint32_t i = this->_size; i != 1; i /= 2)
            go_left.push(i % 2 == 0);

        node *final = this->_root.get();
        while (!go_left.empty()) {
            final = go_left.top() ? final->_children.front() : final->_children.back();
            go_left.pop();
        }

        // swap node positions
        node *to_remove = this->_root.release();
        final->_children.swap(to_remove->_children);
        for (node *child : final->_children) {
            child->_parent = final;
        }
        node *old_parent = final->_parent;
        final->_parent = nullptr;
        auto it = std::find(old_parent->_children.begin(), old_parent->_children.end(), final);
        auto temp = it++;
        old_parent->_children.erase(temp, it);
        this->_root.reset(final);

        delete to_remove;
        to_remove = nullptr;
        --this->_size;

        // now the bubble-down
        while (!final->_children.empty()) {
            node *left_child = final->_children.front();
            node *right_child = final->_children.back();
            bool swap_left = this->_compare(**left_child, **right_child);
            node *compare_ptr = swap_left ? left_child : right_child;
            if (!this->_compare(**final, **compare_ptr))
                _bubble_down(final, swap_left);
            else
                break;
        }

        return to_return;
    }

    template<typename T, typename Compare>
    void binary_heap<T, Compare>::decrease(
            typename binary_heap<T, Compare>::node *target, const T &new_val) {
        if (this->_compare(**target, new_val))
            throw std::invalid_argument("Increasing key");

        **target = new_val;
        // bubble up
        while (target != _root.get()) {
            node *parent = target->_parent;
            bool left_child = (parent->_children.front() == target);
            if (this->_compare(**target, **parent))
                _bubble_down(parent, left_child);
            else
                break;
        }
    }

    template<typename T, typename Compare>
    void binary_heap<T, Compare>::_bubble_down(
            typename binary_heap<T, Compare>::node *parent, bool use_left_child) noexcept {
        // move all pointers in/out of parent and bubbling child,
        // except grandparent pointer (handled below)
        node *to_swap = use_left_child ? parent->_children.front() : parent->_children.back();
        to_swap->_parent = parent->_parent;
        parent->_parent = to_swap;
        parent->_children.swap(to_swap->_children);
        if (use_left_child) {
            to_swap->_children.front() = parent;
            to_swap->_children.back()->parent = to_swap;
        } else {
            to_swap->_children.back() = parent;
            to_swap->_children.front()->parent = to_swap;
        }
        for (node *child : parent->_children)
            child->_parent = parent;

        // re-point grandparent or root pointer
        if (parent == this->_root.get()) {
            this->_root.release();
            this->_root.reset(to_swap);
        } else {
            node *grandparent = to_swap->_parent;
            auto it = std::find(grandparent->_children.begin(), grandparent->_children.end(), parent);
            *it = to_swap;
        }
    }

    template<typename T, typename Compare>
    std::list<T> binary_heap<T, Compare>::s_data(const node *root) {
        std::list<T> result;
        if (root != nullptr) {
            result.push_back(**root);
            for (auto child : root->_children)
                result.splice(result.end(), s_data(child));
        }

        return result;
    }
}

#endif // !NODE_BINARY_HEAP_CPP
