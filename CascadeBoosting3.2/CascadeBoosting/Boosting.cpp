#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <direct.h>
#include "Boosting.h"


Boosting::Boosting(void)
{
	positive_features = NULL;
	negative_features = NULL;

	positive_weights = NULL;
	negative_weights = NULL;

	positive_scores = NULL;
	negative_scores = NULL;

	bin_mins = NULL;
	bin_maxs = NULL;
	bin_widths = NULL;

	feature_values = NULL;
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

	if (bin_mins != NULL)
	{
		delete []bin_mins;
		bin_mins = NULL;
	}
	if (bin_maxs != NULL)
	{
		delete []bin_maxs;
		bin_maxs = NULL;
	}
	if (bin_widths != NULL)
	{
		delete []bin_widths;
		bin_widths = NULL;
	}

	if (feature_values != NULL)
	{
		for (int i=0; i<total_feature_num; i++)
		{
			delete []feature_values[i];
		}
		delete []feature_values;
		feature_values = NULL;
	}
	return 1;
}


int Boosting::init(const TrainParamsT &params)
{
	clearUp();
	
	this->pt_params = (TrainParamsT *)&params;

	haar.init(params.template_w, params.template_h, params.feature_type, params.feature_abs);
	
	int max_pos_sample_num = params.positive_num;
	int max_neg_sample_num = max(params.min_negative_num, params.positive_num) + params.max_neg_per_image * 2;
	
	total_feature_num = haar.getFeatureNum();

	positive_features = new float[max_pos_sample_num * total_feature_num];
	negative_features = new float[max_neg_sample_num * total_feature_num];
	positive_weights = new double[max_pos_sample_num];
	negative_weights = new double[max_neg_sample_num];
	positive_scores = new double[max_pos_sample_num];
	negative_scores = new double[max_neg_sample_num];

	bin_num = params.bin_num;
	bin_mins = new int[total_feature_num];
	bin_maxs = new int[total_feature_num];
	bin_widths = new double[total_feature_num];

	int max_sample_num = max_pos_sample_num + max_neg_sample_num;
	feature_values = new int*[total_feature_num];
	for (int i=0; i<total_feature_num; i++)
	{
		feature_values[i] = new int[max_sample_num];
	}

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


int Boosting::trainNewStage(CascadeModelT &model)
{
	positive_num = pt_params->positive_num;
	negative_num = pt_params->negative_num;
	total_sample_num = positive_num + negative_num;

	printf("------------Extracting Haar Features...------------\n");
	extractFeatures(positive_num, pt_params->positive_data_path, positive_features);
	extractFeatures(negative_num, pt_params->negative_data_path, negative_features);

	packTrainingData(positive_num, positive_features, negative_num, negative_features);

	reweight(model);

	return 1;
}


int Boosting::trainWeakLearner(CascadeModelT &model)
{
	WeakLearner &weak_learner = model.pt_weak_learners[model.weak_learner_num];
	learnOneWeakLearner(weak_learner);
	updateWeights(weak_learner, iteration);
	iteration++;

	double thd = getStrongLearnerThd(model); 
	model.pt_weak_learners[model.weak_learner_num].info.classify_thd = thd;

	return 1;
}


void Boosting::packTrainingData(int positive_num, const float *positive_features, int negative_num, const float *negative_features)
{
	float *raw_values = new float[total_sample_num];

	for (int i=0; i<total_feature_num; i++)
	{
		printf("FeatureID = %d -- %d\r", total_feature_num, i+1);
		for (int j=0; j<positive_num; j++)
		{
			int idx = j * total_feature_num + i;
			raw_values[j] = positive_features[idx];
		}
		for (int j=0; j<negative_num; j++)
		{
			int idx = j * total_feature_num + i;
			raw_values[j+positive_num] = negative_features[idx];
		}
		discretization(raw_values, i);
	}
	delete []raw_values;
}


void Boosting::discretization(const float *raw_values, int feature_idx)
{
	if (isFeatureLearned(feature_idx) == 0)
	{
		double min_positive_value = 999999;
		double max_positive_value = -999999;

		for (int i=0; i<positive_num; i++)
		{
			if (raw_values[i] < min_positive_value) 
				min_positive_value = raw_values[i];

			if (raw_values[i] > max_positive_value)
				max_positive_value = raw_values[i];
		}				

		double min_negative_value = 999999;
		double max_negative_value = -999999;

		for (int i=positive_num; i<total_sample_num; i++)
		{
			if (raw_values[i] < min_negative_value) 
				min_negative_value = raw_values[i];

			if (raw_values[i] > max_negative_value) 
				max_negative_value = raw_values[i];
		}				

		if (max_positive_value > max_negative_value)
			bin_maxs[feature_idx] = ceil(max_negative_value);
		else
			bin_maxs[feature_idx] = ceil(max_positive_value);

		if (min_positive_value > min_negative_value) 
			bin_mins[feature_idx] = floor(min_positive_value);
		else
			bin_mins[feature_idx] = floor(min_negative_value);

		bin_widths[feature_idx] = (bin_maxs[feature_idx] - bin_mins[feature_idx]) / double(bin_num - 2);
	}

	double inv_width = 1.0 / double(bin_widths[feature_idx]);
	for (int i=0; i<total_sample_num; i++)		
	{
		if (raw_values[i] >= bin_maxs[feature_idx])
			feature_values[feature_idx][i] = bin_num - 1;
		else if (raw_values[i] <= bin_mins[feature_idx])
			feature_values[feature_idx][i] = 0;
		else
			feature_values[feature_idx][i] = (raw_values[i] - bin_mins[feature_idx]) * inv_width + 1;
	}
}


int Boosting::learnOneWeakLearner(WeakLearner &weak_learner)
{
	double min_BHC = 99999.0;
	int best_feature_idx;

	for (int i=0; i<total_feature_num; i++)//for all features
	{
		if (isFeatureLearned(i) == 1)
		{
			continue;
		}
		memset(positive_hist, 0, bin_num * sizeof(positive_hist[0]));
		memset(negative_hist, 0, bin_num * sizeof(negative_hist[0]));

		for (int j=0; j<positive_num; j++)
		{
			int bin_idx = feature_values[i][j];
			positive_hist[bin_idx] += positive_weights[j];
		}
		for (int j=0; j<negative_num; j++)
		{
			int bin_idx = feature_values[i][j+positive_num];
			negative_hist[bin_idx] += negative_weights[j];
		}

		double BHC = 0;
		for (int j=0; j<bin_num; j++)
		{
			BHC += sqrt(positive_hist[j] * negative_hist[j]);
		}

		if (BHC < min_BHC)
		{
			min_BHC = BHC;
			best_feature_idx = i;
		}
	}
	
	weak_learner.info.haar_info = haar.pt_feature_infos[best_feature_idx];
	weak_learner.info.bin_num = bin_num;
	weak_learner.info.bin_min = bin_mins[best_feature_idx];
	weak_learner.info.bin_max = bin_maxs[best_feature_idx];
	weak_learner.info.bin_width = bin_widths[best_feature_idx];

	memset(positive_hist, 0, bin_num * sizeof(positive_hist[0]));
	memset(negative_hist, 0, bin_num * sizeof(negative_hist[0]));

	for (int i=0; i<positive_num; i++)
	{
		int binIndex = feature_values[best_feature_idx][i];
		positive_hist[binIndex] += positive_weights[i];
	}
	for (int i=0; i<negative_num; i++)
	{
		int binIndex = feature_values[best_feature_idx][i+positive_num];
		negative_hist[binIndex] += negative_weights[i]; 
	}

	for (int i=0; i<bin_num; i++)
	{
		double part1 = (positive_hist[i] + 0.0001);
		double part2 = (negative_hist[i] + 0.0001);
		weak_learner.info.output[i] = 0.5 * log(part1 / part2);
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
		int bin_idx = feature_values[feature_idx][i];
		double value = weak_learner.info.output[bin_idx];
		positive_weights[i] *= exp(-value);
		weight_sum += positive_weights[i];
	}

	for (int i=0; i<negative_num; i++)
	{
		int bin_idx = feature_values[feature_idx][i+positive_num];
		double value = weak_learner.info.output[bin_idx];
		negative_weights[i] *= exp(value);
		weight_sum += negative_weights[i];
	}

	FILE *fp = fp = fopen(pt_params->positive_weight_path.c_str(), "wt");
	for (int i=0; i<positive_num; i++)
	{
		positive_weights[i] /= weight_sum;
		fprintf(fp, "%lf\n", positive_weights[i]);
	}
	fclose(fp);

	fp = fp = fopen(pt_params->negative_weight_path.c_str(), "wt");
	for (int i=0; i<negative_num; i++)
	{
		negative_weights[i] /= weight_sum;
		fprintf(fp, "%lf\n", negative_weights[i]);
	}
	fclose(fp);

	return 1;
}


double Boosting::getStrongLearnerThd(const CascadeModelT &model)
{
	memset(positive_scores, 0, positive_num * sizeof(positive_scores[0]));

	for (int i=0; i<positive_num; i++)
	{
		for (int j=0; j<iteration; j++)
		{
			int feature_idx = learned_feature_idx[j];
			int idx = feature_values[feature_idx][i];
			double score = model.pt_weak_learners[j].info.output[idx];
			positive_scores[i] += score;
		}
	}

	memset(negative_scores, 0, negative_num * sizeof(negative_scores[0]));

	for (int i=0; i<negative_num; i++)
	{
		for (int j=0; j<iteration; j++)
		{
			int feature_idx = learned_feature_idx[j];
			int idx = feature_values[feature_idx][i+positive_num];
			double score = model.pt_weak_learners[j].info.output[idx];
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


void Boosting::filterTrainingSamples(const CascadeModelT &model)
{
	filterPositiveSamples(model);
	filterNegativeSamples(model);
}


int Boosting::filterPositiveSamples(const CascadeModelT &model)
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


int Boosting::filterNegativeSamples(const CascadeModelT &model)
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


int Boosting::reweight(CascadeModelT &model)
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


int Boosting::extractFeatures(int sample_num, const string &data_path, float *pt_features)
{
	FILE *fp = fopen(data_path.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}
	for (int i=0; i<sample_num; i++)
	{
		printf("%d -- %d\r", sample_num, i+1);
		const float *pt_cur_feature = haar.extractBatchFeatures(fp);
		memcpy(pt_features + i * total_feature_num, pt_cur_feature, total_feature_num * sizeof(pt_features[0]));
	}
	fclose(fp);

	printf("\n");
	return 1;
}