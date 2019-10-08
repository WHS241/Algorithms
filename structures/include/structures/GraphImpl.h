#ifndef GRAPH_IMPL_H
#define GRAPH_IMPL_H
#include <list>
#include <ostream>

// 图的储存表示
class GraphImpl {
public:
    // 输入：dir：该图是不是有向图；weight：图里的边可否标长度
    GraphImpl(bool directed, bool weight)
        : dir(directed)
        , weight(weight) {};
    virtual ~GraphImpl() = default;
    virtual const GraphImpl& copyFrom(const GraphImpl&) = 0;

    // 输出：图的阶
    virtual uint32_t order() const noexcept = 0;
    // 输出：图里是否存在从start到end的边
    virtual bool hasEdge(const uint32_t& start, const uint32_t& dest) const noexcept = 0;
    // 输出：从start的end的边的长度。不存在时输出NaN
    virtual double edgeCost(const uint32_t& start, const uint32_t& dest) const noexcept = 0;
    // 输入：结点数
    // 输出：该结点的（出）度
    virtual uint32_t degree(const uint32_t&) const = 0;
    // 输入：结点数
    // 输出：该结点的（出）边的另一个顶点
    virtual std::list<uint32_t> neighbors(const uint32_t& start) const = 0;
    virtual std::list<std::pair<uint32_t, double>> edges(const uint32_t&) const = 0;

    // 从start到dest加边，长度为cost。若该边已存在，将边的长度设为cost。
    virtual void setEdge(const uint32_t& start, const uint32_t& dest, double cost = 0.0) = 0;
    // 加结点
    // 输出：图阶新值
    virtual uint32_t addVertex() = 0;
    // 输入：边起点与终点
    // 运行：如果存在，将该边删除
    virtual void removeEdge(const uint32_t& start, const uint32_t& dest) = 0;
    // 输入：结点
    // 运行：删除该结点的所有的（出）边
    virtual void isolate(const uint32_t& start) = 0;
    // 输入：结点
    // 运行：将toRemove和最后结点切换，并删除以前的to_remove
    virtual void remove(const uint32_t& toRemove) = 0;
    // 清空整个图
    virtual void clear() noexcept = 0;

    virtual bool directed() const noexcept { return dir; };
    virtual bool weighted() const noexcept { return weight; };

protected:
    bool dir;
    bool weight;
};
/*
std::ostream& operator<<(std::ostream& os, const GraphImpl& graph) {
    for (uint32_t i = 0; i < graph.order(); ++i) {
        os << i << ": ";
        for (auto& edge : graph.edges(i)) {
            os << edge.first;
            if (graph.weighted())
                os << " (" << edge.second << ")";
            os << ", ";
        }
        os << std::endl;
    }
    return os;
}*/

#endif // GRAPH_IMPL_H