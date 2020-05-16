#ifndef FIBONACCI_HEAP_CPP
#define FIBONACCI_HEAP_CPP

#include <algorithm>
#include <iterator>
#include <memory>

namespace heap {
template <typename T, typename Compare>
Fibonacci<T, Compare>::Fibonacci(Compare comp)
    : node_base<T, Compare>(comp)
    , _trees()
    , _min(nullptr)
{
}

template <typename T, typename Compare> Fibonacci<T, Compare>::~Fibonacci() noexcept
{
    for (node* root : _trees)
        delete root;
}

template <typename T, typename Compare>
Fibonacci<T, Compare>::Fibonacci(const Fibonacci<T, Compare>& src)
    : node_base<T, Compare>(src)
    , _trees()
    , _min(nullptr)
{
    try {
        std::transform(src._trees.begin(), src._trees.end(), std::back_inserter(_trees),
            [this, &src](const node* root) {
                std::unique_ptr<node> new_tree(root->_deep_clone());
                this->_s_set_flag(*new_tree, this->_s_get_flag(*root));
                if (root == src._min)
                    _min = new_tree.get();
                return new_tree.release();
            });
    } catch (...) {
        for (node* root : _trees)
            delete root;
        throw;
    }
}

template <typename T, typename Compare>
template <typename It, typename _Compare, typename _Requires>
Fibonacci<T, Compare>::Fibonacci(It first, It last)
    : Fibonacci(first, last, Compare())
{
}

template <typename T, typename Compare>
template <typename It>
Fibonacci<T, Compare>::Fibonacci(It first, It last, Compare comp)
    : node_base<T, Compare>(comp)
    , _trees()
    , _min(nullptr)
{
    try {
        for (; first != last; ++first)
            add(*first);
    } catch (...) {
        for (node* root : _trees)
            delete root;
        throw;
    }
}

template <typename T, typename Compare>
Fibonacci<T, Compare>& Fibonacci<T, Compare>::operator=(const Fibonacci<T, Compare>& rhs)
{
    if (this != &rhs) {
        Fibonacci<T, Compare> temp(rhs);
        *this = std::move(rhs);
    }
    return *this;
}

template <typename T, typename Compare>
Fibonacci<T, Compare>::Fibonacci(Fibonacci<T, Compare>&& src) noexcept
    : Fibonacci<T, Compare>(src.comp)
{
    *this = std::move(src);
}

template <typename T, typename Compare>
Fibonacci<T, Compare>& Fibonacci<T, Compare>::operator=(Fibonacci<T, Compare>&& rhs) noexcept
{
    if (this != &rhs) {
        for (node* root : _trees)
            delete root;
        _min = nullptr;
        this->_size = 0;

        std::swap(_trees, rhs._trees);
        std::swap(_min, rhs._min);
        std::swap(this->_size, rhs._size);
        std::swap(this->_compare, rhs._compare);
    }
    return *this;
}

template <typename T, typename Compare>
typename Fibonacci<T, Compare>::node* Fibonacci<T, Compare>::add(const T& item)
{
    std::unique_ptr<node> new_node(this->s_make_node(item));
    _trees.push_back(new_node.get());
    if (_min == nullptr || this->_compare(item, **_min))
        _min = new_node.get();

    ++this->_size;
    return new_node.release();
}

template <typename T, typename Compare> T Fibonacci<T, Compare>::get_root() const
{
    if (this->empty())
        throw std::underflow_error("Empty heap");
    return **_min;
}

template <typename T, typename Compare> T Fibonacci<T, Compare>::remove_root()
{
    if (this->empty())
        throw std::underflow_error("Empty heap");

    T old_root = **_min;
    if (this->_size == 1) {
        delete _min;
        _trees.clear();
        this->_size = 0;
    } else {

        // number of new trees grows logarithmically (a tree of size n must have height <= log_phi
        // n) for simplicity, we know sqrt(2) < phi < 2
        uint32_t num_trees = 1;
        for (uint32_t i = this->_size - 1; i > 1; i /= 2)
            num_trees += 2;
        std::vector<typename std::list<node*>::iterator> new_roots(num_trees, _trees.end());

        // detach all children
        for (node* child : _min->_children)
            child->_parent = nullptr;
        _trees.splice(_trees.end(), _min->_children);

        // remove min
        auto found = std::find(_trees.begin(), _trees.end(), _min);
        auto after_found = found;
        ++after_found;
        _trees.erase(found, after_found);
        delete _min;
        _min = nullptr;
        --this->_size;

        // consolidate
        for (auto it = _trees.begin(); it != _trees.end(); ++it) {
            uint32_t tree_size = (*it)->_children.size();

            while (new_roots[tree_size] != _trees.end()) {
                auto compare_candidate = new_roots[tree_size];
                if (this->_compare(***compare_candidate, ***it))
                    std::iter_swap(it, compare_candidate);
                auto merge_bound = compare_candidate;
                ++merge_bound;
                (*it)->_children.splice(
                    (*it)->_children.end(), _trees, compare_candidate, merge_bound);
                (*compare_candidate)->_parent = *it;
                new_roots[tree_size] = _trees.end();
                ++tree_size;
            }

            new_roots[tree_size] = it;
        }

        // unmark any roots and find new min
        for (node* root : _trees) {
            this->_s_set_flag(*root, false);
            if (_min == nullptr || this->_compare(**root, **_min))
                _min = root;
        }
    }
    return old_root;
}

template <typename T, typename Compare>
void Fibonacci<T, Compare>::decrease(typename Fibonacci<T, Compare>::node* target, const T& new_val)
{
    **target = new_val;
    if (this->_compare(new_val, **_min))
        _min = target;

    // cut and mark, repeat until root or unmarked
    if (target->_parent != nullptr && this->_compare(**target, **target->_parent)) {
        do {
            node* old_parent = target->_parent;
            target->_parent = nullptr;
            this->_s_set_flag(*target, false);
            auto to_remove
                = std::find(old_parent->_children.begin(), old_parent->_children.end(), target);
            auto to_remove_next = to_remove;
            ++to_remove_next;
            _trees.splice(_trees.end(), old_parent->_children, to_remove, to_remove_next);
            target = old_parent;
        } while (this->_s_get_flag(*target));

        if (target->_parent != nullptr)
            this->_s_set_flag(*target, true);
    }
}

template <typename T, typename Compare>
void Fibonacci<T, Compare>::merge(Fibonacci<T, Compare>& src)
{
    if (_min == nullptr || (src._min != nullptr && this->_compare(**src._min, **_min)))
        _min = src._min;
    _trees.splice(_trees.end(), src._trees);
    this->_size += src._size;
    src._size = 0;
    src._min = nullptr;
}
}

#endif // FIBONACCI_HEAP_CPP