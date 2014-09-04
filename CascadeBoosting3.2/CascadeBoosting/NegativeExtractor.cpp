#include "stdafx.h"
#include <cstdio>
#include <io.h>
#include "NegativeExtractor.h"
using namespace std;

NegativeExtractor::NegativeExtractor(void)
{
}


NegativeExtractor::~NegativeExtractor(void)
{
}


int NegativeExtractor::init(TrainParamsT &params)
{
	pt_params = &params;

	scan_shift_step = params.template_w;
	scan_scale_step = 2.0;
	total_negative_count = 0;

	pool_image_idx = 0;
	
	//----read label setting file
	FILE *fp_setting = fopen(params.label_setting_path.c_str(), "rt");
	fscanf(fp_setting, "%d\n", &label_num);
	for (int i=0; i<label_num; i++)	
	{
		fscanf(fp_setting, "%d %d\n", &char_idx[i].x, &char_idx[i].y);
	}
	fclose(fp_setting);

	//----read negative image list 
	string image_list_path = params.negative_pool_dir + "\\list.txt";
	
	FILE *fp = fopen(image_list_path.c_str(), "rt");
	if (fp == NULL)
	{
		printf("Error! Can't open image list file: %s\n", image_list_path.c_str());
		return -1;
	}

	pool_image_num = 0;
	while (true)
	{
		char tmp_char[MAX_PATH_LEN];
		int rst = fscanf(fp, "%s\n", tmp_char);
		image_file_names[pool_image_num] = tmp_char;
		if (rst <= 0) break;
		pool_image_num++;
	}
	fclose(fp);

	postive_label = params.class_label;
	return 1;
}


int NegativeExtractor::extractSamples(int needed_num, const CascadeModelT *model)
{
	detector.init((CascadeModelT *)model);

	int added_sum = 0;
	total_count = 0;

	while (needed_num > 0)
	{
		string image_path = pt_params->negative_pool_dir + image_file_names[pool_image_idx];
		string label_path = image_path;
		string::size_type pos = label_path.find(".jpg");
		string str = ".jpg";
		label_path.replace(pos, str.length(), ".txt");
		
		cur_neg_name = image_file_names[pool_image_idx];
		pos = cur_neg_name.find(".jpg");
		str = ".jpg";
		cur_neg_name.replace(pos, str.length(), "==");

		pool_image_idx++;

		printf("Extract From : %s\n", image_path.c_str());
		Mat image = imread(image_path, CV_LOAD_IMAGE_GRAYSCALE);
		if (image.data == NULL)
		{
			continue;
		}

		loadLabelsFromFile(label_path);
		int detect_num = detectImage(image);
		added_sum += detect_num;
		printf("shift_step = %lf, scale_step = %lf\n", scan_shift_step, scan_scale_step);
		printf("Extract negetive sample number:%d\n", detect_num);
		needed_num -= detect_num;
		printf("needed_num = %d\n", needed_num);
		
		if (pool_image_idx >= pool_image_num)
		{
			if (scan_shift_step < 1.01 && scan_scale_step < 1.2)
			{
				break;
			}
			
			pool_image_idx = 0;
			
			scan_shift_step *= 0.6;
			if (scan_shift_step < 1)
			{
				scan_shift_step = 1;
			}

			scan_scale_step -= 0.2;
			if (scan_scale_step < 1.15)
			{
				scan_scale_step = 1.15;
			}
		}
	}

	printf("false alarm = %1.10lf\n", (double)added_sum / total_count);

	FILE *fp = fopen(pt_params->train_log_path.c_str(), "at");
	fprintf(fp, " false alarm:%d-%d = %1.10lf\n", added_sum, total_count, (double)added_sum / total_count);
	fclose(fp);

	return added_sum;
}


int NegativeExtractor::detectImage(const Mat &image)
{
	DetectorParamT param;
	param.resize_w = 0;
	param.resize_h = 0;
	param.scan_scale_step = scan_scale_step;
	param.scan_shift_step = scan_shift_step;
	param.min_win_w = pt_params->neg_min_w_r * image.cols;
	param.max_win_w = pt_params->neg_max_w_r * image.cols;

	detector.setScanParams(&param);

	const int MAX_DETECTION = 2000;
	CB_RectT rects[MAX_DETECTION];

	int subwin_count;
	int num = detector.detect(image, MAX_DETECTION, rects, subwin_count);
	total_count += subwin_count;

	int count = 0;
	int w = image.cols;
	int h = image.rows;

	for (int i=0; i<num; i++)
	{
		if ( rects[i].top < pt_params->neg_start_y_r * h ||
			 rects[i].bottom > pt_params->neg_end_y_r * h ||
			 rects[i].left < pt_params->neg_start_x_r * w ||
			 rects[i].right > pt_params->neg_end_x_r * w )
		{
			continue;
		}

		int rect_w = rects[i].right - rects[i].left;
		int rect_h = rects[i].bottom - rects[i].top;
		
		if ( rect_w < pt_params->neg_min_w_r * w || 
			 rect_w > pt_params->neg_max_w_r * w ||
			 rect_h < pt_params->neg_min_h_r * h || 
			 rect_h > pt_params->neg_max_h_r * h )
		{
			continue;
		}

		if (isRectOverlapLabels(rects[i]) == true)
		{
			continue;
		}

		CvRect rect;
		rect.x = rects[i].left;
		rect.y = rects[i].top;
		rect.width = rects[i].right - rects[i].left + 1;
		rect.height = rects[i].bottom - rects[i].top + 1;

		saveTrainingData(image, rect);

		Mat sub_image;
		image(rect).copyTo(sub_image);

		char file_name[200];
		sprintf(file_name, "%s%08d%s.jpg", pt_params->extracted_negative_dir.c_str(), total_negative_count, cur_neg_name.c_str());
		imwrite(file_name, sub_image);
		total_negative_count++;
		count++;
		if (count >= pt_params->max_neg_per_image) 
		{
			return count;
		}
	}
	return count;
}


bool NegativeExtractor::isRectOverlapLabels(CB_RectT rect)
{
	if (total_label_num == 0)
	{
		return false;
	}

	static int count = 0;
	for (int i=0; i<total_label_num; i++)
	{
		if (strcmp(label_type[i].c_str(), postive_label.c_str()) != 0){
			continue;
		}
		if (isRectsOverlap(rect, label_rect[i]) == true){
			return true;
		}
	}
	count++;
	return false;
}


bool NegativeExtractor::isRectsOverlap(CB_RectT rect1, CB_RectT rect2)
{
	int w1 = (rect1.right - rect1.left + 1);
	int w2 = (rect2.right - rect2.left + 1);
	int left = (rect1.left < rect2.left) ? rect1.left:rect2.left;
	int right = (rect1.right > rect2.right) ? rect1.right:rect2.right;

	int overlap_x = (w1 + w2) - (right - left);
	if ((double)overlap_x / w2 < 0.5) 
		return false;

	int h1 = (rect1.bottom - rect1.top + 1);
	int h2 = (rect2.bottom - rect2.top + 1);
	int top = (rect1.top < rect2.top) ? rect1.top:rect2.top;
	int bottom = (rect1.bottom > rect2.bottom) ? rect1.bottom:rect2.bottom;

	int overlap_y = (h1 + h2) - (bottom - top);
	if ((double)overlap_y / h2 < 0.5) 
		return false;

	return true;
}


bool NegativeExtractor::loadLabelsFromFile(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "rt");
	if (fp == NULL)
	{
		total_label_num = 0;
		return false; 
	}
	
	int count;
	fscanf(fp, "%d", &count);

	total_label_num = 0;

	for (int i=0; i<count; i++)
	{
		for (int i=0; i<label_num; i++)
		{
			char file_name[100];
			fscanf(fp, "%s", file_name);

			char type[100];
			fscanf(fp, "%s", type);
			label_type[total_label_num+i] = type;
		}

		int num;
		fscanf(fp, "%d", &num);
		
		CB_PointT head[200];
		CB_PointT tail[200];
		for (int i=0; i<num; i++)
		{
			fscanf(fp, "%d %d %d %d", &head[i].x, &head[i].y, &tail[i].x, &tail[i].y);
		}

		for (int i=0; i<label_num; i++)
		{
			int idx1 = char_idx[i].x;
			int idx2 = char_idx[i].y;
			CB_RectT &rect = label_rect[total_label_num+i];
			if (head[0].x == tail[0].x)
			{
				rect.left = head[idx1].x;
				rect.right = head[idx2].x;
				rect.top = min(head[idx1].y, head[idx2].y);
				rect.bottom = max(tail[idx1].y, tail[idx2].y);
			}
			else
			{
				rect.left = min(head[idx1].x, head[idx2].x);
				rect.right = max(tail[idx1].x, tail[idx2].x);
				rect.top = head[idx1].y;
				rect.bottom = head[idx2].y;
			}
			int w = abs(rect.right - rect.left);
			int h = abs(rect.bottom - rect.top);
			rect.top += 0.5 * h;
		}
		total_label_num += label_num;
	}
	fclose(fp);
	return true;
}


void  NegativeExtractor::saveTrainingData(const Mat &image, CvRect rect)
{
	Mat sub_image;
	image(rect).copyTo(sub_image);

	sample_intg.init(sub_image.cols, sub_image.rows, pt_params->feature_type);
	sample_intg.compute(sub_image.data);
	sample_intg.save(pt_params->negative_data_path);
}