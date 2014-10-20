#pragma once
#include "FernFeature.h"
#include "HaarFeatureHub.h"


class FernFeatureHub : public FeatureHub
{
public:
	FernFeatureHub(void);
	~FernFeatureHub(void);

	int init(const FeatureParamT &param);
	int initTrainingMem(int pos_num, int neg_num);
	int train(int pos_num, const string &pos_data_path, 
	          int neg_num, const string &neg_data_path);
	int getFeatureNum();
	FeatureValueT *createTrainingMemory(int num);
	int releaseTrainingMemory(FeatureValueT* &pt_featureValue);
	Feature *getFeature(int idx);
	int getPosFeatureIdx(int sampleIdx, int featureIdx);
	int getNegFeatureIdx(int sampleIdx, int featureIdx);


private:
	void cleanUp();
	int initFromFile();
	int trainFeatures(int pos_num, int neg_num);
	int getFernsInfo(int flag);
	const FernFeature *extractAllFeatures(FILE *fp);
	int getAllFeatureInfos(int is_extract_feature = 0);
	int computePosFeatureIdx(int sampleIdx, int featureIdx);
	int computeNegFeatureIdx(int sampleIdx, int featureIdx);


public:
	HaarFeatureHub haar_hub;
	FernFeature *p_ferns;
	FernParamT fern_param;
};

