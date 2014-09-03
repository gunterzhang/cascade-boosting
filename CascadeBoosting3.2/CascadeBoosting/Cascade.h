#pragma once
#include <string>
#include <fstream>
#include "TrainParams.h"
#include "Boosting.h"
#include "WeakLearner.h"
#include "NegativeExtractor.h"
#include "PositiveExtractor.h"
#include "HaarFeature.h"


class Cascade
{
public:
	Cascade(void);
	~Cascade(void);

	int train(TrainParamsT &params);

private:
	int init(TrainParamsT &params);
	int loadConfig(TrainParamsT &params);
	int extractPositiveSamples();
	int extractNegativeSamples();
	int isStageLearned(double detection_rate, double false_alarm);
	int saveTrainedModel();
	int saveTrainedStages();
	int initModel();
	int getSampleNum(const string &path);
	int batchTest(int num, const string &data_path);
	
private:
	CascadeModelT model;
	TrainParamsT *pt_params;
	NegativeExtractor neg_extractor;
	PositiveExtractor pos_extractor;
	Boosting boosting;
};

