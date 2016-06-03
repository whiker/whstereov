#include <cmath>
#include "util.h"
#include "combine.h"
using namespace std;

#define WeightType double

WeightType** fixedWndWeight;

void windowInit()
{
#ifdef FixedWndWeight
	MallocMat(fixedWndWeight, WeightType, WndSize, WndSize)
	double dist;
	double A = 8, k = (A-1)/sqrt(2)/Wnd;
	
	for (int j = 0; j < WndSize; j++)
	for (int i = 0; i < WndSize; i++)
	{
		dist = sqrt( pow(j-Wnd,2) + pow(i-Wnd,2) );
		//fixedWndWeight[j][i] = 10*(A - k*dist);
		fixedWndWeight[j][i] = 100*exp(-0.35*dist);
	}
#endif
}

int fixedWindowCombine(int x, int y, int d)
{
	int yMin = y-Wnd, yMax = y+Wnd;
	int xMin = x-Wnd, xMax = x+Wnd;

#ifndef FixedWndWeight
	int cost = 0;
	for (int j = yMin; j <= yMax; j++)
	for (int i = xMin; i <= xMax; i++)
		cost += g_data_cost_fn(i, j, d);
#else
	WeightType cost = 0.0;
	for (int j=yMin, p=0; j <= yMax; j++, p++)
	for (int i=xMin, q=0; i <= xMax; i++, q++)
	{
		cost += g_data_cost_fn(i, j, d) *
				fixedWndWeight[p][q];
	}
#endif

	return cost;
}

// 对于视差d, 找到使代价最小的窗口, 返回最小代价
int multiWindowCombine(int x, int y, int d)
{
	int y1, x1, i, j;
	int cost_min = Int32Max, cost;
	//int sum_cost = 0;
	
	y1 = y+Wnd;
	for (i = 0; i < 3; i++, y1-=Wnd)
	{
		x1 = x+Wnd;
		for (j = 0; j < 3; j++, x1-=Wnd)
		{
			cost = fixedWindowCombine(x1, y1, d);
			cost_min = min(cost, cost_min);
			//sum_cost += cost;
		}
	}
	return cost_min;
	//return sum_cost;
}

void disparity()
{
	int costMin, cost;
	int dsel, d;

	for (int y = RowMin; y <= RowMax; y++)
	for (int x = ColMin; x <= ColMax; x++)
	{
		costMin = Int32Max;
		for (d = 0; d < DisparNum; d++)
		{
			cost = g_combine_fn(x, y, d);
			if (cost < costMin)
			{
				costMin = cost;
				dsel = d;
			}
		}
		dispar[y][x] = dsel;
	}
}