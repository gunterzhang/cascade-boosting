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


int Cascade::work(TrainParamsT &params)
{
	pt_params = &params;

	if (params.is_train_test == 1)
	{
		train();
	}
	else if (params.is_train_test == 0)
	{
		test();
	}
	return 1;
}


int Cascade::train()
{
	int rst = init();
	if (rst <= 0)
	{
		return rst;
	}

	extractPositiveSamples();
	boosting.init(*pt_params);

	int is_new_stage = 1;
	while (model.stage_num < pt_params->stage_num && 
		   model.weak_learner_num < pt_params->max_weak_learner_num)
	{
		printf( "===========  Training stage:%d, round:%d  ===========\n", 
			    (model.stage_num + 1), (model.weak_learner_num + 1) );

		if (is_new_stage > 0)
		{
			boosting.filterTrainingSamples(model);
			extractNegativeSamples();
			if (pt_params->negative_num < pt_params->positive_num)
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


int Cascade::init()
{
	int rst = loadConfig();
	if (rst <= 0)
	{
		return rst;
	}

	TrainParamsT &params = *pt_params;
	params.model_path = params.work_dir + params.class_label + ".model";
	params.new_model_path = params.work_dir + params.class_label + ".model2";
	params.positive_data_path = params.work_dir + params.class_label + ".intg";
	params.negative_data_path = params.work_dir + "negative.intg";
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

	initModel();
	return 1;
}


int Cascade::initModel()
{
	model.init(pt_params->max_weak_learner_num, 
		       pt_params->feature_type, 
		       pt_params->template_w, 
			   pt_params->template_h);

	model.saveToFile(pt_params->model_path);

	return 1;
}


int Cascade::updateStageModel()
{
	model.addNewStage();
	model.saveToFile(pt_params->model_path);

	return 1;
}


int Cascade::extractPositiveSamples()
{
	printf("------------Extracting positive samples...------------\n");
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
	pt_params->negative_num = boosting.getSampleNum(pt_params->negative_data_path);

	int aim_num = max(pt_params->min_negative_num, pt_params->positive_num); 
	int diff_num = aim_num - pt_params->negative_num;
	double rate = (double)diff_num / (double)pt_params->positive_num;

	if (rate > 0)
	{
		pt_params->negative_num += neg_extractor.extractSamples(diff_num, &model);
	}
	return 1;
}


int Cascade::isStageLearned()
{
	double detection_rate;
	double false_alarm;
	boosting.getPerformance(detection_rate, false_alarm);

	int is_stage_learned = 0;
	if ( detection_rate > pt_params->detection_rates[0] && 
		 false_alarm < pt_params->false_alarms[0] )
	{
		is_stage_learned = 1;
	}

	return is_stage_learned;
}

int Cascade::test()
{
	int rst = loadConfig();
	if (rst <= 0)
	{
		return rst;
	}

	Detector detector;
	int result = detector.init(pt_params->model_path);
	if (result == 0)
	{
		return 0;
	}

	DetectorParamT param;
	param.min_win_w = 60;
	param.max_win_w = 250;
	param.resize_w = 0;
	param.resize_h = 0;
	param.scan_shift_step = 2;
	param.scan_scale_step = 1.2;
	param.hot_rect.top = pt_params->neg_start_y_r * 100;
	param.hot_rect.bottom = pt_params->neg_end_y_r * 100;
	param.hot_rect.left = pt_params->neg_start_x_r * 100;
	param.hot_rect.right = pt_params->neg_end_x_r * 100;

	detector.setScanParams(&param);

	string src_folder_dir = pt_params->test_src_path + "\\";
	if (access(src_folder_dir.c_str(), 0) == -1)
	{
		_mkdir(src_folder_dir.c_str());
	}

	string dst_folder_dir = pt_params->test_dst_path + pt_params->class_label + "\\";
	if (access(pt_params->test_dst_path.c_str(), 0) == -1)
	{
		_mkdir(pt_params->test_dst_path.c_str());
	}

	detector.batchDetect(src_folder_dir, dst_folder_dir);

	return 1;
}

int Cascade::loadConfig()
{
	TrainParamsT &params = *pt_params;

	FILE *fp = fopen(params.config_path.c_str(), "rt");
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
		else if (strcmp(tmp_str, "PositivePath:") == 0)
		{
			fscanf(fp, "%s", tmp_str);
			params.positive_pool_dir = tmp_str;
			params.positive_pool_dir += "\\" + params.class_label + "\\";
			printf("PositivePath:    %s\n", params.positive_pool_dir.c_str());
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
	}

	fclose(fp);
	printf("Push any key to start training\n");
	MessageBeep(MB_OK);
	getchar();

	return 1;
}