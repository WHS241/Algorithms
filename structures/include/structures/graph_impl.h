#ifndef GRAPH_IMPL_H
#define GRAPH_IMPL_H
#include <list>
#include <ostream>

namespace graph {
// Forward declaration
template <typename VertexType, bool Directed, bool Weighted, typename EdgeType, typename Hash, typename KeyEqual> class graph;

// 图的储存表示
template <bool Directed, bool Weighted, typename EdgeType> class impl {
public:
    impl() = default;
    
    virtual ~impl() = default;

    virtual const impl& copy_from(const impl&) = 0;

    // 输出：图的阶
    virtual uint32_t order() const noexcept = 0;

    // 输出：图里是否存在从start到end的边
    virtual bool has_edge(const uint32_t& start, const uint32_t& dest) const noexcept = 0;

    // 输出：从start的end的边的长度。不存在时输出NaN
    virtual EdgeType edge_cost(const uint32_t& start, const uint32_t& dest) const = 0;

    // 输入：结点数
    // 输出：该结点的（出）度
    virtual uint32_t degree(const uint32_t&) const = 0;

    // 输入：结点数
    // 输出：该结点的（出）边的另一个顶点
    virtual std::list<uint32_t> neighbors(const uint32_t& start) const = 0;

    virtual std::list<std::pair<uint32_t, EdgeType>> edges(const uint32_t&) const = 0;

    // 输入：结点
    // 输出：导出子图和translation
    virtual std::pair<impl*, std::vector<uint32_t>> induced_subgraph(
        const std::list<uint32_t>&) const = 0;

    // 从start到dest加边，长度为cost。若该边已存在，将边的长度设为cost。
    virtual void set_edge(const uint32_t& start, const uint32_t& dest, const EdgeType& cost) = 0;

    // 加结点
    // 输出：图阶新值
    virtual uint32_t add_vertex() = 0;

    // 输入：边起点与终点
    // 运行：如果存在，将该边删除
    virtual void remove_edge(const uint32_t& start, const uint32_t& dest) = 0;

    // 输入：结点
    // 运行：删除该结点的所有的（出）边
    virtual void isolate(const uint32_t& start) = 0;

    // 输入：结点
    // 运行：将toRemove和最后结点切换，并删除以前的to_remove
    virtual void remove(const uint32_t& toRemove) = 0;

    // 清空整个图
    virtual void clear() noexcept = 0;

protected:
    // 不直接用impl的话应该不需要，以防万一
    void _range_check(uint32_t v) const {
        if (v >= this->order())
            throw std::out_of_range(std::to_string(v));
    }
};
}

#endif // GRAPH_IMPL_H