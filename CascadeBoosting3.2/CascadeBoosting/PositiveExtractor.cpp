#include "stdafx.h"
#include <afx.h>
#include <direct.h>
#include "PositiveExtractor.h"

PositiveExtractor::PositiveExtractor(void)
{
}


PositiveExtractor::~PositiveExtractor(void)
{
}


int PositiveExtractor::extractSamples(const TrainParamsT *pt_params)
{
	IntegralImage intg;

	_chdir(pt_params->positive_pool_dir.c_str());
	CFileFind file_finder;
    bool is_working = file_finder.FindFile();
	int count = 0;

	while (is_working)
	{
		is_working = file_finder.FindNextFile();
		string file_name = file_finder.GetFileName();
		string file_path = file_finder.GetFilePath();

		if (file_name == "." || file_name == ".."|| file_name == "Thumbs.db")
		{
			continue;
		}

		printf("%s\n", file_path.c_str());

		Mat image = imread(file_path, CV_LOAD_IMAGE_GRAYSCALE);
		if (image.data == NULL)
		{
			continue;
		}

		int width = image.cols;
		int height = image.rows;

		if (width < pt_params->template_w || height < pt_params->template_h)
		{
			continue;
		}
	
		intg.init(width, height, pt_params->feature_type);
		intg.compute(image.data);
		intg.save(pt_params->positive_data_path);
		count++;
 	}
	return count;
}