#ifndef APPROX_GRAPH_H
#define APPROX_GRAPH_H

#include <list>
#include <unordered_map>

#include <structures/graph.h>

namespace approx {
// NPC problem: Vertex Cover
// Provides a vertex cover of size < 2 * optimal
// Fanica Gavril and Mihalis Yannakakis
template <typename T, bool Weighted>
std::list<T> vertex_cover_edge_double(graph::graph<T, false, Weighted> input);

// NPC problem: Graph 3-coloring
// Colors a 3-colorable graph with n vertices using O(sqrt(n)) colors
// Avi Wigderson (1983)
template <typename T, bool Weighted>
std::unordered_map<T, uint32_t> three_color_Wigderson(graph::graph<T, false, Weighted> input);
}

#include "../../src/approx/npc_graph.tpp"

#endif // APPROX_GRAPH_H