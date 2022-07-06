#ifndef GRAPH_ALG_ORIENTATION_H
#define GRAPH_ALG_ORIENTATION_H

#include "search.h"

namespace graph_alg {

// WIP
template<typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
graph::graph<Vertex, true, Weighted, EdgeWeight, Args...>
  transitive_orientation(const graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>& input) {
    graph::graph<Vertex, true, Weighted, EdgeWeight, Args...> result;
    for (const Vertex& v : input.vertices())
        result.add_vertex(v);
    if (result.order() == 0)
        return result;

    std::vector<std::list<Vertex>> sets;
    std::unordered_map<Vertex, std::size_t, Args...> get_set;
    std::list<std::size_t> empty_sets;
    std::unordered_map<Vertex, std::pair<std::list<Vertex>, bool>, Args...> unsplit_oriented;

    Vertex start = generate_lex_bfs(input).back();
    for (const std::pair<Vertex, EdgeWeight>& e : input.edges(start)) {
        result.force_add(start, e.first, e.second);
        unsplit_oriented[start].first.push_back(e.first);
    }
    unsplit_oriented[start].second =
      true; // determines orientation of edges: true means pointing to these vertices

    sets.resize(2);
    sets[0].push_back(start);
    get_set[start] = 0;
    for (const Vertex& v : input.vertices())
        if (v != start) {
            sets[1].push_back(v);
            get_set[v] = 1;
        }

    std::vector<std::pair<std::size_t, std::size_t>> split_tracker(input.order() + 1,
                                                                   std::make_pair(0U, 0U));
    std::size_t current_step = 0;
    while (!unsplit_oriented.empty()) {
        ++current_step;
        auto next_it = unsplit_oriented.begin();
    }
}
} // namespace graph_alg

#endif // GRAPH_ALG_ORIENTATION_H
