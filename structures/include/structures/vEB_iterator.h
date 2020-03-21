#ifndef VEB_ITERATOR_H
#define VEB_ITERATOR_H
#include <cstdint>
#include <iterator>
class van_Emde_Boas_tree;

class _vEB_iterator {
public:
    typedef uint32_t value_type;
    typedef const uint32_t* pointer;
    typedef const uint32_t& reference;
    typedef int difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    bool operator==(const _vEB_iterator&) const noexcept;
    bool operator!=(const _vEB_iterator&) const noexcept;
    _vEB_iterator& operator++();
    _vEB_iterator operator++(int);
    _vEB_iterator& operator--();
    _vEB_iterator operator--(int);

    reference operator*() const noexcept;

private:
    _vEB_iterator(const van_Emde_Boas_tree*, uint32_t);
    
    friend class van_Emde_Boas_tree;
    const van_Emde_Boas_tree* _tree;
    uint32_t _current;
};

#endif // VEB_ITERATOR_H