#ifndef B_TREE_TPP
#define B_TREE_TPP

#include <algorithm>

namespace tree {
template<typename T, std::size_t K, typename Compare>
B_tree<T, K, Compare>::node::node() : _size(0) {
    _children.front() = nullptr;
};

template<typename T, std::size_t K, typename Compare>
B_tree<T, K, Compare>::node::~node() noexcept {
    std::for_each(_children.begin(), _children.begin() + _size + 1, [](auto p) { delete p; });
}

template<typename T, std::size_t K, typename Compare>
typename B_tree<T, K, Compare>::node&
  B_tree<T, K, Compare>::node::operator=(B_tree<T, K, Compare>::node&& rhs) {
    if (this != &rhs) {
        std::swap(_vals, rhs._vals);
        std::swap(_children, rhs._children);
        std::swap(_size, rhs._size);
    }
    return *this;
}

template<typename T, std::size_t K, typename Compare>
typename B_tree<T, K, Compare>::node* B_tree<T, K, Compare>::node::clone() const {
    std::unique_ptr<node> c(new node());
    c->_size = _size;
    std::copy(_vals.begin(), _vals.begin() + _size, c->_vals.begin());
    std::transform(_children.begin(), _children.begin() + _size + 1, c->_children.begin(),
                   [](node* p) { return p->clone(); });
    return c.release();
}

template<typename T, std::size_t K, typename Compare>
template<typename, typename>
B_tree<T, K, Compare>::B_tree(bool duplicates) : B_tree(Compare(), duplicates){};

template<typename T, std::size_t K, typename Compare>
B_tree<T, K, Compare>::B_tree(const Compare& comp, bool duplicates) :
    _size(0), _root(), _comp(comp), _dups(duplicates) {}

template<typename T, std::size_t K, typename Compare>
template<typename It, typename, typename>
B_tree<T, K, Compare>::B_tree(It first, It last, bool duplicates) :
    B_tree(first, last, Compare(), duplicates) {}

template<typename T, std::size_t K, typename Compare>
template<typename It>
B_tree<T, K, Compare>::B_tree(It first, It last, const Compare& comp, bool duplicates) :
    _size(0), _root(), _comp(comp), _dups(duplicates) {
    while (first != last)
        insert(*(first++));
}

template<typename T, std::size_t K, typename Compare>
B_tree<T, K, Compare>::B_tree(const B_tree<T, K, Compare>& src) :
    _size(src._size), _root(src._root.clone()), _comp(src._comp), _dups(src._dups) {}

template<typename T, std::size_t K, typename Compare>
B_tree<T, K, Compare>& B_tree<T, K, Compare>::operator=(const B_tree<T, K, Compare>& rhs) {
    if (this != *rhs) {
        B_tree tmp(rhs);
        *this = std::move(tmp);
    }
    return *this;
}

template<typename T, std::size_t K, typename Compare>
bool B_tree<T, K, Compare>::empty() const noexcept {
    return _size == 0;
}

template<typename T, std::size_t K, typename Compare>
typename B_tree<T, K, Compare>::size_type B_tree<T, K, Compare>::size() const noexcept {
    return _size;
}

template<typename T, std::size_t K, typename Compare> void B_tree<T, K, Compare>::clear() noexcept {
    _root.reset();
    _size = 0;
}

template<typename T, std::size_t K, typename Compare>
typename B_tree<T, K, Compare>::iterator B_tree<T, K, Compare>::find(const T& val) const noexcept {
    node* curr = _root.get();
    while (curr != nullptr) {
        bool went_down = false;
        for (std::size_t i = 0; !went_down && i < curr->_size; ++i) {
            if (_comp(val, curr->_vals[i])) {
                curr = curr->_children[i];
                went_down = true;
            } else if (!_comp(curr->_vals[i], val)) {
                return &(curr->_vals[i]);
            }
        }
        if (!went_down) {
            curr = curr->_children[curr->_size];
        }
    }
    return nullptr;
}

template<typename T, std::size_t K, typename Compare>
bool B_tree<T, K, Compare>::contains(const T& val) const noexcept {
    return find(val) != nullptr;
}

template<typename T, std::size_t K, typename Compare>
std::pair<typename B_tree<T, K, Compare>::iterator, bool>
  B_tree<T, K, Compare>::insert(const T& val) {
    T tmp(val);
    return insert(std::move(tmp));
}

template<typename T, std::size_t K, typename Compare>
std::pair<typename B_tree<T, K, Compare>::iterator, bool> B_tree<T, K, Compare>::insert(T&& val) {
    if (!_root) {
        _root.reset(new node());
        _root->_vals.front() = std::move(val);
        std::fill_n(_root->_children.begin(), 2, nullptr);
        _root->_size = 1;
        ++_size;
        return std::make_pair(&(_root->_vals.front()), true);
    }
    if (_root && _root->_size == 2 * K - 1) {
        std::unique_ptr<node> new_node(new node());
        new_node->_size = 1;
        new_node->_vals.front() = std::move(_root->_vals[K - 1]);
        new_node->_children[0] = nullptr;
        new_node->_children[1] = new node();
        new_node->_children[1]->_size = K - 1;
        std::move(_root->_vals.begin() + K, _root->_vals.end(),
                  new_node->_children[1]->_vals.begin());
        std::move(_root->_children.begin() + K, _root->_children.end(),
                  new_node->_children[1]->_children.begin());
        _root->_size = K - 1;
        new_node->_children[0] = _root.get();
        _root.release();
        _root = std::move(new_node);
    }

    node* curr = _root.get(); // curr is always pointing to a node not at capacity
    while (true) {
        std::size_t i = 0;
        while (i < curr->_size) {
            if (_comp(curr->_vals[i], val)) {
                ++i;
            } else if (!_comp(val, curr->_vals[i]) && !_dups) {
                return std::make_pair(&(curr->_vals[i]), false);
            } else {
                break;
            }
        }

        if (curr->_children.front() == nullptr) {
            // leaf: simply insert at the index
            std::move_backward(curr->_vals.begin() + i, curr->_vals.begin() + curr->_size,
                               curr->_vals.begin() + curr->_size + 1);
            curr->_vals[i] = std::move(val);
            ++curr->_size;
            curr->_children[curr->_size] = nullptr;
            ++_size;
            return std::make_pair(&(curr->_vals[i]), true);
        } else {
            if (curr->_children[i]->_size == 2 * K - 1) {
                // full child: extract median and split into two
                std::move_backward(curr->_vals.begin() + i, curr->_vals.begin() + curr->_size,
                                   curr->_vals.begin() + curr->_size + 1);
                std::move_backward(curr->_children.begin() + i + 1,
                                   curr->_children.begin() + curr->_size + 1,
                                   curr->_children.begin() + curr->_size + 2);

                curr->_vals[i] = std::move(curr->_children[i]->_vals[K - 1]);

                curr->_children[i + 1] = new node();
                curr->_children[i + 1]->_size = K - 1;
                std::move(curr->_children[i]->_vals.begin() + K, curr->_children[i]->_vals.end(),
                          curr->_children[i + 1]->_vals.begin());
                std::move(curr->_children[i]->_children.begin() + K,
                          curr->_children[i]->_children.end(),
                          curr->_children[i + 1]->_children.begin());

                curr->_children[i]->_size = K - 1;
                ++curr->_size;

                // update which node we are going down into
                if (_comp(curr->_vals[i], val)) {
                    ++i;
                } else if (!_comp(val, curr->_vals[i]) && !_dups) {
                    return std::make_pair(&(curr->_vals[i]), false);
                }
            }

            curr = curr->_children[i];
        }
    }
}

template<typename T, std::size_t K, typename Compare>
T& B_tree<T, K, Compare>::find_successor(typename B_tree<T, K, Compare>::node* top, bool left) {}

template<typename T, std::size_t K, typename Compare>
typename B_tree<T, K, Compare>::iterator B_tree<T, K, Compare>::erase(const T& val) {
    node* curr = _root.get(); // Always contains at least K values (unless root)
    bool erased = false;

    // Are we looking for the value or a successor?
    T* erase_ptr = nullptr;
    bool find_left = false;
    bool find_right = false;

    while (!erased && curr != nullptr) {
        int i = 0;
        bool found = false;
        if (find_left) {
            i = curr->_size;
        } else if (!find_right) {
            while (i < curr->_size) {
                if (_comp(val, curr->_vals[i])) {
                    break;
                } else if (!_comp(curr->_vals[i], val)) {
                    found = true;
                    break;
                } else {
                    ++i;
                }
            }
        }

        if (found) {
            if (curr->_children.front() == nullptr) {
                // leaf node: can simply erase
                std::move(curr->_vals.begin() + i + 1, curr->_vals.begin() + curr->_size,
                          curr->_vals.begin() + i);
                --curr->_size;
                erased = true;
            } else if (curr->_children[i]->_size >= K) {
                // Push a successor up to curr
                erase_ptr = &(curr->_vals[i]);
                find_left = true;
            } else if (curr->_children[i + 1]->_size >= K) {
                erase_ptr = &(curr->_vals[i]);
                find_right = true;
                ++i;
            } else {
                // left and right children are both size K - 1: merge
                node* merge_tgt = curr->_children[i];
                node* merge_src = curr->_children[i + 1];
                merge_tgt->_vals[merge_tgt->_size] = std::move(curr->_vals[i]);
                std::move(merge_src->_vals.begin(), merge_src->_vals.begin() + K - 1,
                          merge_tgt->_vals.begin() + K);
                std::move(merge_src->_children.begin(), merge_src->_children.begin() + K,
                          merge_tgt->_children.begin() + K);
                merge_tgt->_size = 2 * K - 1;
                std::move(curr->_vals.begin() + i + 1, curr->_vals.begin() + curr->_size,
                          curr->_vals.begin() + i);
                std::move(curr->_children.begin() + i + 2,
                          curr->_children.begin() + curr->_size + 1,
                          curr->_children.begin() + i + 1);
                merge_src->_size = 0;
                merge_src->_children.front() = nullptr;
                delete merge_src;
                --curr->_size;
            }
        } else if (curr->_children.front() == nullptr) {
            // Are we looking for a successor or a DNE element?
            if (find_left) {
                // move successor up to right spot
                *erase_ptr = std::move(curr->_vals[curr->_size - 1]);
                --curr->_size;
                erased = true;
            } else if (find_right) {
                *erase_ptr = std::move(curr->_vals.front());
                std::move(curr->_vals.begin() + 1, curr->_vals.begin() + curr->_size,
                          curr->_vals.begin());
                --curr->_size;
                erased = true;
            }
        } else if (curr->_children[i]->_size < K) {
            // Need to add elements into the child we are going into
            node* add_tgt = curr->_children[i];
            if (i != 0 && curr->_children[i - 1]->_size >= K) {
                // Rotate with left sibling
                std::move_backward(add_tgt->_vals.begin(), add_tgt->_vals.begin() + add_tgt->_size,
                                   add_tgt->_vals.begin() + add_tgt->_size + 1);
                std::move_backward(add_tgt->_children.begin(),
                                   add_tgt->_children.begin() + add_tgt->_size + 1,
                                   add_tgt->_children.begin() + add_tgt->_size + 2);
                add_tgt->_vals.front() = std::move(curr->_vals[i - 1]);

                node* add_src = curr->_children[i - 1];
                add_tgt->_children.front() = add_src->_children[add_src->_size];
                curr->_vals[i - 1] = std::move(add_tgt->_vals[add_src->_size - 1]);
                --add_src->_size;
                ++add_tgt->_size;
            } else if (i != curr->_size && curr->_children[i + 1]->_size >= K) {
                // Rotate with right sibling
                node* add_src = curr->_children[i + 1];
                add_tgt->_vals[add_tgt->_size] = std::move(curr->_vals[i]);
                curr->_vals[i] = std::move(add_src->_vals.front());
                add_tgt->_children[add_tgt->_size + 1] = add_src->_children.front();
                std::move(add_src->_vals.begin() + 1, add_src->_vals.begin() + add_src->_size,
                          add_src->_vals.begin());
                std::move(add_src->_children.begin() + 1,
                          add_src->_children.begin() + add_src->_size + 1,
                          add_src->_children.begin());
                --add_src->_size;
                ++add_tgt->_size;
            } else {
                // Both siblings size K - 1:
                // Merge with a sibling (right if exists, else left)
                if (i == curr->_size) {
                    --i;
                }

                // Merge curr->_children[i] and curr->_children[i + 1]
                node* merge_tgt = curr->_children[i];
                node* merge_src = curr->_children[i + 1];
                merge_tgt->_vals[K - 1] = std::move(curr->_vals[i]);
                std::move(merge_src->_vals.begin(), merge_src->_vals.begin() + K - 1,
                          merge_tgt->_vals.begin() + K);
                std::move(merge_src->_children.begin(), merge_src->_children.begin() + K,
                          merge_tgt->_children.begin() + K);
                std::move(curr->_vals.begin() + i + 1, curr->_vals.begin() + curr->_size,
                          curr->_vals.begin() + i);
                std::move(curr->_children.begin() + i + 2,
                          curr->_children.begin() + curr->_size + 1,
                          curr->_children.begin() + i + 1);
                --curr->_size;
                merge_tgt->_size = 2 * K - 1;
                merge_src->_size = 0;
                merge_src->_children.front() = nullptr;
                delete merge_src;
            }
        }

        curr = curr->_children[i];
    }

    if (erased) {
        --_size;
        if (_root->_size == 0) {
            node* tmp = _root.release();
            _root.reset(tmp->_children.front());
            tmp->_children.front() = nullptr;
            delete tmp;
        }
    }
    return nullptr;
}

} // namespace tree

#endif
