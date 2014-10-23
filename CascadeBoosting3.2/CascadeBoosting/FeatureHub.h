#pragma once
#include "Utils.h"
#include "Feature.h"


class FeatureHub
{
public:
	FeatureHub(void);
	virtual ~FeatureHub(void);

	virtual int initFromConfig(const string &path) = 0;
	virtual int init(const FeatureParamT &param) = 0;
	virtual int getFeatureNum() = 0;
	virtual int getBinNum(){return bin_num;};
	virtual int train(int pos_num, const string &pos_data_path, 
	                  int neg_num, const string &neg_data_path);

	virtual int newTrainingMem(int max_pos_num, int max_neg_num);
	virtual FeatureValueT *createTrainingMemory(int num) = 0;
	virtual int releaseTrainingMemory(FeatureValueT* &pt_featureValue) = 0;
	virtual Feature *getFeature(int idx) = 0;
	virtual int getPosFeatureIdx(int sampleIdx, int featureIdx) = 0;
	virtual int getNegFeatureIdx(int sampleIdx, int featureIdx) = 0;

protected:
	int cleanUp();
	virtual int loadCandid() = 0;
	virtual int trainFeatures(int pos_num, FeatureValueT *pt_pos_values,
		                      int neg_num, FeatureValueT *pt_neg_values) {return 1;};
	virtual int extractFeatures(int sample_num, const string &data_path, FeatureValueT *pt_features) {return 1;};

public:
	int bin_num;
	int feature_num;
	int pos_num;
	int neg_num;

	FeatureValueT *p_pos_features;
	FeatureValueT *p_neg_features;
};

