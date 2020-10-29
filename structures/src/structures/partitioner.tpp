#ifndef PARTITIONER_TPP
#define PARTITIONER_TPP


template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::partitioner(const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& graph) : graph(graph) {
    std::list<Vertex> initial_set;
    for (const Vertex& v : graph.vertices())
        initial_set.push_back(v);
    sets.push_back(std::move(initial_set));
    for (typename std::list<Vertex>::iterator it = sets.front().begin(); it != sets.front().end(); ++it)
        set_ptrs[*it] = std::make_pair(sets.begin(), it);
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
typename std::list<std::list<Vertex>>::iterator partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::get_set(const Vertex& v) const {
    return set_ptrs.at(v).first;
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
std::unordered_set<typename partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::set_ptr, util::it_hash<typename partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::set_ptr>> partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::partition(const Vertex& v, bool include_with_neighbors) {
    std::list<Vertex> targets = graph.neighbors(v);
    if (include_with_neighbors)
        targets.push_back(v);
    std::unordered_set<set_ptr, util::it_hash<set_ptr>> split_sets;

    for (const Vertex& v : targets) {
        if (set_ptrs.find(v) != set_ptrs.end()) {
        auto& ptrs = set_ptrs[v];
        set_ptr source = ptrs.first;

        if (split_sets.find(source) == split_sets.end()) {
            sets.emplace(source);
            split_sets.insert(source);
        }

        set_ptr target = source;
        --target;
        ptrs.first = target;
        target->splice(target->end(), *source, ptrs.second);
        }
    }

    return split_sets;
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
void partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::merge_sets(const Vertex& first_v, const Vertex& second_v) {
    auto first_list_it = set_ptrs.at(first_v).first;
    std::list<Vertex>& first_list = *first_list_it;

    auto second_list_it = set_ptrs.at(second_v).first;
    std::list<Vertex>& second_list = *second_list_it;

    if (first_list_it == second_list_it)
        return;
    
    for (auto it = second_list.begin(); it != second_list.end();) {
        auto temp = it;
        ++it;
        const Vertex& v = *temp;

        set_ptrs.at(v).first = first_list_it;
        first_list.splice(first_list.end(), second_list, temp);
    }

    sets.erase(second_list_it);
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
void partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::clean() noexcept {
    sets.remove_if(std::mem_fn(&std::list<Vertex>::empty));
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
void partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::remove_vertex(const Vertex& v) noexcept {
    if (set_ptrs.find(v) != set_ptrs.end()) {
        auto& ptrs = set_ptrs[v];
        ptrs.first->erase(ptrs.second);
        set_ptrs.erase(v);
    }
}

template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
void partitioner<Vertex, Directed, Weighted, EdgeWeight, Args...>::add_back(const Vertex& v) {
    if (!graph.has_vertex(v))
        throw std::invalid_argument("Not in graph");
    
    std::list<Vertex> new_set;
    new_set.push_back(v);
    sets.push_back(new_set);
}

#endif // PARTITIONER_TPP