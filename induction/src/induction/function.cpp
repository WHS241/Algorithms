#ifndef FUNCTION_CPP
#define FUNCTION_CPP

#include <list>

template <typename T>
std::unordered_set<T> induction::subset_automorphism(const std::unordered_map<T, T>& f)
{
    std::unordered_map<T, uint32_t> map_onto; // the number of values mapped to a particular value
    std::unordered_set<T> result, to_remove;

    // load to_remove and result with all the domain values; take out of set as
    // needed
    for (auto& rel : f) {
        if (map_onto.find(rel.first) == map_onto.end()) {
            map_onto[rel.first] = 0;
            to_remove.insert(rel.first);
        }

        if (map_onto.find(rel.second) == map_onto.end()) {
            map_onto[rel.second] = 1;
        } else {
            if (map_onto[rel.second] == 0)
                to_remove.erase(to_remove.find(rel.second));

            ++map_onto[rel.second];
        }

        result.insert(rel.first);
        result.insert(rel.second);
    }

    // eliminate values that are not mapped to
    while (!to_remove.empty()) {
        T to_delete = *to_remove.begin();
        to_remove.erase(to_remove.begin());
        result.erase(result.find(to_delete));
        if (--map_onto.at(f.at(to_delete)) == 0) {
            to_remove.insert(f.at(to_delete));
        }
    }

    return result;
}

template <typename T> std::unordered_set<T> induction::subset_automorphism(const std::map<T, T>& f)
{
    return subset_automorphism(std::unordered_map<T, T>(f.begin(), f.end()));
}

#endif // FUNCTION_CPP