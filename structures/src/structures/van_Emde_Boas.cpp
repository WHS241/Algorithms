#include <structures/van_Emde_Boas.h>

#include <cmath>

#include <structures/vEB_iterator.h>

van_Emde_Boas_tree::van_Emde_Boas_tree() noexcept
    : van_Emde_Boas_tree(0)
{
}

van_Emde_Boas_tree::van_Emde_Boas_tree(uint32_t range)
    : _uint_allocator()
    , _tree_allocator()
    , _RANGE(range)
    , _size(0)
    , _num_trees(std::round(std::sqrt((double)range)))
    , _check_size(0)
    , _aux(nullptr)
    , _min(range)
    , _max(-1)
{
    if (_min == _max)
        throw std::invalid_argument("Invalid range");
    
    if (_num_trees > 1) { // condition to prevent infinite recursion
        try {
            _subtrees = _tree_allocator.allocate(_num_trees);
            _init_check = _uint_allocator.allocate(_num_trees);
            _rev_check = _uint_allocator.allocate(_num_trees);
            _aux = new van_Emde_Boas_tree(_num_trees);
        } catch (...) {
            if (_rev_check)
                _uint_allocator.deallocate(_rev_check, _num_trees);
            if (_init_check)
                _uint_allocator.deallocate(_init_check, _num_trees);
            if (_subtrees)
                _tree_allocator.deallocate(_subtrees, _num_trees);
            throw;
        }
    }

    _subtree_size = _RANGE / _num_trees;
        if(_RANGE % _num_trees != 0) 
            ++_subtree_size;
}

van_Emde_Boas_tree::~van_Emde_Boas_tree() noexcept
{
    delete _aux;
    _aux = nullptr;

    // only need to free the ones we created; stored in _rev_check
    for (uint32_t i = 0; i < _check_size; ++i)
        _tree_allocator.destroy(_subtrees + _rev_check[i]);
    if(_num_trees > 1) {
    _uint_allocator.deallocate(_rev_check, _num_trees);
    _uint_allocator.deallocate(_init_check, _num_trees);
    _tree_allocator.deallocate(_subtrees, _num_trees);
    }
}

van_Emde_Boas_tree::van_Emde_Boas_tree(const van_Emde_Boas_tree& src)
    : _uint_allocator(src._uint_allocator)
    , _tree_allocator(src._tree_allocator)
    , _RANGE(src._RANGE)
    , _size(src._size)
    , _num_trees(src._num_trees)
    , _subtree_size(src._subtree_size)
    , _check_size(0)
    , _min(src._min)
    , _max(src._max)
{
    bool threw_in_sub = false;
    if (_num_trees != 1) {
        try {
            _subtrees = _tree_allocator.allocate(_num_trees);
            _init_check = _uint_allocator.allocate(_num_trees);
            _rev_check = _uint_allocator.allocate(_num_trees);
            if (src._aux)
                _aux = new van_Emde_Boas_tree(*src._aux);
            
            for(; _check_size != src._check_size; ++_check_size) {
                _rev_check[_check_size] = src._rev_check[_check_size];
                _init_check[_rev_check[_check_size]] = src._init_check[_rev_check[_check_size]];
                threw_in_sub = true;
                _tree_allocator.construct(_subtrees + _rev_check[_check_size], src._subtrees[_rev_check[_check_size]]);
                threw_in_sub = false;
            }
        } catch (...) {
            delete _aux;
            if(threw_in_sub)
                --_check_size;
            for(uint32_t i = 0; i < _check_size; ++i)
                _tree_allocator.destroy(_subtrees + _rev_check[i]);
            
            if (_rev_check)
                _uint_allocator.deallocate(_rev_check, _num_trees);
            if (_init_check)
                _uint_allocator.deallocate(_init_check, _num_trees);
            if (_subtrees)
                _tree_allocator.deallocate(_subtrees, _num_trees);
            throw;
        }
    }
}

van_Emde_Boas_tree& van_Emde_Boas_tree::operator=(const van_Emde_Boas_tree& rhs) {
    if(this != &rhs) {
        van_Emde_Boas_tree temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

van_Emde_Boas_tree::van_Emde_Boas_tree(van_Emde_Boas_tree&& src) noexcept : van_Emde_Boas_tree() {
    *this = std::move(src);
}

van_Emde_Boas_tree& van_Emde_Boas_tree::operator=(van_Emde_Boas_tree&& rhs) noexcept {
    if(this != &rhs) {
        clear();

        _uint_allocator = std::move(rhs._uint_allocator);
        _tree_allocator = std::move(rhs._tree_allocator);
        _RANGE = std::move(rhs._RANGE);
        _size = std::move(rhs._size);
        _subtrees = std::move(rhs._subtrees);
        _num_trees = std::move(rhs._num_trees);
        _subtree_size = std::move(rhs._subtree_size);
        _init_check = std::move(rhs._init_check);
        _rev_check = std::move(rhs._rev_check);
        _check_size = std::move(rhs._check_size);
        _aux = std::move(rhs._aux);
        _min = std::move(rhs._min);
        _max = std::move(rhs._max);
    }
    return *this;
}

van_Emde_Boas_tree::iterator van_Emde_Boas_tree::begin() noexcept {
    return cbegin();
}

van_Emde_Boas_tree::iterator van_Emde_Boas_tree::end() noexcept {
    return cend();
}

van_Emde_Boas_tree::const_iterator van_Emde_Boas_tree::cbegin() const noexcept {
    return const_iterator(this, _min);
}

van_Emde_Boas_tree::const_iterator van_Emde_Boas_tree::cend() const noexcept {
    return const_iterator(this, _RANGE);
}

bool van_Emde_Boas_tree::empty() const noexcept {
    return _size == 0;
}

uint32_t van_Emde_Boas_tree::size() const noexcept {
    return _size;
}

uint32_t van_Emde_Boas_tree::max_size() const noexcept {
    return _RANGE;
}

void van_Emde_Boas_tree::clear() noexcept {
    if(_aux) {
        _aux->clear();
    }
    if(_num_trees > 1)
        for(uint32_t i = 0; i < _check_size; ++i)
            _subtrees[_rev_check[i]].clear();
    
    _size = 0;
    _min = _RANGE;
    _max = -1;
}

std::pair<van_Emde_Boas_tree::iterator, bool> van_Emde_Boas_tree::insert(uint32_t insert_value) {
    uint32_t value = insert_value; // need to do processing
    if(value >= _RANGE)
        throw std::out_of_range(std::to_string(value));
    if(_min == value || _max == value)
        return std::make_pair(iterator(this, insert_value), false);
    // empty tree
    if(_max == -1) {
        _min = _max = value;
        ++_size;
        return std::make_pair(iterator(this, insert_value), true);
    }

    // new min: insert old min into trees
    if(value < _min)
        std::swap(value, _min);
    // new max: update only, handle max
    if(value > _max)
        _max = value;
    if(_aux) { // not base case
        uint32_t target = value / _subtree_size;
        uint32_t mod = value % _subtree_size;
        _create_or_noop(target);
        std::pair<van_Emde_Boas_tree::iterator, bool> sub_result = _subtrees[target].insert(mod);
        if(!sub_result.second)
            return std::make_pair(iterator(this, insert_value), false);
        if(_subtrees[target]._min == _subtrees[target]._max)
            _aux->insert(target);
    }
    ++_size;
    return std::make_pair(iterator(this, insert_value), true);
}

void van_Emde_Boas_tree::erase(uint32_t value) noexcept {
    if(!contains(value))
        return;
    if(_min == value && _max == value) // only element 
    {
        _min = _RANGE;
        _max = -1;
        _size = 0;
        return;
    }
    if(!_aux) { 
        // base case, we know _RANGE = _size = 2 and we are dealing with either the min or max, 
        // otherwise this would be handled above
        if(_min == value)
            _min = _max;
        else
            _max = _min;
        --_size;
        return;
    }
    if(_min == value) {
        // reset _min, delete new min from its subtree
        _min = _aux->_min * _subtree_size + _subtrees[_aux->_min]._min;
        value = _min;
    }
    uint32_t target = value / _subtree_size;
    uint32_t mod = value % _subtree_size;
    _subtrees[target].erase(mod);

    // remove from aux if empty
    if(_subtrees[target].empty())
        _aux->erase(target);
    if (value == _max)
        _max = _aux->empty() ? _min : (_aux->_max * _subtree_size + _subtrees[_aux->_max]._max);
    --_size;
}

van_Emde_Boas_tree::iterator van_Emde_Boas_tree::find(const uint32_t& key) const {
    if(key >= _RANGE)
        throw std::out_of_range(std::to_string(key));
    return contains(key) ? iterator(this, key) : cend();
}

bool van_Emde_Boas_tree::contains(const uint32_t& key) const {
    if(key >= _RANGE)
        throw std::out_of_range(std::to_string(key));
    if(key == _min || key == _max)
        return true;
    
    if(_aux) {
        uint32_t target = key / _subtree_size;
        uint32_t mod = key % _subtree_size;
        if(!_constructed(target))
            return false;
        return _subtrees[target].contains(mod);
    }
    return false;
}

uint32_t van_Emde_Boas_tree::_find_next(uint32_t current) const {
    if(current < _min)
        return _min;
    if(_max == -1 || current >= _max)
        return _RANGE; // no successor; go to end()/cend() value
    
    // base case: current is either min or max, or something non-existent
    // only the min case is not covered by the above
    if(!_aux) {
        return _max;
    }
    uint32_t target = current / _subtree_size;
    uint32_t mod = current % _subtree_size;
    if(_constructed(target) && mod < _subtrees[target]._max)
        return target * _subtree_size + _subtrees[target]._find_next(mod);
    else {
        uint32_t next_tree = _aux->_find_next(target);
        return next_tree * _subtree_size + _subtrees[next_tree]._min;
    }
}

uint32_t van_Emde_Boas_tree::_find_prev(uint32_t current) const {
    if(_max == -1 || current > _max)
        return _max;
    if(current <= _min)
        return -1; // no successor; go to last value

    // base case: current is either min or max, or something non-existent
    // only max case is not covered above
    if(_RANGE == 2 && current == _max) {
        return _min;
    }
    uint32_t target = current / _subtree_size;
    uint32_t mod = current % _subtree_size;
    if(_constructed(target) && mod > _subtrees[target]._min)
        return target * _subtree_size + _subtrees[target]._find_prev(mod);
    else {
        uint32_t next_tree = _aux->_find_prev(target);
        // if there is no prev in aux, predecessor is the min
        return next_tree == -1 ? _min : (next_tree * _subtree_size + _subtrees[next_tree]._max);
    }
}

bool van_Emde_Boas_tree::_constructed(uint32_t index) const noexcept {
    return _init_check[index] < _check_size && _rev_check[_init_check[index]] == index;
}

void van_Emde_Boas_tree::_create_or_noop(uint32_t index) {
    if(_constructed(index))
        return;
    // edge case; last tree may be of different size
    _tree_allocator.construct(_subtrees + index, ((index == _num_trees - 1) && (_RANGE % _num_trees != 0)) ? _RANGE % _subtree_size : _subtree_size);
    _init_check[index] = _check_size;
    _rev_check[_check_size] = index;
    ++_check_size;
}