#pragma once
#include "Utils.h"
#include "IntegralImage.h"
#include "Feature.h"

const int HFT_STEP1 = 100;

typedef enum {
	HFT_X_AB = 0,
	HFT_Y_AB = 1,
	HFT_X_ABA = 2,
	HFT_Y_ABA = 3,
	HFT_X_ABBA = 4,
	HFT_Y_ABBA = 5,
	HFT_XY_ABA = 6,
	HFT_XY_ABBA = 7,
	HFT_L_AB = 8,
	HFT_R_AB = 9,
	HFT_L_ABA = 10,
	HFT_R_ABA = 11,
	HFT_L_ABBA = 12,
	HFT_R_ABBA = 13,
	HFT_A_B = 14,

	HFT_SQ_X_AB = HFT_X_AB + HFT_STEP1,
	HFT_SQ_Y_AB = HFT_Y_AB + HFT_STEP1,
	HFT_SQ_X_ABA = HFT_X_ABA + HFT_STEP1,
	HFT_SQ_Y_ABA = HFT_Y_ABA + HFT_STEP1,
	HFT_SQ_X_ABBA = HFT_X_ABBA + HFT_STEP1,
	HFT_SQ_Y_ABBA = HFT_Y_ABBA + HFT_STEP1,
	HFT_SQ_XY_ABA = HFT_XY_ABA + HFT_STEP1,
	HFT_SQ_XY_ABBA = HFT_XY_ABBA + HFT_STEP1,
	HFT_SQ_L_AB = HFT_L_AB + HFT_STEP1,
	HFT_SQ_R_AB = HFT_R_AB + HFT_STEP1,
	HFT_SQ_L_ABA = HFT_L_ABA + HFT_STEP1,
	HFT_SQ_R_ABA = HFT_R_ABA + HFT_STEP1,
	HFT_SQ_L_ABBA = HFT_L_ABBA + HFT_STEP1,
	HFT_SQ_R_ABBA = HFT_R_ABBA + HFT_STEP1,
	HFT_SQ_A_B = HFT_A_B + HFT_STEP1
}FeatureTypeT;


struct HaarParamT : FeatureParamT
{
	int is_abs;
	int feature_types;
	CB_PointT tpl_size;

	~HaarParamT();
	virtual int saveToModel(FILE *fp);
	virtual int loadFromModel(FILE *fp);
	virtual int loadFromConfig(FILE *fp);
	virtual CB_PointT getTemplateSize();
	virtual int getFeatureTypes();
};


typedef struct
{
	CB_PointT tpl_size;
	int abs;
	int type;
	CB_PointT pos1;
	CB_PointT pos2;
	CB_PointT size;
	double inv_area;

	int bin_num;
	double bin_min;
	double bin_max;
	double bin_width;
	double inv_bin_width;
}HaarFeatureInfoT;


struct HaarFeatureValueT : FeatureValueT
{
	float value;
};


class HaarFeature : public Feature
{
public:
	HaarFeature(void);
	~HaarFeature(void);

	int initFromFile(FILE *fp, const FeatureParamT &param);
	int loadFromFile(FILE *fp, const FeatureParamT &init_param);
	int saveToFile(FILE *fp);
	int computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const;
	int computeFeatureIndex(const IntegralImage &intg, const SubwinInfoT &subwin) const;
	int computeFeatureIndex(FeatureValueT &feature_value) const;
	int getBinnum(){return info.bin_num;};

public:
	static int slantToRect(const CB_SlantT &slant, CB_RectangleT &rect, const CB_PointT &image_size);
	static inline int isPointValid(const CB_PointT &point, const CB_PointT &image_size);

public:
	HaarFeatureInfoT info;
};

