#include <string>
#include <unistd.h>

#include "global.h"

char *imgLname, *imgRname;

std::string g_error;

int r1[Row][Col], g1[Row][Col], b1[Row][Col];
int r2[Row][Col], g2[Row][Col], b2[Row][Col];
int dispar[Row][Col];

bool bp_edge[Row][Col];
bool bp_flag[Row][Col];

Cost g_data_cost_fn = NULL;
Cost g_combine_fn = NULL;

timeval g_timeStart, g_timeEnd;

int WndSize = 7;
int Wnd = WndSize / 2;
int WndNum = WndSize * WndSize;
void gSetWndSize(int size)
{
	WndSize=size; Wnd=WndSize/2; WndNum=WndSize*WndSize;
}