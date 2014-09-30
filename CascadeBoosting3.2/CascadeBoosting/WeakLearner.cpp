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

	haar.saveToFile(fp);

	for (int i=0; i<haar.info.bin_num; i++)
	{
		fprintf(fp, "%lf ", output[i]);
	}
	fprintf(fp, "%lf\n", classify_thd);
	
	fclose(fp);

	return 1;
}

int WeakLearner::loadFromFile(const FILE *fp, int template_w, int template_h)
{
	haar.loadFromFile(fp, template_w, template_h);

	for (int i=0; i<haar.info.bin_num; i++)
	{
		fscanf((FILE *)fp, "%lf ", &(output[i]));
	}
	fscanf((FILE*)fp, "%lf\n", &classify_thd);

	return 1;
}


double WeakLearner::test(const IntegralImage &intg, const SubwinInfoT &subwin)
{
	int bin_index = haar.computeFeatureIndex(intg, subwin);

	return output[bin_index];
}