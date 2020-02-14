// 文件名：graph_adjacency_matrix
// 作者：吴汉森
// 邮箱: hansen.wu@vanderbilt.edu
// 创建日期: 2018-12-13（戊戌年冬月初七）

#ifndef ADJACENCY_MATRIX_H
#define ADJACENCY_MATRIX_H

#include "graph_impl.h"

#include <utility>
#include <vector>
namespace graph {
// 图的邻接矩阵储存表示
// 空间: O(V^2)
template <bool Directed, bool Weighted>
class adjacency_matrix : public impl<Directed, Weighted> {
public:
    virtual ~adjacency_matrix() = default;

    const impl<Directed, Weighted>& copy_from(const impl<Directed, Weighted>&) override;

    // 输出：图的阶
    // O(1)
    uint32_t order() const noexcept override;

    // 输出：图里是否存在从start到end的边
    // O(1)
    bool has_edge(const uint32_t& start, const uint32_t& dest) const noexcept override;

    // 输出：从start的end的边的长度。不存在时输出NaN
    // O(1)
    double edge_cost(const uint32_t& start, const uint32_t& dest) const noexcept override;

    // 输入：结点数
    // 输出：该结点的（出）度
    // O(V)
    uint32_t degree(const uint32_t&) const override;

    // 输入：结点数
    // 输出：该结点的（出）边的另一个顶点
    // O(V)
    std::list<uint32_t> neighbors(const uint32_t& start) const override;

    std::list<std::pair<uint32_t, double>> edges(const uint32_t&) const override;
    std::pair<impl<Directed, Weighted>*, std::vector<uint32_t>> induced_subgraph(const std::list<uint32_t>&) const override;

    // 从start到dest加边，长度为cost。若该边已存在，将边的长度设为cost。
    // O(1)
    void set_edge(const uint32_t& start, const uint32_t& dest, double cost = 0) override;

    // 加结点
    // 输出：图阶新值
    // O(V)
    uint32_t add_vertex() override;

    // 输入：边起点与终点
    // 运行：如果存在，将该边删除
    // O(1)
    void remove_edge(const uint32_t& start, const uint32_t& dest) override;

    // 输入：结点
    // 运行：删除该结点的所有的（出）边
    // O(V)
    void isolate(const uint32_t& start) override;

    // 输入：结点
    // 运行：将to_remove和最后结点切换，并删除以前的to_remove
    // O(V)
    void remove(const uint32_t& target) override;

    // 清空整个图
    // O(1)
    void clear() noexcept override;

private:
    typedef std::vector<std::vector<double>> _t_graph_rep;

    _t_graph_rep _graph;
};
}

#include <src/structures/graph_adjacency_matrix.cpp>

#endif // ADJACENCY_MATRIX_H
