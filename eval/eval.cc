#include <stdio.h>
#include <cmath>
#include <opencv2/opencv.hpp>

typedef unsigned char uchar;

int main(int argc, char* argv[])
{
	IplImage* img_ret = cvLoadImage(argv[1], 0);
	IplImage* img_truth = cvLoadImage(argv[2], 0);
	IplImage* img_nonocc = cvLoadImage(argv[3], 0);
	
	int H = img_ret->height, W = img_ret->width;
	int WS = img_ret->widthStep;
	
	uchar* d1 = (uchar*) img_ret->imageData;
	uchar* d2 = (uchar*) img_truth->imageData;
	uchar* d3 = (uchar*) img_nonocc->imageData;
	
	int nPix = 0, nBad1 = 0;
	int64_t err, sum_err = 0;
	const int Border = 7;
	
	for (int y = Border; y < H-Border; y++)
	{
		uchar* p1 = d1 + y * WS;
		uchar* p2 = d2 + y * WS;
		uchar* p3 = d3 + y * WS;
		
		for (int x = Border; x < W-Border; x++, p1++, p2++, p3++)
		{
			if (*p3 != 255) continue;
			nPix++;
			err = *p1;
			err = abs(err - *p2);
			sum_err += err;
			if (err > 1)
				nBad1++;
		}
	}
	
	float avg_err = sum_err / (float) nPix;
	float bad1 = 100*nBad1 / (float) nPix;
	printf("error match percent: %.2f\n", bad1);
	
	cvReleaseImage(&img_ret);
	cvReleaseImage(&img_truth);
	cvReleaseImage(&img_nonocc);
}