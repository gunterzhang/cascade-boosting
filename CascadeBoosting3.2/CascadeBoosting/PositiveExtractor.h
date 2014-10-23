#pragma once
#include "TrainParams.h"
#include "IntegralImage.h"
#include "PatternModel.h"

class PositiveExtractor
{
public:
	PositiveExtractor(void);
	~PositiveExtractor(void);

	int extractSamples(const TrainParamsT *pt_params, const PatternModel *model);
	int getSampleNum();

private:
	int sample_num;
};

