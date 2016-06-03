#ifndef STEREOV_SIFT_MATCH_H
#define STEREOV_SIFT_MATCH_H

#include <vector>

struct PointPair;

void siftMatch();

// private
void drawMatchPoint(std::vector<PointPair>& match_points);

#endif