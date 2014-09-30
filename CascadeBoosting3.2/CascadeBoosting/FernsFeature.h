#pragma once
#include "HaarFeature.h"

const int MAX_FERN_CELL_NUM = 10;

typedef struct
{
	HaarFeatureValueT haar_value[MAX_FERN_CELL_NUM];
	int index;
}FernFeatureValueT;


class FernsFeature
{
public:
	FernsFeature(void);
	~FernsFeature(void);

	int init();
	int computeFeature(const IntegralImage &intg, const SubwinInfoT &subwin);

private:
	int cell_num;
	HaarFeatureInfoT haars[MAX_FERN_CELL_NUM];
};

