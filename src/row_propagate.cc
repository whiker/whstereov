#include <cmath>
#include <memory.h>
#include "global.h"
#include "image.h"
#include "cost.h"
#include "sift_cost.h"
#include "filter.h"
#include "row_propagate.h"
using namespace std;

#ifdef RowPropagateCensus
const int ColorDiffMax = 20;
const int LineMax = 17;
const double DisparReilable = 1.1;
#else
const int ColorDiffMax = 30;
const int LineMax = 20;
const double DisparReilable = 1.1;
#endif

int lineLeft[Row][Col], lineRigh[Row][Col];
int rineLeft[Row][Col], rineRigh[Row][Col];

int  seed[Row][Col], dispar_t[Row][Col];
bool nonRelia[Row][Col];

void rowPropagate()
{
	calcRowline();
	calcRowrine();
	calcSeed();
	
	/*
	propagate();
	//vote();
	doFilter();
	//*/
}

void calcRowline()
{
	int xMin, xMax;
	int x, y, i;
	double r, g, b;

	for (x = 0; x < Col; x++)
	{
		xMin = max(0, x-LineMax);
		xMax = min(Col1, x+LineMax);

		for (y = 0; y < Row; y++)
		{
			r = r1[y][x], g = g1[y][x], b = b1[y][x];

			i = x-1;
			while (i>=xMin && similarColor(i, y, r, g, b)) i--;
			lineLeft[y][x] = i+1;  // 左边界

			i = x+1;
			while (i<=xMax && similarColor(i, y, r, g, b)) i++;
			lineRigh[y][x] = i-1;  // 右边界
		}
	}
}

void calcRowrine()
{
	int xMin, xMax;
	int x, y, i;
	int r, g, b;

	for (x = 0; x < Col; x++)
	{
		xMin = max(0, x-LineMax);
		xMax = min(Col1, x+LineMax);

		for (y = 0; y < Row; y++)
		{
			r = r2[y][x], g = g2[y][x], b = b2[y][x];

			i = x-1;
			while (i>=xMin && similarColorR(i, y, r, g, b)) i--;
			rineLeft[y][x] = i+1;  // 左边界

			i = x+1;
			while (i<=xMax && similarColorR(i, y, r, g, b)) i++;
			rineRigh[y][x] = i-1;  // 右边界
		}
	}
}

void calcSeed()
{
	int costs[DisparNum], costMin;
	int x, y, d, dsel;

	double costMin_r, cost_r;
	int dsel_r;

	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
	{
		// 计算出视差估计值, 并默认设成非可靠点
		costMin = Int32Max;
		for (d = 0; d < DisparNum; d++)
		{
			costs[d] = lineCombine(x, y, d);
			if (costs[d] < costMin)
			{
				costMin = costs[d];
				dsel = d;
			}
		}
		dispar_t[y][x] = dsel;
		nonRelia[y][x] = true;

		// 可靠点的条件1, costMin有足够优势
		for (d = 0; d < DisparNum; d++)
		{
			if (d != dsel && costs[d] < costMin * DisparReilable)
				break;  // 非可靠点
		}
		if (d < DisparNum)
			continue;

		// 可靠点的条件2, 左右一致性
		// 把左图[x-dsel, x-dsel+DisparNum-1]的点与右图(x-dsel)点比较
		// g_cost_fn(x1,y1,d)是比较左图的(x1,y1)和右图的(x1-d,y1)
		costMin_r = Int32Max;
		for (d = 0; d < DisparNum; d++)
		{
			cost_r = lineCombine(x-dsel+d, y, d);
			if (cost_r < costMin_r)
			{
				costMin_r = cost_r;
				dsel_r = d;
			}
		}
		if (dsel_r == dsel)
			nonRelia[y][x] = false;
	}

	// 种子图
	int i, i_max;
	memset(seed, 0, Row * Col * sizeof(int));
	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; )
	{
		i_max = lineRigh[y][x];
		for (i=x; i<=i_max && nonRelia[y][i]; i++);
		if (i<=i_max) seed[y][i] = dispar_t[y][i];
		x = i_max + 1;
	}

	saveDepth("ret/depth_1.png", dispar_t);
	saveDepth("ret/seed.png", seed);
}

void propagate()
{
	int x, y, i1, i2, i_min, i_max;

	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
	{
		if (seed[y][x] > 0)
		{
			dispar_t[y][x] = seed[y][x];
			continue;
		}

		// 找到左边和右边的一个种子点
		i_min = lineLeft[y][x], i_max = lineRigh[y][x];
		for (i1 = x-1; i1>=i_min && seed[y][i1]==0; i1--);
		for (i2 = x+1; i2<=i_max && seed[y][i2]==0; i2++);

		if (i1 >= i_min && i2 <= i_max)
			dispar_t[y][x] = seed[y][x] = min(seed[y][i1], seed[y][i2]);
		else if (i1 >= i_min && i2 > i_max)
			dispar_t[y][x] = seed[y][x] = seed[y][i1];
		else if (i1 < i_min && i2 <= i_max)
			dispar_t[y][x] = seed[y][x] = seed[y][i2];
	}

	saveDepth("ret/depth_2.png", dispar_t);
}

void vote()
{
	const int size = 4;
	int* hist = new int[DisparNum];
	int histMax, dsel, x, y, i, j;

	for (y = RowMin; y <= RowMax; y++)
	for (x = ColMin; x <= ColMax; x++)
	{
		if (!nonRelia[y][x])
		{
			dispar[y][x] = dispar_t[y][x];
			continue;
		}

		memset(hist, 0, DisparNum * sizeof(int));
		for (j = y-size; j <= y+size; j++)
			hist[dispar_t[j][x]]++;

		histMax = hist[0], dsel = 0;
		for (i = 1; i < DisparNum; i++)
		{
			if (hist[i] > histMax)
			{
				histMax = hist[i];
				dsel = i;
			}
		}
		dispar[y][x] = dsel;
	}
	delete[] hist;
}

void doFilter(int type)
{
	memcpy(dispar, dispar_t, Row*Col*sizeof(4));
	filter(type);
}

bool similarColor(int x, int y, int r, int g, int b)
{
	return ( max(max(abs(r1[y][x]-r), abs(g1[y][x]-g)), abs(b1[y][x]-b))
			 < ColorDiffMax );
}

bool similarColorR(int x, int y, int r, int g, int b)
{
	return ( max(max(abs(r2[y][x]-r), abs(g2[y][x]-g)), abs(b2[y][x]-b))
			 < ColorDiffMax );
}

double lineCombine(int x, int y, int d)
{
	double cost = 0.0;
	int i_max = lineRigh[y][x];
	
	for (int i = lineLeft[y][x]; i <= i_max; i++)
	{
#ifdef RowPropagateCensus
		cost += min(adCost(i, y, d), 60) + censusCost(i, y, d);
#else
		cost += min(adCost(i, y, d), 80);
#endif
	}

#ifndef RowPropagateCensus
	cost = cost*15 + siftCost(x, y, d);
#endif

	return cost;
}