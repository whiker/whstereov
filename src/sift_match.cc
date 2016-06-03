#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace std;

extern "C" {
#include "opensift/include/sift.h"
#include "opensift/include/imgfeatures.h"
#include "opensift/include/kdtree.h"
}

#include "global.h"
#include "sift_match.h"

#define KnnNum 200
#define NnnDistRatio 0.49 // threshold on squared ratio of distances between NN and 2nd NN

inline CvPoint getPointFromFeat(const feature* feat)
{
	return cvPoint( cvRound(feat->x), cvRound(feat->y) );
}

struct PointPair
{
	CvPoint p1, p2;
};

void siftMatch()
{
	IplImage* img1 = cvLoadImage(imgLname, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* img2 = cvLoadImage(imgRname, CV_LOAD_IMAGE_GRAYSCALE);
	if (img1 == NULL || img2 == NULL)
		return;
	feature *feat1, *feat2;
	int n1 = sift_features(img1, &feat1);
	int n2 = sift_features(img2, &feat2);
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	printf("feature num: %d, %d\n", n1, n2);
	
	vector<PointPair> match_points;
	kd_node* kd_root = kdtree_build(feat2, n2);
	
	for (int i = 0; i < n1; i++)
	{
		feature* feat = feat1 + i;
		feature **matchFeats;
		
		/*
		if (kdtree_bbf_knn(kd_root, feat, 2, &matchFeats, KnnNum) == 2)
		{
			int d0 = descr_dist_sq(feat, matchFeats[0]);
			int d1 = descr_dist_sq(feat, matchFeats[1]);
			if(d0 < d1 * NnnDistRatio)
			{
				PointPair pp;
				pp.p1 = getPointFromFeat(feat);
				pp.p2 = getPointFromFeat(matchFeats[0]);
				match_points.push_back(pp);
				feat->fwd_match = matchFeats[0];
			}
		}
		//*/
		
		int n3 = kdtree_bbf_knn(kd_root, feat, n2, &matchFeats, KnnNum);
		CvPoint featPt = getPointFromFeat(feat), matchPt;
		int dist = Int32Max, index = -1;
		
		for (int j = 0; j < n3; j++)
		{
			CvPoint p = getPointFromFeat(matchFeats[j]);
			if (p.y == featPt.y)
			{
				int d = descr_dist_sq(feat, matchFeats[j]);
				if (d < dist)
				{
					dist = d;
					index = j;
					matchPt = p;
				}
			}
		}
		
		if (dist < Int32Max)
		{
			PointPair pp = { featPt, matchPt };
			match_points.push_back(pp);
			feat->fwd_match = matchFeats[index];
		}
		
		free(matchFeats);
	}
	
	drawMatchPoint(match_points);
	printf("match num: %d\n", (int)match_points.size());
	
	free(feat1);
	free(feat2);
	kdtree_release(kd_root);
}

void drawMatchPoint(vector<PointPair>& match_points)
{
	IplImage* img1 = cvLoadImage(imgLname, CV_LOAD_IMAGE_COLOR);
	IplImage* img2 = cvLoadImage(imgRname, CV_LOAD_IMAGE_COLOR);
	
	int w = img1->width + img2->width;
	int h = max(img1->height, img2->height);
	
	IplImage* match_img = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 3);
	cvSetImageROI(match_img, cvRect(0,0,img1->width,img1->height));
	cvCopy(img1, match_img, NULL);
	cvSetImageROI(match_img, cvRect(img1->width,0,img2->width,img2->height));
	cvCopy(img2, match_img, NULL);
	cvResetImageROI(match_img);
	
	for (int i = match_points.size()-1; i >= 0; i--)
	{
		PointPair& pp = match_points[i];
		pp.p2.x += img1->width;
		cvLine(match_img, pp.p1, pp.p2, CV_RGB(30,30,255), 1, 8, 0);
	}
	
	cvSaveImage("ret/sift_match.png", match_img);
	
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&match_img);
}