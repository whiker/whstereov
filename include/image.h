#ifndef STEREOV_IMAGE_H
#define STEREOV_IMAGE_H

#include <opencv2/opencv.hpp>

int  loadRgb(const char* filename, int r[Row][Col], int g[Row][Col], int b[Row][Col]);
void saveDepth(const char* filename, int depth[Row][Col], int gray_inter=DisparInter);
void saveEdgeImage(const char* filename);

void convertDisparImg(int dispar[Row][Col], IplImage* img, int gray_inter, bool flag);

void toGray(int gray[Row][Col], int r[Row][Col], int g[Row][Col], int b[Row][Col]);

#endif