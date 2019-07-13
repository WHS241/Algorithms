#ifndef FUNCTION_CPP
#define FUNCTION_CPP

#include <list>

template<typename T>
std::unordered_set<T> Function::subsetAutomorphism(const std::unordered_map<T, T>& f) {
    std::unordered_map<T, uint32_t> mapOnto;
    std::unordered_set<T> result, toRemove;
    for (auto& rel : f) {
        if (mapOnto.find(rel.first) == mapOnto.end()) {
            mapOnto[rel.first] = 0;
            toRemove.insert(rel.first);
        }

        if (mapOnto.find(rel.second) == mapOnto.end()) {
            mapOnto[rel.second] = 1;
        }
        else {
            if (mapOnto[rel.second] == 0)
                toRemove.erase(toRemove.find(rel.second));
            
            ++mapOnto[rel.second];
        }

        result.insert(rel.first);
        result.insert(rel.second);
    }

    while (!toRemove.empty()) {
        T toDelete = *toRemove.begin();
        toRemove.erase(toRemove.begin());
        result.erase(result.find(toDelete));
        if (--mapOnto.at(f.at(toDelete)) == 0) {
            toRemove.insert(f.at(toDelete));
        }
    }

    return result;
}

template<typename T>
std::unordered_set<T> Function::subsetAutomorphism(const std::map<T, T>& f) {
    return subsetAutomorphism(std::unordered_map<T, T>(f.begin(), f.end()));
}

#endif // FUNCTION_CPP