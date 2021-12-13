#ifndef VAN_EMDE_BOAS_MAP_H
#define VAN_EMDE_BOAS_MAP_H

#include "van_Emde_Boas_tree.h"

// This file implements a more useful version of van Emde Boas trees, where the integer values are
// associated with some values.

template<typename T> class van_Emde_Boas_map;

template<typename T> class _vEB_map_iterator {
    public:
    typedef int64_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::pair<const std::size_t, T> value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef const value_type* const_pointer;
    typedef const value_type& const_reference;

    reference operator*() noexcept;
    const_reference operator*() const noexcept;
    pointer operator->() noexcept;
    const_pointer operator->() const noexcept;
    _vEB_map_iterator& operator++();
    _vEB_map_iterator operator++(int);
    _vEB_map_iterator& operator--();
    _vEB_map_iterator operator--(int);
    bool operator==(const _vEB_map_iterator& rhs) const noexcept;

    private:
    _vEB_map_iterator(van_Emde_Boas_map<T>*, std::size_t);
    friend class van_Emde_Boas_map<T>;

    van_Emde_Boas_map<T>* _tree;
    std::size_t _curr_index;
};

template<typename T> class _vEB_map_const_iterator {
    public:
    typedef int64_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::pair<const std::size_t, T> value_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    _vEB_map_const_iterator& operator++();
    _vEB_map_const_iterator operator++(int);
    _vEB_map_const_iterator& operator--();
    _vEB_map_const_iterator operator--(int);
    bool operator==(const _vEB_map_const_iterator& rhs) const noexcept;

    private:
    _vEB_map_const_iterator(const van_Emde_Boas_map<T>*, std::size_t);
    friend class van_Emde_Boas_map<T>;

    const van_Emde_Boas_map<T>* _tree;
    std::size_t _curr_index;
};

template<typename T> class van_Emde_Boas_map {
    public:
    typedef std::size_t key_type;
    typedef T mapped_type;
    typedef std::pair<const std::size_t, T> value_type;
    typedef std::size_t size_type;
    typedef int64_t difference_type;
    typedef std::allocator<value_type> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef _vEB_map_iterator<T> iterator;
    typedef _vEB_map_const_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit van_Emde_Boas_map(std::size_t range = 0);

    ~van_Emde_Boas_map() noexcept;
    van_Emde_Boas_map(const van_Emde_Boas_map<T>&);
    van_Emde_Boas_map<T>& operator=(const van_Emde_Boas_map<T>&);
    van_Emde_Boas_map(van_Emde_Boas_map<T>&&) noexcept;
    van_Emde_Boas_map<T>& operator=(van_Emde_Boas_map<T>&&) noexcept;

    // Iterators
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // Capacity
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;

    // Modifiers
    void clear() noexcept;
    std::pair<iterator, bool> insert(const value_type& value);
    std::pair<iterator, bool> insert(value_type&& value);
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(iterator first, iterator last);
    iterator erase(const_iterator first, const_iterator last);
    size_type erase(const key_type& key);
    void swap(van_Emde_Boas_map<T>& other);

    // Lookup
    mapped_type& at(const key_type& key);
    const mapped_type& at(const key_type& key) const;
    mapped_type& operator[](const key_type& key);
    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;

    private:
    friend class _vEB_map_iterator<T>;
    friend class _vEB_map_const_iterator<T>;

    void valid_check(const key_type&) const;

    van_Emde_Boas_tree _tree;
    std::allocator<value_type> _allocator;
    value_type* _elements;
};

#include "../../src/structures/van_Emde_Boas_map.tpp"

#endif // VAN_EMDE_BOAS_MAP_H
