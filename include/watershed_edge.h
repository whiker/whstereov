#ifndef STEREOV_WATERSHED_H
#define STEREOV_WATERSHED_H

#include <vector>
#include <opencv2/opencv.hpp>

void getWatershedEdge();

// private
void calcDispar();
void setDisparBinaryImg(IplImage* dispar_img, int dispar_i);
int  getPointNumThresh(CvSeq* contours);
void readContour(CvSeq* contour, std::vector<CvPoint>& points);
void setMarker(std::vector<CvPoint>& points, const int dispar_i);
void watershed();
void convertMarkerImgMat(IplImage* marker_img, bool matToImg);
void trimWatershed();
void trimClosedEdge(int y0, int x0, int mark_curr,
		std::vector<int>& ys, std::vector<int>& xs);

void drawContours(CvSeq* contours, const char* filename);
void drawMarks(const char* filename);
void saveMarkerImage(const char* filename, IplImage* marker_img);
void mySaveEdgeImage(const char* filename);

#endif