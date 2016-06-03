#include <memory.h>
#include <cmath>

#include "global.h"
#include "image.h"
#include "cost.h"

int adCost(int x, int y, int d)
{
	return ( abs(r1[y][x] - r2[y][x-d]) +
			 abs(g1[y][x] - g2[y][x-d]) +
			 abs(b1[y][x] - b2[y][x-d]) );
}

int sdCost(int x, int y, int d)
{
	return ( pow(r1[y][x] - r2[y][x-d], 2) +
			 pow(g1[y][x] - g2[y][x-d], 2) +
			 pow(b1[y][x] - b2[y][x-d], 2) );
}

// gradient
int gadCost(int x, int y, int d)
{
	return ( abs(gradient(r1, x, y) - gradient(r2, x-d, y)) +
			 abs(gradient(g1, x, y) - gradient(g2, x-d, y)) +
			 abs(gradient(b1, x, y) - gradient(b2, x-d, y)) );
}

int gradient(int I[Row][Col], int x, int y)
{
	return sqrt( pow(I[y][x+1]-I[y][x-1], 2) +
				 pow(I[y+1][x]-I[y-1][x], 2) );
}

// ncc
double ncc_r1[Row][Col], ncc_g1[Row][Col], ncc_b1[Row][Col];
double ncc_r2[Row][Col], ncc_g2[Row][Col], ncc_b2[Row][Col];

double nccCost(int x, int y, int d)
{
	return ( ncc(ncc_r1, ncc_r2, x, y, d) +
			 ncc(ncc_g1, ncc_g2, x, y, d) +
			 ncc(ncc_b1, ncc_b2, x, y, d) );
}

void nccInit()
{
	nccDemean(r1, ncc_r1); nccDemean(g1, ncc_g1); nccDemean(b1, ncc_b1);
	nccDemean(r2, ncc_r2); nccDemean(g2, ncc_g2); nccDemean(b2, ncc_b2);
}

void nccDemean(int I[Row][Col], double Id[Row][Col])
{
	for (int y = RowMin; y <= RowMax; y++)
	for (int x = ColMin; x <= ColMax; x++)
	{
		double sum = 0.0;
		for (int j = y-Wnd; j <= y+Wnd; j++)
		for (int i = x-Wnd; i <= x+Wnd; i++)
			sum += I[j][i];
		Id[y][x] = I[y][x] - sum / WndNum;
	}
}

double ncc(double Id1[Row][Col], double Id2[Row][Col], int x, int y, int d)
{
	int x_min = x-Wnd, x_max = x+Wnd;
	double a1, a2, s1, s2, s3;
	s1 = s2 = s3 = 0.0;

	for (int j = y-Wnd; j <= y+Wnd; j++)
	for (int i = x_min; i <= x_max; i++)
	{
		a1 = Id1[j][i]; a2 = Id2[j][i-d];
		s1 += a1 * a2;
		s2 += a1 * a1;
		s3 += a2 * a2;
	}

	return -s1 / sqrt(s2 * s3);
}

// census
uint64_t csur1[Row][Col],  csug1[Row][Col],  csub1[Row][Col];
uint64_t csur2[Row][Col],  csug2[Row][Col],  csub2[Row][Col];
uint64_t csura1[Row][Col], csuga1[Row][Col], csuba1[Row][Col];
uint64_t csura2[Row][Col], csuga2[Row][Col], csuba2[Row][Col];

int censusCost(int x, int y, int d)
{
	return ( censusVal(x, y, d, csur1,  csur2)  +
			 censusVal(x, y, d, csug1,  csug2)  +
			 censusVal(x, y, d, csub1,  csub2)  +
			 censusVal(x, y, d, csura1, csura2) +
			 censusVal(x, y, d, csuga1, csuga2) +
			 censusVal(x, y, d, csuba1, csuba2) );
}

void censusInit()
{
	censusCode(r1, csur1, csura1); censusCode(r2, csur2, csura2);
	censusCode(g1, csug1, csuga1); censusCode(g2, csug2, csuga2);
	censusCode(b1, csub1, csuba1); censusCode(b2, csub2, csuba2);
}

#define CensusX 4
#define CensusY 4

const uint64_t IntOne = 1;

void censusCode(int I[Row][Col], uint64_t C[Row][Col], uint64_t Ca[Row][Col])
{
	int pix, k;
    int x_max, y_max, i_max, j_max;
    
    x_max = Col-CensusX, y_max = Row-CensusY;
    
    for (int y = CensusY; y < y_max; y++)
    for (int x = CensusX; x < x_max; x++)
    {
		C[y][x] = Ca[y][x] = 0;
        
        pix = I[y][x];
		k = 0, i_max = x+CensusX, j_max = y+CensusY;
        
		for (int j = y-CensusY; j <= j_max; j++)
		for (int i = x-CensusX; i <= i_max; i++)
		{
			if (I[j][i] > pix)
			{
				if (k < 64)
					C[y][x] |= (IntOne<<k);
				else
					Ca[y][x] |= (IntOne<<(k-64));
			}
			k++;
		}
    }
}

int censusVal(int x, int y, int d, uint64_t C1[Row][Col], uint64_t C2[Row][Col])
{
	uint64_t diff = C1[y][x] ^ C2[y][x-d];
	int cnt = 0;
	while (diff)
	{
		cnt++;
		diff &= (diff-1);
	}
	return cnt;
}

// rank
int rankv1[Row][Col], rankv2[Row][Col];

int rankCost(int x, int y, int d)
{
	return abs(rankv1[y][x]-rankv2[y][x-d]);
}

void rankInit()
{
	toGray(r1, r1, g1, b1); rankValue(r1, rankv1);
	toGray(r2, r2, g2, b2); rankValue(r2, rankv2);
}

#define RankWnd 1

void rankValue(int gray[Row][Col], int rankv[Row][Col])
{
	for (int y = RowMin; y <= RowMax; y++)
	for (int x = ColMin; x <= ColMax; x++)
	{
		int cnt=0, gray0=gray[y][x];
		for (int j = y-RankWnd; j <= y+RankWnd; j++)
		for (int i = x-RankWnd; i <= x+RankWnd; i++)
		{
			if (gray[j][i] < gray0)
				cnt++;
		}
		rankv[y][x] = cnt;
	}
}