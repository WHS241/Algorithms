#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <algorithm>
#include <functional>
#include <vector>

#include "binary_tree_base.h"

namespace tree {
// A Binary Search Tree (BST) abstract class
// The Compare function should be anti-reflexive, anti-symmetric, and transitive (total order
// without equality) a and b are considered equal if both compare(a, b) and compare(b, a) are false
template <typename T, typename Compare> class binary_search_tree : public binary_tree<T> {
public:
    typedef T value_type;
    typedef Compare value_compare;
    typedef typename binary_tree<T>::iterator iterator;
    typedef typename binary_tree<T>::const_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    // initialize empty BST
    template <typename _C = Compare,
        typename = std::enable_if_t<std::is_default_constructible_v<_C>>>
    explicit binary_search_tree(bool duplicates = false);
    explicit binary_search_tree(const Compare& comp, bool duplicates = false);

    // populate with data; [first, last) can be dereferenced to reach data. Need
    // not be sorted.
    template <typename It, typename _Compare = Compare,
        typename _Requires = std::enable_if_t<std::is_default_constructible_v<_Compare>>>
    binary_search_tree(It first, It last, bool duplicates = false);
    template <typename It>
    binary_search_tree(It first, It last, const Compare& comp, bool duplicates = false);

    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    using binary_tree<T>::empty, binary_tree<T>::size, binary_tree<T>::clear, binary_tree<T>::erase,
        binary_tree<T>::insert;

    // searches the BST for an element. Returns true if element exists
    virtual bool contains(const T&) const noexcept;

    // returns an iterator to an element. Returns end() if none found
    virtual iterator find(const T&);
    // const version of find. Returns cend() if none found
    virtual const_iterator find(const T&) const;

protected:
    Compare _compare;
    bool _allow_duplicates;

    typename binary_tree<T>::node* _find(const T&) const noexcept override;
    void _rotate(typename binary_tree<T>::node* upper, bool use_left_child);
};
}

#include "src/structures/binary_search_tree.tpp"

#endif // BINARY_SEARCH_TREE_H