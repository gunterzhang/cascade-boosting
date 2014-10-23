#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <direct.h>
#include "Boosting.h"


Boosting::Boosting(void)
{
	max_pos_sample_num = 0;
	max_neg_sample_num = 0;

	positive_weights = NULL;
	negative_weights = NULL;

	positive_scores = NULL;
	negative_scores = NULL;

	p_ft_hub = NULL;
}


Boosting::~Boosting(void)
{
	cleanUp();
}


int Boosting::cleanUp()
{
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
	if (p_ft_hub != NULL)
	{
		delete p_ft_hub;
		p_ft_hub = NULL;
	}
	return 1;
}


int Boosting::init(TrainParamsT &params)
{
	cleanUp();
	
	ptr_params = (TrainParamsT *)&params;
	if (params.feature_type == FeatureTypeE::FERN)
		p_ft_hub = new FernFeatureHub;
	else if (params.feature_type == FeatureTypeE::HAAR)
		p_ft_hub = new HaarFeatureHub;
	else 
		return 0;

	int rst = p_ft_hub->initFromConfig(params.ft_config_path);
	if (rst <= 0)
	{
		return 0;
	}

	feature_num = p_ft_hub->getFeatureNum();
	bin_num = p_ft_hub->getBinNum();

	FILE *fp = fopen(ptr_params->train_log_path.c_str(), "at");
	fprintf(fp, "feature num:%d\n", feature_num);
	fclose(fp);

	max_pos_sample_num = params.positive_num;
	max_neg_sample_num = max(params.min_negative_num, params.positive_num) + params.max_neg_per_image * 2;

	p_ft_hub->newTrainingMem(max_pos_sample_num, max_neg_sample_num);

	positive_weights = new double[max_pos_sample_num];
	negative_weights = new double[max_neg_sample_num];
	positive_scores = new double[max_pos_sample_num];
	negative_scores = new double[max_neg_sample_num];

	iteration = 0;
	return 1;
}



int Boosting::prepareNewStage(PatternModel &model)
{
	positive_num = ptr_params->positive_num;
	negative_num = ptr_params->negative_num;

	printf("------------Extracting Haar Features...------------\n");
	p_ft_hub->train(positive_num, ptr_params->positive_data_path, negative_num, ptr_params->negative_data_path);

	reweight(model);

	return 1;
}


int Boosting::trainWeakLearner(PatternModel &model)
{
	WeakLearner &weak_learner = model.p_weak_learners[model.weak_learner_num];
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

	for (int i=0; i<feature_num; i++)
	{
		if (isFeatureLearned(i) == 1)
		{
			continue;
		}
		memset(positive_hist, 0, bin_num * sizeof(positive_hist[0]));
		memset(negative_hist, 0, bin_num * sizeof(negative_hist[0]));

		for (int j=0; j<positive_num; j++)
		{
			int bin_idx = p_ft_hub->getPosFeatureIdx(j, i);
			positive_hist[bin_idx] += positive_weights[j];
		}
		for (int j=0; j<negative_num; j++)
		{
			int bin_idx = p_ft_hub->getNegFeatureIdx(j, i);
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

	weak_learner.setFeature(p_ft_hub->getFeature(best_feature_idx));

	memset(positive_hist, 0, bin_num * sizeof(positive_hist[0]));
	memset(negative_hist, 0, bin_num * sizeof(negative_hist[0]));

	for (int i=0; i<positive_num; i++)
	{
		int binIndex = p_ft_hub->getPosFeatureIdx(i, best_feature_idx);
		positive_hist[binIndex] += positive_weights[i];
	}
	for (int i=0; i<negative_num; i++)
	{
		int binIndex = p_ft_hub->getNegFeatureIdx(i, best_feature_idx); 
		negative_hist[binIndex] += negative_weights[i]; 
	}

	for (int i=0; i<bin_num; i++)
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
		int bin_idx = p_ft_hub->getPosFeatureIdx(i, feature_idx);
		double value = weak_learner.output[bin_idx];
		positive_weights[i] *= exp(-value);
		weight_sum += positive_weights[i];
	}
	for (int i=0; i<negative_num; i++)
	{
		int bin_idx = p_ft_hub->getNegFeatureIdx(i, feature_idx);
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
			int binIndex = p_ft_hub->getPosFeatureIdx(i, feature_idx);
			double score = model.p_weak_learners[j].output[binIndex];
			positive_scores[i] += score;
		}
	}

	memset(negative_scores, 0, negative_num * sizeof(negative_scores[0]));

	for (int i=0; i<negative_num; i++)
	{
		for (int j=0; j<=iteration; j++)
		{
			int feature_idx = learned_feature_idx[j];
			int binIndex = p_ft_hub->getNegFeatureIdx(i, feature_idx);
			double score = model.p_weak_learners[j].output[binIndex];
			negative_scores[i] += score;
		}
	}
	double thd = adjustThreshold(ptr_params->detection_rates[0], ptr_params->false_alarms[0]);
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
	temp_file_path = ptr_params->work_dir + "temp.intg";

	FILE *fp = fopen(ptr_params->positive_data_path.c_str(), "rb");

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
	ptr_params->positive_num = count;
	fclose(fp);

	remove(ptr_params->positive_data_path.c_str());
	rename(temp_file_path.c_str(), ptr_params->positive_data_path.c_str());

	return 1;
}


int Boosting::filterNegativeSamples(const PatternModel &model)
{
	string temp_file_path;
	temp_file_path = ptr_params->work_dir + "temp.intg";

	FILE *fp = fopen(ptr_params->negative_data_path.c_str(), "rb");

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
	ptr_params->negative_num = count;
	fclose(fp);

	remove(ptr_params->negative_data_path.c_str());
	rename(temp_file_path.c_str(), ptr_params->negative_data_path.c_str());
	
	return 1;
}


int Boosting::reweight(PatternModel &model)
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

	for (int i=0; i<model.weak_learner_num; i++)
	{
		updateWeights(model.p_weak_learners[i], i);
	}
	return 1;
}
