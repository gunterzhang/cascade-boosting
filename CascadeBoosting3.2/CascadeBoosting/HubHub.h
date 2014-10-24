#pragma once
#include "FernFeatureHub.h"
#include "HaarFeatureHub.h"


class HubHub : public FeatureHub
{
public:
	HubHub(void);
	~HubHub(void);

	virtual int initFromConfig(const string &path);
	virtual int init(const FeatureParamT &param);
	virtual int getFeatureNum();
	virtual int getBinNum(){return bin_num;};
	virtual int train(int pos_num, const string &pos_data_path, 
	                  int neg_num, const string &neg_data_path);

	virtual int newTrainingMem(int max_pos_num, int max_neg_num);
	virtual FeatureValueT *createTrainingMemory(int num) = 0;
	virtual int releaseTrainingMemory(FeatureValueT* &pt_featureValue) = 0;

	virtual Feature *getFeature(int idx) = 0;
	
	virtual int getPosFeatureIdx(int sampleIdx, int featureIdx) = 0;
	virtual int getNegFeatureIdx(int sampleIdx, int featureIdx) = 0;

private:
	FernFeatureHub fern_hub;
	HaarFeatureHub haar_hub;
};

