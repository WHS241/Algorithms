// 文件名：adjacency_list
// 作者：吴汉森
// 邮箱: hansen.wu@vanderbilt.edu
// 创建日期: 2018-12-14（戊戌年冬月初八）

#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H
#include <vector>

#include "graph_impl.h"

namespace graph {
/*
图的邻接表储存表示
空间：O(V+E)
*/
template <bool Directed, bool Weighted, typename EdgeType> class adjacency_list : public impl<Directed, Weighted, EdgeType> {
public:
    adjacency_list() = default;

    virtual ~adjacency_list() = default;

    const impl<Directed, Weighted, EdgeType>& copy_from(const impl<Directed, Weighted, EdgeType>&) override;

    // 输出：图的阶
    // O(1)
    uint32_t order() const noexcept override;

    // 输出：图里是否存在从start到end的边
    // O(deg(V))
    bool has_edge(const uint32_t& start, const uint32_t& dest) const noexcept override;

    // 输出：从start的end的边的长度。不存在时输出NaN
    // O(deg(V))
    EdgeType edge_cost(const uint32_t& start, const uint32_t& dest) const override;

    // 输入：结点数
    // 输出：该结点的（出）度
    // O(1)
    uint32_t degree(const uint32_t&) const override;

    // 输入：结点数
    // 输出：该结点的（出）边的另一个顶点
    // O(1) (找）+ O(deg(V))（复制）
    std::list<uint32_t> neighbors(const uint32_t& start) const override;

    std::list<std::pair<uint32_t, EdgeType>> edges(const uint32_t&) const override;
    std::pair<impl<Directed, Weighted, EdgeType>*, std::vector<uint32_t>> induced_subgraph(
        const std::list<uint32_t>&) const override;

    // 从start到dest加边，长度为cost。若该边已存在，将边的长度设为cost。
    // O(deg(V))
    void set_edge(const uint32_t& start, const uint32_t& dest, const EdgeType& cost) override;
    // multigraph或已知无边时使用；不查edge是不是已经存在
    // O(1)
    void force_add(const uint32_t& start, const uint32_t& dest, const EdgeType& cost);

    // 加结点
    // 输出：图阶新值
    // O(1)
    uint32_t add_vertex() override;

    // 输入：边起点与终点
    // 运行：如果存在，将该边删除
    // O(deg(V))
    void remove_edge(const uint32_t& start, const uint32_t& dest) override;

    // 输入：结点
    // 运行：删除该结点的所有的（出）边
    // 有向图：O(deg(V))；无向图：O(E)
    void isolate(const uint32_t& start) override;

    // 输入：结点
    // 运行：将to_remove和最后结点切换，并删除以前的to_remove
    // O(E)
    void remove(const uint32_t& to_remove) override;

    // 清空整个图
    // O(1)
    void clear() noexcept override;

private:
    typedef std::pair<uint32_t, EdgeType> _t_edge;
    typedef std::vector<std::list<_t_edge>> _t_graph_rep;

    _t_graph_rep _graph;
};
}

#include <src/structures/graph_adjacency_list.tpp>

#endif // CS_3250_ADJACENCYLIST_H
