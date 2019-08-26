#ifndef UNION_FIND_CPP
#define UNION_FIND_CPP

#include <stdexcept>

template <typename T>
UnionFind<T>::UnionFind()
    : sets()
{
}

template <typename T> void UnionFind<T>::insert(const T& item)
{
    if (sets.find(item) != sets.end())
        throw std::invalid_argument("Already exists");

    Data startValue;
    startValue.parent = item;
    startValue.size = 1;

    sets.insert(std::make_pair(item, startValue));
}

template <typename T> void UnionFind<T>::setUnion(const T& first, const T& second)
{
    T firstParent = find(first);
    T secondParent = find(second);
    Data& firstParentData = sets[firstParent];
    Data& secondParentData = sets[secondParent];

    if (firstParentData.size < secondParentData.size) {
        firstParentData.parent = secondParent;
        secondParentData.size += firstParentData.size;
    } else {
        secondParentData.parent = firstParent;
        firstParentData.size += secondParentData.size;
    }
}

template <typename T> T UnionFind<T>::find(const T& item)
{
    // at() throws std::out_of_range
    auto& data = sets.at(item);

    if (data.parent != item) {
        data.parent = find(data.parent);
    }
    return data.parent;
}

template <typename T> void UnionFind<T>::removeSet(const T& member)
{
    auto removeSet = find(member);
    for (auto it(sets.begin()); it != sets.end();) {
        if (removeSet == find(it->first))
            it = sets.erase(it);
        else
            ++it;
    }
}

template <typename T> void UnionFind<T>::clear() noexcept { sets.clear(); }

template <typename T> uint32_t UnionFind<T>::size() const noexcept { return sets.size(); }

#endif // !UNION_FIND_CPP
