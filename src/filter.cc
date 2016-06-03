#include "global.h"
#include "image.h"
using namespace cv;

void filter(int type)
{
	IplImage* src_img = cvCreateImage(cvSize(Col, Row), IPL_DEPTH_8U, 1);
	convertDisparImg(dispar, src_img, 1, 1);
	Mat src(src_img), dst(Row, Col, CV_8U);
	
	if (type == 1)
		medianBlur(src, dst, 3);  // 中值滤波
	else if (type == 2)
		blur(src, dst, Size(3,3));  // 均值滤波, 效果不好
	else if (type == 3)
		bilateralFilter(src, dst, 5, 20, 20);  // 双边滤波, 效果不好
	
	cvReleaseImage(&src_img);
	IplImage dst_img(dst);
	convertDisparImg(dispar, &dst_img, 1, 0);
}