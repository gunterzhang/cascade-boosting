#pragma once
#include "FernsFeature.h"

class FernFeatureHub
{
public:
	FernFeatureHub(void);
	~FernFeatureHub(void);

	int getFernsNum();

private:
	FernsFeature *pt_ferns;
	int fern_num;
};

