#include "stdafx.h"
#include "Utils.h"


int waitForKey(const char* str)
{
	printf("%s\n", str);
	MessageBeep(MB_OK);
	getchar();
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