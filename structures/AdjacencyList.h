// 文件名：AdjacencyList
// 作者：吴汉森
// 邮箱: hansen.wu@vanderbilt.edu
// 创建日期: 2018-12-14（戊戌年冬月初八）

#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H
#include <vector>

#include "GraphImpl.h"

/*
图的邻接表储存表示
空间：O(V+E)
*/
class AdjacencyList : virtual public GraphImpl {
public:
    // 输入：dir：该图是不是有向图；weight：图里的边可否标长度
    AdjacencyList(bool directed, bool weighted);

	~AdjacencyList() override = default; 
     const GraphImpl& copyFrom(const GraphImpl&) override;

    // 输出：图的阶
    // O(1) 
    uint32_t order() const noexcept override;
    // 输出：图里是否存在从start到end的边
    // O(deg(V))
    bool hasEdge(const uint32_t &start, const uint32_t &dest) const noexcept override;
    // 输出：从start的end的边的长度。不存在时输出NaN
    // O(deg(V))
    double edgeCost(const uint32_t &start, const uint32_t &dest) const noexcept override;
    // 输入：结点数
    // 输出：该结点的（出）度
    // O(1)
    uint32_t degree(const uint32_t &) const override;
    // 输入：结点数
    // 输出：该结点的（出）边的另一个顶点
    // O(1) (找）+ O(deg(V))（复制）
    std::list<uint32_t> neighbors(const uint32_t &start) const override;
    std::list<std::pair<uint32_t, double>> edges(const uint32_t &) const override;

    // 从start到dest加边，长度为cost。若该边已存在，将边的长度设为cost。
    // O(deg(V))
    void setEdge(const uint32_t &start, const uint32_t &dest, double cost = 0) override;
    // 加结点
    // 输出：图阶新值
    // O(1)
    uint32_t addVertex() override;
    // 输入：边起点与终点
    // 运行：如果存在，将该边删除
    // O(deg(V))
    void removeEdge(const uint32_t &start, const uint32_t &dest) override;
    // 输入：结点
    // 运行：删除该结点的所有的（出）边
    // 有向图：O(deg(V))；无向图：O(E)
    void isolate(const uint32_t &start) override;
    // 输入：结点
    // 运行：将to_remove和最后结点切换，并删除以前的to_remove
    // O(E)
    void remove(const uint32_t &to_remove) override;
    // 清空整个图
    // O(1)
    void clear() noexcept override;

private:
    typedef std::pair<uint32_t, double> EDGE;
    typedef std::vector<std::list<EDGE>> GRAPH_REP;

    GRAPH_REP graph;
};

#endif // CS_3250_ADJACENCYLIST_H