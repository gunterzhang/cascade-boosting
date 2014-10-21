#pragma once
#include "HaarFeature.h"

const int MAX_FERN_CELL_NUM = 10;


struct FernParamT : FeatureParamT
{
	HaarParamT haar_param;
	int fern_cell_num;

	virtual ~FernParamT();
	virtual int saveToModel(FILE *fp);
	virtual int loadFromModel(FILE *fp);
	virtual int loadFromConfig(FILE *fp);
	virtual CB_PointT getTemplateSize();
	virtual int getFeatureTypes();
};


struct FernFeatureValueT : FeatureValueT
{
	HaarFeatureValueT haar_value[MAX_FERN_CELL_NUM];
};


class FernFeature : public Feature
{
public:
	FernFeature(void);
	~FernFeature(void);

	int init(const FeatureParamT &init_param);
	int loadFromFile(FILE *fp, const FeatureParamT &init_param);
	int saveToFile(FILE *fp);
	int computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const;
	int computeFeatureIndex(const IntegralImage &intg, const SubwinInfoT &subwin) const;
	int computeFeatureIndex(FeatureValueT &src_feature_value) const;
	int getBinnum(){return param.bin_num;};

public:
	FernParamT param;
	HaarFeature *ptr_haars[MAX_FERN_CELL_NUM];
	int haars_idx[MAX_FERN_CELL_NUM];
};

