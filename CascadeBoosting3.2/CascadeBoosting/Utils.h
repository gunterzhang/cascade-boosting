#pragma once

#include <string>
#include <afx.h>
#include <io.h> 
#include <direct.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


const int MAX_CASCADE_STAGE_NUM = 25;
const int MAX_ROUND_NUM = 200;
const int MAX_BIN_NUM = 5000;
const int MAX_NEG_SAMPLE_NUM = 20000;
const int MAX_LABEL_PER_IMAGE = 20;
const int MAX_PATH_LEN = 500;

const int MIN_MEAN = 20;
const int MAX_MEAN = 240;
const int MIN_VAR = 10;
const int MAX_VAR = 100;

const double MAX_OBJ_OVERLAP_X = 0.4;
const double MAX_OBJ_OVERLAP_Y = 0.4;

typedef enum 
{
	HAAR = 0, 
	FERN = 1,
	UNKNOWN = 1000
}FeatureTypeE;

typedef enum 
{
	UPRIGHT_HAAR = 1, 
	SLANT_HAAR = 2, 
	UPRIGHT_FAR_HAAR = 4, 
	UPRIGHT_SQ_HAAR = 8, 
	UPRIGHT_SQ_FAR_HAAR = 16
}Haar_FeatureTypeE;


typedef struct
{
	int x;
	int y;
}CB_PointT;


typedef struct
{
	double x;
	double y;
}CB_DBl_PointT;

typedef struct
{
	int left;
	int right;
	int top;
	int bottom;
}CB_RectT;


typedef struct
{
	CB_PointT left;
	CB_PointT right;
	CB_PointT top;
	CB_PointT bottom;
}CB_RectangleT;


typedef struct
{
	int x;
	int y;
	int L;
	int R;
}CB_SlantT;


typedef struct
{
	CB_PointT image_size;
	CB_PointT win_size;	
	CB_PointT win_pos;
	double mean;
	double var;
}SubwinInfoT;


int waitForKey(const char* str);
int getCurrentDir(string &path);
