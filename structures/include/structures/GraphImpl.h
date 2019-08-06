#ifndef GRAPH_IMPL_H
#define GRAPH_IMPL_H
#include <list>
#include <ostream>

// ͼ�Ĵ����ʾ
class GraphImpl {
public:
    // ���룺dir����ͼ�ǲ�������ͼ��weight��ͼ��ı߿ɷ�곤��
    GraphImpl(bool directed, bool weight) : dir(directed), weight(weight) {};
    virtual ~GraphImpl() = default;
    virtual const GraphImpl& copyFrom(const GraphImpl&) = 0;

    // �����ͼ�Ľ�
    virtual uint32_t order() const noexcept = 0;
    // �����ͼ���Ƿ���ڴ�start��end�ı�
    virtual bool hasEdge(const uint32_t& start, const uint32_t& dest) const noexcept = 0;
    // �������start��end�ıߵĳ��ȡ�������ʱ���NaN
    virtual double edgeCost(const uint32_t& start, const uint32_t& dest) const noexcept = 0;
    // ���룺�����
    // ������ý��ģ�������
    virtual uint32_t degree(const uint32_t&) const = 0;
    // ���룺�����
    // ������ý��ģ������ߵ���һ������
    virtual std::list<uint32_t> neighbors(const uint32_t& start) const = 0;
    virtual std::list<std::pair<uint32_t, double>> edges(const uint32_t&) const = 0;

    // ��start��dest�ӱߣ�����Ϊcost�����ñ��Ѵ��ڣ����ߵĳ�����Ϊcost��
    virtual void setEdge(const uint32_t& start, const uint32_t& dest, double cost = 0.0) = 0;
    // �ӽ��
    // �����ͼ����ֵ
    virtual uint32_t addVertex() = 0;
    // ���룺��������յ�
    // ���У�������ڣ����ñ�ɾ��
    virtual void removeEdge(const uint32_t& start, const uint32_t& dest) = 0;
    // ���룺���
    // ���У�ɾ���ý������еģ�������
    virtual void isolate(const uint32_t& start) = 0;
    // ���룺���
    // ���У���toRemove��������л�����ɾ����ǰ��to_remove
    virtual void remove(const uint32_t& toRemove) = 0;
    // �������ͼ
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