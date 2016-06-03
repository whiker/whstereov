#include "global.h"
#include "canny_edge.h"
#include "watershed_edge.h"
#include "mrf.h"
#include "maxprod_bp.h"

int bp_edge_kind = 1;

struct BpNode
{
	int index;
	int* data_cost;  // 每个标签的数据项代价值
	int64_t curr_msg[LabelNum][4];  // 第2维是邻域点索引
	int64_t next_msg[LabelNum][4];  // 当前迭代收到的消息
	int64_t sum_msg[LabelNum];
};

BpNode** bp_nodes;
int SmoothMatrix[LabelNum][LabelNum];

void maxprodBp(int nIter, int edge_kind, Cost data_cost_fn)
{
	bp_edge_kind = edge_kind;
	mbpInit(data_cost_fn);
	mbpPropagate(nIter);
	mbpBeliefToLabel();
	mrfSiteLabelToDispar();
}

void mbpInit(Cost data_cost_fn)
{
	if (bp_edge_kind == 1)
		getWatershedEdge();
	else if (bp_edge_kind == 2)
		getCannyEdge();
	
	mrfInitCost(data_cost_fn);
	mrfInitLabel(true);
	mbpInitSmoothMatrix();
	mbpInitNode();
}

void mbpInitSmoothMatrix()
{
	for (int l1 = 0; l1 < LabelNum; l1++)
	for (int l2 = 0; l2 < LabelNum; l2++)
		SmoothMatrix[l1][l2] = SmoothCost(l1, l2);
}

void mbpInitNode()
{
	int x, y, i_site;
	
	bp_nodes = new BpNode*[Row];
	i_site = 0;
	for (y = RowMin; y <= RowMax; y++)
	{
		bp_nodes[y] = new BpNode[Col];
		for (x = ColMin; x <= ColMax; x++)
		{
			bp_nodes[y][x].index = i_site;
			bp_nodes[y][x].data_cost = data_cost[i_site];
			++i_site;
			for (int l = 0; l < LabelNum; l++)
			{
				for (int i = 0; i < 4; i++)
					bp_nodes[y][x].curr_msg[l][i] = 0;
				bp_nodes[y][x].sum_msg[l] = 0;
			}
		}
	}
}

void mbpPropagate(const int nIter)
{
	for (int iter = 0; iter < nIter; iter++)
	{
		mbpLeftRight();
		mbpUpDown();
		mbpAdjust();
		cout << mrfEngry() << endl;
	}
}

void mbpLeftRight()
{
	int x, y;
	
	// to right
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x < ColMax; x++)
		mbpSendMsg(x, y, x+1, y, 1, 0);
	
	// to left
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin+1; x <= ColMax; x++)
		mbpSendMsg(x, y, x-1, y, 0, 1);
}

void mbpUpDown()
{
	int x, y;
	
	// to down
	for (y = RowMin; y < RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
		mbpSendMsg(x, y, x, y+1, 3, 2);
	
	// to up
	for (y = RowMin+1; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
		mbpSendMsg(x, y, x, y-1, 2, 3);
}

void mbpSendMsg(int x1, int y1, int x2, int y2, int pos1, int pos2)
{
	if (bp_edge_kind!=0 && bp_edge[y1][x1])
		return;
	
	int64_t min_msg, msg;
	for (int l2 = 0; l2 < LabelNum; l2++)
	{
		min_msg = Int64Max;
		for (int l1 = 0; l1 < LabelNum; l1++)
		{
			msg = bp_nodes[y1][x1].data_cost[l1];
			msg += SmoothMatrix[l1][l2];
			msg += bp_nodes[y1][x1].sum_msg[l1];
			
			// 消息往右传播时, pos1 == 1, 减去右边节点在上一步传来的消息
			msg -= bp_nodes[y1][x1].curr_msg[l1][pos1];
			
			if (msg < min_msg)
				min_msg = msg;
		}
		
		// 消息往右传播时, pos2 == 0, 存放左边节点传来的消息
		// 消息往左传播时, pos2 == 1, 存放左边节点传来的消息
		bp_nodes[y2][x2].next_msg[l2][pos2] = (min_msg<<1);
	}
}

void mbpAdjust()
{
	int x, y, l, i;
	int64_t sum_msg, msg;
	
	// 把当前迭代收到的消息next_msg复制到curr_msg
	// 统计收到的消息的和
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
	{
		for (l = 0; l < LabelNum; l++)
		{
			sum_msg = 0;
			for (i = 0; i < 4; i++)
			{
				msg = bp_nodes[y][x].next_msg[l][i];
				bp_nodes[y][x].curr_msg[l][i] = msg;
				sum_msg += msg;
			}
			bp_nodes[y][x].sum_msg[l] = sum_msg;
		}
	}
}

void mbpBeliefToLabel()
{
	int x, y, l, label;
	int i_site = 0;
	int64_t min_belief, belief;
	
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++, i_site++)
	{
		min_belief = Int64Max;
		for (l = 0; l < LabelNum; l++)
		{
			belief = bp_nodes[y][x].data_cost[l] +
					 bp_nodes[y][x].sum_msg[l];
			if (belief < min_belief)
			{
				min_belief = belief;
				label = l;
			}
		}
		site_label[i_site] = label;
	}
}