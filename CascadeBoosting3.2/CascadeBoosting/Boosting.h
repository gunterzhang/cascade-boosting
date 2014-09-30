#pragma once
#include "TrainParams.h"
#include "WeakLearner.h"
#include "HaarFeatureHub.h"
#include "PatternModel.h"

class Boosting
{
public:
	Boosting(void);
	~Boosting(void);

	int init(const TrainParamsT &train_params);
	int prepareNewStage(PatternModel &model);
	int trainWeakLearner(PatternModel &model);
	void getPerformance(double &detection_rate, double &false_alarm);
	double getStrongLearnerThd(const PatternModel &model);
	void filterTrainingSamples(const PatternModel &model);
	int getSampleNum(const string &path);

private:
	int clearUp();
	int initWeights();
	int extractFeatures(int sample_num, const string &data_path, HaarFeatureValueT **pt_features);
	int reweight(PatternModel &model);
	void packFeatures();
	void discretization(int feature_idx);
	int learnOneWeakLearner(WeakLearner &weak_learner);
	int updateWeights(const WeakLearner &weak_learner, int iteration_idx);
	double adjustThreshold(double detection_rate_aim, double false_alarm_aim);
	double computeDetectionRate(double thd);
	double computeFalseAlarm(double thd);
	int filterPositiveSamples(const PatternModel &model);
	int filterNegativeSamples(const PatternModel &model);
	int isFeatureLearned(int idx);

private:
	TrainParamsT *pt_params;

	HaarFeatureHub haar_hub;

	int max_pos_sample_num;
	int max_neg_sample_num;

	int positive_num;
	int negative_num;

	HaarFeatureValueT **positive_features;
	HaarFeatureValueT **negative_features;

	double *positive_weights;
	double *negative_weights;

	double *positive_scores;
	double *negative_scores;

	int total_feature_num;

	int learned_feature_idx[MAX_ROUND_NUM];

	double positive_hist[MAX_BIN_NUM];
	double negative_hist[MAX_BIN_NUM];

	int iteration;
	double detection_rate;
	double false_alarm;
};

