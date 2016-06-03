#ifndef STEREOV_COST_H
#define STEREOV_COST_H

#include "global.h"

int adCost (int x, int y, int d);
int sdCost (int x, int y, int d);

// gradient
int gradientCost(int x, int y, int d);
int gradient(int I[Row][Col], int x, int y);

// ncc
double nccCost (int x, int y, int d);
void   nccInit();
void   nccDemean(int I[Row][Col], double Id[Row][Col]);
double ncc(double I1[Row][Col], double I2[Row][Col], int x, int y, int d);

// census
int  censusCost(int x, int y, int d);
void censusInit();
void censusCode(int I[Row][Col], uint64_t C[Row][Col], uint64_t Ca[Row][Col]);
int  censusVal(int x, int y, int d, uint64_t C1[Row][Col], uint64_t C2[Row][Col]);

// rank
int  rankCost(int x, int y, int d);
void rankInit();
void rankValue(int gray[Row][Col], int rankv[Row][Col]);

#endif