#include "stdafx.h"
#include "PatternModel.h"


PatternModel::PatternModel(void)
{
	p_ft_param = NULL;
	p_features = NULL;
	p_weak_learners = NULL;
}


PatternModel::~PatternModel(void)
{
	cleanUp();
}


void PatternModel::cleanUp()
{
	if (p_ft_param == NULL)
	{
		delete p_ft_param;
		p_ft_param = NULL;
	}
	if (p_features != NULL)
	{
		delete []p_features;
		p_features = NULL;
	}
	if (p_weak_learners != NULL)
	{
		delete []p_weak_learners;
		p_weak_learners = NULL;
	}
}


int PatternModel::init(int max_wl_num, const FeatureParamT *ptr_param, FeatureTypeE type)
{
	cleanUp();
	p_weak_learners = new WeakLearner[max_wl_num];

	feature_type = type;
	if (type == FeatureTypeE::FERN)
	{
		p_features = new FernFeature[max_wl_num];
		p_ft_param = new FernParamT;
		(*(FernParamT *)p_ft_param) = *((FernParamT *)ptr_param);
	}
	else if (type == FeatureTypeE::HAAR)
	{
		p_features = new HaarFeature[max_wl_num];
		p_ft_param = new HaarParamT;
		(*(HaarParamT *)p_ft_param) = *((HaarParamT *)ptr_param);
	}

	for (int i=0; i<max_wl_num; i++)
	{
		p_weak_learners->init(p_features);
	}

	stage_num = 0;
	weak_learner_num = 0;

	return 1;
}


int PatternModel::loadFromFile(const string &file_path)
{
	cleanUp();
	
	FILE *fp = fopen(file_path.c_str(), "rt");
	fscanf(fp, "%d\n", &feature_type);
	
	if (feature_type == FeatureTypeE::FERN)
	{
		p_features = new FernFeature[weak_learner_num];
		p_ft_param = new FernParamT;
	}
	else if (feature_type == FeatureTypeE::HAAR)
	{
		p_features = new HaarFeature[weak_learner_num];
		p_ft_param = new HaarParamT;
	}
	p_ft_param->loadFromModel(fp);
	fscanf(fp, "%d\n", &weak_learner_num);
	fscanf(fp, "%d\n", &stage_num);

	if (feature_type == FeatureTypeE::FERN)
		p_features = new FernFeature[weak_learner_num];
	else if (feature_type == FeatureTypeE::HAAR)
		p_features = new HaarFeature[weak_learner_num];

	for (int i=0; i<stage_num; i++)
	{
		fscanf(fp, "%d %lf\n", &stage_idx[i], &stage_thd[i]);
	}

	for (int i=0; i<weak_learner_num; i++)
	{
		p_weak_learners->init(p_features);
		p_weak_learners[i].loadFromFile(fp, *(p_ft_param));
	}

	fclose(fp);
	return 1;
}


int PatternModel::saveToFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "wt");
	fprintf(fp, "%d\n", feature_type);
	p_ft_param->saveToModel(fp);
	fprintf(fp, "%d\n", weak_learner_num);
	fprintf(fp, "%d\n", stage_num);

	for (int i=0; i<stage_num; i++)
	{
		fprintf(fp, "%d %lf\n", stage_idx[i], stage_thd[i]);
	}
	fclose(fp);

	for (int i=0; i<weak_learner_num; i++)
	{
		p_weak_learners[i].saveToFile(file_path);
	}
	return 1;
}


int PatternModel::addNewStage()
{
	stage_idx[stage_num] = weak_learner_num - 1;
	double thd = p_weak_learners[weak_learner_num - 1].classify_thd;
	stage_thd[stage_num] = thd;
	stage_num++;
	return 1;
}