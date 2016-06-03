#include "global.h"
#include "image.h"

int loadRgb(const char* filename, int r[Row][Col], int g[Row][Col], int b[Row][Col])
{
	IplImage* img = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	if (img == NULL)
	{
		g_error = "image.cc loadRgb cvLoadImage";
		return -1;
	}
	if (img->height != Row || img->width != Col)
	{
		g_error = "image.cc loadRgb img's height and width don't match";
		return -1;
	}

	uchar* data = (uchar*)img->imageData;
	int widthStep = img->widthStep;

	for (int y = 0; y < Row; y++)
	{
		uchar* p = data + y * widthStep;
		for (int x = 0; x < Col; x++)
		{
			b[y][x] = *(p++);
			g[y][x] = *(p++);
			r[y][x] = *(p++);
		}
	}

	cvReleaseImage(&img);
	return 0;
}

void saveDepth(const char* filename, int depth[Row][Col], int gray_inter)
{
	IplImage* img = cvCreateImage(cvSize(Col, Row), IPL_DEPTH_8U, 1);
	convertDisparImg(depth, img, gray_inter, 1);
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

void saveEdgeImage(const char* filename)
{
	IplImage* img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	uchar* data = (uchar*) img->imageData;
	int width_step = img->widthStep;
	
	for (int y = 0; y < Row; y++)
	{
		uchar* p = data + y * width_step;
		for (int x = 0; x < Col; x++, p++)
			*p = bp_edge[y][x] ? 0 : 255;
	}
	
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

void convertDisparImg(int dispar[Row][Col], IplImage* img,
					  int gray_inter, bool flag)
{
	uchar* data = (uchar*)img->imageData;
	int width_step = img->widthStep;
	for (int y = 0; y < Row; y++)
	{
		uchar* p = data + y * width_step;
		if (flag)
			for (int x = 0; x < Col; x++, p++)
				*p = dispar[y][x] * gray_inter;
		else
			for (int x = 0; x < Col; x++, p++)
				dispar[y][x] = (*p) / gray_inter;
	}
}

void toGray(int gray[Row][Col], int r[Row][Col], int g[Row][Col], int b[Row][Col])
{
	for (int y = 0; y < Row; y++)
	for (int x = 0; x < Col; x++)
	{
		gray[y][x] = (r[y][x]*299+g[y][x]*587+b[y][x]*114+500)/1000;
	}
}