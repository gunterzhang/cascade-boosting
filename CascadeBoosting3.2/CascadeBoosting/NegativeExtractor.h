#pragma once
#include "TrainParams.h"
#include "IntegralImage.h"
#include "Detector.h"

using namespace cv;

class NegativeExtractor
{
public:
	NegativeExtractor(void);
	~NegativeExtractor(void);

	int init(TrainParamsT &param);
	int extractSamples(int num, const PatternModel *model);
	int getSampleNum(const string &path);

private:
	int detectImage(const Mat &image);
	bool isRectOverlapLabels(CB_RectT rect);
	bool isRectsOverlap(CB_RectT rect1, CB_RectT rect2);
	bool loadLabelsFromFile(const string &file_path);
	void saveTrainingData(const Mat &image, CvRect rect);

private:
	TrainParamsT *ptr_params;
	IntegralImage sample_intg;
	Detector detector;

	double scan_shift_step;
	double scan_scale_step;

	int pool_image_idx;
	int pool_image_num;
	string image_file_names[MAX_NEG_SAMPLE_NUM];

	long long total_negative_count;
	long long total_count;
	long long detect_count;

	string postive_label;

	int label_num;
	int total_label_num;
	CB_RectT label_rect[MAX_LABEL_PER_IMAGE];
	string label_type[MAX_LABEL_PER_IMAGE];
	CB_PointT char_idx[100];

	string cur_neg_name;
};

