#include "util.h"
#include "image.h"
#include "stereo.h"
#include "filter.h"
#include "sift_match.h"
#include "params.h"

void doSiftMatch();
void doStereo(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	if (argc == 1)
		doSiftMatch();
	else if (argc == 5)
		doStereo(argc, argv);
	else
		return -1;
	
	return 0;
}

#include <iostream>
using namespace std;

void doSiftMatch()
{
	imgLname = (char*)"test/imL.png";
	imgRname = (char*)"test/imR.png";
	siftMatch();
}

void doStereo(int argc, char* argv[])
{
	const char* imgLeft   = imgLname = argv[1];
	const char* imgRight  = imgRname = argv[2];
	const char* disparRet = argv[3];
	const char* imgRet    = argv[4];
	
	if ( loadRgb(imgLeft,  r1, g1, b1) ||
		 loadRgb(imgRight, r2, g2, b2) )
		errExit();
	markTime(0);
	
	//censusAlgorithm();
	//windowAlgorithm();
    //rowPropagateAlgorithm();
    //gcoAlgorithm();
	maxprodBpAlgorithm(20, 2);
	
	markTime(1);
	saveDepth(disparRet, dispar, 1);
	saveDepth(imgRet, dispar);
}