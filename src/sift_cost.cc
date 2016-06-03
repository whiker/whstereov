#include <math.h>
#include "global.h"
#include "image.h"
#include "sift_cost.h"

double sift_feats_l[Row][Col][128];
double sift_feats_r[Row][Col][128];

void siftInit()
{
	int gray1[Row][Col], gray2[Row][Col];
	
	toGray(gray1, r1, g1, b1);
	toGray(gray2, r2, g2, b2);

	for (int y = RowMin; y <= RowMax; y++)
	for (int x = ColMin; x <= ColMax; x++)
	{
		siftFeature(y, x, gray1, sift_feats_l[y][x]);
		siftFeature(y, x, gray2, sift_feats_r[y][x]);
	}
}

double siftCost(int x, int y, int d)
{
	if (x-d < ColMin)
		return 0.0;
	
	double cost = featureCost(
					sift_feats_l[y][x],
					sift_feats_r[y][x-d]
				  );
	return cost;
}

#define More 1

#ifdef More
#define Shift   7
#define Step    4
#define FeatNum 128
#else
#define Shift   3
#define Step    3
#define FeatNum 32
#endif

void siftFeature(int y0, int x0, int gray[Row][Col], double feature[])
{
	int y1, x1, i, j, k;
	for (i = 0; i < FeatNum; i++)
		feature[i] = 0.0;
	k = 0;
	
	y1 = y0-Shift;
	for (j = 0; j < 2; j++, y1+=Step)
	{
		x1 = x0-Shift;
		for (i = 0; i < 2; i++, x1+=Step, k+=8)
			oneBins(y1, x1, gray, feature+k);
#ifdef More
		x1 = x0;
		for (i = 0; i < 2; i++, x1+=4, k+=8)
			oneBins(y1, x1, gray, feature+k);
#endif
	}
	
#ifdef More
	y1 = y0;
	for (j = 0; j < 2; j++, y1+=4)
	{
		x1 = x0-Shift;
		for (i = 0; i < 2; i++, x1+=4, k+=8)
			oneBins(y1, x1, gray, feature+k);
		x1 = x0;
		for (i = 0; i < 2; i++, x1+=4, k+=8)
			oneBins(y1, x1, gray, feature+k);
	}
#endif
}

double featureCost(double A[], double B[])
{
	double cost;
	cost = 0.0;
	for (int i = 0; i < FeatNum; i++)
		cost += abs(A[i]-B[i]);
	return cost;
}

void oneBins(int y0, int x0, int gray[Row][Col], double bins[])
{
	double magn;
	int direct;
	for (int j = 0; j < 4; j++,y0++)
	for (int i = 0; i < 4; i++,x0++)
	{
		grad(y0, x0, gray, &magn, &direct);
		bins[direct] += magn;
	}
}

void grad(int y0, int x0, int gray[Row][Col], double* magn, int* direct)
{
	int y1 = y0==Row1 ? (y0-1) : y0;
	int x1 = x0==Col1 ? (x0-1) : x0;
	
	int dy = gray[y1+1][x0] - gray[y1][x0];
	int dx = gray[y0][x1+1] - gray[y0][x1];
	
	*magn = sqrt(dy*dy + dx*dx);
	
	// (0 1 2 3 4 5 6 7) => (0 1 3 2 4 5 7 6)
	int d;
	if (dy > 0)
		d = dx>0 ? 0 : 2;
	else if (dy < 0)
		d = dx<0 ? 4 : 6;
	else
		d = dx<0 ? 4 : 0;
	if (dy<0) dy=-dy;
	if (dx<0) dx=-dx;
	*direct = dy>dx ? (d+1) : d;
}