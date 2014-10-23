#pragma once
#include "TrainParams.h"
#include "WeakLearner.h"
#include "FernFeatureHub.h"
#include "HaarFeatureHub.h"
#include "PatternModel.h"


class Boosting
{
public:
	Boosting(void);
	~Boosting(void);

	int init(TrainParamsT &train_params);
	int prepareNewStage(PatternModel &model);
	int trainWeakLearner(PatternModel &model);
	void getPerformance(double &detection_rate, double &false_alarm);
	void filterTrainingSamples(const PatternModel &model);

private:
	int cleanUp();
	int reweight(PatternModel &model);
	int learnOneWeakLearner(WeakLearner &weak_learner);
	int updateWeights(const WeakLearner &weak_learner, int iteration_idx);
	double getStrongLearnerThd(const PatternModel &model);
	double adjustThreshold(double detection_rate_aim, double false_alarm_aim);
	double computeDetectionRate(double thd);
	double computeFalseAlarm(double thd);
	int filterPositiveSamples(const PatternModel &model);
	int filterNegativeSamples(const PatternModel &model);
	int isFeatureLearned(int idx);

private:
	TrainParamsT *ptr_params;
	PatternModel *ptr_model;

	FeatureHub *p_ft_hub;

	int positive_num;
	int negative_num;
	int max_pos_sample_num;
	int max_neg_sample_num;

	double *positive_weights;
	double *negative_weights;
	double *positive_scores;
	double *negative_scores;

	int feature_num;
	int bin_num;

	int learned_feature_idx[MAX_ROUND_NUM];

	double positive_hist[MAX_BIN_NUM];
	double negative_hist[MAX_BIN_NUM];

	int iteration;
	double detection_rate;
	double false_alarm;
};

