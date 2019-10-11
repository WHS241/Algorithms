#ifndef FIBONACCI_HEAP_CPP
#define FIBONACCI_HEAP_CPP

#include <algorithm>
#include <iterator>
#include <memory>

namespace heap {
    template<typename T, typename Compare>
    Fibonacci<T, Compare>::Fibonacci(Compare comp)
            : node_base<T, Compare>(comp), trees(), min(nullptr) {
    }

    template<typename T, typename Compare>
    Fibonacci<T, Compare>::~Fibonacci() noexcept {
        for (node *root : trees)
            delete root;
    }

    template<typename T, typename Compare>
    Fibonacci<T, Compare>::Fibonacci(const Fibonacci <T, Compare> &src)
            : node_base<T, Compare>(src), trees(), min(nullptr) {
        try {
            std::transform(src.trees.begin(), src.trees.end(), std::back_inserter(trees),
                           [this, &src](const node *root) {
                               std::unique_ptr<node> new_tree(root->deepClone());
                               new_tree->_flag = root->_flag;
                               if (root == src.min)
                                   min = new_tree.get();
                               return new_tree.release();
                           });
        } catch (...) {
            for (node *root : trees)
                delete root;
            throw;
        }
    }

    template<typename T, typename Compare>
    Fibonacci <T, Compare> &Fibonacci<T, Compare>::operator=(
            const Fibonacci <T, Compare> &rhs) {
        if (this != &rhs) {
            Fibonacci<T, Compare> temp(rhs);
            *this = std::move(rhs);
        }
        return *this;
    }

    template<typename T, typename Compare>
    Fibonacci<T, Compare>::Fibonacci(Fibonacci <T, Compare> &&src) noexcept
            : Fibonacci<T, Compare>(src.comp) {
        *this = std::move(src);
    }

    template<typename T, typename Compare>
    Fibonacci <T, Compare> &Fibonacci<T, Compare>::operator=(Fibonacci <T, Compare> &&rhs) noexcept {
        if (this != &rhs) {
            for (node *root : trees)
                delete root;
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
    typename Fibonacci<T, Compare>::node *Fibonacci<T, Compare>::add(const T &item) {
        std::unique_ptr<node> new_node(this->s_make_node(item));
        trees.push_back(new_node.get());
        if (min == nullptr || this->_compare(item, **min))
            min = new_node.get();

        ++this->_size;
        return new_node.release();
    }

    template<typename T, typename Compare>
    T Fibonacci<T, Compare>::get_root() const {
        if (this->empty())
            throw std::underflow_error("Empty heap");
        return **min;
    }

    template<typename T, typename Compare>
    T Fibonacci<T, Compare>::remove_root() {
        if (this->empty())
            throw std::underflow_error("Empty heap");

        T old_root = **min;
        if (this->_size == 1) {
            delete min;
            trees.clear();
            this->_size = 0;
        } else {

            // number of new trees grows logarithmically (a tree of size n must have height <= log_phi
            // n) for simplicity, we know sqrt(2) < phi < 2
            uint32_t num_trees = 1;
            for (uint32_t i = this->_size - 1; i > 1; i /= 2)
                num_trees += 2;
            std::vector<typename std::list<node *>::iterator> new_roots(num_trees, trees.end());

            // detach all children
            for (node *child : min->_children)
                child->_parent = nullptr;
            trees.splice(trees.end(), min->_children);

            // remove min
            auto found = std::find(trees.begin(), trees.end(), min);
            auto after_found = found;
            ++after_found;
            trees.erase(found, after_found);
            delete min;
            min = nullptr;
            --this->_size;


            // consolidate
            for (auto it = trees.begin(); it != trees.end(); ++it) {
                uint32_t tree_size = (*it)->_children.size();

                while (new_roots[tree_size] != trees.end()) {
                    auto compare_candidate = new_roots[tree_size];
                    if (this->_compare(***compare_candidate, ***it))
                        std::iter_swap(it, compare_candidate);
                    auto merge_bound = compare_candidate;
                    ++merge_bound;
                    (*it)->_children.splice((*it)->_children.end(), trees, compare_candidate, merge_bound);
                    (*compare_candidate)->_parent = *it;
                    new_roots[tree_size] = trees.end();
                    ++tree_size;
                }

                new_roots[tree_size] = it;
            }

            // unmark any roots and find new min
            for (node *root : trees) {
                root->_flag = false;
                if (min == nullptr || this->_compare(**root, **min))
                    min = root;
            }
        }
        return old_root;
    }

    template<typename T, typename Compare>
    void Fibonacci<T, Compare>::decrease(
            typename Fibonacci<T, Compare>::node *target, const T &new_val) {
        **target = new_val;
        if (this->_compare(new_val, **min))
            min = target;

        // cut and mark, repeat until root or unmarked
        if (target->_parent != nullptr && this->_compare(**target, **target->_parent)) {
            do {
                node *old_parent = target->_parent;
                target->_parent = nullptr;
                target->_flag = false;
                auto to_remove
                        = std::find(old_parent->_children.begin(), old_parent->_children.end(), target);
                auto to_remove_next = to_remove;
                ++to_remove_next;
                trees.splice(trees.end(), old_parent->_children, to_remove, to_remove_next);
                target = old_parent;
            } while (target->_flag);

            if (target->_parent != nullptr)
                target->_flag = true;
        }
    }

    template<typename T, typename Compare>
    void Fibonacci<T, Compare>::merge(Fibonacci <T, Compare> &src) {
        if (min == nullptr || (src.min != nullptr && this->_compare(**src.min, **min)))
            min = src.min;
        trees.splice(trees.end(), src.trees);
        this->_size += src._size;
        src._size = 0;
        src.min = nullptr;
    }
}

#endif // FIBONACCI_HEAP_CPP