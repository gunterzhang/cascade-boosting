#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <direct.h>
#include "Boosting.h"


Boosting::Boosting(void)
{
	max_pos_sample_num = 0;
	max_neg_sample_num = 0;

	positive_features = NULL;
	negative_features = NULL;

	positive_weights = NULL;
	negative_weights = NULL;

	positive_scores = NULL;
	negative_scores = NULL;
}


Boosting::~Boosting(void)
{
	clearUp();
}


int Boosting::clearUp()
{
	if (positive_features != NULL)
	{
		delete []positive_features;
		positive_features = NULL;
	}
	if (negative_features = NULL)
	{
		delete []negative_features;
		negative_features = NULL;
	}

	if (positive_weights != NULL)
	{
		delete []positive_weights;
		positive_weights = NULL;
	}
	if (negative_weights != NULL)
	{
		delete []negative_weights;
		negative_weights = NULL;
	}

	if (positive_scores != NULL)
	{
		delete []positive_scores;
		positive_scores = NULL;
	}
	if (negative_scores != NULL)
	{
		delete []negative_scores;
		negative_scores = NULL;
	}

	return 1;
}


int Boosting::init(const TrainParamsT &params)
{
	clearUp();
	
	this->pt_params = (TrainParamsT *)&params;

	haar_hub.init(params.template_w, params.template_h, params.feature_type, params.feature_abs);
	total_feature_num = haar_hub.getFeatureNum();

	FILE *fp = fopen(pt_params->train_log_path.c_str(), "at");
	fprintf(fp, "feature num:%d\n", total_feature_num);
	fclose(fp);

	max_pos_sample_num = params.positive_num;
	max_neg_sample_num = max(params.min_negative_num, params.positive_num) + params.max_neg_per_image * 2;

	positive_features = new HaarFeatureValueT[max_pos_sample_num * total_feature_num];
	negative_features = new HaarFeatureValueT[max_neg_sample_num * total_feature_num];
	positive_weights = new double[max_pos_sample_num];
	negative_weights = new double[max_neg_sample_num];
	positive_scores = new double[max_pos_sample_num];
	negative_scores = new double[max_neg_sample_num];

	iteration = 0;
	return 1;
}


int Boosting::initWeights()
{
	double weight = 0.5 / positive_num;
	for (int i=0; i<positive_num; i++)
	{
		positive_weights[i] = weight;
	}

	weight = 0.5 / negative_num;
	for (int i=0; i<negative_num; i++)
	{
		negative_weights[i] = weight;
	}
	return 1;
}


int Boosting::prepareNewStage(PatternModel &model)
{
	positive_num = pt_params->positive_num;
	negative_num = pt_params->negative_num;

	printf("------------Extracting Haar Features...------------\n");
	haar_hub.extractFeatures(positive_num, pt_params->positive_data_path, positive_features);
	haar_hub.extractFeatures(negative_num, pt_params->negative_data_path, negative_features);

	haar_hub.train(positive_num, positive_features,
		           negative_num, negative_features,
				   pt_params->bin_num);

	reweight(model);

	return 1;
}


int Boosting::trainWeakLearner(PatternModel &model)
{
	WeakLearner &weak_learner = model.pt_weak_learners[model.weak_learner_num];
	learnOneWeakLearner(weak_learner);

	double thd = getStrongLearnerThd(model); 
	weak_learner.classify_thd = thd;

	updateWeights(weak_learner, iteration);
	model.weak_learner_num++;
	iteration++;
	return 1;
}


int Boosting::learnOneWeakLearner(WeakLearner &weak_learner)
{
	double min_BHC = 99999.0;
	int best_feature_idx;

	for (int i=0; i<total_feature_num; i++)
	{
		if (isFeatureLearned(i) == 1)
		{
			continue;
		}
		memset(positive_hist, 0, pt_params->bin_num * sizeof(positive_hist[0]));
		memset(negative_hist, 0, pt_params->bin_num * sizeof(negative_hist[0]));

		for (int j=0; j<positive_num; j++)
		{
			int idx = i * positive_num + j;
			int bin_idx = positive_features[idx].index;
			positive_hist[bin_idx] += positive_weights[j];
		}
		for (int j=0; j<negative_num; j++)
		{
			int idx = i * negative_num + j;
			int bin_idx = negative_features[idx].index;
			negative_hist[bin_idx] += negative_weights[j];
		}

		double BHC = 0;
		for (int j=0; j<pt_params->bin_num; j++)
		{
			BHC += sqrt(positive_hist[j] * negative_hist[j]);
		}

		if (BHC < min_BHC)
		{
			min_BHC = BHC;
			best_feature_idx = i;
		}
	}

	weak_learner.haar = haar_hub.pt_haars[best_feature_idx];

	memset(positive_hist, 0, pt_params->bin_num * sizeof(positive_hist[0]));
	memset(negative_hist, 0, pt_params->bin_num * sizeof(negative_hist[0]));

	for (int i=0; i<positive_num; i++)
	{
		int idx = best_feature_idx * positive_num + i;
		int binIndex = positive_features[idx].index;
		positive_hist[binIndex] += positive_weights[i];
	}
	for (int i=0; i<negative_num; i++)
	{
		int idx = best_feature_idx * negative_num + i;
		int binIndex = negative_features[idx].index;
		negative_hist[binIndex] += negative_weights[i]; 
	}

	for (int i=0; i<pt_params->bin_num; i++)
	{
		double part1 = (positive_hist[i] + 0.0001);
		double part2 = (negative_hist[i] + 0.0001);
		weak_learner.output[i] = 0.5 * log(part1 / part2);
	}
    learned_feature_idx[iteration] = best_feature_idx;

	return 1;
}


int Boosting::isFeatureLearned(int idx)
{
	for (int i=0; i<iteration; i++)
	{
		if (idx == learned_feature_idx[i])
		{
			return 1;
		}
	}
	return 0;	
}


int Boosting::updateWeights(const WeakLearner &weak_learner, int iteration_idx)
{
	int feature_idx = learned_feature_idx[iteration_idx];

	double weight_sum = 0;
	for (int i=0; i<positive_num; i++)
	{
		int idx = feature_idx * positive_num + i;
		int bin_idx = positive_features[idx].index;
		double value = weak_learner.output[bin_idx];
		positive_weights[i] *= exp(-value);
		weight_sum += positive_weights[i];
	}

	for (int i=0; i<negative_num; i++)
	{
		int idx = feature_idx * negative_num + i;
		int bin_idx = negative_features[idx].index;
		double value = weak_learner.output[bin_idx];
		negative_weights[i] *= exp(value);
		weight_sum += negative_weights[i];
	}
	return 1;
}


double Boosting::getStrongLearnerThd(const PatternModel &model)
{
	memset(positive_scores, 0, positive_num * sizeof(positive_scores[0]));

	for (int i=0; i<positive_num; i++)
	{
		for (int j=0; j<=iteration; j++)
		{
			int feature_idx = learned_feature_idx[j];
			int idx = feature_idx * positive_num + i;
			int binIndex = positive_features[idx].index;

			double score = model.pt_weak_learners[j].output[binIndex];
			positive_scores[i] += score;
		}
	}

	memset(negative_scores, 0, negative_num * sizeof(negative_scores[0]));

	for (int i=0; i<negative_num; i++)
	{
		for (int j=0; j<=iteration; j++)
		{
			int feature_idx = learned_feature_idx[j];
			int idx = feature_idx * negative_num + i;
			int binIndex = negative_features[idx].index;

			double score = model.pt_weak_learners[j].output[binIndex];
			negative_scores[i] += score;
		}
	}
	double thd = adjustThreshold(pt_params->detection_rates[0], pt_params->false_alarms[0]);
	printf("Detection Rate:%lf;    False alarm Rate:%lf\n", detection_rate, false_alarm);
	return thd;
}


void Boosting::getPerformance(double &detection_rate, double &false_alarm)
{
	detection_rate = this->detection_rate;
	false_alarm = this->false_alarm;
}


double Boosting::adjustThreshold(double detection_rate_aim, double false_alarm_aim)
{
	double min_thd = 99999;
	double max_thd = -99999;

	for (int i=0; i<positive_num; i++)
	{
		if (positive_scores[i] < min_thd)
		{
			min_thd = positive_scores[i];
		}
	}
	min_thd -= 1;

	for (int i=0; i<negative_num; i++)
	{
		if (negative_scores[i] > max_thd)
		{
			max_thd = negative_scores[i];
		}
	}
	max_thd += 1;

	detection_rate = 0.0;
	false_alarm = 1.0;
	double final_thd = 0;

	const int MAX_COUNT = 500;
	int count = 0;

	while (detection_rate < detection_rate_aim || false_alarm > false_alarm_aim)
	{
		double thd = (min_thd + max_thd) / 2;

		detection_rate = computeDetectionRate(thd);

		if (detection_rate - detection_rate_aim > 0.000001)
		{
			min_thd = thd;	
			final_thd = thd;
		}
		else
		{
			max_thd = thd;
		}

		count++;
		if (count > MAX_COUNT || abs(max_thd - min_thd) < 0.0001)
		{
			break;
		}
	}

	detection_rate = computeDetectionRate(final_thd);
	false_alarm = computeFalseAlarm(final_thd);

	final_thd -= 0.0001;

	return final_thd;
}


double Boosting::computeDetectionRate(double thd)
{
	int correct_num = 0;

	for (int i=0; i<positive_num; i++)
	{
		if (positive_scores[i] > thd)
		{
			correct_num += 1;			
		}
	}
	double ratio = double(correct_num) / double(positive_num);

	return ratio;
}


double Boosting::computeFalseAlarm(double thd)
{
	int correct_num = 0;

	for (int i=0; i<negative_num; i++)
	{
		if (negative_scores[i] < thd)
		{
			correct_num += 1;
		}
	}
	double ratio = 1 - double(correct_num) / double(negative_num);

	return ratio;
}


void Boosting::filterTrainingSamples(const PatternModel &model)
{
	if (model.stage_num > 0)
	{
		printf("--------------Filter training samples...--------------\n");
		filterPositiveSamples(model);
		filterNegativeSamples(model);
	}
}


int Boosting::filterPositiveSamples(const PatternModel &model)
{
	string temp_file_path;
	temp_file_path = pt_params->work_dir + "temp.intg";

	FILE *fp = fopen(pt_params->positive_data_path.c_str(), "rb");

	IntegralImage intg;
	int count = 0;
	for (int i=0; i<positive_num; i++)
	{
		intg.load(fp);
		if (positive_scores[i] > model.stage_thd[model.stage_num-1])
		{
			intg.save(temp_file_path);
			count++;
		}
	}
	pt_params->positive_num = count;
	fclose(fp);

	remove(pt_params->positive_data_path.c_str());
	rename(temp_file_path.c_str(), pt_params->positive_data_path.c_str());

	return 1;
}


int Boosting::filterNegativeSamples(const PatternModel &model)
{
	string temp_file_path;
	temp_file_path = pt_params->work_dir + "temp.intg";

	FILE *fp = fopen(pt_params->negative_data_path.c_str(), "rb");

	IntegralImage intg;
	int count = 0;
	for (int i=0; i<negative_num; i++)
	{
		intg.load(fp);
		if (negative_scores[i] > model.stage_thd[model.stage_num-1])
		{
			intg.save(temp_file_path.c_str());
			count++;
		}
	}
	pt_params->negative_num = count;
	fclose(fp);

	remove(pt_params->negative_data_path.c_str());
	rename(temp_file_path.c_str(), pt_params->negative_data_path.c_str());
	
	return 1;
}


int Boosting::reweight(PatternModel &model)
{
	initWeights();

	if (iteration == 0)
	{
		return 1;
	}

	for (int i=0; i<model.weak_learner_num; i++)
	{
		updateWeights(model.pt_weak_learners[i], i);
	}
	return 1;
}


int Boosting::getSampleNum(const string &path)
{
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}
	
	int count = 0;
	IntegralImage intg;

	while (true)
	{
		if (intg.load(fp) == 0)
		{
			break;
		}
		count++;
	}

	fclose(fp);
	return count;
}