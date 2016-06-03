#include "global.h"
#include "cost.h"
#include "combine.h"
#include "sift_cost.h"
#include "row_propagate.h"
#include "gco.h"
#include "maxprod_bp.h"

void nccAlgorithm()
{
	nccInit();
	double costMin, cost;
	int dsel, d;
	for (int y = RowMin; y <= RowMax; y++)
	for (int x = ColMin; x <= ColMax; x++)
	{
		costMin = Int32Max;
		for (d = 0; d < DisparNum; d++)
		{
			cost = nccCost(x, y, d);
			if (cost < costMin)
			{
				costMin = cost;
				dsel = d;
			}
		}
		dispar[y][x] = dsel;
	}
}

void censusAlgorithm()
{
	censusInit();
	
	g_combine_fn = censusCost;
	//g_data_cost_fn = censusCost;
	//g_combine_fn = fixedWindowCombine;
	
	disparity();
}

void rankAlgorithm()
{
	rankInit();
	g_data_cost_fn = rankCost;
	g_combine_fn = fixedWindowCombine;
	disparity();
}

void windowAlgorithm()
{
	windowInit();
	g_data_cost_fn = adCost;
	g_combine_fn = fixedWindowCombine;
	//g_combine_fn = multiWindowCombine;
	disparity();
}

void rowPropagateAlgorithm()
{
#ifdef RowPropagateCensus
	censusInit();
#else
	siftInit();
#endif
	rowPropagate();
}

void gcoAlgorithm()
{
	g_data_cost_fn = adCost;
	gco(fixedWindowCombine);
}

void maxprodBpAlgorithm(int nIter, int edge_kind)
{
	g_data_cost_fn = adCost;
	maxprodBp(nIter, edge_kind, fixedWindowCombine);
}