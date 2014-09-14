// CascadeBoosting.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <afx.h>
#include <io.h> 
#include <direct.h>
#include "TrainParams.h"
#include "Cascade.h"
#include "Tester.h"
#include "Detector.h"

using namespace std;

void detect(const TrainParamsT &params);
int getCurrentDir(string &path);

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Start Cascade Boosting Training.........\n");

	TrainParamsT params;
	getCurrentDir(params.work_dir);
	
	printf("%s\n", params.work_dir.c_str());
	
	params.class_label = argv[1];

	if (argv[2] != NULL && argv[3] != NULL)
	{
		params.test_src_path = argv[2];
		params.test_dst_path = argv[3];
	}

	params.label_setting_path = params.work_dir + "setting.txt";
	params.share_config_path = params.work_dir + "config.txt";
	params.config_path = params.work_dir + "config\\config_" + params.class_label + ".txt";
	params.negative_pool_dir = params.work_dir + "negatives\\";
	params.work_dir += "data\\" + params.class_label + "\\";

	if (access(params.work_dir.c_str(), 0) == -1)
	{
		_mkdir(params.work_dir.c_str());
	}

	printf("-------Configure Infos-------\n");
	printf("Class Label:  %s\n", params.class_label.c_str());
	printf("Work Space:   %s\n", params.work_dir.c_str());
	
	//detect(params);
	Cascade cascade;
	cascade.train(params);
	
	return 1;
}


int getCurrentDir(string &path)
{
	char file_path[MAX_PATH_LEN];
	GetModuleFileName(NULL, file_path, MAX_PATH_LEN);

	char drive_path[MAX_PATH_LEN];
	char dir_path[MAX_PATH_LEN];
	char file_name[MAX_PATH_LEN];
	char exe_name[MAX_PATH_LEN];
	_splitpath(file_path, drive_path, dir_path, file_name, exe_name);

	sprintf(file_path, "%s%s", drive_path, dir_path);
	path = file_path;

	return 1;
}


void detect(const TrainParamsT &params)
{
	Detector detector;
	string model_path = params.work_dir + params.class_label + ".model";
	int result = detector.init(model_path);
	if (result == 0)
	{
		return;
	}

	DetectorParamT param;
	param.min_win_w = 60;
	param.max_win_w = 250;
	param.resize_w = 0;
	param.resize_h = 0;
	param.scan_shift_step = 2;
	param.scan_scale_step = 1.2;
	param.hot_rect.top = params.neg_start_y_r * 100;
	param.hot_rect.bottom = params.neg_end_y_r * 100;
	param.hot_rect.left = params.neg_start_x_r * 100;
	param.hot_rect.right = params.neg_end_x_r * 100;

	detector.setScanParams(&param);

	string src_folder_dir = params.test_src_path + "\\";
	if (access(src_folder_dir.c_str(), 0) == -1)
	{
		_mkdir(src_folder_dir.c_str());
	}

	string dst_folder_dir = params.test_dst_path + params.class_label + "\\";
	if (access(params.test_dst_path.c_str(), 0) == -1)
	{
		_mkdir(params.test_dst_path.c_str());
	}

	detector.batchDetect(src_folder_dir, dst_folder_dir);
}
