#pragma once
#include "HaarFeature.h"
#include "FeatureHub.h"

const int HAAR_SHIFT_STEP_X = 2;
const int HAAR_SHIFT_STEP_Y = 2;
const int HAAR_SCALE_STEP_X = 3;
const int HAAR_SCALE_STEP_Y = 2;
const int FEATURE_MARGIN = 1;

const double INV_AREA_R = 500.0;
const int MAX_FEATURE_TYPE_NUM = 200;


class HaarFeatureHub : public FeatureHub
{
public:
	HaarFeatureHub(void);
	~HaarFeatureHub(void);

	int initFromConfig(const string &path);
	int init(const FeatureParamT &param);
	int loadCandid();
	int getFeatureNum();
	FeatureValueT *createTrainingMemory(int num);
	int releaseTrainingMemory(FeatureValueT* &pt_featureValue);

	int getPosFeatureIdx(int sampleIdx, int featureIdx);
	int getNegFeatureIdx(int sampleIdx, int featureIdx);

	virtual Feature *getFeature(int idx);

private:
	void cleanUp();
	int trainFeatures(int pos_num, FeatureValueT *pt_pos_haar_values,
		              int neg_num, FeatureValueT *pt_neg_haar_values);
	int extractFeatures(int sample_num, const string &data_path, FeatureValueT *pt_features);
	int getAllFeatureInfos(int is_extract_feature = 0);
	float extractFeature(const HaarFeature &haar);
	int extractAllFeatures(FILE *fp, HaarFeatureValueT *pt_features);
	int extractOneTypeFeatures(int is_extract_feature, HaarFeatureInfoT &info);
	int extractOneTypeFeatures45(int is_extract_feature, HaarFeatureInfoT &info);
	int extractOneTypeFeaturesAB(int is_extract_feature, HaarFeatureInfoT &info);

public:	
	HaarParamT haar_param;
	HaarFeature *p_haars;
	IntegralImage intg;

	int feature_count;

	CB_PointT feature_sizes[MAX_FEATURE_TYPE_NUM];
	int feature_inv_ratio[MAX_FEATURE_TYPE_NUM];
};

