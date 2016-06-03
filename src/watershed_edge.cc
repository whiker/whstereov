#include "global.h"
#include "stereo.h"
#include "image.h"
#include "watershed_edge.h"
using namespace std;
using namespace cv;

const float NpointThresh = 0.05;
const float Drate = 0.1;
const int   Ndrate = 1/Drate-1;
const int   TrimThresh = 300;
const int   MarkNum = DisparNum + 1;

int  marker[Row][Col];
int  site_refresh[TrimThresh];
int  mark_cnt[MarkNum];
char mark_flag[MarkNum];

void getWatershedEdge()
{
	calcDispar();
	vector<CvPoint> points;
	memset(marker, 0, Row*Col*sizeof(int));
	IplImage* dispar_img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	
	for (int dispar_i = 0; dispar_i < DisparNum; dispar_i++)
	{
		setDisparBinaryImg(dispar_img, dispar_i);
		
		CvSeq* contours = NULL;
		CvMemStorage* store = cvCreateMemStorage(0);
		cvFindContours(dispar_img, store, &contours, sizeof(CvContour),
			CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		
		if (dispar_i == 10)
			drawContours(contours, "ret/contour_0.png");
		int npoint_thresh = getPointNumThresh(contours);
		
		for (CvSeq* cont=contours; cont!=NULL; cont=cont->h_next)
		{
			if (cont->total <= npoint_thresh) continue;
			readContour(cont, points);
			setMarker(points, dispar_i);
		}
		cvReleaseMemStorage(&store);
	}
	cvReleaseImage(&dispar_img);
	drawMarks("ret/marker.png");
	
	watershed();
}

void calcDispar()
{
	gSetWndSize(9);
	windowAlgorithm();
	gSetWndSize(3);
}

void setDisparBinaryImg(IplImage* dispar_img, int dispar_i)
{
	uchar *data = (uchar*)dispar_img->imageData, *p;
	int width_step = dispar_img->widthStep;
	memset(data, 0, dispar_img->imageSize);
	
	for (int y = RowMin; y <= RowMax; y++)
	{
		p = data + y * width_step;
		for (int x = ColMin; x <= ColMax; x++, p++)
			*p = dispar[y][x]==dispar_i ? 255 : 0;
	}
}

int getPointNumThresh(CvSeq* contours)
{
	int npoint_max = 0;
	for (; contours != NULL; contours = contours->h_next)
		if (contours->total > npoint_max)
			npoint_max = contours->total;
	return (npoint_max * NpointThresh);
}

void readContour(CvSeq* contour, vector<CvPoint>& points)
{
	CvSeqReader seq_reader;
	cvStartReadSeq(contour, &seq_reader);
	
	int npoint = contour->total;
	points.resize(npoint);
	CvPoint point;
	for (int i = 0; i < npoint; i++)
	{
		CV_READ_SEQ_ELEM(point, seq_reader);
		points[i] = point;
	}
}

void setMarker(vector<CvPoint>& points, const int dispar_i)
{
	const int npoint = points.size();
	int y, x, dy, dx, y1, x1;
	
	int i=0, j=npoint/2;
	for (; j < npoint; i++, j++)
	{
		y=points[i].y; x=points[i].x;
		dy=points[j].y-y; dx=points[j].x-x;
		float rate = 0.0;
		for (int k = Ndrate; k >= 0; k--)
		{
			rate += Drate;
			y1=y+rate*dy; x1=x+rate*dx;
			if (dispar[y1][x1] == dispar_i)
				marker[y1][x1] = dispar_i+1;
		}
	}
}

void watershed()
{
	IplImage* marker_img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_32S, 1);
	convertMarkerImgMat(marker_img, 1);
	
	IplImage* img = cvLoadImage(imgLname, CV_LOAD_IMAGE_COLOR);
	cvWatershed(img, marker_img);
	convertMarkerImgMat(marker_img, 0);
	saveMarkerImage("ret/segment.png", marker_img);
	
	mySaveEdgeImage("ret/watershed_0.png");
	trimWatershed();
	mySaveEdgeImage("ret/watershed_1.png");
	
	cvReleaseImage(&marker_img);
	cvReleaseImage(&img);
}

void convertMarkerImgMat(IplImage* marker_img, bool matToImg)
{
	char* img_data = marker_img->imageData;
	int width_step = marker_img->widthStep;
	
	for (int y = 0; y < Row; y++)
	{
		int* p = (int*) (img_data + y * width_step);
		if (matToImg)
			for (int x = 0; x < Col; x++, p++)
				*p = marker[y][x];
		else
			for (int x = 0; x < Col; x++, p++)
				marker[y][x] = *p;
	}
}

#define ForeachNeigh \
	yMin=max(0,y0-1), yMax=min(Row1,y0+1); \
	xMin=max(0,x0-1), xMax=min(Col1,x0+1); \
	for (y = yMin; y <= yMax; y++) \
	for (x = xMin; x <= xMax; x++) \

inline bool isEdgeOfMarkerI(int y0, int x0, int mark_i)
{
	int yMin, yMax, xMin, xMax, y, x;
	ForeachNeigh {
		if (marker[y][x] == mark_i)
			return true;
	}
}

void trimWatershed()
{
	vector<int> ys, xs;
	
	for (int mark_i = 1; mark_i <= DisparNum; mark_i++)
	{
		memset(bp_flag, 0, Row*Col);
		for (int y = 0; y < Row; y++)
		for (int x = 0; x < Col; x++)
			bp_flag[y][x] = ( marker[y][x] == -1 &&
				isEdgeOfMarkerI(y, x, mark_i) );
		
		for (int y = 0; y < Row; y++)
		for (int x = 0; x < Col; x++)
		{
			if (bp_flag[y][x])
				trimClosedEdge(y, x, mark_i, ys, xs);
		}
	}
}

#define PushSite(y,x) ys.push_back(y), xs.push_back(x);

void trimClosedEdge(int y0, int x0, int mark_curr,
					vector<int>& ys, vector<int>& xs)
{
	ys.clear(); xs.clear();
	PushSite(y0, x0)
	int yMin, yMax, xMin, xMax, y, x;
	
	for (int i = 0; i < ys.size(); i++)
	{
		y0=ys[i], x0=xs[i];
		ForeachNeigh {
			if (bp_flag[y][x]) {
				bp_flag[y][x] = 0;
				PushSite(y,x)
			}
		}
	}
	if (ys.size() > TrimThresh)
		return;
	
	int ys_size = ys.size();
	int flag_cnt, j, k;
	memset( mark_cnt, 0, (MarkNum<<2) );
	
	for (int i = 0; i < ys_size; i++)
	{
		memset(mark_flag, 1, MarkNum);
		flag_cnt = 0;
		y0=ys[i], x0=xs[i];
		
		ForeachNeigh {
			j = marker[y][x];
			if (j<1 || j==mark_curr)
				continue;
			mark_cnt[j]++;
			flag_cnt += mark_flag[j];
			mark_flag[j] = 0;
			k = j;
		}
		if (flag_cnt == 1)
			site_refresh[i] = k;
		else
			site_refresh[i] = -1;
	}
	
	int mark_new = 1, max_cnt = mark_cnt[1];
	for (int i = 2; i < MarkNum; i++)
	{
		if (mark_cnt[i] > max_cnt)
		{
			max_cnt = mark_cnt[i];
			mark_new = i;
		}
	}
	for (int i = 0; i < ys_size; i++)
	{
		if (site_refresh[i] > 0)
			marker[ys[i]][xs[i]] = mark_new;
	}
}

void drawContours(CvSeq* contours, const char* filename)
{
	IplImage* img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	memset(img->imageData, 255, img->imageSize);
	
	CvScalar color = cvScalar(0,0,0);
	cvRectangle(img, cvPoint(0,0), cvPoint(Col1,Row1), color, 2);
	cvDrawContours(img, contours, color, CV_RGB(255,255,255), 1, 1, 1);
	
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

void drawMarks(const char* filename)
{
	IplImage* img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	memset(img->imageData, 255, img->imageSize);
	
	CvScalar color = cvScalar(0,0,0);
	cvRectangle(img, cvPoint(0,0), cvPoint(Col1,Row1), color, 2);
	for (int y = 0; y < Row; y++)
	for (int x = 0; x < Col; x++)
	{
		if (marker[y][x] > 0)
			cvCircle(img, cvPoint(x,y), 1, color, -1);
	}
	
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

void saveMarkerImage(const char* filename, IplImage* marker_img)
{
	IplImage* img = cvCreateImage(cvSize(Col,Row), IPL_DEPTH_8U, 1);
	const char* src_data = marker_img->imageData;
	char* dst_data = img->imageData;
	int src_ws = marker_img->widthStep;
	int dst_ws = img->widthStep;
	
	for (int y = 0; y < Row; y++)
	{
		int* pSrc = (int*) (src_data + y * src_ws);
		uchar* pDst = (uchar*) (dst_data + y * dst_ws);
		
		for (int x = 0; x < Col; x++, pSrc++, pDst++)
			*pDst = saturate_cast<uchar>( (*pSrc) * DisparInter );
	}
	
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

void mySaveEdgeImage(const char* filename)
{
	for (int y = 0; y < Row; y++)
	for (int x = 0; x < Col; x++)
		bp_edge[y][x] = (marker[y][x] == -1);
	saveEdgeImage(filename);
}