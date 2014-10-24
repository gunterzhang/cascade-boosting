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


int PatternModel::init(const TrainParamsT *ptr_params)
{
	cleanUp();

	int max_wl_num = ptr_params->max_weak_learner_num;
	p_weak_learners = new WeakLearner[max_wl_num];

	feature_type = ptr_params->feature_type;
	if (feature_type == FeatureTypeE::FERN)
	{
		p_features = new FernFeature[max_wl_num];
		p_ft_param = new FernParamT;
		for (int i=0; i<max_wl_num; i++)
		{
			FernFeature *ptr_feature = (FernFeature *)p_features;
			p_weak_learners[i].setFeature(&(ptr_feature[i]));
		}
	}
	else if (feature_type == FeatureTypeE::HAAR)
	{
		p_features = new HaarFeature[max_wl_num];
		p_ft_param = new HaarParamT;
		for (int i=0; i<max_wl_num; i++)
		{
			HaarFeature *ptr_feature = (HaarFeature *)p_features;
			p_weak_learners[i].setFeature(&(ptr_feature[i]));
		}
	}

	FILE *fp = fopen(ptr_params->ft_config_path.c_str(), "rt");
	p_ft_param->initFromConfig(fp);
	fclose(fp);

	stage_num = 0;
	weak_learner_num = 0;

	return 1;
}


int PatternModel::loadFromModel(const string &file_path)
{
	cleanUp();
	
	FILE *fp = fopen(file_path.c_str(), "rt");
	fscanf(fp, "%d\n", &feature_type);
	
	if (feature_type == FeatureTypeE::FERN)
		p_ft_param = new FernParamT;
	else if (feature_type == FeatureTypeE::HAAR)
		p_ft_param = new HaarParamT;

	p_ft_param->loadFromModel(fp);
	fscanf(fp, "%d\n", &weak_learner_num);
	fscanf(fp, "%d\n", &stage_num);

	for (int i=0; i<stage_num; i++)
	{
		fscanf(fp, "%d %lf\n", &stage_idx[i], &stage_thd[i]);
	}

	p_weak_learners = new WeakLearner[weak_learner_num];

	if (feature_type == FeatureTypeE::FERN)
	{
		p_features = new FernFeature[weak_learner_num];
		for (int i=0; i<weak_learner_num; i++)
		{
			FernFeature *ptr_feature = (FernFeature *)p_features;
			p_weak_learners[i].setFeature(&(ptr_feature[i]));
			p_weak_learners[i].loadFromModel(fp);
		}
	}
	else if (feature_type == FeatureTypeE::HAAR)
	{
		p_features = new HaarFeature[weak_learner_num];
		for (int i=0; i<weak_learner_num; i++)
		{
			HaarFeature *ptr_feature = (HaarFeature *)p_features;
			p_weak_learners[i].setFeature(&(ptr_feature[i]));
			p_weak_learners[i].loadFromModel(fp);
		}
	}

	fclose(fp);
	return 1;
}


int PatternModel::saveToModel(const string &file_path)
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
		p_weak_learners[i].saveToModel(file_path);
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