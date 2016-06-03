#include "global.h"
#include "mrf.h"
using namespace std;

int**  data_cost;     // 节点的每个标签的代价值
int    SiteNum;       // 实际节点数
int    CostMean;      // 代价项均值
int    SmoothWeight;  // 平滑项权重
int*   site_label;    // 节点的当前标签
int**  site_neigh;    // 节点的邻域点集合

void mrfInitCost(Cost data_cost_fn)
{
	// 初始化数组data_cost
    data_cost = new int*[SiteMaxNum];
    for (int i = 0; i < SiteMaxNum; i++)
        data_cost[i] = new int[LabelNum];
    int64_t k = 0, cost_sum = 0;
    for (int y = RowMin; y <= RowMax; y++)
    for (int x = ColMin; x <= ColMax; x++)
    {
        for (int l = 0; l < LabelNum; l++)
            data_cost[k][l] = data_cost_fn(x, y, l);
        cost_sum += data_cost[k][0];
        k++;
    }

    SiteNum = k;
    CostMean = cost_sum/k;
	
	// SmoothCost最大值是 abs( (LabelNum-1) - 0 )
    SmoothWeight = CostMean/LabelNum;
}

void mrfInitLabel(bool is_min_cost)
{
    site_label = new int[SiteNum];
    int label, cost_min;
    for (int i = 0; i < SiteNum; i++)
    {
        label = 0;
		if (is_min_cost)
		{
			cost_min = data_cost[i][0];
        	for (int l = 1; l < LabelNum; l++)
        	{
            	if (data_cost[i][l] < cost_min)
            	{
                	cost_min = data_cost[i][l];
                	label = l;
            	}
        	}
		}
        site_label[i] = label;
    }
}

void mrfInitNeighPart()
{
    int i, j, x, y;
    int jmax, imax, imin;
    int i_site, n_neigh;
    
    int** site_idx = new int*[Row];
    i_site = 0;
    for (y = RowMin; y <= RowMax; y++)
    {
        site_idx[y] = new int[Col];
        for (x = ColMin; x <= ColMax; x++)
            site_idx[y][x] = i_site++;
    }
    
	// 初始化数组site_neigh
    site_neigh = new int*[SiteMaxNum];
    i_site = 0;
    for (y = RowMin; y <= RowMax; y++)
    for (x = ColMin; x <= ColMax; x++)
    {
        site_neigh[i_site] = new int[NeighNum+1];
        jmax = min(RowMax, y + NeighWnd);
        imax = min(ColMax, x + NeighWnd);
        imin = max(ColMin, x - NeighWnd);
        
        n_neigh = 0;
        for (i = x+1; i <= imax; i++)
            site_neigh[i_site][++n_neigh] = site_idx[y][i];
        for (j = y+1; j <= jmax; j++)
        for (i = imin; i <= imax; i++)
            site_neigh[i_site][++n_neigh] = site_idx[j][i];
        
        site_neigh[i_site++][0] = n_neigh;
    }
    
    for (y = RowMin; y <= RowMax; y++)
        delete[] site_idx[y];
    delete[] site_idx;
}

int64_t mrfEngry()
{
    int64_t engry = 0, label;
    for (int i = 0; i < SiteNum; i++)
    {
        label = site_label[i];
        // Cost
        engry += data_cost[i][label];
        // Smooth
        for (int j = site_neigh[i][0]; j > 0; j--)
            engry += SmoothCost(label, site_label[site_neigh[i][j]]);
    }
    return engry;
}

void mrfSiteLabelToDispar()
{
	int x, y;
	int i_site = 0;
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
		dispar[y][x] = site_label[i_site++];
}