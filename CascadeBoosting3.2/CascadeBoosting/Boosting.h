#pragma once
#include "TrainParams.h"
#include "WeakLearner.h"
#include "HaarFeature.h"

typedef struct
{
	int stage_num;
	int stage_idx[MAX_CASCADE_STAGE_NUM];
	double stage_thd[MAX_CASCADE_STAGE_NUM];

	int feature_type;
	int weak_learner_num;
	WeakLearner *pt_weak_learners;

	int template_w;
	int template_h;
}CascadeModelT;


class Boosting
{
public:
	Boosting(void);
	~Boosting(void);

	int init(int w, int h, int type);
	int init(const TrainParamsT &train_params);
	int trainNewStage(CascadeModelT &model);
	int trainWeakLearner(CascadeModelT &model);
	void getPerformance(double &detection_rate, double &false_alarm);
	double getStrongLearnerThd(const CascadeModelT &model);
	void filterTrainingSamples(const CascadeModelT &model);

private:
	int clearUp();
	int extractFeatures(int sample_num, const string &data_path, float *pt_features);
	int reweight(CascadeModelT &model);
	int initWeights();
	void packTrainingData(int positive_num, const float *positive_features, int negative_num, const float *negative_features);
	void discretization(const float *raw_values, int feature_idx);
	int learnOneWeakLearner(WeakLearner &weak_learner);
	int updateWeights(const WeakLearner &weak_learner, int iteration_idx);
	double adjustThreshold(double detection_rate_aim, double false_alarm_aim);
	double computeDetectionRate(double thd);
	double computeFalseAlarm(double thd);
	int filterPositiveSamples(const CascadeModelT &model);
	int filterNegativeSamples(const CascadeModelT &model);
	int isFeatureLearned(int idx);

private:
	TrainParamsT *pt_params;

	HaarFeature haar;

	int total_sample_num;
	int positive_num;
	int negative_num;

	float *positive_features;
	float *negative_features;

	double *positive_weights;
	double *negative_weights;

	double *positive_scores;
	double *negative_scores;

	int total_feature_num;

	int bin_num;
	int *bin_mins;
	int *bin_maxs;
	double *bin_widths;

	int **feature_values;

	int learned_feature_idx[MAX_ROUND_NUM];

	double positive_hist[MAX_BIN_NUM];
	double negative_hist[MAX_BIN_NUM];

	int iteration;
	double detection_rate;
	double false_alarm;
};

