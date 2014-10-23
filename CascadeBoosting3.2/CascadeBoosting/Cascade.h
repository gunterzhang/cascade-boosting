#pragma once
#include <string>
#include <fstream>
#include "TrainParams.h"
#include "Boosting.h"
#include "PatternModel.h"
#include "NegativeExtractor.h"
#include "PositiveExtractor.h"


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

	int initWorkDir();
	int loadConfig(int is_train);
	int parseSearchParams(char *tmp_str, FILE *fp);

	int extractPositiveSamples();
	int extractNegativeSamples();

	int isStageLearned();
	int updateStageModel();

private:
	TrainParamsT *ptr_params;
	Boosting boosting;
	PatternModel model;
	NegativeExtractor neg_extractor;
	PositiveExtractor pos_extractor;
};