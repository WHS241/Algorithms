#include <structures/vEB_iterator.h>

#include <structures/van_Emde_Boas_tree.h>

bool _vEB_iterator::operator==(const _vEB_iterator& rhs) const noexcept {
    return _tree == rhs._tree && _current == rhs._current;
}

bool _vEB_iterator::operator!=(const _vEB_iterator& rhs) const noexcept {
    return !(*this == rhs);
}

_vEB_iterator& _vEB_iterator::operator++() {
    _current = _tree->_find_next(_current);
    return *this;
}

_vEB_iterator _vEB_iterator::operator++(int) {
    _vEB_iterator temp(*this);
    ++*this;
    return temp;
}

_vEB_iterator& _vEB_iterator::operator--() {
    _current = _tree->_find_prev(_current);
    return *this;
}

_vEB_iterator _vEB_iterator::operator--(int) {
    _vEB_iterator temp(*this);
    --*this;
    return temp;
}

const _vEB_iterator::reference _vEB_iterator::operator*() const noexcept {
    return _current;
}

_vEB_iterator::_vEB_iterator(const van_Emde_Boas_tree* tree, uint32_t current) : _tree(tree), _current(current){}