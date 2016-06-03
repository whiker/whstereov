#ifndef STEREOV_GLOBAL_H
#define STEREOV_GLOBAL_H

#include <cmath>
#include <string>
#include <unistd.h>
#include "params.h"

const int32_t Int32Max = 0x7FFFFFFF;
const int64_t Int64Max = 0x7FFFFFFFFFFFFFFF;

// Type
typedef int (*Cost) (int x, int y, int d);

// Parameters
const int Row = HEIGHT;
const int Col = WIDTH;
const int DisparNum = DISPARNUM;

const int Border = 7;
const int RowMin = Border;
const int ColMin = Border;
const int RowMax = Row - Border - 1;
const int ColMax = Col - Border - 1;
const int Row1   = Row - 1;
const int Col1   = Col - 1;
const int DisparInter = 256 / DisparNum;

// mrf parameters
const int SiteMaxNum = (RowMax+1)*(ColMax+1);  // 最大总点数
const int LabelNum = DisparNum;  // 标签个数
const int NeighWnd = 3 / 2;      // 邻域窗半径
const int NeighNum = 3 * 3 / 2;  // 最大邻域点个数

// Global Var
extern char *imgLname, *imgRname;

extern std::string g_error;

extern int r1[Row][Col], g1[Row][Col], b1[Row][Col];
extern int r2[Row][Col], g2[Row][Col], b2[Row][Col];
extern int dispar[Row][Col];

extern bool bp_edge[Row][Col];
extern bool bp_flag[Row][Col];

extern Cost g_data_cost_fn;
extern Cost g_combine_fn;

extern timeval g_timeStart, g_timeEnd;

extern int WndSize;
extern int Wnd;
extern int WndNum;
void gSetWndSize(int size);

#endif