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
	int extractFeatures(int sample_num, const string &data_path, float *pt_features);
	int reweight(PatternModel &model);
	void packTrainingData(int positive_num, const float *positive_features, int negative_num, const float *negative_features);
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

	int positive_num;
	int negative_num;

	float *positive_features;
	float *negative_features;

	double *positive_weights;
	double *negative_weights;

	double *positive_scores;
	double *negative_scores;

	int total_feature_num;

	int **feature_values;

	int learned_feature_idx[MAX_ROUND_NUM];

	double positive_hist[MAX_BIN_NUM];
	double negative_hist[MAX_BIN_NUM];

	int iteration;
	double detection_rate;
	double false_alarm;
};

