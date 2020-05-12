#ifndef GRAPH_COMPONENTS_H
#define GRAPH_COMPONENTS_H
#include <iterator>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include <structures/graph.h>

namespace graph_alg {
/*
Determine the connected components using DFS
*/
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::list<std::unordered_set<Vertex, Args...>> connected_components(
    const graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>& src)
{
    std::list<std::unordered_set<Vertex, Args...>> result;
    if (src.order() == 0)
        return result;

    std::unordered_set<Vertex, Args...> current;
    depth_first_forest(
        src, src.vertices().front(), [&current](const Vertex& item) { current.insert(item); },
        [](const Vertex&, const Vertex&) {},
        [&result, &current](const Vertex&) {
            result.push_back(current);
            current.clear();
        });

    return result;
}

/*
Find all articulation points of a graph
John Hopcroft, Robert Tarjan
Algorithm 447: efficient algorithms for graph manipulation
(1973) doi:10.1145/362248.362272
Θ(V+E)
*/
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::unordered_set<Vertex, Args...> articulation_points(
    const graph::graph<Vertex, false, Weighted, EdgeWeight, Args...>& src)
{
    std::unordered_map<Vertex, bool, Args...> visited;
    std::unordered_map<Vertex, uint32_t, Args...> search_number, low;
    uint32_t current_dfs_num = 0;
    std::unordered_set<Vertex, Args...> result;

    if (src.order() == 0)
        return result;

    std::vector<Vertex> vertices = src.vertices();
    for (const Vertex& v : vertices)
        visited[v] = false;

    for (auto it = visited.cbegin(); it != visited.cend();
         it = std::find_if(
             it, visited.cend(), [](const std::pair<Vertex, bool>& val) { return !val.second; })) {
        const Vertex& start = it->first;
        uint32_t num_children_of_root = 0;

        depth_first(
            src, start,
            [&current_dfs_num, &search_number, &low, &visited](const Vertex& curr) {
                // on visit, we simply assign a DFS number and base low number
                search_number[curr] = low[curr] = current_dfs_num++;
                visited[curr] = true;
            },
            [&src, &search_number, &low, &result, &num_children_of_root, &start](
                const Vertex& parent, const Vertex& child) {
                if (parent == start) {
                    // root is articulation point if it has multiple children
                    ++num_children_of_root;
                } else {
                    // calculate low value of DFS for child (min low of children
                    // currently stored)
                    std::list<Vertex> neighbors = src.neighbors(child);
                    uint32_t& low_ref = low[child];
                    for (const Vertex& v : neighbors) {
                        if (low_ref > search_number[v])
                            low_ref = search_number[v];
                    }

                    // condition for biconnectivity
                    if (low_ref == search_number[parent]) {
                        result.insert(parent);
                    }

                    // update low value of parent
                    if (low_ref < low[parent])
                        low[parent] = low_ref;
                }
            });

        // determining if the root of DFS tree is an articulation point
        if (num_children_of_root > 1) {
            result.insert(start);
        }
    }

    return result;
}

/*
Find all strongly connected components
Robert Tarjan
Depth-First Search and Linear Graph Algorithms
(1972) doi:10.1137/0201010
Θ(V+E)
*/
template <typename Vertex, bool Weighted, typename EdgeWeight, typename... Args>
std::list<std::unordered_set<Vertex, Args...>> strongly_connected_components(
    const graph::graph<Vertex, true, Weighted, EdgeWeight, Args...>& src)
{
    std::unordered_map<Vertex, bool, Args...> finished;
    std::unordered_map<Vertex, uint32_t, Args...> search_number, low;
    uint32_t current_num = 0;
    std::list<std::unordered_set<Vertex, Args...>> result;
    std::unordered_map<Vertex, std::unordered_set<Vertex, Args...>, Args...> component;

    if (src.order() == 0)
        return result;

    std::vector<Vertex> vertices = src.vertices();
    for (Vertex& v : vertices)
        finished[v] = false;

    depth_first_forest(
        src, src.vertices().front(),
        [&current_num, &search_number, &low, &component](const Vertex& curr) {
            search_number[curr] = low[curr] = current_num++;
            std::unordered_set<Vertex, Args...> currComp;
            currComp.insert(curr);
            component[curr] = std::move(currComp);
        },
        [&component, &finished, &low, &result, &search_number, &src](
            const Vertex& parent, const Vertex& child) {
            // calculate low value for child (stored min low value of children)
            // Do not consider children that have already been placed in an SCC (search tree cross
            // edge)
            std::list<Vertex> neighbors = src.neighbors(child);
            for (const Vertex& v : neighbors) {
                if (!finished[v] && (low[child] > search_number[v]))
                    low[child] = search_number[v];
            }

            // break off SCC if found
            if (low[child] == search_number[child]) {
                std::unordered_set<Vertex, Args...>& toAdd = component[child];
                for (const Vertex& member : toAdd)
                    finished[member] = true;
                result.push_back(std::move(toAdd));
            }

            // if not SCC, parent must be part of true SCC
            if (!finished[child]) {
                component[parent].merge(std::move(component[child]));
            }

            // update low value of parent
            if (low[child] < low[parent])
                low[parent] = low[child];
        },
        [&component, &result, &finished](const Vertex& root) {
            // Handle root
            std::unordered_set<Vertex, Args...>& toAdd = component[root];
            for (const Vertex& member : toAdd)
                finished[member] = true;
            result.push_back(toAdd);
        });

    return result;
}
} // namespace GraphAlg

#endif // GRAPH_COMPONENTS_H