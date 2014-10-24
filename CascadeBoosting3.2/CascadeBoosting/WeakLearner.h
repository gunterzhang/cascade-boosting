#pragma once
#include "Utils.h"
#include "Feature.h"


class WeakLearner
{
public:
	WeakLearner(void);
	virtual ~WeakLearner(void);

	int setFeature(Feature *ptr_feature);
	int copyFeature(Feature *ptr_feature);
	double test(const IntegralImage &intg, const SubwinInfoT &subwin);

	int loadFromModel(const FILE *fp);
	int saveToModel(const string &file_path);
	int getBinNum();

public:
	Feature *ptr_feature;
	double output[MAX_BIN_NUM];
	double classify_thd;
};

