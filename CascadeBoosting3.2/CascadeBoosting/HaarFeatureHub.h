#pragma once
#include "HaarFeature.h"

const int HAAR_SHIFT_STEP_X = 2;
const int HAAR_SHIFT_STEP_Y = 2;
const int HAAR_SCALE_STEP_X = 3;
const int HAAR_SCALE_STEP_Y = 2;
const int FEATURE_MARGIN = 1;

const double INV_AREA_R = 500.0;
const int MAX_FEATURE_TYPE_NUM = 200;

class HaarFeatureHub
{
public:
	HaarFeatureHub(void);
	~HaarFeatureHub(void);

	int init(int w, int h, int type, int is_abs);
	int getFeatureNum();
	const HaarFeatureValueT *extractAllFeatures(FILE *fp);

private:
	void clearUp();
	float extractFeature(const HaarFeature &haar);
	int getAllFeatureInfos(int is_extract_feature = 0, FILE *fp = NULL);
	int extractOneTypeFeatures(int is_extract_feature, HaarFeature &haar);
	int extractOneTypeFeatures45(int is_extract_feature, HaarFeature &haar);
	int extractOneTypeFeaturesAB(int is_extract_feature, HaarFeature &haar);

public:	
	HaarFeature *pt_haars;

private:
	IntegralImage intg;

	HaarFeatureValueT *pt_features;

	int template_w;
	int template_h;

	int feature_types;
	int feature_abs;

	int feature_count;
	int feature_num;

	CB_PointT feature_sizes[MAX_FEATURE_TYPE_NUM];
	int feature_inv_ratio[MAX_FEATURE_TYPE_NUM];
};

