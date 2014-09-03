#include "stdafx.h"
#include "WeakLearner.h"


WeakLearner::WeakLearner(void)
{
}


WeakLearner::~WeakLearner(void)
{
}


int WeakLearner::saveToFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "at");
	HaarFeatureInfoT &haar_info = info.haar_info;
	fprintf(fp, "%d %d %d %d %d %d %d %lf ", haar_info.type, 
		    haar_info.pos1.x, haar_info.pos1.y,
		    haar_info.pos2.x, haar_info.pos2.y,	
			haar_info.size.x, haar_info.size.y, 
			haar_info.inv_area);

	fprintf(fp, "%d %lf %lf %lf ", info.bin_num, info.bin_min, info.bin_max, info.bin_width);
	for (int i=0; i<info.bin_num; i++)
	{
		fprintf(fp, "%lf ", info.output[i]);
	}
	fprintf(fp, "%lf\n", info.classify_thd);
	
	fclose(fp);

	return 1;
}


double WeakLearner::test(float feature_value)
{
	int bin_index;

	if (feature_value < info.bin_min)
		bin_index = 0;
	else if (feature_value > info.bin_max)
		bin_index = info.bin_num - 1;
	else 
		bin_index = (feature_value - info.bin_min) / info.bin_width + 1;

	return info.output[bin_index];
}


double WeakLearner::test(const IntegralImage &intg, const SubwinInfoT &subwin)
{
	float value = HaarFeature::computeFeature((IntegralImage &)intg, subwin, info.haar_info);
	double result = test(value);

	return result;
}