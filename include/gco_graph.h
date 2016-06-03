#ifndef STEREOV_GCO_GRAPH_H
#define STEREOV_GCO_GRAPH_H

/*
please refer to following papers:

[1] An experimental comparison of min-cut/max-flow algorithms for energy minimization in vision.
    Boykov, Yuri, and Vladimir Kolmogorov. Pattern Analysis and Machine Intelligence, 2004.

*/

// 节点
struct GcoNode
{
    int cap;  // 边<src,p>或边<p,sink>的残余容量
};

// 有向图
class GcoGraph
{
public:
    GcoGraph(int max_node_num) { m_nodes = new GcoNode[max_node_num]; }
    ~GcoGraph() { delete[] m_nodes; }

    void setNodeNum(int node_num) { m_node_num = node_num; }

    void tlink(int p, int src_cap, int sink_cap);  // 有向边<源src, p>和<p, 汇sink>
    void nlink(int p, int q, int cap, int rcap);   // 有向边<p, q>和<q, p>

private:
    int m_node_num;
    GcoNode* m_nodes;
};

#endif