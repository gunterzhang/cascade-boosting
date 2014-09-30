#include "stdafx.h"
#include "PatternModel.h"


PatternModel::PatternModel(void)
{
	pt_weak_learners = NULL;
}


PatternModel::~PatternModel(void)
{
	clearUp();
}


int PatternModel::init(int max_wl_num, int feature_type, int template_w, int template_h)
{
	clearUp();
	pt_weak_learners = new WeakLearner[max_wl_num];

	stage_num = 0;
	weak_learner_num = 0;
	this->feature_type = feature_type;
	this->template_w = template_w;
	this->template_h = template_h;

	return 1;
}


void PatternModel::clearUp()
{
	if (pt_weak_learners != NULL)
	{
		delete []pt_weak_learners;
	}
}


int PatternModel::loadFromFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "rt");

	fscanf(fp, "%d %d\n", &template_w, &template_h);
	fscanf(fp, "%d\n", &feature_type);
	fscanf(fp, "%d\n", &weak_learner_num);
	fscanf(fp, "%d\n", &stage_num);

	for (int i=0; i<stage_num; i++)
	{
		fscanf(fp, "%d %lf\n", &stage_idx[i], &stage_thd[i]);
	}

	for (int i=0; i<weak_learner_num; i++)
	{
		pt_weak_learners[i].loadFromFile(fp, template_w, template_h);
	}

	fclose(fp);

	return 1;
}


int PatternModel::addNewStage()
{
	stage_idx[stage_num] = weak_learner_num - 1;
	double thd = pt_weak_learners[weak_learner_num - 1].classify_thd;
	stage_thd[stage_num] = thd;
	stage_num++;

	return 1;
}


int PatternModel::saveToFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "wt");
	fprintf(fp, "%d %d\n", template_w, template_h);
	fprintf(fp, "%d\n", feature_type);
	fprintf(fp, "%d\n", weak_learner_num);
	fprintf(fp, "%d\n", stage_num);

	for (int i=0; i<stage_num; i++)
	{
		fprintf(fp, "%d %lf\n", stage_idx[i], stage_thd[i]);
	}
	fclose(fp);

	for (int i=0; i<weak_learner_num; i++)
	{
		pt_weak_learners[i].saveToFile(file_path);
	}

	return 1;
}