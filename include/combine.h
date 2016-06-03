#ifndef STEREOV_COMBINE_H
#define STEREOV_COMBINE_H

//#define FixedWndWeight 1

void windowInit();
int  fixedWindowCombine(int x, int y, int d);
int  multiWindowCombine(int x, int y, int d);

void disparity();

#endif