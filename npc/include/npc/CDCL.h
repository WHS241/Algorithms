#ifndef NPC_CNF_CDCL_H
#define NPC_CNF_CDCL_H

#include <algorithm>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <structures/graph.h>

namespace NP_complete {
template <typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
std::unordered_map<T, bool, Hash, KeyEqual> SAT_solver_CDCL(
    std::list<std::list<std::pair<T, bool>>> expr, Hash hash = Hash(), KeyEqual key_eq = KeyEqual())
{
    typedef typename std::list<std::list<std::pair<T, bool>>>::iterator clause_ptr;

    struct it_hash {
        std::size_t operator()(clause_ptr ptr) const
        {
            return std::hash<typename clause_ptr::pointer>()(ptr.operator->());
        }
    };

    std::unordered_map<T, bool, Hash, KeyEqual> result(1, hash, key_eq), no_res(result);
    std::unordered_map<T, std::unordered_set<clause_ptr, it_hash>, Hash, KeyEqual> clause_map(
        1, hash, key_eq);
    std::unordered_map<T, std::size_t, Hash, KeyEqual> step_map(1, hash, key_eq);
    std::unordered_map<T, clause_ptr, Hash, KeyEqual> forced_by(1, hash, key_eq);
    std::unordered_set<clause_ptr, it_hash> satisfied_clauses;
    std::vector<std::pair<clause_ptr, T>> steps;
    std::unordered_map<clause_ptr, std::size_t, it_hash> step_map_clause;
    std::list<std::pair<std::pair<T, bool>, clause_ptr>> forced_variables;

    std::list<T> removed_vars;

    std::pair<T, clause_ptr> conflict;

    std::unordered_map<T, bool, Hash, KeyEqual> forced_singles(1, hash, key_eq);

    // Propagate the forced_variables; returns true if successful, returns false and sets conflict
    // if not
    auto propagate
        = [&forced_variables, &satisfied_clauses, &clause_map, &conflict, &step_map_clause, &result,
              &step_map, &forced_by, &expr](std::size_t step_num) {
              while (!forced_variables.empty()) {
                  std::pair<std::pair<T, bool>, clause_ptr> next = forced_variables.front();
                  forced_variables.pop_front();

                  if (result.find(next.first.first) != result.end()) {
                      if (result.at(next.first.first) != next.first.second) {
                          conflict = { next.first.first, next.second };
                          return false;
                      }
                  } else {
                      result[next.first.first] = next.first.second;
                      step_map[next.first.first] = step_num;
                      if (next.second != expr.end()) {
                          step_map_clause[next.second] = step_num;
                          satisfied_clauses.insert(next.second);
                      }
                      forced_by[next.first.first] = next.second;

                      const T& current = next.first.first;
                      for (clause_ptr ptr : clause_map[current]) {
                          if (satisfied_clauses.find(ptr) == satisfied_clauses.end()) {
                              std::list<std::pair<T, bool>> unset;
                              for (const std::pair<T, bool>& literal : *ptr) {
                                  if (result.find(literal.first) == result.end())
                                      unset.push_back(literal);
                                  else if (result.at(literal.first) == literal.second) {
                                      satisfied_clauses.insert(ptr);
                                      step_map_clause[ptr] = step_num;
                                      unset.clear();
                                      break;
                                  }
                              }

                              if (unset.size() == 1)
                                  forced_variables.emplace_back(unset.front(), ptr);
                          }
                      }
                  }
              }

              return true;
          };

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

    // Initialize forced_variables with single-variable clauses
    for (auto it = expr.begin(); it != expr.end(); ++it) {
        if (it->size() == 1) {
            forced_variables.emplace_back(it->front(), it);
            forced_singles[it->front().first] = it->front().second;
        }
    }

    // Setting & propagation; Any conflict at this point means statement is unsatisfiable
    if (!propagate(0))
        return no_res;

    // Populate forced_singles
    for (const std::pair<T, bool>& init_lits : result)
        forced_singles.insert(init_lits);

    // Get rid of the inversed set variables: changing them is guaranteed unsatisfiable
    // Also get rid of satisfied clauses
    for (const std::pair<T, bool>& setting : result) {
        std::unordered_set<clause_ptr, it_hash>& curr_clauses = clause_map.at(setting.first);
        for (auto clause_it_ptr = curr_clauses.begin(); clause_it_ptr != curr_clauses.end();) {
            clause_ptr clause_it = *clause_it_ptr;
            bool erase = false;
            for (auto it = clause_it->begin(); it != clause_it->end();) {
                if (result.find(it->first) != result.end()) {
                    if (result[it->first] == it->second) {
                        erase = true;
                        break;
                    } else {
                        if (clause_map.find(it->first != setting.first && it->first) != clause_map.end())
                            clause_map[it->first].erase(clause_it);
                        it = clause_it->erase(it);
                    }
                } else {
                    ++it;
                }
            }

            if (erase) {
                for (const std::pair<T, bool>& lit : *clause_it)
                    if (lit.first != setting.first)
                        clause_map[lit.first].erase(clause_it);
                expr.erase(clause_it);
                clause_it_ptr = curr_clauses.erase(clause_it_ptr);
            } else {
                ++clause_it;
            }
        }

        clause_map.erase(setting.first);
    }
    satisfied_clauses.clear();
    forced_by.clear();
    step_map.clear();
    step_map_clause.clear();

    auto it = expr.begin();
    while (satisfied_clauses.size() != expr.size() && it != expr.end()) {
        // At start of each loop:
        // Satisfied clauses is fully populated
        // No unsatisfiable clauses/conflicts
        if (satisfied_clauses.find(it) == satisfied_clauses.end()) {
            // Find an unset variable and set it
            std::pair<T, bool> curr_literal
                = *std::find_if(it->begin(), it->end(), [&result](const std::pair<T, bool>& x) {
                      return result.find(x.first) == result.end();
                  });
            steps.emplace_back(it, curr_literal.first);
            forced_variables.emplace_back(curr_literal, expr.end());

            bool satisfiable = propagate(steps.size());
            if (satisfiable)
                ++it;
            else {
                forced_variables.clear();

                // Find all selected/unforced assignments that led here
                std::unordered_map<T, bool, Hash, KeyEqual> new_clause_gen(1, hash, key_eq);
                std::list<std::pair<T, clause_ptr>> forced;
                forced.push_back(conflict);
                forced.emplace_back(conflict.first, forced_by.at(conflict.first));
                while (!forced.empty()) {
                    std::pair<T, clause_ptr> c_ptr = forced.front();
                    forced.pop_front();
                    if (c_ptr.second == expr.end()) {
                        new_clause_gen[c_ptr.first] = !result[c_ptr.first];
                    } else {
                        for (const std::pair<T, bool>& lit : *c_ptr.second)
                            if (lit.first != c_ptr.first) {
                                if (forced_by.at(lit.first) == expr.end())
                                    new_clause_gen[lit.first] = lit.second;
                                else
                                    forced.push_back(*forced_by.find(lit.first));
                            }
                    }
                }
                std::list<std::pair<T, bool>> new_clause(
                    new_clause_gen.begin(), new_clause_gen.end());
                expr.push_back(new_clause);

                // Update the clause_map
                clause_ptr new_clause_ptr = --expr.end();
                for (const std::pair<T, bool>& lit : new_clause)
                    clause_map[lit.first].insert(new_clause_ptr);

                // clause generated, now to determine where to backtrack to
                std::vector<std::size_t> assign_steps(new_clause.size());
                std::transform(new_clause.begin(), new_clause.end(), assign_steps.begin(),
                    [&step_map](const std::pair<T, bool>& lit) { return step_map.at(lit.first); });
                std::sort(assign_steps.begin(), assign_steps.end());
                std::size_t back_track_level
                    = (assign_steps.size() == 1) ? 0 : assign_steps[assign_steps.size() - 2];

                // Backtrack
                for (auto it = step_map.begin(); it != step_map.end();) {
                    if (it->second >= back_track_level) {
                        result.erase(it->first);
                        forced_by.erase(it->first);
                        it = step_map.erase(it);
                    } else {
                        ++it;
                    }
                }

                for (auto it = step_map_clause.begin(); it != step_map_clause.end();) {
                    if (it->second >= back_track_level) {
                        satisfied_clauses.erase(it->first);
                        it = step_map_clause.erase(it);
                    } else {
                        ++it;
                    }
                }
                if (back_track_level == 0) {
                    steps.clear();
                        
                    // Repopulate singles in result
                    for (const std::pair<T, bool>& req : forced_singles)
                        result.insert(req);

                    if (result.find(new_clause.front().first) != result.end()
                        && result.at(new_clause.front().first) != new_clause.front().second)
                        // Contradiction between single clauses
                        return no_res;
                    
                    if (new_clause.size() == 1) {
                        forced_variables.emplace_back(new_clause.front(), --expr.end());
                        forced_singles.insert(new_clause.front());
                    }
                    if (!propagate(0))
                        return no_res;
                    for (const std::pair<T, bool>& init_lits : result)
                        forced_singles.insert(init_lits);

                    for (const std::pair<T, bool>& setting : result) {
                        if (clause_map.find(setting.first) != clause_map.end()) {
                            std::unordered_set<clause_ptr, it_hash>& curr_clauses = clause_map.at(setting.first);
                            for (auto clause_it_ptr = curr_clauses.begin(); clause_it_ptr != curr_clauses.end();) {
                                clause_ptr clause_it = *clause_it_ptr;
                                bool erase = false;
                                for (auto it = clause_it->begin(); it != clause_it->end();) {
                                    if (result.find(it->first) != result.end()) {
                                        if (result[it->first] == it->second) {
                                            erase = true;
                                            break;
                                        } else {
                                            if (it->first != setting.first && clause_map.find(it->first) != clause_map.end())
                                                clause_map[it->first].erase(clause_it);
                                            it = clause_it->erase(it);
                                        }
                                    } else {
                                        ++it;
                                    }
                                }

                                if (erase) {
                                    for (const std::pair<T, bool>& lit : *clause_it)
                                        if (lit.first != setting.first && clause_map.find(lit.first) != clause_map.end())
                                            clause_map[lit.first].erase(clause_it);
                                    expr.erase(clause_it);
                                    clause_it_ptr = curr_clauses.erase(clause_it_ptr);
                                } else {
                                    ++clause_it_ptr;
                                }
                            }

                            clause_map.erase(setting.first);
                        }
                    }
                    satisfied_clauses.clear();
    forced_by.clear();
    step_map.clear();
    step_map_clause.clear();
    
                    it = expr.begin();
                } else {
                    it = steps[back_track_level - 1].first;
                    steps.resize(back_track_level - 1);
                }
            }
        } else {
            ++it;
        }
    }

    for (const std::pair<T, std::unordered_set<clause_ptr, it_hash>>& var : clause_map)
        if (result.find(var.first) == result.end())
            result[var.first] = true;

    for (const T& var : removed_vars)
        if (result.find(var) == result.end())
            result[var] = true;

    return result;
}
}

#endif // NPC_CNF_CDCL_H
