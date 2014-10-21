#pragma once
#include "Utils.h"
#include "ObjMerger.h"
#include "PatternModel.h"
#include "FernFeatureHub.h"


typedef struct
{
	int resize_w;
	int resize_h;
	double scan_shift_step;
	double scan_scale_step;
	int min_win_w;
	int max_win_w;
	CB_RectT hot_rect;
}DetectorParamT;


class Detector
{
public:
	Detector(void);
	~Detector(void);

	int init(const string &model_path);
	int init(PatternModel *pt_model);
	int detect(const Mat &image, int max_num, CB_RectT *pt_rects, int &subwin_count);
	int batchDetect(const string &src_image_folder, const string &dst_image_folder);
	int setScanParams(const DetectorParamT *pt_param);
	int test(IntegralImage &intg, SubwinInfoT &subwin);

private:
	int detect(IntegralImage &intg, int max_num, CB_RectT *pt_rects, int &subwin_count);
	int preTest(IntegralImage &intg, SubwinInfoT &subwin);
	int drawRects(Mat &image, int num, CB_RectT *pt_rects, Scalar &color, int thickness, double r);

private:
	PatternModel model;
	PatternModel *ptr_model;
	DetectorParamT param;
	ObjMerger merger;
};

