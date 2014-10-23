#pragma once
#include "Utils.h"
#include "IntegralImage.h"


struct FeatureParamT
{
	int bin_num;
	int is_candid;
	string candid_path;

	virtual ~FeatureParamT(){};
	virtual int initFromConfig(FILE *fp) = 0;
	virtual int saveToModel(FILE *fp) = 0;
	virtual int loadFromModel(FILE *fp) = 0;
	virtual CB_PointT getTemplateSize() = 0;
	virtual int getFeatureTypes() = 0;
};


struct FeatureValueT
{
	int index;
};


class Feature
{
public:
	Feature(void);
	virtual ~Feature(void);

	virtual int loadFromModel(FILE *fp) = 0;
	virtual int saveToModel(const string &file_path) = 0;
	virtual int computeFeature(const IntegralImage &intg, const SubwinInfoT &subwin) const{return 1;};
	virtual int computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const{return 1;};
	virtual int computeFeatureIndex(const FeatureValueT &feature_value) const{return 1;};
	virtual int getBinNum() = 0;
	virtual int setParam(FeatureParamT *ptr_param) = 0;
};

