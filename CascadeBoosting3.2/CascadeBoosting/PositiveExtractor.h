#pragma once
#include "TrainParams.h"
#include "IntegralImage.h"

class PositiveExtractor
{
public:
	PositiveExtractor(void);
	~PositiveExtractor(void);

	int extractSamples(const TrainParamsT *pt_params);
};

