#include "stdafx.h"
#include <afx.h>
#include <io.h>
#include <direct.h>
#include "Detector.h"


Detector::Detector(void)
{
	model.pt_weak_learners = NULL;

	param.scan_shift_step = 2;
	param.scan_scale_step = 1.2;
}


Detector::~Detector(void)
{
	if (model.pt_weak_learners != NULL)
	{
		delete []model.pt_weak_learners;
	}
}


int Detector::setScanParams(const DetectorParamT *pt_param)
{
	if (pt_param != NULL)
	{
		param = *pt_param;
	}
	return 1;
}


int Detector::init(CascadeModelT *pt_model)
{
	this->pt_model = pt_model;
	return 1;
}


int Detector::init(const string &model_path)
{
	FILE *fp = fopen(model_path.c_str(), "rt");
	if (fp == NULL)
	{
		model.weak_learner_num = 0;
		model.stage_num = 0;
		return 0;
	}

	fscanf(fp, "%d %d\n", &model.template_w, &model.template_h);
	fscanf(fp, "%d\n", &model.feature_type);

	fscanf(fp, "%d\n", &model.weak_learner_num);
	if (model.pt_weak_learners != NULL)
	{
		delete []model.pt_weak_learners;
	}
	model.pt_weak_learners = new WeakLearner[model.weak_learner_num];

	fscanf(fp, "%d\n", &model.stage_num);
	for (int i=0; i<model.stage_num; i++)
	{
		fscanf(fp, "%d %lf\n", &model.stage_idx[i], &model.stage_thd[i]);
	}

	for (int i=0; i<model.weak_learner_num; i++)
	{
		model.pt_weak_learners[i].loadFromFile(fp, model.template_w, model.template_h);
	}

	fclose(fp);

	pt_model = &model;

	MergerParamT merger_param;
	merger_param.min_hit = 3;
	merger_param.overlap_r_x = 0.8;
	merger_param.overlap_r_y = 0.8;
	merger_param.max_ratio_x = 0.6;
	merger_param.max_ratio_y = 0.6;
	merger.init(merger_param);

	return 1;
}


int Detector::batchDetect(const string &src_image_folder, const string &dst_image_folder)
{
	if (access(src_image_folder.c_str(), 0) == -1)
	{
		return 0;
	}

	if (access(dst_image_folder.c_str(), 0) == -1)
	{
		_mkdir(dst_image_folder.c_str());
	}


	const int MAX_DETECTION = 500;
	CB_RectT rects[MAX_DETECTION];

	_chdir(src_image_folder.c_str());
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
		double ratio = 1.0;
		if (param.resize_w > 0 && param.resize_h > 0)
		{
			int w = param.resize_w;
			int h = image.rows * w / image.cols;
			ratio = (double)image.cols / w;
			Size size(w, h);
			resize(image, image, size);
		}

		int subwin_count;
		int num = detect(image, MAX_DETECTION, rects, subwin_count);

		Mat dst_image = imread(file_path, CV_LOAD_IMAGE_COLOR);
		file_path = dst_image_folder + "\\" + file_name;
		drawRects(dst_image, num, rects, Scalar(0, 255, 0), 3, ratio);
		
		num = merger.merge(num, rects);
		drawRects(dst_image, num, rects, Scalar(0, 0, 255), 7, ratio);

		imwrite(file_path, dst_image);
		count++;
 	}
	return 1;
}


int Detector::drawRects(Mat &image, int num, CB_RectT *pt_rects, Scalar &color, int thickness, double r)
{
	for (int i=0; i<num; i++)
	{
		Rect rect;
		rect.x = pt_rects[i].left * r;
		rect.y = pt_rects[i].top * r;
		rect.width = (pt_rects[i].right - pt_rects[i].left - 1) * r;
		rect.height = (pt_rects[i].bottom - pt_rects[i].top - 1) * r;
		rectangle(image, rect, color, thickness);
	}
	return 1;
}


int Detector::detect(const Mat &image, int max_num, CB_RectT *pt_rects, int &subwin_count)
{
	int image_w = image.cols;
	int image_h = image.rows;

	IntegralImage intg;
	intg.init(image_w, image_h, pt_model->feature_type);
	intg.compute(image.data);

	int num = detect(intg, max_num, pt_rects, subwin_count);
	return num;
}


int Detector::detect(IntegralImage &intg, int max_num, CB_RectT *pt_rects, int &subwin_count)
{
	int image_w = intg.width;
	int image_h = intg.height;

	SubwinInfoT subwin;
	subwin.image_size.x = image_w;
	subwin.image_size.y = image_h;
	subwin.win_size.x = pt_model->template_w;
	subwin.win_size.y = pt_model->template_h;

	double cur_scan_scale = 1.0;

	int detection_count = 0;
	subwin_count = 0;
	while (subwin.win_size.x < image_w && subwin.win_size.y < image_h) 
	{
		if (subwin.win_size.x < param.min_win_w || subwin.win_size.x > param.max_win_w)
		{
			cur_scan_scale *= param.scan_scale_step;
			subwin.win_size.x = pt_model->template_w * cur_scan_scale;
			subwin.win_size.y = pt_model->template_h * cur_scan_scale;
			continue;
		}
		subwin.win_pos.y = param.hot_rect.top;
		while (subwin.win_pos.y + subwin.win_size.y < param.hot_rect.bottom - 1)
		{
			subwin.win_pos.x = param.hot_rect.left;
			while (subwin.win_pos.x + subwin.win_size.x < param.hot_rect.right - 1)
			{
				subwin_count++;
				if (test(intg, subwin) <= 0)
				{
					subwin.win_pos.x += param.scan_shift_step * cur_scan_scale;
					continue;
				}
					
				CB_RectT rect; 
				rect.left = subwin.win_pos.x;
				rect.right = rect.left + subwin.win_size.x - 1;
				rect.top = subwin.win_pos.y;
				rect.bottom = rect.top + subwin.win_size.y - 1;

				pt_rects[detection_count] = rect;
				detection_count++;

				if (detection_count >= max_num)
				{
					return detection_count;
				}
				subwin.win_pos.x += param.scan_shift_step * cur_scan_scale;
			}
			subwin.win_pos.y += param.scan_shift_step * cur_scan_scale;
		}
		cur_scan_scale *= param.scan_scale_step;
		subwin.win_size.x = pt_model->template_w * cur_scan_scale;
		subwin.win_size.y = pt_model->template_h * cur_scan_scale;
	}
	printf("%d\n", detection_count);
	return detection_count;
}


int Detector::preTest(IntegralImage &intg, SubwinInfoT &subwin)
{
	intg.computeSubwinMeanVar(subwin);

	if (subwin.mean >= MAX_MEAN || subwin.mean <= MIN_MEAN)
	{
		return 0;
	}

	if (subwin.var < MIN_VAR || subwin.var > MAX_VAR)
	{
		return 0;
	}
	return 1;
}


int Detector::test(IntegralImage &intg, SubwinInfoT &subwin)
{
	if (preTest(intg, subwin) == 0)
	{
		return 0;
	}

	double value = 0;
	int stage_count = 0;
	for (int i=0; i<pt_model->weak_learner_num; i++)
	{
		double temp = pt_model->pt_weak_learners[i].test(intg, subwin);
		value += temp;

		if (i != pt_model->stage_idx[stage_count])
		{
			continue;
		}

		if (value < pt_model->stage_thd[stage_count])
		{
			return 0;		
		}
		stage_count++;
	}
	return 1;
}