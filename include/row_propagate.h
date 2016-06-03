#ifndef STEREOV_ROW_PROPAGATE_H
#define STEREOV_ROW_PROPAGATE_H

/*
please refer to following papers:

[1] Stereo matching with reliable disparity propagation.
	Sun, Xun, et al. In 3DIMPVT, 2011.
*/

//#define RowPropagateCensus 1

void rowPropagate();

// private
void calcRowline();
void calcRowrine();
void calcSeed();
void propagate();
void vote();
void doFilter(int type = 1);

bool similarColor(int x, int y, int r, int g, int b);
bool similarColorR(int x, int y, int r, int g, int b);
double lineCombine(int x, int y, int d);

#endif