#ifndef STEREOV_TRIM_CANNY_H
#define STEREOV_TRIM_CANNY_H

void getCannyEdge();

// private
void trimCanny();
void trimCannySearch(int y0, int x0, int& ind, int& bsind);

#endif