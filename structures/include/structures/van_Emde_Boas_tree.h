// 庚子年二月十三

#ifndef VAN_EMDE_BOAS_TREE_H
#define VAN_EMDE_BOAS_TREE_H
#include <cstdint>
#include <memory>

/*
Peter van Emde Boas
Preserving order in a forest in less than logarithmic time
(1975)

Currently not exception-safe
*/
class van_Emde_Boas_tree {
public:
    explicit van_Emde_Boas_tree(uint32_t range = 0);

    ~van_Emde_Boas_tree() noexcept;
    van_Emde_Boas_tree(const van_Emde_Boas_tree&);
    van_Emde_Boas_tree& operator=(const van_Emde_Boas_tree&);
    van_Emde_Boas_tree(van_Emde_Boas_tree&&) noexcept = default;
    van_Emde_Boas_tree& operator=(van_Emde_Boas_tree&&) noexcept;

    bool empty() const noexcept;
    uint32_t size() const noexcept;
    uint32_t max_size() const noexcept;

    void clear() noexcept;

    // Θ(log log max_size())
    bool insert(uint32_t value);
    void erase(uint32_t key) noexcept;

    bool contains(const uint32_t& key) const;

    uint32_t min() const noexcept;
    uint32_t max() const noexcept;

    // the important methods for vEB trees
    // Θ(log log _RANGE)
    uint32_t find_next(uint32_t current) const;
    uint32_t find_prev(uint32_t current) const;

private:
    // determines if subtree index has been constructed
    bool _constructed(uint32_t index) const noexcept;
    // construct subtree at index if non-existent; return true if we did construct
    bool _create_or_noop(uint32_t index);

    // for allocating memory
    std::allocator<uint32_t> _uint_allocator;
    std::allocator<van_Emde_Boas_tree> _tree_allocator;

    uint32_t _RANGE; // stores values 0 to _RANGE - 1
    uint32_t _size; // num values currently stored

    van_Emde_Boas_tree* _subtrees;
    uint32_t _num_trees;
    uint32_t _subtree_size; // size of every tree, except possibly final

    // space-initialization trick
    uint32_t* _init_check;
    uint32_t* _rev_check;
    uint32_t _check_size;

    // keep track of the _subtrees
    van_Emde_Boas_tree* _aux;

    uint32_t _min;
    uint32_t _max;
};

#endif // VAN_EMDE_BOAS_TREE_H