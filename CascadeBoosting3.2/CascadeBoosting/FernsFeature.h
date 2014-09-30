#pragma once

#include "HaarFeature.h"

typedef struct
{
	int cell_num;
	const static int MAX_FERN_CELL_NUM = 10;
	HaarFeatureInfoT haars[MAX_FERN_CELL_NUM];
	int bin_num;
	int bin_idx;
}FernsFeatureInfoT;


class FernsFeature
{
public:
	FernsFeature(void);
	~FernsFeature(void);

	int computeFeature();

private:

};

