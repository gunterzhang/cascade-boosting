#include "stdafx.h"
#include "HaarFeatureHub.h"


HaarFeatureHub::HaarFeatureHub(void)
{
	feature_num = 0;
	pt_haars = NULL;
	pt_features = NULL;

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

	//HFT_SQ_X_Y
	feature_sizes[HFT_SQ_X_AB].x = 2;
	feature_sizes[HFT_SQ_X_AB].y = 1;
	feature_inv_ratio[HFT_SQ_X_AB] = 1;
	
	feature_sizes[HFT_SQ_Y_AB].x = 1;
	feature_sizes[HFT_SQ_Y_AB].y = 2;
	feature_inv_ratio[HFT_SQ_Y_AB] = 1;

	feature_sizes[HFT_SQ_X_ABA].x = 3;
	feature_sizes[HFT_SQ_X_ABA].y = 1;
	feature_inv_ratio[HFT_SQ_X_ABA] = 2;
	
	feature_sizes[HFT_SQ_Y_ABA].x = 1;
	feature_sizes[HFT_SQ_Y_ABA].y = 3;
	feature_inv_ratio[HFT_SQ_Y_ABA] = 2;

	feature_sizes[HFT_SQ_X_ABBA].x = 4;
	feature_sizes[HFT_SQ_X_ABBA].y = 1;
	feature_inv_ratio[HFT_SQ_X_ABBA] = 2;
	
	feature_sizes[HFT_SQ_Y_ABBA].x = 1;
	feature_sizes[HFT_SQ_Y_ABBA].y = 4;
	feature_inv_ratio[HFT_SQ_Y_ABBA] = 2;

	feature_sizes[HFT_SQ_XY_ABA].x = 3;
	feature_sizes[HFT_SQ_XY_ABA].y = 3;
	feature_inv_ratio[HFT_SQ_XY_ABA] = 8;

	feature_sizes[HFT_SQ_XY_ABBA].x = 4;
	feature_sizes[HFT_SQ_XY_ABBA].y = 4;
	feature_inv_ratio[HFT_SQ_XY_ABBA] = 12;

	//HFT_SQ_A_B
	feature_sizes[HFT_SQ_A_B].x = 1;
	feature_sizes[HFT_SQ_A_B].y = 1;
	feature_inv_ratio[HFT_SQ_A_B] = 1;
}


HaarFeatureHub::~HaarFeatureHub(void)
{
	clearUp();
}


void HaarFeatureHub::clearUp()
{
	feature_num = 0;

	if (pt_haars != NULL)
	{
		delete []pt_haars;
		pt_haars = NULL;
	}

	if (pt_features != NULL)
	{
		delete []pt_features;
		pt_features = NULL;
	}
}


int HaarFeatureHub::init(int w, int h, int type, int is_abs)
{
	clearUp();

	feature_types = type;
	feature_abs = is_abs;

	template_w = w;
	template_h = h;

	intg.init(w, h, type);

	feature_num = getAllFeatureInfos(-1);

	pt_features = new float[feature_num];
	pt_haars = new HaarFeature[feature_num];
	
	getAllFeatureInfos(0);

	return 1;
}


const float *HaarFeatureHub::extractAllFeatures(FILE *fp)
{
	int rst = getAllFeatureInfos(1, fp);

	if (rst <0)
		return NULL;
	else
		return pt_features;
}


int HaarFeatureHub::getFeatureNum()
{
	return feature_num;
}


int HaarFeatureHub::getAllFeatureInfos(int is_extract_feature, FILE *fp)
{
	if (is_extract_feature > 0)
	{
		if (fp == NULL)
		{
			return -1;
		}
		intg.load(fp);
	}

	HaarFeature haar;
	HaarFeatureInfoT &info = haar.info;
	info.abs = feature_abs;
	info.tpl_size.x = template_w;
	info.tpl_size.y = template_h;
	info.pos2.x = 0;
	info.pos2.y = 0;

	feature_count = 0;

	if ((feature_types & UPRIGHT_HAAR) > 0)
	{
		info.type = HFT_X_AB; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_Y_AB;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_X_ABA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_Y_ABA;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_X_ABBA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_Y_ABBA;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_XY_ABA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_XY_ABBA;
		extractOneTypeFeatures(is_extract_feature, haar);
	}


	if ((feature_types & UPRIGHT_SQ_HAAR) > 0)
	{
		info.type = HFT_SQ_X_AB; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_Y_AB;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_X_ABA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_Y_ABA;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_X_ABBA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_Y_ABBA;
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_XY_ABA; 
		extractOneTypeFeatures(is_extract_feature, haar);
		info.type = HFT_SQ_XY_ABBA;
		extractOneTypeFeatures(is_extract_feature, haar);
	}

	if ((feature_types & SLANT_HAAR) > 0)
	{
		info.type = HFT_L_AB; 
		extractOneTypeFeatures45(is_extract_feature, haar);
		info.type = HFT_R_AB;
		extractOneTypeFeatures45(is_extract_feature, haar);
		info.type = HFT_L_ABA; 
		extractOneTypeFeatures45(is_extract_feature, haar);
		info.type = HFT_R_ABA;
		extractOneTypeFeatures45(is_extract_feature, haar);
		info.type = HFT_L_ABBA; 
		extractOneTypeFeatures45(is_extract_feature, haar);
		info.type = HFT_R_ABBA;
		extractOneTypeFeatures45(is_extract_feature, haar);
	}

	if ((feature_types & UPRIGHT_FAR_HAAR) > 0)
	{
		info.type = HFT_A_B; 
		extractOneTypeFeaturesAB(is_extract_feature, haar);
	}

	if ((feature_types & UPRIGHT_SQ_FAR_HAAR) > 0)
	{
		info.type = HFT_SQ_A_B; 
		extractOneTypeFeaturesAB(is_extract_feature, haar);
	}

	if (is_extract_feature < 0)
	{
		feature_num = feature_count;
		printf("feature Num = %d\n", feature_num);
	}

	return feature_num;
}


int HaarFeatureHub::extractOneTypeFeatures(int is_extract_feature, HaarFeature &haar)
{
	HaarFeatureInfoT &info = haar.info;
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
						pt_haars[feature_count].info = info;
					}
					else if (is_extract_feature > 0)
					{
						*(pt_features + feature_count) = extractFeature(pt_haars[feature_count]);
					}
					feature_count++;
					info.size.x += HAAR_SCALE_STEP_X;
				}
				info.size.y += HAAR_SCALE_STEP_Y;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}


int HaarFeatureHub::extractOneTypeFeatures45(int is_extract_feature, HaarFeature &haar)
{
	HaarFeatureInfoT &info = haar.info;
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
					int result = HaarFeature::slantToRect(slant, rect, image_size);
					if (result == 0)
					{
						info.size.x += HAAR_SCALE_STEP_X;
						continue;
					}
					if (is_extract_feature == 0)
					{
						info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
						pt_haars[feature_count].info = info;
					}
					else if (is_extract_feature > 0)
					{
						*(pt_features + feature_count) = extractFeature(pt_haars[feature_count]);
					}
					feature_count++;
					info.size.x += HAAR_SCALE_STEP_X;
				}
				info.size.y += HAAR_SCALE_STEP_Y;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}


int HaarFeatureHub::extractOneTypeFeaturesAB(int is_extract_feature, HaarFeature &haar)
{
	HaarFeatureInfoT &info = haar.info;
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
							pt_haars[feature_count].info = info;
						}
						else if (is_extract_feature > 0)
						{
							*(pt_features + feature_count) = extractFeature(pt_haars[feature_count]);
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
							pt_haars[feature_count].info = info;
						}
						else if (is_extract_feature > 0)
						{
							*(pt_features + feature_count) = extractFeature(pt_haars[feature_count]);
						}
						info.pos2.y += HAAR_SHIFT_STEP_Y * 2;
						feature_count++;
					}
					info.size.x += HAAR_SCALE_STEP_X;
				}
				info.size.y += HAAR_SCALE_STEP_Y;
			}
			info.pos1.x += HAAR_SHIFT_STEP_X;
		}
		info.pos1.y += HAAR_SHIFT_STEP_Y;
	}
	return feature_count;
}


float HaarFeatureHub::extractFeature(const HaarFeature &haar)
{
	SubwinInfoT subwin;
	subwin.image_size.x = intg.width;
	subwin.image_size.y = intg.height;
	subwin.win_size = subwin.image_size;
	subwin.win_pos.x = 0;
	subwin.win_pos.y = 0;

	intg.computeSubwinMeanVar(subwin);

	float feature_value = haar.computeFeature(intg, subwin);
	return feature_value;
}