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
    explicit van_Emde_Boas_tree(std::size_t range = 0);

    ~van_Emde_Boas_tree() noexcept;
    van_Emde_Boas_tree(const van_Emde_Boas_tree&);
    van_Emde_Boas_tree& operator=(const van_Emde_Boas_tree&);
    van_Emde_Boas_tree(van_Emde_Boas_tree&&) noexcept = default;
    van_Emde_Boas_tree& operator=(van_Emde_Boas_tree&&) noexcept;

    bool empty() const noexcept;
    std::size_t size() const noexcept;
    std::size_t max_size() const noexcept;

    void clear() noexcept;

    // Θ(log log max_size())
    bool insert(std::size_t value);
    void erase(std::size_t key) noexcept;

    bool contains(const std::size_t& key) const;

    std::size_t min() const noexcept;
    std::size_t max() const noexcept;

    // the important methods for vEB trees
    // Θ(log log max_size())
    std::size_t find_next(std::size_t current) const;
    std::size_t find_prev(std::size_t current) const;

    private:
    // determines if subtree index has been constructed
    bool _constructed(std::size_t index) const noexcept;
    // construct subtree at index if non-existent; return true if we did construct
    bool _create_or_noop(std::size_t index);

    // for allocating memory
    std::allocator<std::size_t> _uint_allocator;
    std::allocator<van_Emde_Boas_tree> _tree_allocator;

    std::size_t _RANGE; // stores values 0 to _RANGE - 1
    std::size_t _size;  // num values currently stored

    van_Emde_Boas_tree* _subtrees;
    std::size_t _num_trees;
    std::size_t _subtree_size; // size of every tree, except possibly final

    // space-initialization trick
    std::size_t* _init_check;
    std::size_t* _rev_check;
    std::size_t _check_size;

    // keep track of the _subtrees
    van_Emde_Boas_tree* _aux;

    std::size_t _min;
    std::size_t _max;
};

#endif // VAN_EMDE_BOAS_TREE_H
