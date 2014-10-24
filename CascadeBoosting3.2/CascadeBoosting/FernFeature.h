#pragma once
#include "HaarFeature.h"

const int MAX_FERN_CELL_NUM = 2;


struct FernParamT : FeatureParamT
{
	HaarParamT haar_param;
	int fern_cell_num;

	virtual ~FernParamT();
	virtual int initFromConfig(FILE *fp);
	virtual int saveToModel(FILE *fp);
	virtual int loadFromModel(FILE *fp);
	virtual CB_PointT getTemplateSize();
	virtual int getFeatureTypes();
};

typedef struct
{
	FernParamT param;
	HaarFeature *ptr_haars[MAX_FERN_CELL_NUM];
	int haars_idx[MAX_FERN_CELL_NUM];
}FernFeatureInfoT;


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
	int copy(Feature *ptr_feature);
	int loadFromModel(FILE *fp);
	int saveToModel(const string &file_path);
	int computeFeature(const IntegralImage &intg, const SubwinInfoT &subwin) const;
	int computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const;
	int computeFeatureIndex(const FeatureValueT &src_feature_value) const;
	int getBinNum(){return info.param.bin_num;};
	int setParam(FeatureParamT *ptr_param);

public:
	FernFeatureInfoT info;
};

