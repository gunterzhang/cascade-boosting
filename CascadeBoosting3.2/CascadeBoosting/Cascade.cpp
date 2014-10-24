#include "stdafx.h"
#include <io.h>
#include <afx.h>
#include <direct.h>
#include "Cascade.h"


Cascade::Cascade(void)
{
}


Cascade::~Cascade(void)
{
}


int Cascade::work(TrainParamsT *ptr_params)
{
	this->ptr_params = ptr_params;
	initWorkDir();

	if (ptr_params->is_train_test == 1)
		train();
	else if (ptr_params->is_train_test == 0)	
		test();

	return 1;
}


int Cascade::train()
{
	int rst = trainInit();
	if (rst <= 0) return rst;

	waitForKey("Push any key to start training");

	extractPositiveSamples();
	boosting.init(*ptr_params);

	int is_new_stage = 1;
	while (model.stage_num < ptr_params->stage_num && 
		   model.weak_learner_num < ptr_params->max_weak_learner_num)
	{
		printf( "===========  Training stage:%d, round:%d  ===========\n", 
			    (model.stage_num + 1), (model.weak_learner_num + 1) );
		
		string log_path = ptr_params->work_dir + "\\train_log.txt"; 
		FILE *fp = fopen(log_path.c_str(), "at");
		fprintf(fp, "===========  Training stage:%d, round:%d  ===========\n", 
			    (model.stage_num + 1), (model.weak_learner_num + 1) );
		fclose(fp);

		if (is_new_stage > 0)
		{
			boosting.filterTrainingSamples(model);
			extractNegativeSamples();
			if (ptr_params->negative_num < ptr_params->positive_num)
			{
				break;
			}
			boosting.prepareNewStage(model);
		}
		boosting.trainWeakLearner(model);
		MessageBeep(MB_OK);

		is_new_stage = 0;
		if (isStageLearned() > 0)
		{
			updateStageModel();
			is_new_stage = 1;
		}
	}
	return 1;
}


int Cascade::trainInit()
{
	int rst = loadConfig(ptr_params->is_train_test);
	if (rst <= 0) return rst;

	printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

	if (ptr_params->feature_type == FeatureTypeE::FERN)
		ptr_params->ft_config_path = ptr_params->config_dir + "fern_feature.txt";
	else if (ptr_params->feature_type == FeatureTypeE::HAAR)
		ptr_params->ft_config_path = ptr_params->config_dir + "haar_feature.txt";

	model.init(ptr_params);
	model.saveToModel(ptr_params->model_path);

	rst = neg_extractor.init(*ptr_params, &model);
	if (rst <= 0) return rst;

	printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	return 1;
}


int Cascade::updateStageModel()
{
	model.addNewStage();
	model.saveToModel(ptr_params->model_path);

	return 1;
}


int Cascade::extractPositiveSamples()
{
	printf("------------Extracting positive samples...------------\n");
	ptr_params->positive_num = pos_extractor.extractSamples(ptr_params, &model);
	ptr_params->ori_positive_num = ptr_params->positive_num;
	return 1;
}


int Cascade::extractNegativeSamples()
{
	FILE *fp = fopen(ptr_params->train_log_path.c_str(), "at");
	fprintf(fp, "stage:%d, detection rate: %lf, ", 
		    model.stage_num, (double)ptr_params->positive_num / ptr_params->ori_positive_num);
	fclose(fp);

	printf("------------Extracting negative samples...------------\n");
	ptr_params->negative_num = neg_extractor.getSampleNum(ptr_params->negative_data_path);

	int aim_num = max(ptr_params->min_negative_num, ptr_params->positive_num); 
	int diff_num = aim_num - ptr_params->negative_num;

	double rate = (double)diff_num / (double)ptr_params->positive_num;
	if (rate > 0.01)
	{
		ptr_params->negative_num += neg_extractor.extractSamples(diff_num);
	}
	return 1;
}


int Cascade::isStageLearned()
{
	double detection_rate;
	double false_alarm;
	boosting.getPerformance(detection_rate, false_alarm);

	int is_stage_learned = 0;
	if ( detection_rate > ptr_params->detection_rates[0] && 
		 false_alarm < ptr_params->false_alarms[0] )
	{
		is_stage_learned = 1;
	}

	return is_stage_learned;
}


int Cascade::loadConfig(int is_train)
{
	printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	TrainParamsT &params = *ptr_params;
	params.feature_type = FeatureTypeE::UNKNOWN;

	string config_path;
	if (is_train == 1)
	{
		config_path = params.train_config_path;
	}
	else
	{
		config_path = params.test_config_path;
	}

	FILE *fp = fopen(config_path.c_str(), "rt");
	if (fp == NULL)
	{
		printf("Can't open configure file: %s\n", config_path.c_str());
		return -1;
	}

	while (true)
	{
		char tmp_str[100];
		int count = fscanf(fp, "%s", tmp_str);
		if (count <= 0)	break;

		int is_matched = parseSearchParams(tmp_str, fp);
		if (is_matched == 1 || is_train == 0)
		{
			continue;
		}

		if (strcmp(tmp_str, "Feature:") == 0)
		{
			char feature_name[100];
			fscanf(fp, "%s", &feature_name);
			if (strcmp(feature_name, "fern") == 0)
			{
				params.feature_type = FeatureTypeE::FERN;
			}
			else if (strcmp(feature_name, "haar") == 0)
			{
				params.feature_type = FeatureTypeE::HAAR;
			}
			else continue;

			printf("Feature:         %s\n", feature_name);
		}
		else if (strcmp(tmp_str, "StageNum:") == 0)
		{
			fscanf(fp, "%d", &params.stage_num);
			printf("StageNum:        %d\n", params.stage_num);
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
		else if (strcmp(tmp_str, "PositivePath:") == 0)
		{
			fscanf(fp, "%s", tmp_str);
			params.positive_pool_dir = tmp_str;
			params.positive_pool_dir += "\\" + params.class_label + "\\";
			printf("PositivePath:    %s\n", params.positive_pool_dir.c_str());
		}
	}
	fclose(fp);
	return 1;
}


int Cascade::initWorkDir()
{
	TrainParamsT &params = *ptr_params;

	params.config_dir = params.work_dir + "config\\";
	if (access(params.config_dir.c_str(), 0) == -1)
	{
		return 0;
	}
	params.label_setting_path = params.config_dir + "setting.txt";
	params.train_config_path = params.config_dir + "train_config.txt";
	params.test_config_path = params.config_dir + "test_config.txt";
	params.feature_list_path = params.config_dir + "feature_list.txt";
	params.negative_pool_dir = params.work_dir + "negatives\\";

	params.work_dir += "data\\" + params.class_label + "\\";
	if (access(params.work_dir.c_str(), 0) == -1)
	{
		_mkdir(params.work_dir.c_str());
	}
	printf("Class Label:  %s\n", params.class_label.c_str());
	printf("Work Space:   %s\n", params.work_dir.c_str());
	params.model_path = params.work_dir + params.class_label + ".model";
	params.positive_data_path = params.work_dir + params.class_label + ".intg";
	params.negative_data_path = params.work_dir + "negative.intg";
	params.train_log_path = params.work_dir + "log.txt";

	if (params.is_train_test == 0)
	{
		return 1;
	}
	params.extracted_negative_dir = params.work_dir + "neg_icons\\";
	if (access(params.extracted_negative_dir.c_str(), 0) == -1)
	{
		_mkdir(params.extracted_negative_dir.c_str());
	}
	return 1;
}


int Cascade::parseSearchParams(char *tmp_str, FILE *fp)
{
	TrainParamsT &params = *ptr_params;

	if(strcmp(tmp_str, "Neg_Start_X_R:") == 0)
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
	else if (strcmp(tmp_str, "Size_Ratio_On:") == 0)
	{
		fscanf(fp, "%d", &params.is_size_ratio_on);
		printf("Size_Ratio_On:   %d\n", params.is_size_ratio_on);
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
	else if (strcmp(tmp_str, "Size_Length_On:") == 0)
	{
		fscanf(fp, "%d", &params.is_size_len_on);
		printf("Size_Length_On:  %d\n", params.is_size_len_on);
	}
	else if(strcmp(tmp_str, "Neg_Min_W:") == 0)
	{
		fscanf(fp, "%d", &params.neg_min_w);
		printf("Neg_Min_W:       %d\n", params.neg_min_w);
	}
	else if(strcmp(tmp_str, "Neg_Max_W:") == 0)
	{
		fscanf(fp, "%d", &params.neg_max_w);
		printf("Neg_Max_W:       %d\n", params.neg_max_w);
	}
	else
	{
		return 0;
	}
	return 1;
}


int Cascade::test()
{
	int rst = loadConfig(ptr_params->is_train_test);
	if (rst <= 0) return rst;

	Detector detector;
	int result = detector.init(ptr_params->model_path);
	if (result == 0) return 0;

	DetectorParamT param;
	param.min_win_w = 60;
	param.max_win_w = 250;
	param.resize_w = 0;
	param.resize_h = 0;
	param.scan_shift_step = 2;
	param.scan_scale_step = 1.2;
	param.hot_rect.top = ptr_params->neg_start_y_r * 100;
	param.hot_rect.bottom = ptr_params->neg_end_y_r * 100;
	param.hot_rect.left = ptr_params->neg_start_x_r * 100;
	param.hot_rect.right = ptr_params->neg_end_x_r * 100;

	detector.setScanParams(&param);

	string src_folder_dir = ptr_params->test_src_path + "\\";
	if (access(src_folder_dir.c_str(), 0) == -1)
	{
		_mkdir(src_folder_dir.c_str());
	}

	string dst_folder_dir = ptr_params->test_dst_path + ptr_params->class_label + "\\";
	if (access(ptr_params->test_dst_path.c_str(), 0) == -1)
	{
		_mkdir(ptr_params->test_dst_path.c_str());
	}

	detector.batchDetect(src_folder_dir, dst_folder_dir);

	return 1;
}