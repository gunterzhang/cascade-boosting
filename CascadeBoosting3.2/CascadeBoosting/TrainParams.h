#pragma once
#include <string>
#include <afx.h>
#include <io.h> 
#include <direct.h>
#include "Utils.h"
#include "Feature.h"

using namespace std;

struct TrainParamsT
{
	int is_train_test;

	FeatureTypeE feature_type;
	
	string class_label;
	string work_dir;
	string config_dir;
	string label_setting_path;
	string train_config_path;
	string test_config_path;
	string ft_config_path;

	string positive_pool_dir;
	string negative_pool_dir;

	string positive_data_path;
	string negative_data_path;

	string train_log_path;
	int ori_positive_num;
	int positive_num;
	int negative_num;

	int min_negative_num;
	string test_src_path;
	string test_dst_path;

	string model_path;
	string feature_list_path;

	int max_weak_learner_num;
	int stage_num;
	double detection_rates[MAX_CASCADE_STAGE_NUM];
	double false_alarms[MAX_CASCADE_STAGE_NUM];

	int max_neg_per_image;
	string extracted_negative_dir;

	double neg_start_x_r;
	double neg_end_x_r;
	double neg_start_y_r;
	double neg_end_y_r;

	int is_size_ratio_on;
	double neg_min_w_r;
	double neg_max_w_r;
	
	int is_size_len_on;
	int neg_min_w;
	int neg_max_w;

	FeatureParamT *ptr_ft_param;
};
