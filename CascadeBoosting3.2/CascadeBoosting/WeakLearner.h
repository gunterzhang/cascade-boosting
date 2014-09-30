#pragma once
#include "TrainParams.h"
#include "HaarFeature.h"


class WeakLearner
{
public:
	WeakLearner(void);
	~WeakLearner(void);

	double test(const IntegralImage &intg, const SubwinInfoT &subwin);
	int saveToFile(const string &file_path);
	int loadFromFile(const FILE *fp, int template_w, int template_h);

public:
	HaarFeature haar;
	double output[MAX_BIN_NUM];
	double classify_thd;
};

