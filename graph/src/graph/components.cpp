#ifndef GRAPH_COMPONENTS_CPP
#define GRAPH_COMPONENTS_CPP
#include <iterator>
#include <stdexcept>
#include <unordered_map>

#include <graph/search.h>

template <typename T, bool Weighted>
std::list<std::unordered_set<T>> graph_alg::connected_components(
    const graph::graph<T, false, Weighted>& src)
{
    std::list<std::unordered_set<T>> result;
    if (src.order() == 0)
        return result;

    std::unordered_set<T> current;
    depth_first_forest(
        src, src.vertices().front(), [&current](const T& item) { current.insert(item); },
        [](const T&, const T&) {},
        [&result, &current](const T&) {
            result.push_back(current);
            current.clear();
        });

    return result;
}

template <typename T, bool Weighted>
std::unordered_set<T> graph_alg::articulation_points(const graph::graph<T, false, Weighted>& src)
{
    std::unordered_map<T, bool> visited;
    std::unordered_map<T, uint32_t> search_number, low;
    uint32_t current_dfs_num = 0;
    std::unordered_set<T> result;

    if (src.order() == 0)
        return result;

    std::vector<T> vertices = src.vertices();
    for (const T& v : vertices)
        visited[v] = false;

    for (auto it = visited.cbegin(); it != visited.cend();
         it = std::find_if(it, visited.cend(), [](const std::pair<T, bool>& val) { return !val.second; })) {
        const T& start = it->first;
        uint32_t num_children_of_root = 0;

        depth_first(
            src, start,
            [&current_dfs_num, &search_number, &low, &visited](const T& curr) {
                search_number[curr] = low[curr] = current_dfs_num++;
                visited[curr] = true;
            },
            [&src, &search_number, &low, &result, &num_children_of_root, &start](
                const T& parent, const T& child) {
                if (parent == start) {
                    ++num_children_of_root;
                } else {
                    // calculate low value of DFS for child (min low of children
                    // currently stored)
                    std::list<T> neighbors = src.neighbors(child);
                    for (const T& v : neighbors) {
                        if (low[child] > search_number[v])
                            low[child] = search_number[v];
                    }

                    // condition for biconnectivity
                    if (low[child] == search_number[parent]) {
                        result.insert(parent);
                    }

                    // update low value of parent
                    if (low[child] < low[parent])
                        low[parent] = low[child];
                }
            });

        if (num_children_of_root > 1) {
            result.insert(start);
        }
    }

    return result;
}

template <typename T, bool Weighted>
std::list<std::unordered_set<T>> graph_alg::strongly_connected_components(
    const graph::graph<T, true, Weighted>& src)
{
    std::unordered_map<T, bool> finished;
    std::unordered_map<T, uint32_t> search_number, low;
    uint32_t current_num = 0;
    std::list<std::unordered_set<T>> result;
    std::unordered_map<T, std::unordered_set<T>> component;

    if (src.order() == 0)
        return result;

    std::vector<T> vertices = src.vertices();
    for (T& v : vertices)
        finished[v] = false;

    depth_first_forest(
        src, src.vertices().front(),
        [&current_num, &search_number, &low, &component](const T& curr) {
            search_number[curr] = low[curr] = current_num++;
            std::unordered_set<T> currComp;
            currComp.insert(curr);
            component[curr] = std::move(currComp);
        },
        [&component, &finished, &low, &result, &search_number, &src](
            const T& parent, const T& child) {
            // calculate low value for child (stored min low value of children)
            std::list<T> neighbors = src.neighbors(child);
            for (const T& v : neighbors) {
                if (!finished[v] && (low[child] > search_number[v]))
                    low[child] = search_number[v];
            }

            // break off SCC if found
            if (low[child] == search_number[child]) {
                std::unordered_set<T>& toAdd = component[child];
                for (const T& member : toAdd)
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
        [&component, &result, &finished](const T& root) {
            // Handle root
            std::unordered_set<T>& toAdd = component[root];
            for (const T& member : toAdd)
                finished[member] = true;
            result.push_back(toAdd);
        });

    return result;
}

#endif // GRAPH_COMPONENTS_CPP