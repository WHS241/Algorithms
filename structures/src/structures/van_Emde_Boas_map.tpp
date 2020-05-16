// 庚子年二月廿八

#ifndef VAN_EMDE_BOAS_MAP_CPP
#define VAN_EMDE_BOAS_MAP_CPP
#include <structures/van_Emde_Boas_map.h>

template <typename T>
typename _vEB_map_iterator<T>::reference _vEB_map_iterator<T>::operator*() noexcept
{
    return const_cast<reference>(static_cast<const _vEB_map_iterator*>(this)->operator*());
}

template <typename T>
typename _vEB_map_iterator<T>::pointer _vEB_map_iterator<T>::operator->() noexcept
{
    return const_cast<pointer>(static_cast<const _vEB_map_iterator*>(this)->operator->());
}

template <typename T>
typename _vEB_map_iterator<T>::const_reference _vEB_map_iterator<T>::operator*() const noexcept
{
    return _tree->_elements[_curr_index];
}

template <typename T>
typename _vEB_map_iterator<T>::const_pointer _vEB_map_iterator<T>::operator->() const noexcept
{
    return _tree->_elements + _curr_index;
}

template <typename T> _vEB_map_iterator<T>& _vEB_map_iterator<T>::operator++()
{
    _curr_index = _tree->_tree.find_next(_curr_index);
    return *this;
}

template <typename T> _vEB_map_iterator<T> _vEB_map_iterator<T>::operator++(int)
{
    _vEB_map_iterator temp(*this);
    ++*this;
    return temp;
}

template <typename T> _vEB_map_iterator<T>& _vEB_map_iterator<T>::operator--()
{
    _curr_index = _tree->_tree.find_prev(_curr_index);
    return *this;
}

template <typename T> _vEB_map_iterator<T> _vEB_map_iterator<T>::operator--(int)
{
    _vEB_map_iterator temp(*this);
    --*this;
    return temp;
}

template <typename T>
bool _vEB_map_iterator<T>::operator==(const _vEB_map_iterator<T>& rhs) const noexcept
{
    return _tree == rhs._tree && _curr_index == rhs._curr_index;
}

template <typename T>
bool _vEB_map_iterator<T>::operator!=(const _vEB_map_iterator<T>& rhs) const noexcept
{
    return !(*this == rhs);
}

template <typename T>
_vEB_map_iterator<T>::_vEB_map_iterator(van_Emde_Boas_map<T>* _tree, uint32_t index)
    : _tree(_tree)
    , _curr_index(index)
{
}

template <typename T>
typename _vEB_map_const_iterator<T>::reference
_vEB_map_const_iterator<T>::operator*() const noexcept
{
    return _tree->_elements[_curr_index];
}

template <typename T>
typename _vEB_map_const_iterator<T>::pointer _vEB_map_const_iterator<T>::operator->() const noexcept
{
    return _tree->_elements + _curr_index;
}

template <typename T> _vEB_map_const_iterator<T>& _vEB_map_const_iterator<T>::operator++()
{
    _curr_index = _tree->_tree.find_next(_curr_index);
    return *this;
}

template <typename T> _vEB_map_const_iterator<T> _vEB_map_const_iterator<T>::operator++(int)
{
    _vEB_map_const_iterator temp(*this);
    ++*this;
    return temp;
}

template <typename T> _vEB_map_const_iterator<T>& _vEB_map_const_iterator<T>::operator--()
{
    _curr_index = _tree->_tree.find_prev(_curr_index);
    return *this;
}

template <typename T> _vEB_map_const_iterator<T> _vEB_map_const_iterator<T>::operator--(int)
{
    _vEB_map_const_iterator temp(*this);
    --*this;
    return temp;
}

template <typename T>
bool _vEB_map_const_iterator<T>::operator==(const _vEB_map_const_iterator<T>& rhs) const noexcept
{
    return _tree == rhs._tree && _curr_index == rhs._curr_index;
}

template <typename T>
bool _vEB_map_const_iterator<T>::operator!=(const _vEB_map_const_iterator<T>& rhs) const noexcept
{
    return !(*this == rhs);
}

template <typename T>
_vEB_map_const_iterator<T>::_vEB_map_const_iterator(
    const van_Emde_Boas_map<T>* _tree, uint32_t index)
    : _tree(_tree)
    , _curr_index(index)
{
}

template <typename T>
van_Emde_Boas_map<T>::van_Emde_Boas_map(uint32_t range)
    : _tree(range)
    , _allocator()
{
    _elements = _allocator.allocate(range);
}

template <typename T> van_Emde_Boas_map<T>::~van_Emde_Boas_map() noexcept
{
    clear();
    _allocator.deallocate(_elements, _tree.max_size());
}

template <typename T>
van_Emde_Boas_map<T>::van_Emde_Boas_map(const van_Emde_Boas_map<T>& src)
    : _tree(src._tree)
    , _allocator()
{
    _elements = _allocator.allocate(_tree.max_size());
    uint32_t last = -1;
    try {
        for (const value_type& val : src) {
            _allocator.construct(_elements + val.first, val);
            last = val.first;
        }
    } catch (...) {
        for (const value_type& val : src) {
            if (last == -1 || val.first > last)
                break;
            else
                _allocator.destroy(_elements + val.first);
        }
        _allocator.deallocate(_elements, _tree.max_size());
        throw;
    }
}

template <typename T>
van_Emde_Boas_map<T>& van_Emde_Boas_map<T>::operator=(const van_Emde_Boas_map<T>& rhs)
{
    if (this != &rhs) {
        van_Emde_Boas_map<T> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T>
van_Emde_Boas_map<T>::van_Emde_Boas_map(van_Emde_Boas_map<T>&& src) noexcept
    : _tree(std::move(src._tree))
    , _allocator(std::move(src._allocator))
    , _elements(std::move(src._elements))
{
}

template <typename T>
van_Emde_Boas_map<T>& van_Emde_Boas_map<T>::operator=(van_Emde_Boas_map<T>&& rhs) noexcept
{
    if (this != &rhs) {
        clear();
        std::swap(_tree, rhs._tree);
        _allocator = std::move(rhs._allocator);
        std::swap(_elements, rhs._elements);
    }
    return *this;
}

template <typename T> typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::begin() noexcept
{
    return iterator(this, _tree.min());
}

template <typename T> typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::end() noexcept
{
    return iterator(this, _tree.max_size());
}

template <typename T>
typename van_Emde_Boas_map<T>::const_iterator van_Emde_Boas_map<T>::cbegin() const noexcept
{
    return const_iterator(this, _tree.min());
}

template <typename T>
typename van_Emde_Boas_map<T>::const_iterator van_Emde_Boas_map<T>::cend() const noexcept
{
    return const_iterator(this, _tree.max_size());
}

template <typename T>
typename van_Emde_Boas_map<T>::reverse_iterator van_Emde_Boas_map<T>::rbegin() noexcept
{
    return std::make_reverse_iterator(end());
}

template <typename T>
typename van_Emde_Boas_map<T>::reverse_iterator van_Emde_Boas_map<T>::rend() noexcept
{
    return std::make_reverse_iterator(begin());
}

template <typename T>
typename van_Emde_Boas_map<T>::const_reverse_iterator van_Emde_Boas_map<T>::crbegin() const noexcept
{
    return std::make_reverse_iterator(cend());
}

template <typename T>
typename van_Emde_Boas_map<T>::const_reverse_iterator van_Emde_Boas_map<T>::crend() const noexcept
{
    return std::make_reverse_iterator(cbegin());
}

template <typename T> bool van_Emde_Boas_map<T>::empty() const noexcept { return size() != 0; }

template <typename T>
typename van_Emde_Boas_map<T>::size_type van_Emde_Boas_map<T>::size() const noexcept
{
    return _tree.size();
}

template <typename T>
typename van_Emde_Boas_map<T>::size_type van_Emde_Boas_map<T>::max_size() const noexcept
{
    return _tree.max_size();
}

template <typename T> void van_Emde_Boas_map<T>::clear() noexcept
{
    for (uint32_t i = _tree.min();; i = _tree.find_next(i)) {
        _allocator.destroy(_elements + i);
        if (i == _tree.max())
            break;
    }
    _tree.clear();
}

template <typename T>
std::pair<typename van_Emde_Boas_map<T>::iterator, bool> van_Emde_Boas_map<T>::insert(
    const typename van_Emde_Boas_map<T>::value_type& value)
{
    value_type temp(value);
    return insert(std::move(temp));
}

template <typename T>
std::pair<typename van_Emde_Boas_map<T>::iterator, bool> van_Emde_Boas_map<T>::insert(
    typename van_Emde_Boas_map<T>::value_type&& value)
{
    bool new_insert = !_tree.contains(value.first);
    iterator ret_val(this, value.first);
    if (new_insert) {
        _tree.insert(value.first);
        try {
            _allocator.construct(_elements + value.first, std::move(value));
        } catch (...) {
            _tree.erase(value.first);
            throw;
        }
    }

    return { ret_val, new_insert };
}

template <typename T>
typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::erase(
    typename van_Emde_Boas_map<T>::iterator pos)
{
    if (pos == end() || pos._tree != this)
        throw std::invalid_argument("Invalid iterator: does not point into this tree");
    uint32_t index = pos._curr_index;
    if (!_tree.contains(index))
        throw std::invalid_argument("Invalid iterator: element does not exist");
    ++pos;
    _tree.erase(index);
    _allocator.destroy(_elements + index);
    return pos;
}

template <typename T>
typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::erase(
    typename van_Emde_Boas_map<T>::const_iterator pos)
{
    if (pos == cend() || pos._tree != this)
        throw std::invalid_argument("Invalid iterator: does not point into this tree");
    uint32_t index = pos._curr_index;
    if (!_tree.contains(index))
        throw std::invalid_argument("Invalid iterator: element does not exist");
    ++pos;
    _tree.erase(index);
    _allocator.destroy(_elements + index);
    return iterator(this, pos._curr_index);
}

template <typename T>
typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::erase(
    typename van_Emde_Boas_map<T>::iterator first, typename van_Emde_Boas_map<T>::iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return last;
    }

template <typename T>
typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::erase(
    typename van_Emde_Boas_map<T>::const_iterator first, typename van_Emde_Boas_map<T>::const_iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return iterator(this, last._curr_index);
    }

template <typename T>
typename van_Emde_Boas_map<T>::size_type van_Emde_Boas_map<T>::erase(const typename van_Emde_Boas_map<T>::key_type& key) {
    if (_tree.contains(key)) {
        _allocator.destroy(_elements + key);
        _tree.erase(key);
        return 1U;
    }
    return 0U;
}

template <typename T>
void van_Emde_Boas_map<T>::swap(van_Emde_Boas_map<T>& other) {
    std::swap(_tree, other._tree);
    std::swap(_elements, other._elements);
}

template<typename T>
typename van_Emde_Boas_map<T>::mapped_type& van_Emde_Boas_map<T>::at(const typename van_Emde_Boas_map<T>::key_type& key) {
    return const_cast<mapped_type&>(static_cast<const van_Emde_Boas_map<T>*>(this)->at(key));
}

template<typename T>
const typename van_Emde_Boas_map<T>::mapped_type& van_Emde_Boas_map<T>::at(const typename van_Emde_Boas_map<T>::key_type& key) const {
    valid_check(key);
    return _elements[key].second;
}

template <typename T>
typename van_Emde_Boas_map<T>::mapped_type& van_Emde_Boas_map<T>::operator[](const typename van_Emde_Boas_map<T>::key_type& key) {
    if (!_tree.contains(key))
        insert({key, mapped_type()});
    return _elements[key].second;
}

template <typename T>
typename van_Emde_Boas_map<T>::iterator van_Emde_Boas_map<T>::find(const typename van_Emde_Boas_map<T>::key_type& key) {
    return _tree.contains(key) ? iterator(this, key) : end();
}

template <typename T>
typename van_Emde_Boas_map<T>::const_iterator van_Emde_Boas_map<T>::find(const typename van_Emde_Boas_map<T>::key_type& key) const {
    return _tree.contains(key) ? iterator(this, key) : cend();
}

template <typename T>
void van_Emde_Boas_map<T>::valid_check(const typename van_Emde_Boas_map<T>::key_type& key) const {
    if (!_tree.contains(key))
        throw std::out_of_range("van Emde Boas");
}

#endif // VAN_EMDE_BOAS_MAP_CPP
