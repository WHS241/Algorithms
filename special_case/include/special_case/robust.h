#ifndef ROBUST_SPECIAL_CASE_H
#define ROBUST_SPECIAL_CASE_H

#include <algorithm>
#include <functional>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include <util/it_hash.h>

namespace special_case {

/*
 * Exception to be thrown by robust algorithms when they detect input is not in the special case
 */
class not_special_case : public std::domain_error {
public:
    not_special_case()
        : std::domain_error("Not instance of this special case")
    {
    }
};

/*
 * 2-SAT
 * Each clause in CNF statement has <= 2 literals
 *
 * O(n^2)
 */
template <typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
std::unordered_map<T, bool, Hash, KeyEqual> CNF_2_SAT(
    std::list<std::list<std::pair<T, bool>>> expr, Hash hash = Hash(), KeyEqual key_eq = KeyEqual())
{
    typedef typename std::list<std::list<std::pair<T, bool>>>::iterator clause_ptr;

    std::unordered_map<T, Hash, KeyEqual> result(1, hash, key_eq);
    std::unordered_map<T, std::unordered_set<clause_ptr, util::it_hash<clause_ptr>>, Hash, KeyEqual>
        clause_map(1, hash, key_eq);
    std::list<T> removed_vars;

    std::list<std::pair<T, bool>> single_variables;

    // Attempt to apply a setting and propagate ("forcing")
    //
    // Successful: Remove forced/set variables from clause_map, remove satisfied clauses from expr,
    // return map containing all forced settings (including original, so never empty) Unsuccessful:
    // expr and clause_map untouched, return empty map
    //
    // If given a variable already set, returns a map with only the input setting if matches. If
    // conflicts, returns unsuccessful
    auto propagate = [&expr, &clause_map, &result, &hash, &key_eq](const T& var,
                         bool setting) -> std::unordered_map<T, bool, Hash, KeyEqual> {
        std::unordered_map<T, bool, Hash, KeyEqual> subresult(1, hash, key_eq);

        if (result.find(var) != result.end()) {
            if (result[var] == setting)
                subresult[var] = setting;

            return subresult;
        }

        std::list<std::list<std::pair<T, bool>>> removed_clauses;
        std::list<std::pair<T, bool>> to_set;
        to_set.emplace_back(var, setting);

        bool conflict = false;
        while (!to_set.empty() && !conflict) {
            std::pair<T, bool> next = to_set.front();
            to_set.pop_front();

            if (subresult.find(next.first) != subresult.end()) {
                if (subresult.at(next.first) == !next.second)
                    conflict = true;
            } else {
                subresult.insert(next);

                for (clause_ptr it : clause_map.at(next.first)) {
                    // clause either has matching literal or negated
                    // Matching -> satisfied, negated -> forcing
                    std::pair<T, bool> found = *std::find_if(
                        it->begin(), it->end(), [&next, &key_eq](const std::pair<T, bool>& x) {
                            return key_eq(x.first, next.first);
                        });
                    if (found.second == next.second) {
                        // Satisfied: remove from expr and clause_map
                        removed_clauses.splice(removed_clauses.end(), expr, it);
                        for (const std::pair<T, bool>& lit : *it)
                            if (!key_eq(next.first, lit.first)) // to avoid breaking outer for-loop
                                clause_map.at(lit.first).erase(it);
                    } else if (it->size() > 2) {
                        throw not_special_case();
                    } else if (it->size() == 1) {
                        conflict = true;
                        break;
                    } else {
                        auto it2 = it->begin();
                        if (key_eq(it2->first, next.first))
                            // Sanitized input: don't need to check literal value
                            ++it2;
                        // it2 is now the forced variable
                        to_set.push_back(*it2);
                    }
                }

                clause_map.at(next.first).clear();
            }
        }

        if (conflict) {
            // Add back from removed_clauses
            for (auto it = removed_clauses.begin(); it != removed_clauses.end(); ++it)
                for (const std::pair<T, bool>& lit : *it)
                    clause_map[lit.first].insert(it);

            expr.splice(expr.end(), removed_clauses);
            subresult.clear();
            return subresult;
        } else {
            // Clean clause_map before returning
            for (auto it = clause_map.begin(); it != clause_map.end();)
                if (it->second.empty())
                    it = clause_map.erase(it);
                else
                    ++it;

            return subresult;
        }
    };

    if (expr.empty())
        return result;

    // Sanitize: Each variable occurs at most once a clause, a clause x + x' is always satisfiable
    // and can be removed
    expr.remove_if([&hash, &key_eq, &removed_vars](std::list<std::pair<T, bool>>& clause) {
        if (clause.empty())
            return true;
        std::unordered_map<T, bool, Hash, KeyEqual> lit_list(1, hash, key_eq);
        for (auto it = clause.begin(); it != clause.end();) {
            if (lit_list.find(it->first) != lit_list.end()) {
                if (lit_list.at(it->first) == it->second) {
                    it = clause.erase(it);
                } else {
                    removed_vars.push_back(it->first);
                    return true;
                }
            } else {
                ++it;
            }
        }

        return false;
    });

    // Fill in the clause pointers
    for (auto it = expr.begin(); it != expr.end(); ++it)
        for (const std::pair<T, bool>& literal : *it)
            clause_map[literal.first].insert(it);

    // Find single-variable clauses; can only be set a certain way
    for (const std::list<std::pair<T, bool>>& clause : expr)
        if (clause.size() == 1)
            single_variables.push_back(clause.front());

    for (const std::pair<T, bool>& forced : single_variables) {
        std::unordered_map<T, bool, Hash, KeyEqual> subresult
            = propagate(forced.first, forced.second);
        if (subresult.empty())
            // statement not satisfiable
            return subresult;

        result.merge(subresult);
    }

    while (!expr.empty()) {
        std::pair<T, bool> next_to_set = expr.front().front();

        std::unordered_map<T, bool, Hash, KeyEqual> subresult
            = propagate(next_to_set.first, next_to_set.second);

        if (subresult.empty())
            subresult = propagate(next_to_set.first, !next_to_set.second);

        // Tried both, neither successful -> not satisfiable
        if (subresult.empty())
            return subresult;

        // Remaining clauses in expr are independent of this setting; is valid
        result.merge(subresult);
    }

    // Set all the removed vars
    for (const T& x : removed_vars)
        result[x] = true;

    return result;
}
}

#endif // ROBUST_SPECIAL_CASE_H
