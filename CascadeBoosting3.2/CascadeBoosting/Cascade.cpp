#include "stdafx.h"
#include <io.h>
#include <afx.h>
#include <direct.h>
#include "Cascade.h"


Cascade::Cascade(void)
{
	model.pt_weak_learners = NULL;
}


Cascade::~Cascade(void)
{
	if (model.pt_weak_learners != NULL)
	{
		delete []model.pt_weak_learners;
	}
}


int Cascade::train(TrainParamsT &params)
{
	int rst = init(params);
	if (rst <= 0)
	{
		return rst;
	}

	extractPositiveSamples();
	boosting.init(params);

	int is_new_stage = 1;
	while (model.stage_num < params.stage_num && model.weak_learner_num < params.max_weak_learner_num)
	{
		if (is_new_stage > 0)
		{
			extractNegativeSamples();
			if (params.negative_num < params.positive_num)
			{
				break;
			}
			boosting.trainNewStage(model);
			if (model.stage_num >= 1)
			{
				boosting.getStrongLearnerThd(model);
			}
		}

		printf( "===========  Training stage:%d, round:%d  ===========\n", 
			    (model.stage_num + 1), (model.weak_learner_num + 1) );

		boosting.trainWeakLearner(model);
		model.weak_learner_num++;

		double detection_rate;
		double false_alarm;
		boosting.getPerformance(detection_rate, false_alarm);
	
		if (isStageLearned(detection_rate, false_alarm) == 0) //stong learner not ready
		{
			is_new_stage = 0;
			continue;
		}

		//new stage learned
		is_new_stage = 1;
		model.stage_idx[model.stage_num] = model.weak_learner_num - 1;
		double thd = model.pt_weak_learners[model.weak_learner_num - 1].info.classify_thd;
		model.stage_thd[model.stage_num] = thd;
		model.stage_num++;
		saveTrainedStages();

		if (model.stage_num == params.stage_num)
		{
			break;
		}

		printf("--------------Filter training samples...--------------\n");
		boosting.filterTrainingSamples(model);
	}
	return 1;
}


int Cascade::init(TrainParamsT &params)
{
	pt_params = &params;

	//load config file
	int rst = loadConfig(params);
	if (rst <= 0)
	{
		return rst;
	}

	params.model_path = params.work_dir + "model.txt";
	params.stage_path = params.work_dir + params.class_label + ".model";

	params.positive_data_path = params.work_dir + params.class_label + ".intg";
	params.negative_data_path = params.work_dir + "neg.intg";

	params.positive_feature_path = params.work_dir + "positive.ft";
	params.negative_feature_path = params.work_dir + "negative.ft";

	params.positive_weight_path = params.work_dir + "positive_weight.txt";
	params.negative_weight_path = params.work_dir + "negative_weight.txt";

	params.train_log_path = params.work_dir + "log.txt";

	params.extracted_negative_dir = params.work_dir + "neg_icons\\";
	if (access(params.extracted_negative_dir.c_str(), 0) == -1)
	{
		_mkdir(params.extracted_negative_dir.c_str());
	}
	
	rst = neg_extractor.init(params);
	if (rst <= 0)
	{
		return rst;
	}

	if (model.pt_weak_learners == NULL)
	{
		model.pt_weak_learners = new WeakLearner[params.max_weak_learner_num];
	}

	initModel();
	saveTrainedStages();
	return 1;
}


int Cascade::initModel()
{
	FILE *fp = fopen(pt_params->stage_path.c_str(), "rt");
	if (fp == NULL)
	{
		model.stage_num = 0;
		model.weak_learner_num = 0;
		model.feature_type = pt_params->feature_type;
		model.template_w = pt_params->template_w;
		model.template_h = pt_params->template_h;
		return 0;
	}

	fscanf(fp, "%d %d\n", &model.template_w, &model.template_h);
	fscanf(fp, "%d\n", &model.feature_type);
	fscanf(fp, "%d\n", &model.weak_learner_num);
	model.pt_weak_learners = new WeakLearner[model.weak_learner_num];

	fscanf(fp, "%d\n", &model.stage_num);
	for (int i=0; i<model.stage_num; i++)
	{
		fscanf(fp, "%d %lf\n", &model.stage_idx[i], &model.stage_thd[i]);
	}

	for (int i=0; i<model.weak_learner_num; i++)
	{
		WeakLearnerInfoT &info = model.pt_weak_learners[i].info;
		HaarFeatureInfoT &haar_info = info.haar_info;
		fscanf(fp, "%d %d %d %d %d %d %d %d %lf", 
			   &haar_info.type, &haar_info.is_abs,
			   &haar_info.pos1.x, &haar_info.pos1.y,
			   &haar_info.pos2.x, &haar_info.pos2.y,
			   &haar_info.size.x, &haar_info.size.y, 
			   &haar_info.inv_area);

		haar_info.tpl_size.x = model.template_w;
		haar_info.tpl_size.y = model.template_h;
		fscanf(fp, "%d %lf %lf %lf ", &info.bin_num, &info.bin_min, &info.bin_max, &info.bin_width);
		for (int i=0; i<info.bin_num; i++)
		{
			fscanf(fp, "%lf ", &info.output[i]);
		}
		fscanf(fp, "%lf\n", &info.classify_thd);
	}
	fclose(fp);

	return 1;
}


int Cascade::loadConfig(TrainParamsT &params)
{
	FILE *fp = fopen(params.share_config_path.c_str(), "rt");
	if (fp == NULL)
	{
		printf("Can't open configure file: %s\n", params.config_path.c_str());
		return -1;
	}

	while (true)
	{
		char tmp_str[100];
		int count = fscanf(fp, "%s", tmp_str);
		if (count <= 0)
		{
			break;
		}

		if (strcmp(tmp_str, "StageNum:") == 0)
		{
			fscanf(fp, "%d", &params.stage_num);
			printf("StageNum:        %d\n", params.stage_num);
		}
		else if (strcmp(tmp_str, "BinNum:") == 0)
		{
			fscanf(fp, "%d", &params.bin_num);
			printf("BinNum:          %d\n", params.bin_num);
		}
		else if (strcmp(tmp_str, "MinNegNum:") == 0)
		{
			fscanf(fp, "%d", &params.min_negative_num);
			printf("MinNegNum:       %d\n", params.min_negative_num);
		}
		else if (strcmp(tmp_str, "MaxLearnerNum:") == 0)
		{
			fscanf(fp, "%d", &params.max_weak_learner_num);
			printf("MaxLearnerNum:   %d\n", params.max_weak_learner_num);
		}
		else if (strcmp(tmp_str, "MaxNegPerImage:") == 0)
		{
			fscanf(fp, "%d", &params.max_neg_per_image);
			printf("MaxNegPerImage:  %d\n", params.max_neg_per_image);
		}
		else if (strcmp(tmp_str, "DetectionRate:") == 0)
		{
			fscanf(fp, "%lf", &params.detection_rates[0]);
			printf("DetectionRate:   %lf\n", params.detection_rates[0]);
		}
		else if (strcmp(tmp_str, "FalseAlarm:") == 0)
		{
			fscanf(fp, "%lf", &params.false_alarms[0]);
			printf("FalseAlarm:      %lf\n", params.false_alarms[0]);
		}
		else if (strcmp(tmp_str, "TemplateWidth:") == 0)
		{
			fscanf(fp, "%d", &params.template_w);
			printf("TemplateWidth:   %d\n", params.template_w);
		}
		else if (strcmp(tmp_str, "TemplateHeight:") == 0)
		{
			fscanf(fp, "%d", &params.template_h);
			printf("TemplateHeight:  %d\n", params.template_h);
		}
		else if (strcmp(tmp_str, "FeatureType:") == 0)
		{
			fscanf(fp, "%d", &params.feature_type);
			printf("FeatureType:     %d\n", params.feature_type);
		}
		else if (strcmp(tmp_str, "FeatureAbs:") == 0)
		{
			fscanf(fp, "%d", &params.feature_abs);
			printf("FeatureAbs:      %d\n", params.feature_abs);
		}
		else if (strcmp(tmp_str, "ExtractPositive:") == 0)
		{
			fscanf(fp, "%d", &params.is_extract_positive);
			printf("ExtractPositive: %d\n", params.is_extract_positive);
		}
		else if (strcmp(tmp_str, "PositivePath:") == 0)
		{
			fscanf(fp, "%s", tmp_str);
			params.positive_pool_dir = tmp_str;
			params.positive_pool_dir += "\\" + params.class_label + "\\";
			printf("PositivePath:    %s\n", params.positive_pool_dir.c_str());
		}
		else if(strcmp(tmp_str, "SaveFeatures:") == 0)
		{
			fscanf(fp, "%d", &params.is_save_feature2file);
			printf("SaveFeatures:    %d\n", params.is_save_feature2file);
		}
		else if(strcmp(tmp_str, "Neg_Start_X_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_start_x_r);
			printf("Neg_Start_X_R:   %lf\n", params.neg_start_x_r);
		}
		else if(strcmp(tmp_str, "Neg_End_X_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_end_x_r);
			printf("Neg_End_X_R:     %lf\n", params.neg_end_x_r);
		}
		else if(strcmp(tmp_str, "Neg_Start_Y_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_start_y_r);
			printf("Neg_Start_Y_R:   %lf\n", params.neg_start_y_r);
		}
		else if(strcmp(tmp_str, "Neg_End_Y_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_end_y_r);
			printf("Neg_End_Y_R:     %lf\n", params.neg_end_y_r);
		}
		else if(strcmp(tmp_str, "Neg_Min_W_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_min_w_r);
			printf("Neg_Min_W_R:     %lf\n", params.neg_min_w_r);
		}
		else if(strcmp(tmp_str, "Neg_Max_W_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_max_w_r);
			printf("Neg_Max_W_R:     %lf\n", params.neg_max_w_r);
		}
		else if(strcmp(tmp_str, "Neg_Min_H_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_min_h_r);
			printf("Neg_Min_H_R:     %lf\n", params.neg_min_h_r);
		}
		else if(strcmp(tmp_str, "Neg_Max_H_R:") == 0)
		{
			fscanf(fp, "%lf", &params.neg_max_h_r);
			printf("Neg_Max_H_R:     %lf\n", params.neg_max_h_r);
		}
	}
	fclose(fp);
	printf("Push any key to start training\n");
	getchar();

	return 1;
}


int Cascade::saveTrainedModel()
{
	FILE *fp = fopen(pt_params->model_path.c_str(), "wt");
	fprintf(fp, "%d\n", model.weak_learner_num);
	fclose(fp);

	for (int i=0; i<model.weak_learner_num; i++)
	{
		model.pt_weak_learners[i].saveToFile(pt_params->model_path);
	}
	return 1;
}


int Cascade::saveTrainedStages()
{
	FILE *fp = fopen(pt_params->stage_path.c_str(), "wt");
	fprintf(fp, "%d %d\n", pt_params->template_w, pt_params->template_h);
	fprintf(fp, "%d\n", model.feature_type);
	fprintf(fp, "%d\n", model.weak_learner_num);
	fprintf(fp, "%d\n", model.stage_num);

	for (int i=0; i<model.stage_num; i++)
	{
		fprintf(fp, "%d %lf\n", model.stage_idx[i], model.stage_thd[i]);
	}
	fclose(fp);
	for (int i=0; i<model.weak_learner_num; i++)
	{
		model.pt_weak_learners[i].saveToFile(pt_params->stage_path);
	}

	return 1;
}


int Cascade::extractPositiveSamples()
{
	printf("------------Extracting positive samples...------------\n");
	if (pt_params->is_extract_positive == 0)
	{
		pt_params->positive_num = getSampleNum(pt_params->positive_data_path);
		return 1;
	}

	pt_params->positive_num = pos_extractor.extractSamples(pt_params);
	pt_params->ori_positive_num = pt_params->positive_num;
	return 1;
}


int Cascade::extractNegativeSamples()
{
	FILE *fp = fopen(pt_params->train_log_path.c_str(), "at");
	fprintf(fp, "stage:%d, detection rate: %lf, ", 
		    model.stage_num, (double)pt_params->positive_num / pt_params->ori_positive_num);
	fclose(fp);

	printf("------------Extracting negative samples...------------\n");
	pt_params->negative_num = getSampleNum(pt_params->negative_data_path);

	int aim_num = max(pt_params->min_negative_num, pt_params->positive_num); 
	int diff_num = aim_num - pt_params->negative_num;
	double rate = (double)diff_num / (double)pt_params->positive_num;

	if (rate > 0.01)
	{
		pt_params->negative_num += neg_extractor.extractSamples(diff_num, &model);
	}
	return 1;
}


int Cascade::isStageLearned(double detection_rate, double false_alarm)
{
	if ( detection_rate > pt_params->detection_rates[0] && 
		 false_alarm < pt_params->false_alarms[0] )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int Cascade::getSampleNum(const string &path)
{
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}
	
	int count = 0;
	IntegralImage intg;

	while(true)
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


int Cascade::batchTest(int num, const string &data_path)
{
	Detector detector;
	detector.init(&model);

	FILE *fp = fopen(data_path.c_str(), "rb");
	if (fp == NULL)
	{
		return -1;
	}

	IntegralImage intg;
	int count = 0;
	for (int i=0; i<num; i++)
	{
		intg.load(fp);
		SubwinInfoT subwin;
		subwin.image_size.x = intg.width;
		subwin.image_size.y = intg.height;
		subwin.win_size.x = intg.width;
		subwin.win_size.y = intg.height;
		subwin.win_pos.x = 0;
		subwin.win_pos.y = 0;
		int result = detector.test(intg, subwin);
		if (result > 0)
		{
			count++;
		}
	}
	fclose(fp);
	return count;
}