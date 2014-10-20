#pragma once
#include "Utils.h"
#include "Feature.h"

class WeakLearner
{
public:
	WeakLearner(void);
	virtual ~WeakLearner(void);

	int init(Feature *ptr_feature);
	double test(const IntegralImage &intg, const SubwinInfoT &subwin);
	int saveToFile(const string &file_path);
	int loadFromFile(const FILE *fp, const FeatureParamT &param);

public:
	Feature *ptr_feature;
	double output[MAX_BIN_NUM];
	double classify_thd;
};

