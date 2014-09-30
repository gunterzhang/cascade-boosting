#pragma once
#include "TrainParams.h"
#include "WeakLearner.h"


class PatternModel
{
public:
	PatternModel(void);
	~PatternModel(void);

	int init(int max_wl_num, int feature_type, int template_w, int template_h);
	int loadFromFile(const string &file_path);
	int saveToFile(const string &file_path);
	int addNewStage();

private:
	void clearUp();

public:
	int feature_type;
	int template_w;
	int template_h;

	int stage_num;
	int stage_idx[MAX_CASCADE_STAGE_NUM];
	double stage_thd[MAX_CASCADE_STAGE_NUM];

	int weak_learner_num;
	WeakLearner *pt_weak_learners;
};

