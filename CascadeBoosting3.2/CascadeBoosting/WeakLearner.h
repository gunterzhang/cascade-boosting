#pragma once
#include "TrainParams.h"
#include "HaarFeature.h"

typedef struct
{
	HaarFeatureInfoT haar_info;
	int bin_num;
	double bin_min;
	double bin_max;
	double bin_width;
	double output[MAX_BIN_NUM];
	double classify_thd;
}WeakLearnerInfoT;


class WeakLearner
{
public:
	WeakLearner(void);
	~WeakLearner(void);

	double test(float feature_value);
	double test(const IntegralImage &intg, const SubwinInfoT &subwin);
	int saveToFile(const string &file_path);

public:
	WeakLearnerInfoT info;
};

