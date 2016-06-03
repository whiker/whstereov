#ifndef STEREOV_SIFT_ROWPROPAGATE_H
#define STEREOV_SIFT_ROWPROPAGATE_H

void siftInit();
double siftCost(int x, int y, int d);

// private
void siftFeature(int y0, int x0, int gray[Row][Col], double feature[]);
double featureCost(double A[], double B[]);

void oneBins(int y0, int x0, int gray[Row][Col], double bins[]);
void grad(int y0, int x0, int gray[Row][Col], double* magn, int* direct);

#endif