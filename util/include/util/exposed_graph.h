#ifndef UTIL_EXPOSED_GRAPH_H
#define UTIL_EXPOSED_GRAPH_H

#include <structures/graph.h>

namespace util {
    /*
     * Generate a copy of the underlying representation
     * Can be directly edited (bypasses OOP principles) without affecting the original input
     * Useful for edge manipulation via iterators
     */
    template <typename Vertex, bool Directed, bool Weighted, typename EdgeWeight, typename... Args>
    std::vector<std::list<std::pair<std::size_t, EdgeWeight>>> get_list_rep(const graph::graph<Vertex, Directed, Weighted, EdgeWeight, Args...>& src) {
        std::vector<std::list<std::pair<std::size_t, EdgeWeight>>> output(src.order());
        std::vector<Vertex> input_vertices = src.vertices();

        for (std::size_t i = 0; i < input_vertices.size(); ++i)
            for (const std::pair<Vertex, EdgeWeight>& e : src.edges(input_vertices[i]))
                output[i].push_back(std::make_pair((std::size_t) (src.get_translation().at(e.first)), e.second));
        
        return output;
    }

    template <typename EdgeWeight>
    using exposed_edge_ptr = typename std::list<std::pair<std::size_t, EdgeWeight>>::iterator;
    
    /*
     * Generate a pointer map for an exposed adjacency list
     * Each entry is a bool/ptr pair. The bool determines if the ptr is valid (i.e. does edge actually exist)
     * O(V^2)
     */
    template <typename EdgeWeight>
    std::vector<std::vector<std::pair<bool, exposed_edge_ptr<EdgeWeight>>>> list_pointer_map(std::vector<std::list<std::pair<std::size_t, EdgeWeight>>>& input) {
        std::vector<std::vector<std::pair<bool, exposed_edge_ptr<EdgeWeight>>>> output(input.size(), std::vector<std::pair<bool, exposed_edge_ptr<EdgeWeight>>>(input.size(), std::make_pair(false, exposed_edge_ptr<EdgeWeight>())));
        for (std::size_t i = 0; i < input.size(); ++i)
            for (exposed_edge_ptr<EdgeWeight> ptr = input[i].begin(); ptr != input[i].end(); ++ptr)
                output[i][ptr->first] = std::make_pair(true, ptr);
        return output;
    }
}

#endif // UTIL_EXPOSED_GRAPH_H
