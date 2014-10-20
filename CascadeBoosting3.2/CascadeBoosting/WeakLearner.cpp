#include "stdafx.h"
#include "WeakLearner.h"


WeakLearner::WeakLearner(void)
{
}


WeakLearner::~WeakLearner(void)
{
}


int WeakLearner::init(Feature *ptr_feature)
{
	this->ptr_feature = ptr_feature;
	return 1;
}


int WeakLearner::saveToFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "at");
	ptr_feature->saveToFile(fp);

	for (int i=0; i<ptr_feature->getBinnum(); i++)
	{
		fprintf(fp, "%lf ", output[i]);
	}
	fprintf(fp, "%lf\n", classify_thd);
	
	fclose(fp);

	return 1;
}

int WeakLearner::loadFromFile(const FILE *fp, const FeatureParamT &param)
{
	ptr_feature->loadFromFile(fp, param);

	for (int i=0; i<ptr_feature->getBinnum(); i++)
	{
		fscanf((FILE *)fp, "%lf ", &(output[i]));
	}
	fscanf((FILE*)fp, "%lf\n", &classify_thd);

	return 1;
}


double WeakLearner::test(const IntegralImage &intg, const SubwinInfoT &subwin)
{
	int bin_index = ptr_feature->computeFeatureIndex(intg, subwin);
	return output[bin_index];
}