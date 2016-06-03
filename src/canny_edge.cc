#include <string.h>
#include "global.h"
#include "image.h"
#include "canny_edge.h"

#define AddSite(y,x) trace[y][x]=++ind; ys[ind]=y,xs[ind]=x; y0=y,x0=x;

int LineThresh   = Row;
int CircleThresh = Row<<1;
const int XsLen = Row * 60;

int trace[Row][Col];
int xs[XsLen], ys[XsLen], bs[XsLen];  // bs是branch

void getCannyEdge()
{
	IplImage* tmp[5];
	for (int i = 0; i < 4; i++)
		tmp[i] = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	tmp[4] = cvLoadImage(imgLname);
	IplImage *src=tmp[4], *dst=tmp[3];
	cvSplit(src, tmp[0], tmp[1], tmp[2], 0);
	
	memset(bp_edge, 0, Row*Col);
	uchar* data = (uchar*) dst->imageData;
	int width_step = dst->widthStep;
	
	// 获取canny_edge
	for (int i = 0; i < 3; i++)
	{
		cvCanny(tmp[i], dst, 128, 255, 3);
		for (int y = 0; y < Row; y++)
		{
			uchar* p = data + y * width_step;
			for (int x = 0; x < Col; x++, p++)
				bp_edge[y][x] = bp_edge[y][x] || (*p==255);
		}
	}
	
	// 修剪canny_edge
	saveEdgeImage("ret/canny_0.png");
	trimCanny();
	saveEdgeImage("ret/canny_1.png");
	
	for (int i = 0; i < 5; i++)
		cvReleaseImage(&tmp[i]);
	gSetWndSize(3);
}

void trimCanny()
{
	int y, x, ind, bsind;
	for (x = 0; x < Col; x++)
		bp_edge[0][x] = bp_edge[Row1][x] = 0;
	for (y = 0; y < Row; y++)
		bp_edge[y][0] = bp_edge[y][Col1] = 0;
	memcpy(bp_flag, bp_edge, Row*Col);
	
	for (y = 1; y < Row1; y++)
	for (x = 1; x < Col1; x++)
	{
		if (bp_flag[y][x]) {
			for (int i = 0; i < Row; i++)
			for (int j = 0; j < Col; j++)
				trace[i][j] = -1;
			ind = bsind = -1;
			trimCannySearch(y, x, ind, bsind);
		}
	}
}

void trimCannySearch(int y0, int x0, int& ind, int& bsind)
{
	int y, x, y1, x1, xy2[8];
	int ind1, ind2, cnt1, cnt2, i, j;
	AddSite(y0, x0)
	for (;;)
	{
		cnt1=0, cnt2=0;
		for (y = y0-1; y <= y0+1; y++)
		for (x = x0-1; x <= x0+1; x++)
		{
			if (bp_flag[y][x])
			{
				if (trace[y][x] < 0)
					y1=y, x1=x, cnt1++;
				else if (trace[y][x] < ind-2)
					xy2[cnt2++]=trace[y][x];
			}
		}
		if (cnt2 > 0) break;  // 存在圆
		
		if (cnt1 == 0)  // 到了线段终点
		{
			ind1 = bsind>=0 ? bs[bsind] : -1;
			if (ind < LineThresh)
				for (i = ind; i > ind1; i--)
					bp_edge[ys[i]][xs[i]] = bp_flag[ys[i]][xs[i]] = 0;
			else
				for (i = ind; i > ind1; i--)
					bp_flag[ys[i]][xs[i]] = 0;
			if (ind1 == -1) return;  // 没有其他分支
			ind=ind1; bsind--;
			y0=ys[ind], x0=xs[ind];
		}
		else if (cnt1 > 1)  // 这是一个分支点
		{
			bs[++bsind] = ind;
			AddSite(y1, x1)
		}
		else  // cnt1==1, 新点
		{
			AddSite(y1, x1)
		}
	}
	
	// 找到圆起点(xs的第ind1个, bs的第j个)
	for (i = 0; i < cnt2; i++)
	{
		ind1 = xy2[i];
		for (j=bsind; j>=0 && bs[j]!=ind1; j--);
		if (j >= 0) break;
	}
	
	if ( i<cnt2 && (ind-ind1)<CircleThresh )
	{
		for (i = ind; i > ind1; i--)
			bp_edge[ys[i]][xs[i]] = 0;
	}
	for (i = ind; i >= 0; i--)
		bp_flag[ys[i]][xs[i]] = 0;
}