#pragma once
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

const int MAX_CASCADE_STAGE_NUM = 30;
const int MAX_ROUND_NUM = 1000;
const int MAX_BIN_NUM = 100;
const int MAX_NEG_SAMPLE_NUM = 20000;
const int MAX_LABEL_PER_IMAGE = 100;
const int MAX_PATH_LEN = 255;

const int MIN_MEAN = 10;
const int MAX_MEAN = 240;
const int MIN_VAR = 5;
const int MAX_VAR = 200;

const double MAX_OBJ_OVERLAP_X = 0.4;
const double MAX_OBJ_OVERLAP_Y = 0.4;

typedef enum 
{
	UPRIGHT_HAAR = 1, SLANT_HAAR = 2, UPRIGHT_SLANT_HAAR = 3
}FeatureTypeE;


typedef struct
{
	int x;
	int y;
}CB_PointT;


typedef struct
{
	double x;
	double y;
}CB_DBl_PointT;

typedef struct
{
	int left;
	int right;
	int top;
	int bottom;
}CB_RectT;


typedef struct
{
	CB_PointT left;
	CB_PointT right;
	CB_PointT top;
	CB_PointT bottom;
}CB_RectangleT;


typedef struct
{
	int x;
	int y;
	int L;
	int R;
}CB_SlantT;


typedef struct
{
	string class_label;
	string work_dir;
	string label_setting_path;
	string share_config_path;
	string config_path;

	int is_extract_positive;
	string positive_pool_dir;
	string negative_pool_dir;

	string positive_data_path;
	string negative_data_path;

	string positive_feature_path;
	string negative_feature_path;

	string positive_weight_path;
	string negative_weight_path;

	string train_log_path;

	int template_w;
	int template_h;

	int ori_positive_num;
	int positive_num;
	int negative_num;

	int min_negative_num;
	string test_src_path;
	string test_dst_path;

	string stage_path;
	string model_path;

	int feature_type;
	int max_weak_learner_num;
	int bin_num;
	int stage_num;
	double detection_rates[MAX_CASCADE_STAGE_NUM];
	double false_alarms[MAX_CASCADE_STAGE_NUM];
	int max_neg_per_image;
	string extracted_negative_dir;
	int is_save_feature2file;

	double neg_start_x_r;
	double neg_end_x_r;
	double neg_start_y_r;
	double neg_end_y_r;
	double neg_min_w_r;
	double neg_max_w_r;
	double neg_min_h_r;
	double neg_max_h_r;
}TrainParamsT;


typedef struct
{
	CB_PointT image_size;
	CB_PointT win_size;	
	CB_PointT win_pos;
	double mean;
	double var;
}SubwinInfoT;
