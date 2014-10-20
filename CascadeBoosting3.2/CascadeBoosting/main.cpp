// CascadeBoosting.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Cascade.h"
#include "Tester.h"
#include "Utils.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Start Cascade Boosting ... ...\n");

	TrainParamsT params;
	params.class_label = argv[2];
	getCurrentDir(params.work_dir);
	
	if (strcmp(argv[1], "-train") == 0)
	{
		params.is_train_test = 1;
	}
	else if(strcmp(argv[1], "-test") == 0)
	{
		params.is_train_test = 0;
		if (argv[3] == NULL && argv[4] == NULL)
		{
			waitForKey("error: check testing params!!!");
			return 0;
		}
		params.test_src_path = argv[3];
		params.test_dst_path = argv[4];
	}
	else
	{
		waitForKey("error: check command params!!!");
		return 0;
	}

	Cascade cascade;
	cascade.work(&params);
	
	return 1;
}