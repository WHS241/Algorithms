#ifndef FUNCTION_H
#define FUNCTION_H

#include <map>
#include <unordered_map>
#include <unordered_set>

namespace Induction {
    // input: endomorphism (range is contained in domain) (f[x] = y instead of f(x) = y)
    // output: largest subset of domain s.t. function is automorphic (bijective endomorphic)
    template<typename T>
    std::unordered_set<T> subsetAutomorphism(const std::unordered_map<T, T>& f);
    template<typename T>
    std::unordered_set<T> subsetAutomorphism(const std::map<T, T>& f);
};

#include <src/induction/function.cpp>

#endif //FUNCTION_H