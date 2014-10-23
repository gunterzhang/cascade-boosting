#include "stdafx.h"
#include "WeakLearner.h"


WeakLearner::WeakLearner(void)
{
}


WeakLearner::~WeakLearner(void)
{
}


int WeakLearner::setFeature(Feature *ptr_feature)
{
	this->ptr_feature = ptr_feature;
	return 1;
}


int WeakLearner::saveToModel(const string &file_path)
{
	ptr_feature->saveToModel(file_path);

	FILE *fp = fopen(file_path.c_str(), "at");

	for (int i=0; i<ptr_feature->getBinNum(); i++)
	{
		fprintf(fp, "%lf ", output[i]);
	}
	fprintf(fp, "%lf\n", classify_thd);
	
	fclose(fp);

	return 1;
}


int WeakLearner::loadFromModel(const FILE *fp)
{
	ptr_feature->loadFromModel((FILE *)fp);

	for (int i=0; i<ptr_feature->getBinNum(); i++)
	{
		fscanf((FILE *)fp, "%lf ", &(output[i]));
	}
	fscanf((FILE*)fp, "%lf\n", &classify_thd);

	return 1;
}


double WeakLearner::test(const IntegralImage &intg, const SubwinInfoT &subwin)
{
	int bin_index = ptr_feature->computeFeature(intg, subwin);
	return output[bin_index];
}


int WeakLearner::getBinNum()
{
	return ptr_feature->getBinNum();
}