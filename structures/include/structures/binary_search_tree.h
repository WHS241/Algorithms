#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <algorithm>
#include <functional>
#include <vector>

#include "binary_tree_base.h"

namespace tree {
// A Binary Search Tree (BST) abstract class
    template<typename T, typename Compare = std::less<>>
    class binary_search_tree : public binary_tree<T> {
    public:
        binary_search_tree() = default;

        // initialize empty BST
        explicit binary_search_tree(Compare comp)
                : binary_tree<T>(), _compare(comp) {};

        // populate with data; [first, last) can be dereferenced to reach data. Need
        // not be sorted.
        template<typename It, typename = std::enable_if_t<std::is_default_constructible_v<Compare>>>
        binary_search_tree(It first, It last) : binary_search_tree(first, last, Compare()) {};
        template<typename It>
        binary_search_tree(It first, It last, Compare comp)
                : binary_tree<T>(), _compare(comp) {
            std::vector<T> elements(first, last);
            std::sort(elements.begin(), elements.end(), comp);
            this->_root.reset(this->_generate(elements, 0, elements.size(), nullptr));
            this->_size = elements.size();
        }

        // searches the BST for an element. Returns true if element exists
        virtual bool contains(const T &) const noexcept;

    protected:
        Compare _compare;

        static typename binary_tree<T>::node *_generate(
                const std::vector<T> &, uint32_t, uint32_t, typename binary_tree<T>::node *);
        typename binary_tree<T>::node *_find(const T &) const noexcept override;
        void _rotate(typename binary_tree<T>::node *upper, bool use_left_child);
        void _verify(typename binary_tree<T>::iterator check);
    };
}

#include "src/structures/binary_search_tree.cpp"

#endif // BINARY_SEARCH_TREE_H