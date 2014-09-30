// CascadeBoosting.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "TrainParams.h"
#include "Cascade.h"
#include "Tester.h"
#include "Detector.h"

using namespace std;

int getCurrentDir(string &path);

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Start Cascade Boosting ... ...\n");

	TrainParamsT params;
	getCurrentDir(params.work_dir);
	
	params.class_label = argv[2];
	params.label_setting_path = params.work_dir + "setting.txt";
	params.config_path = params.work_dir + "config.txt";
	params.negative_pool_dir = params.work_dir + "negatives\\";
	params.work_dir += "data\\" + params.class_label + "\\";

	if (access(params.work_dir.c_str(), 0) == -1)
	{
		_mkdir(params.work_dir.c_str());
	}

	printf("-------Configure Infos-------\n");
	printf("Class Label:  %s\n", params.class_label.c_str());
	printf("Work Space:   %s\n", params.work_dir.c_str());
	
	if (strcmp(argv[1], "-train") == 0)
	{
		params.is_train_test = 1;
		printf("training mode\n");
	}
	else if(strcmp(argv[1], "-test") == 0)
	{
		params.is_train_test = 0;
		printf("testing mode\n");
		if (argv[3] != NULL && argv[4] != NULL)
		{
			params.test_src_path = argv[3];
			params.test_dst_path = argv[4];
		}
		else
		{
			printf("error: check testing params!!!\n");
			MessageBeep(MB_ICONERROR);
			getchar();
			return 0;
		}
	}
	else
	{
		printf("error: check command params!!!\n");
		MessageBeep(MB_ICONERROR);
		getchar();
		return 0;
	}

	Cascade cascade;
	cascade.work(params);
	
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