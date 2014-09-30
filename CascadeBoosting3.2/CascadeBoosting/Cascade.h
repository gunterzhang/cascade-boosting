#pragma once
#include <string>
#include <fstream>
#include "TrainParams.h"
#include "Boosting.h"
#include "NegativeExtractor.h"
#include "PositiveExtractor.h"
#include "PatternModel.h"

class Cascade
{
public:
	Cascade(void);
	~Cascade(void);

	int work(TrainParamsT &params);

private:
	int init();
	int train();
	int test();

	int loadConfig();
	int initModel();
	int extractPositiveSamples();
	int extractNegativeSamples();
	int isStageLearned();
	int updateStageModel();

private:
	TrainParamsT *pt_params;
	Boosting boosting;
	PatternModel model;
	NegativeExtractor neg_extractor;
	PositiveExtractor pos_extractor;
};

