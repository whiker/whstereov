#ifndef STEREOV_MAXPROD_BP_H
#define STEREOV_MAXPROD_BP_H

/*
please refer to following papers:

[1] Stereo Matching Using Belief Propagation.
	Jian Sun, Heung-Yeung Shum, Nan-Ning Zheng.
	ECCV 2002Springer Berlin Heidelberg, 2002
*/

void maxprodBp(int nIter, int edge_kind, Cost data_cost_fn);

// private
void mbpInit(Cost data_cost_fn);
void mbpInitSmoothMatrix();
void mbpInitNode();

void mbpPropagate(const int nIter);
void mbpLeftRight();
void mbpUpDown();
inline void mbpSendMsg(int x1, int y1, int x2, int y2, int pos1, int pos2);
void mbpAdjust();

void mbpBeliefToLabel();

#endif