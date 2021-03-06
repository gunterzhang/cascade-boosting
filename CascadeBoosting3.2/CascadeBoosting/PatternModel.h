#pragma once
#include "Utils.h"
#include "WeakLearner.h"
#include "FernFeature.h"
#include "TrainParams.h"

class PatternModel
{
public:
	PatternModel(void);
	~PatternModel(void);

	int init(const TrainParamsT *ptr_params);
	int loadFromModel(const string &file_path);
	int saveToModel(const string &file_path);
	int addNewStage();

private:
	void cleanUp();

public:
	FeatureParamT *p_ft_param;
	int stage_num;
	int stage_idx[MAX_CASCADE_STAGE_NUM];
	double stage_thd[MAX_CASCADE_STAGE_NUM];

	int weak_learner_num;
	Feature *p_features;
	WeakLearner *p_weak_learners;

	FeatureTypeE feature_type;
};

