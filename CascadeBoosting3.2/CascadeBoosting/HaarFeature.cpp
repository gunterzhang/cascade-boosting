#include "stdafx.h"
#include "HaarFeature.h"


HaarFeature::HaarFeature(void)
{
	feature_num = 0;
	pt_features = NULL;
	pt_feature_infos = NULL;

	//HFT_X_Y
	feature_sizes[HFT_X_AB].x = 2;
	feature_sizes[HFT_X_AB].y = 1;
	feature_inv_ratio[HFT_X_AB] = 1;
	
	feature_sizes[HFT_Y_AB].x = 1;
	feature_sizes[HFT_Y_AB].y = 2;
	feature_inv_ratio[HFT_Y_AB] = 1;

	feature_sizes[HFT_X_ABA].x = 3;
	feature_sizes[HFT_X_ABA].y = 1;
	feature_inv_ratio[HFT_X_ABA] = 2;
	
	feature_sizes[HFT_Y_ABA].x = 1;
	feature_sizes[HFT_Y_ABA].y = 3;
	feature_inv_ratio[HFT_Y_ABA] = 2;

	feature_sizes[HFT_X_ABBA].x = 4;
	feature_sizes[HFT_X_ABBA].y = 1;
	feature_inv_ratio[HFT_X_ABBA] = 2;
	
	feature_sizes[HFT_Y_ABBA].x = 1;
	feature_sizes[HFT_Y_ABBA].y = 4;
	feature_inv_ratio[HFT_Y_ABBA] = 2;

	feature_sizes[HFT_XY_ABA].x = 3;
	feature_sizes[HFT_XY_ABA].y = 3;
	feature_inv_ratio[HFT_XY_ABA] = 8;

	feature_sizes[HFT_XY_ABBA].x = 4;
	feature_sizes[HFT_XY_ABBA].y = 4;
	feature_inv_ratio[HFT_XY_ABBA] = 12;

	//HFT_L_R
	feature_sizes[HFT_L_AB].x = 2;
	feature_sizes[HFT_L_AB].y = 1;
	feature_inv_ratio[HFT_L_AB] = 1;
	
	feature_sizes[HFT_R_AB].x = 1;
	feature_sizes[HFT_R_AB].y = 2;
	feature_inv_ratio[HFT_R_AB] = 1;

	feature_sizes[HFT_L_ABA].x = 3;
	feature_sizes[HFT_L_ABA].y = 1;
	feature_inv_ratio[HFT_L_ABA] = 2;
	
	feature_sizes[HFT_R_ABA].x = 1;
	feature_sizes[HFT_R_ABA].y = 3;
	feature_inv_ratio[HFT_R_ABA] = 2;

	feature_sizes[HFT_L_ABBA].x = 4;
	feature_sizes[HFT_L_ABBA].y = 1;
	feature_inv_ratio[HFT_L_ABBA] = 2;
	
	feature_sizes[HFT_R_ABBA].x = 1;
	feature_sizes[HFT_R_ABBA].y = 4;
	feature_inv_ratio[HFT_R_ABBA] = 2;

	//HFT_A_B
	feature_sizes[HFT_A_B].x = 1;
	feature_sizes[HFT_A_B].y = 1;
	feature_inv_ratio[HFT_A_B] = 1;

}


HaarFeature::~HaarFeature(void)
{
	clearUp();
}


void HaarFeature::clearUp()
{
	if (pt_features != NULL)
	{
		delete []pt_features;
		pt_features = NULL;
	}
	if (pt_feature_infos != NULL)
	{
		delete []pt_feature_infos;
		pt_feature_infos = NULL;
	}
	feature_num = 0;
}


int HaarFeature::init(int w, int h, int type, int is_abs)
{
	clearUp();

	feature_types = type;
	this->is_abs = is_abs;

	template_w = w;
	template_h = h;

	intg.init(w, h, type);

	feature_num = getAllFeatureInfos(-1);

	pt_features = new float[feature_num];
	pt_feature_infos = new HaarFeatureInfoT[feature_num];
	
	getAllFeatureInfos(0);

	return 1;
}


const float *HaarFeature::extractBatchFeatures(FILE *fp)
{
	getAllFeatureInfos(1, fp, pt_features);
	return pt_features;
}


int HaarFeature::getFeatureNum()
{
	return feature_num;
}


int HaarFeature::getAllFeatureInfos(int is_extract_feature, FILE *fp, float *pt_feature)
{
	if (is_extract_feature > 0)
	{
		if (fp == NULL)
		{
			return -1;
		}
		intg.load(fp);
	}

	HaarFeatureInfoT info;
	info.is_abs = is_abs;
	info.tpl_size.x = template_w;
	info.tpl_size.y = template_h;
	info.pos2.x = 0;
	info.pos2.y = 0;

	feature_count = 0;

	if ((feature_types & UPRIGHT_HAAR) > 0)
	{
		info.type = HFT_X_AB; 
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_Y_AB;
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_X_ABA; 
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_Y_ABA;
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_X_ABBA; 
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_Y_ABBA;
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_XY_ABA; 
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
		info.type = HFT_XY_ABBA;
		extractOneTypeFeatures(is_extract_feature, info, pt_feature);
	}

	if ((feature_types & SLANT_HAAR) > 0)
	{
		info.type = HFT_L_AB; 
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
		info.type = HFT_R_AB;
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
		info.type = HFT_L_ABA; 
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
		info.type = HFT_R_ABA;
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
		info.type = HFT_L_ABBA; 
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
		info.type = HFT_R_ABBA;
		extractOneTypeFeatures45(is_extract_feature, info, pt_feature);
	}

	if ((feature_types & UPRIGHT_FAR_HAAR) > 0)
	{
		info.type = HFT_A_B; 
		extractOneTypeFeaturesAB(is_extract_feature, info, pt_feature);
	}

	if (is_extract_feature < 0)
	{
		feature_num = feature_count;
		printf("feature Num = %d\n", feature_num);
	}

	return feature_num;
}


int HaarFeature::extractOneTypeFeatures(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature)
{
	int type = info.type;
	int x_r = feature_sizes[type].x;
	int y_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type];

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < template_h - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < template_w - 1)
		{
			info.size.y = 2;
			while (info.pos1.y + y_r * info.size.y - 1 < template_h - FEATURE_MARGIN)
			{
				info.size.x = 2; 
				while (info.pos1.x + x_r * info.size.x - 1 < template_w - FEATURE_MARGIN)
				{
					if (is_extract_feature == 0)
					{
						info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
						pt_feature_infos[feature_count] = info;
					}
					else if (is_extract_feature > 0)
					{
						*(pt_feature + feature_count) = extractFeature(pt_feature_infos[feature_count]);
					}
					feature_count++;
					info.size.x += HAAR_SCALE_STEP;
				}
				info.size.y += HAAR_SCALE_STEP;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}

int HaarFeature::extractOneTypeFeatures45(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature)
{
	int type = info.type;
	int L_r = feature_sizes[type].x;
	int R_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type] * 2;

	CB_PointT image_size = {template_w, template_h};

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < template_h - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < template_w - 1)
		{
			info.size.y = 2;
			while (info.pos1.x + R_r * info.size.y < template_w)
			{
				info.size.x = 2; 
				while (info.pos1.x - L_r * info.size.x > 0)
				{
					CB_SlantT slant = {info.pos1.x, info.pos1.y, L_r * info.size.x, R_r * info.size.y};
					CB_RectangleT rect;
					int result = slantToRect(slant, rect, image_size);
					if (result == 0)
					{
						info.size.x += HAAR_SCALE_STEP;
						continue;
					}
					if (is_extract_feature == 0)
					{
						info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
						pt_feature_infos[feature_count] = info;
					}
					else if (is_extract_feature > 0)
					{
						*(pt_feature + feature_count) = extractFeature(pt_feature_infos[feature_count]);
					}
					feature_count++;
					info.size.x += HAAR_SCALE_STEP;
				}
				info.size.y += HAAR_SCALE_STEP;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}


int HaarFeature::extractOneTypeFeaturesAB(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature)
{
	int type = info.type;
	int x_r = feature_sizes[type].x;
	int y_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type];

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < template_h - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < template_w - 1)
		{
			info.size.y = 2;
			while (info.pos1.y + y_r * info.size.y - 1 < template_h - FEATURE_MARGIN)
			{
				info.size.x = 2; 
				while (info.pos1.x + x_r * info.size.x - 1 < template_w - FEATURE_MARGIN)
				{
					info.pos2.y = info.pos1.y;
					info.pos2.x = info.pos1.x + info.size.x + HAAR_SHIFT_STEP_X;
					while (info.pos2.x + x_r * info.size.x - 1 < template_w - FEATURE_MARGIN)
					{
						if (is_extract_feature == 0)
						{
							info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
							pt_feature_infos[feature_count] = info;
						}
						else if (is_extract_feature > 0)
						{
							*(pt_feature + feature_count) = extractFeature(pt_feature_infos[feature_count]);
						}
						info.pos2.x += HAAR_SHIFT_STEP_X * 2;
						feature_count++;
					}

					info.pos2.x = info.pos1.x;
					info.pos2.y = info.pos1.y + info.size.y + HAAR_SHIFT_STEP_Y;
					while (info.pos2.y + y_r * info.size.y - 1 < template_h - FEATURE_MARGIN)
					{
						if (is_extract_feature == 0)
						{
							info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
							pt_feature_infos[feature_count] = info;
						}
						else if (is_extract_feature > 0)
						{
							*(pt_feature + feature_count) = extractFeature(pt_feature_infos[feature_count]);
						}
						info.pos2.y += HAAR_SHIFT_STEP_Y * 2;
						feature_count++;
					}
					info.size.x += HAAR_SCALE_STEP;
				}
				info.size.y += HAAR_SCALE_STEP;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}


float HaarFeature::extractFeature(const HaarFeatureInfoT &info)
{
	SubwinInfoT subwin;
	subwin.image_size.x = intg.width;
	subwin.image_size.y = intg.height;
	subwin.win_size = subwin.image_size;
	subwin.win_pos.x = 0;
	subwin.win_pos.y = 0;

	intg.computeSubwinMeanVar(subwin);

	float feature_value = computeFeature(intg, subwin, info);
	return feature_value;
}


float HaarFeature::computeFeature(IntegralImage &intg, const SubwinInfoT &subwin, const HaarFeatureInfoT &haar)
{
	int cur_scan_pos_x = subwin.win_pos.x;
	int cur_scan_pos_y = subwin.win_pos.y;

	double cur_scan_scale = (double)subwin.win_size.x / haar.tpl_size.x;
	HaarFeatureInfoT haar_feature = haar;

	haar_feature.pos1.x = haar_feature.pos1.x * cur_scan_scale;
	haar_feature.pos1.y = haar_feature.pos1.y * cur_scan_scale;
	haar_feature.pos2.x = haar_feature.pos2.x * cur_scan_scale;
	haar_feature.pos2.y = haar_feature.pos2.y * cur_scan_scale;
	haar_feature.size.x *= cur_scan_scale;
	haar_feature.size.y *= cur_scan_scale;

	double real_scale_square = (double)(haar_feature.size.x * haar_feature.size.y) / (haar.size.x * haar.size.y);

	double result;
	if (haar_feature.type == HFT_X_AB)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_middle = x_left + haar_feature.size.x - 1;
		int x_right = x_middle + haar_feature.size.x;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_bottom = y_top + haar_feature.size.y - 1;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_middle+1, x_right, y_top, y_bottom};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 2 * result2;
	}
	else if (haar_feature.type == HFT_Y_AB)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_right = x_left + haar_feature.size.x - 1;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_middle = y_top + haar_feature.size.y - 1;
		int y_bottom = y_middle + haar_feature.size.y;
		
		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_left, x_right, y_middle+1, y_bottom};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 2 * result2;
	}
	else if (haar_feature.type == HFT_X_ABA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_middleL = x_left + haar_feature.size.x - 1;
		int x_middleR = x_middleL + haar_feature.size.x;
		int x_right = x_middleR + haar_feature.size.x;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_bottom = y_top + haar_feature.size.y - 1;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_middleL + 1, x_middleR, y_top, y_bottom};

		double result1 = intg.getRectValue_0(rect1); 
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 3 * result2;
	}
	else if (haar_feature.type == HFT_Y_ABA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_right = x_left + haar_feature.size.x - 1;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_middleT = y_top + haar_feature.size.y - 1;
		int y_middleB = y_middleT + haar_feature.size.y;
		int y_bottom = y_middleB + haar_feature.size.y;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_left, x_right, y_middleT + 1, y_middleB};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 3 * result2;
	}
	else if (haar_feature.type == HFT_X_ABBA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_middleL = x_left + haar_feature.size.x - 1;
		int x_middleR = x_middleL + haar_feature.size.x * 2;
		int x_right = x_middleR + haar_feature.size.x;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_bottom = y_top + haar_feature.size.y - 1;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_middleL + 1, x_middleR, y_top, y_bottom};

		double result1 = intg.getRectValue_0(rect1); 
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 2 * result2;
	}
	else if (haar_feature.type == HFT_Y_ABBA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_right = x_left + haar_feature.size.x - 1;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_middleT = y_top + haar_feature.size.y - 1;
		int y_middleB = y_middleT + haar_feature.size.y * 2;
		int y_bottom = y_middleB + haar_feature.size.y;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_left, x_right, y_middleT + 1, y_middleB};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 2 * result2;
	}
	else if (haar_feature.type == HFT_XY_ABA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_middleL = x_left + haar_feature.size.x - 1;
		int x_middleR = x_middleL + haar_feature.size.x;
		int x_right = x_middleR + haar_feature.size.x;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_middleT = y_top + haar_feature.size.y - 1;
		int y_middleB = y_middleT + haar_feature.size.y;
		int y_bottom = y_middleB + haar_feature.size.y;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_middleL + 1, x_middleR, y_middleT + 1, y_middleB};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 9 * result2;
	}
	else if (haar_feature.type == HFT_XY_ABBA)
	{
		int x_left = cur_scan_pos_x + haar_feature.pos1.x;
		int x_middleL = x_left + haar_feature.size.x - 1;
		int x_middleR = x_middleL + haar_feature.size.x * 2;
		int x_right = x_middleR + haar_feature.size.x;
		int y_top = cur_scan_pos_y + haar_feature.pos1.y;
		int y_middleT = y_top + haar_feature.size.y - 1;
		int y_middleB = y_middleT + haar_feature.size.y * 2;
		int y_bottom = y_middleB + haar_feature.size.y;

		CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
		CB_RectT rect2 = {x_middleL + 1, x_middleR, y_middleT + 1, y_middleB};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - 4 * result2;
	}
	else if (haar_feature.type == HFT_L_AB)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.left.x;
		y = rect1.left.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);

		result = result1 - result2;
	}
	else if (haar_feature.type == HFT_R_AB)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.right.x;
		y = rect1.right.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);

		result = result1 - result2;
	}
	else if (haar_feature.type == HFT_L_ABA)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.left.x;
		y = rect1.left.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		x = rect2.left.x;
		y = rect2.left.y;
		CB_SlantT slant3 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect3;
		slantToRect(slant3, rect3, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);
		double result3 = intg.getRectValue_45(rect3);

		result = result1 + result3 - 2 * result2;
	}
	else if (haar_feature.type == HFT_R_ABA)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.right.x;
		y = rect1.right.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		x = rect2.right.x;
		y = rect2.right.y;
		CB_SlantT slant3 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect3;
		slantToRect(slant3, rect3, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);
		double result3 = intg.getRectValue_45(rect3);

		result = result1 + result3 - 2 * result2;
	}
	else if (haar_feature.type == HFT_L_ABBA)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.left.x;
		y = rect1.left.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x * 2, haar_feature.size.y};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		x = rect2.left.x;
		y = rect2.left.y;
		CB_SlantT slant3 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect3;
		slantToRect(slant3, rect3, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);
		double result3 = intg.getRectValue_45(rect3);

		result = result1 + result3 - result2;
	}
	else if (haar_feature.type == HFT_R_ABBA)
	{
		int x = cur_scan_pos_x + haar_feature.pos1.x;
		int y = cur_scan_pos_y + haar_feature.pos1.y;
		CB_SlantT slant1 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect1;
		slantToRect(slant1, rect1, subwin.image_size);

		x = rect1.right.x;
		y = rect1.right.y;
		CB_SlantT slant2 = {x, y, haar_feature.size.x, haar_feature.size.y * 2};
		CB_RectangleT rect2;
		slantToRect(slant2, rect2, subwin.image_size);

		x = rect2.right.x;
		y = rect2.right.y;
		CB_SlantT slant3 = {x, y, haar_feature.size.x, haar_feature.size.y};
		CB_RectangleT rect3;
		slantToRect(slant3, rect3, subwin.image_size);

		double result1 = intg.getRectValue_45(rect1);
		double result2 = intg.getRectValue_45(rect2);
		double result3 = intg.getRectValue_45(rect3);

		result = result1 + result3 - result2;
	}
	else if (haar_feature.type == HFT_A_B)
	{
		int x_left1 = cur_scan_pos_x + haar_feature.pos1.x;
		int x_right1 = x_left1 + haar_feature.size.x - 1;
		int y_top1 = cur_scan_pos_y + haar_feature.pos1.y;
		int y_bottom1 = y_top1 + haar_feature.size.y - 1;

		int x_left2 = cur_scan_pos_x + haar_feature.pos2.x;
		int x_right2 = x_left2 + haar_feature.size.x - 1;
		int y_top2 = cur_scan_pos_y + haar_feature.pos2.y;
		int y_bottom2 = y_top2 + haar_feature.size.y - 1;

		CB_RectT rect1 = {x_left1, x_right1, y_top1, y_bottom1};
		CB_RectT rect2 = {x_left2, x_right2, y_top2, y_bottom2};

		double result1 = intg.getRectValue_0(rect1);
		double result2 = intg.getRectValue_0(rect2);
		result = result1 - result2;
	}

	result *= haar_feature.inv_area;
	result = result / subwin.var;
	result /= real_scale_square;

	if (haar.is_abs == 1)
	{
		result = fabs(result);
	}
	return result;
}


int HaarFeature::slantToRect(const CB_SlantT &slant, CB_RectangleT &rect, const CB_PointT &image_size)
{
	CB_PointT left = {slant.x-slant.L, slant.y+slant.L};
	CB_PointT right = {slant.x+slant.R, slant.y+slant.R};
	CB_PointT top = {slant.x, slant.y};
	CB_PointT bottom = {slant.x-slant.L+slant.R, slant.y+slant.L+slant.R};

	if ( isPointValid(left, image_size) == 0 || 
		 isPointValid(right, image_size) == 0 ||
         isPointValid(bottom, image_size) == 0 )
	{
		return 0;
	}

	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;

	return 1;
}


inline int HaarFeature::isPointValid(const CB_PointT &point, const CB_PointT &image_size)
{
	if (point.x < 0 || point.x >= image_size.x || point.y < 0 || point.y >= image_size.y)
	{
		return 0;
	}
	return 1;
}
