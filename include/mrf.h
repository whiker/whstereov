#ifndef STEREOV_MRF_H
#define STEREOV_MRF_H

extern int**  data_cost;     // 节点的每个标签的代价值
extern int    SiteNum;       // 实际节点数
extern int    CostMean;      // 代价项均值
extern int    SmoothWeight;  // 平滑项权重
extern int*   site_label;    // 节点的当前标签
extern int**  site_neigh;    // 节点的邻域点集合

// 平滑代价是标签差值的绝对值乘权重
inline int SmoothCost(int l1, int l2)
{
	return SmoothWeight * (l1>l2 ? l1-l2 : l2-l1);
}

void mrfInitCost(Cost data_cost_fn);
void mrfInitLabel(bool is_min_cost);

void mrfInitNeighPart();  // 只包括索引比自己大的邻域点

int64_t mrfEngry();

void mrfSiteLabelToDispar();

#endif