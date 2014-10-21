#pragma once
#include "Utils.h"
#include "IntegralImage.h"


struct FeatureParamT
{
	int bin_num;
	int is_candid;
	string candid_path;

	virtual ~FeatureParamT(){};
	virtual int saveToModel(FILE *fp) = 0;
	virtual int loadFromModel(FILE *fp) = 0;
	virtual int loadFromConfig(FILE *fp) = 0;
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

	virtual int loadFromFile(FILE *fp, const FeatureParamT &init_param) = 0;
	virtual int saveToFile(FILE *fp){return 1;};
	virtual int computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const{return 1;};
	virtual int computeFeatureIndex(const IntegralImage &intg, const SubwinInfoT &subwin) const{return 1;};
	virtual int computeFeatureIndex(FeatureValueT &feature_value) const{return 1;};
	virtual int getBinnum() = 0;
};

