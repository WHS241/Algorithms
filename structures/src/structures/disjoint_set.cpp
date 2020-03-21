#ifndef UNION_FIND_CPP
#define UNION_FIND_CPP

#include <stdexcept>
#include <unordered_set>

template <typename T, typename Hash, typename KeyEqual>
template <typename It>
disjoint_set<T, Hash, KeyEqual>::disjoint_set(It first, It last)
    : _sets()
    , _roots()
{
    std::for_each(first, last, [this](const T& item) { insert(item); });
};

template <typename T, typename Hash, typename KeyEqual> void disjoint_set<T, Hash, KeyEqual>::insert(const T& item)
{
    if (_sets.find(item) != _sets.end())
        throw std::invalid_argument("Already exists");

    _t_data start_value;
    start_value.parent = item;
    start_value.size = 1;

    _sets.insert(std::make_pair(item, start_value));
    _roots.insert(item);
}

template <typename T, typename Hash, typename KeyEqual> void disjoint_set<T, Hash, KeyEqual>::union_sets(const T& first, const T& second)
{
    T first_parent = find(first);
    T second_parent = find(second);
    if (first_parent == second_parent)
        return;
    _t_data& first_data = _sets[first_parent];
    _t_data& second_data = _sets[second_parent];

    if (first_data.size < second_data.size) {
        first_data.parent = second_parent;
        second_data.size += first_data.size;
        _roots.erase(first_parent);
    } else {
        second_data.parent = first_parent;
        first_data.size += second_data.size;
        _roots.erase(second_parent);
    }
}

template <typename T, typename Hash, typename KeyEqual> T disjoint_set<T, Hash, KeyEqual>::find(const T& item)
{
    // at() throws std::out_of_range
    _t_data& data = _sets.at(item);

    if (data.parent != item) {
        data.parent = find(data.parent);
    }
    return data.parent;
}

template <typename T, typename Hash, typename KeyEqual> void disjoint_set<T, Hash, KeyEqual>::remove_set(const T& member)
{
    T to_remove = find(member);
    for (auto it(_sets.begin()); it != _sets.end();) {
        if (to_remove == find(it->first))
            it = _sets.erase(it);
        else
            ++it;
    }
    _roots.erase(to_remove);
}

template <typename T, typename Hash, typename KeyEqual> void disjoint_set<T, Hash, KeyEqual>::clear() noexcept
{
    _sets.clear();
    _roots.clear();
}

template <typename T, typename Hash, typename KeyEqual> uint32_t disjoint_set<T, Hash, KeyEqual>::size() const noexcept { return _sets.size(); }

template <typename T, typename Hash, typename KeyEqual> uint32_t disjoint_set<T, Hash, KeyEqual>::num_sets() const noexcept { return _roots.size(); }

#endif // !UNION_FIND_CPP
