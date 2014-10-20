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

	int work(TrainParamsT *ptr_params);

private:
	int train();
	int test();

	int trainInit();
	int cleanUp();

	int initWorkDir();
	int parseSearchParams(char *tmp_str, FILE *fp);
	int loadConfig(int is_train);
	int loadFeatureConfig();

	int extractPositiveSamples();
	int extractNegativeSamples();

	int isStageLearned();
	int updateStageModel();

private:
	TrainParamsT *ptr_train_params;
	FeatureParamT *p_ft_params;

	Boosting boosting;
	PatternModel model;
	NegativeExtractor neg_extractor;
	PositiveExtractor pos_extractor;
};