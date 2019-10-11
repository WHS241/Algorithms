#ifndef BINOMIAL_HEAP_CPP
#define BINOMIAL_HEAP_CPP

#include <cmath>
#include <stdexcept>

namespace heap {
    template<typename T, typename Compare>
    binomial<T, Compare>::binomial(Compare comp)
            : node_base<T, Compare>(comp), trees(), min(nullptr) {
    }

    template<typename T, typename Compare>
    binomial<T, Compare>::~binomial() noexcept {
        for (node *root : trees)
            delete root;
    }

    template<typename T, typename Compare>
    binomial<T, Compare>::binomial(const binomial <T, Compare> &src)
            : node_base<T, Compare>(src), trees(src.trees.size(), nullptr), min(nullptr) {
        this->_size = src._size;
        try {
            std::transform(src.trees.begin(), src.trees.end(), trees.begin(), [this, &src](node *root) {
                if (root == nullptr)
                    return root;

                std::unique_ptr<node> clone(root->_deep_clone());
                if (root == src.min)
                    this->min == clone.get();
                return clone.release();
            });
        } catch (...) {
            for (node *root : trees)
                delete root;
        }
    }

    template<typename T, typename Compare>
    binomial <T, Compare> &binomial<T, Compare>::operator=(const binomial <T, Compare> &rhs) {
        if (this != &rhs) {
            binomial<T, Compare> temp(rhs);
            *this = std::move(temp);
        }
        return *this;
    }

    template<typename T, typename Compare>
    binomial<T, Compare>::binomial(binomial <T, Compare> &&src) noexcept
            : binomial<T, Compare>(src._compare) {
        *this = std::move(src);
    }

    template<typename T, typename Compare>
    binomial <T, Compare> &binomial<T, Compare>::operator=(binomial <T, Compare> &&rhs) noexcept {
        if (this != &rhs) {
            for (node *root : trees) {
                delete root;
            }
            trees.clear();
            min = nullptr;
            this->_size = 0;
            std::swap(trees, rhs.trees);
            std::swap(min, rhs.min);
            std::swap(this->_size, rhs._size);
            std::swap(this->_compare, rhs._compare);
        }
        return *this;
    }

    template<typename T, typename Compare>
    typename binomial<T, Compare>::node *binomial<T, Compare>::add(const T &item) {
        binomial<T, Compare> temp(this->_compare);
        std::unique_ptr<node> add_node(this->s_make_node(item));
        temp._size = 1;
        temp.trees.push_back(add_node.get());
        temp.min = add_node.get();
        merge(temp);
        return add_node.release();
    }

    template<typename T, typename Compare>
    T binomial<T, Compare>::get_root() const {
        if (this->empty())
            throw std::underflow_error("Empty heap");
        return **min;
    }

    template<typename T, typename Compare>
    T binomial<T, Compare>::remove_root() {
        if (this->empty())
            throw std::underflow_error("Empty heap");

        if (this->_size == 1) {
            T old_root = **min;
            delete min;
            this->_size = 0;
            trees.clear();
            min = nullptr;
            return old_root;
        }

        T oldRoot = **min;

        // break into two heaps and merge
        std::vector<node *> subtrees(min->_children.size());
        for (node *child : min->_children)
            // the number of children determines the size of the tree: size = 2^num children
            // Because of the way binomial heaps are built, none of these will be nullptr
            subtrees[child->_children.size()] = child;

        binomial<T, Compare> temp(this->_compare);
        temp.trees = std::move(subtrees);
        auto old_size = this->_size;
        auto old_min = min;
        auto it = std::find(trees.begin(), trees.end(), min);
        try {
            // find the minimum values, since merge() will not compare all roots
            temp.min = temp.trees.empty() ? nullptr
                                          : *std::min_element(temp.trees.begin(), temp.trees.end(),
                                                              [this](node *x, node *y) {
                                                                  return this->_compare(**x, **y);
                                                              });

            // We can now extract the node, updating min
            *it = nullptr;
            min = *std::min_element(trees.begin(), trees.end(), [this](node *x, node *y) {
                // here we actually have to account for nullptr
                return x != nullptr && (y == nullptr || this->_compare(**x, **y));
            });

            // We don't actually need to recalculate the sizes; as long as temp._size reflects emptiness
            // and the two values add up to new size
            temp._size = temp.trees.size();
            this->_size -= (temp._size + 1);

            merge(temp);
        } catch (...) {
            min = old_min;
            *it = min;
            this->_size = old_size;
            throw;
        }

        // isolate old_min and delete (merge should have removed old_min as the parent of the children)
        old_min->_children.clear();
        delete old_min;
        old_min = nullptr;
        return oldRoot;
    }

    template<typename T, typename Compare>
    void binomial<T, Compare>::decrease(
            typename binomial<T, Compare>::node *target, const T &new_value) {
        if (this->_compare(**target, new_value))
            throw std::invalid_argument("Increasing key");

        // use same bubble-up strategy as binary heap
        **target = new_value;
        if (this->_compare(new_value, **min))
            min = target;

        while (target->_parent != nullptr) {
            if (this->_compare(**target, **target->_parent)) {
                auto to_swap = target->_parent;
                target->_parent = target;
                auto it = std::find(to_swap->_children.begin(), to_swap->_children.end(), target);
                *it = to_swap;
                std::swap(target->_parent, to_swap->_parent);
                std::swap(target->_children, to_swap->_children);

                for (node *child : target->_children)
                    child->_parent = target;

                for (node *child : to_swap->_children)
                    child->_parent = to_swap;

                if (target->_parent != nullptr) {
                    // still need to account for next level up
                    it = std::find(
                            target->_parent->_children.begin(), target->_parent->_children.end(), to_swap);
                    *it = target;
                } else {
                    auto tree_it = std::find(trees.begin(), trees.end(), to_swap);
                    *tree_it = target;
                }
            } else {
                break;
            }
        }
    }

    template<typename T, typename Compare>
    void binomial<T, Compare>::merge(binomial <T, Compare> &src) {
        if (this == &src || src.empty())
            return;
        if (this->empty()) {
            *this = std::move(src);
            return;
        }

        uint32_t new_size = this->_size + src._size;
        uint32_t vector_size = 0;
        for (uint32_t i = new_size; i != 0; i /= 2)
            ++vector_size;

        trees.resize(vector_size, nullptr);
        node *carry = nullptr;

        // can "add" like normal arithmetic
        // Comment notation: 0 = nullptr, 1 = not nullptr
        // abc = carry is a, *it1 is b, *it2 is c
        for (auto it1 = trees.begin(), it2 = src.trees.begin(); it1 != trees.end(); ++it1) {
            // Of the 8 possibilities, 000 and 010 can be no-ops
            if (!(carry == nullptr && (it2 == src.trees.end() || *it2 == nullptr))) {
                if (carry == nullptr) {
                    if (*it1 == nullptr) { // 001
                        *it1 = *it2;
                        (*it2)->parent = nullptr;
                    } else { // 011
                        if (this->_compare(***it1, ***it2)) {
                            std::iter_swap(it1, it2);
                        }

                        (*it2)->children.push_back(*it1);
                        (*it1)->parent = *it2;
                        carry = *it2;
                        *it1 = nullptr;
                    }
                } else if (*it1 == nullptr && (it2 == src.trees.end() || *it2 == nullptr)) { // 100
                    *it1 = carry;
                    carry->_parent = nullptr;
                    carry = nullptr;
                } else if (*it1 == nullptr || it2 == src.trees.end() || *it2 == nullptr) {
                    if (*it1 == nullptr) // 101
                        std::iter_swap(it1, it2);

                    // 110
                    if (this->_compare(***it1, **carry))
                        std::swap(carry, *it1);

                    carry->_children.push_back(*it1);
                    (*it1)->parent = carry;
                    *it1 = nullptr;
                } else { // 111
                    // we'll keep it1 and merge it2 into carry
                    if (this->_compare(***it2, **carry))
                        std::swap(*it2, carry);

                    carry->_children.push_back(*it2);
                    (*it2)->parent = carry;
                }
            }

            if (it2 != src.trees.end())
                ++it2;
        }

        min = (min != nullptr && this->_compare(**min, **src.min)) ? min : src.min;
        src.min = nullptr;
        src.trees.clear();
        src._size = 0;
        this->_size = new_size;
    }
}
#endif // BINOMIAL_HEAP_CPP
