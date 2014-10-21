#include "stdafx.h"
#include "HaarFeatureHub.h"


HaarFeatureHub::HaarFeatureHub(void)
{
	p_haars = NULL;

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
	cleanUp();
}


void HaarFeatureHub::cleanUp()
{
	feature_num = 0;

	if (p_haars != NULL)
	{
		delete []p_haars;
		p_haars = NULL;
	}
}


int HaarFeatureHub::init(const FeatureParamT &param)
{
	cleanUp();
	haar_param = (const HaarParamT &)param;
	intg.init(haar_param.tpl_size.x, haar_param.tpl_size.y, haar_param.feature_types);

	if (haar_param.is_candid == 1)
	{
		int rst = initFromFile();
		return rst;
	}

	feature_num = getAllFeatureInfos(-1);
	p_haars = new HaarFeature[feature_num];
	getAllFeatureInfos(0);

	return 1;
}


int HaarFeatureHub::initFromFile()
{
	FILE *fp = fopen(haar_param.candid_path.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}

	CB_PointT tpl_size;
	fscanf(fp, "%d %d\n", &tpl_size.x, &tpl_size.y);
	if (tpl_size.x != haar_param.tpl_size.x || tpl_size.y != haar_param.tpl_size.y)
	{
		return 0;
	}

	fscanf(fp, "%d\n", &feature_num);
	p_haars = new HaarFeature[feature_num];

	for (int i=0; i<feature_num; i++)
	{
		p_haars[i].initFromFile(fp, haar_param);
	}
	fclose(fp);

	printf("\n");
	return 1;
}


int HaarFeatureHub::extractAllFeatures(FILE *fp, HaarFeatureValueT *pt_features)
{
	int rst = intg.load(fp);
	if (rst == 0)
	{
		return 0;
	}

	for (int i=0; i<feature_num; i++)
	{
		(pt_features + i)->value = extractFeature(p_haars[i]);
	}
	return 1;
}


int HaarFeatureHub::getFeatureNum()
{
	return feature_num;
}


int HaarFeatureHub::getAllFeatureInfos(int is_extract_feature)
{
	HaarFeatureInfoT info;
	info.abs = haar_param.is_abs;
	info.bin_min = haar_param.bin_num;
	info.tpl_size = haar_param.tpl_size;
	info.bin_num = haar_param.bin_num;

	info.pos2.x = 0;
	info.pos2.y = 0;

	feature_count = 0;

	if ((haar_param.feature_types & UPRIGHT_HAAR) > 0)
	{
		info.type = HFT_X_AB; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_Y_AB;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_X_ABA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_Y_ABA;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_X_ABBA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_Y_ABBA;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_XY_ABA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_XY_ABBA;
		extractOneTypeFeatures(is_extract_feature, info);
	}

	if ((haar_param.feature_types & UPRIGHT_SQ_HAAR) > 0)
	{
		info.type = HFT_SQ_X_AB; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_Y_AB;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_X_ABA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_Y_ABA;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_X_ABBA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_Y_ABBA;
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_XY_ABA; 
		extractOneTypeFeatures(is_extract_feature, info);
		info.type = HFT_SQ_XY_ABBA;
		extractOneTypeFeatures(is_extract_feature, info);
	}

	if ((haar_param.feature_types & SLANT_HAAR) > 0)
	{
		info.type = HFT_L_AB; 
		extractOneTypeFeatures45(is_extract_feature, info);
		info.type = HFT_R_AB;
		extractOneTypeFeatures45(is_extract_feature, info);
		info.type = HFT_L_ABA; 
		extractOneTypeFeatures45(is_extract_feature, info);
		info.type = HFT_R_ABA;
		extractOneTypeFeatures45(is_extract_feature, info);
		info.type = HFT_L_ABBA; 
		extractOneTypeFeatures45(is_extract_feature, info);
		info.type = HFT_R_ABBA;
		extractOneTypeFeatures45(is_extract_feature, info);
	}

	if ((haar_param.feature_types & UPRIGHT_FAR_HAAR) > 0)
	{
		info.type = HFT_A_B; 
		extractOneTypeFeaturesAB(is_extract_feature, info);
	}

	if ((haar_param.feature_types & UPRIGHT_SQ_FAR_HAAR) > 0)
	{
		info.type = HFT_SQ_A_B; 
		extractOneTypeFeaturesAB(is_extract_feature, info);
	}

	if (is_extract_feature < 0)
	{
		feature_num = feature_count;
		printf("feature Num = %d\n", feature_num);
	}

	return feature_num;
}


int HaarFeatureHub::extractOneTypeFeatures(int is_extract_feature, HaarFeatureInfoT &info)
{
	int type = info.type;
	int x_r = feature_sizes[type].x;
	int y_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type];

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < haar_param.tpl_size.y - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < haar_param.tpl_size.x - 1)
		{
			info.size.y = 2;
			while (info.pos1.y + y_r * info.size.y - 1 < haar_param.tpl_size.y - FEATURE_MARGIN)
			{
				info.size.x = 2; 
				while (info.pos1.x + x_r * info.size.x - 1 < haar_param.tpl_size.x - FEATURE_MARGIN)
				{
					if (is_extract_feature == 0)
					{
						info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
						p_haars[feature_count].info = info;
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


int HaarFeatureHub::extractOneTypeFeatures45(int is_extract_feature, HaarFeatureInfoT &info)
{
	int type = info.type;
	int L_r = feature_sizes[type].x;
	int R_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type] * 2;

	CB_PointT image_size = haar_param.tpl_size;

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < haar_param.tpl_size.y - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < haar_param.tpl_size.x - 1)
		{
			info.size.y = 2;
			while (info.pos1.x + R_r * info.size.y < haar_param.tpl_size.x)
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
						p_haars[feature_count].info = info;
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


int HaarFeatureHub::extractOneTypeFeaturesAB(int is_extract_feature, HaarFeatureInfoT &info)
{
	int type = info.type;
	int x_r = feature_sizes[type].x;
	int y_r = feature_sizes[type].y;
	double inv_r = feature_inv_ratio[type];

	info.pos1.y = FEATURE_MARGIN;
	while (info.pos1.y < haar_param.tpl_size.y - 1)
	{
		info.pos1.x = FEATURE_MARGIN;
		while (info.pos1.x < haar_param.tpl_size.x - 1)
		{
			info.size.y = 2;
			while (info.pos1.y + y_r * info.size.y - 1 < haar_param.tpl_size.y - FEATURE_MARGIN)
			{
				info.size.x = 2; 
				while (info.pos1.x + x_r * info.size.x - 1 < haar_param.tpl_size.x - FEATURE_MARGIN)
				{
					info.pos2.y = info.pos1.y;
					info.pos2.x = info.pos1.x + info.size.x + HAAR_SHIFT_STEP_X;
					while (info.pos2.x + x_r * info.size.x - 1 < haar_param.tpl_size.x - FEATURE_MARGIN)
					{
						if (is_extract_feature == 0)
						{
							info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
							p_haars[feature_count].info = info;
						}
						info.pos2.x += HAAR_SHIFT_STEP_X * 2;
						feature_count++;
					}

					info.pos2.x = info.pos1.x;
					info.pos2.y = info.pos1.y + info.size.y + HAAR_SHIFT_STEP_Y;
					while (info.pos2.y + y_r * info.size.y - 1 < haar_param.tpl_size.y - FEATURE_MARGIN)
					{
						if (is_extract_feature == 0)
						{
							info.inv_area = INV_AREA_R / (info.size.x * info.size.y * inv_r);
							p_haars[feature_count].info = info;
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

	HaarFeatureValueT value;
	haar.computeFeatureValue(intg, subwin, value);
	return value.value;
}


int HaarFeatureHub::trainFeatures(int pos_num, FeatureValueT *ptr_pos_values,
	                              int neg_num, FeatureValueT *ptr_neg_values)
{
	HaarFeatureValueT *ptr_pos_haar_values = (HaarFeatureValueT*)ptr_pos_values;
	HaarFeatureValueT *ptr_neg_haar_values = (HaarFeatureValueT*)ptr_neg_values;

	for (int i=0; i<feature_num; i++)
	{
		printf("Haar Feature = %d -- %d\r", feature_num, i+1);
		HaarFeature &haar = p_haars[i];
		HaarFeatureInfoT &info = haar.info;

		//-------------------------------------
		double min_positive_value = 999999;
		double max_positive_value = -999999;

		for (int j=0; j<pos_num; j++)
		{
			int idx = j * feature_num + i;
			double value = ptr_pos_haar_values[idx].value;
			if (value < min_positive_value) 
				min_positive_value = value;

			if (value > max_positive_value)
				max_positive_value = value;
		}				

		//-------------------------------------
		double min_negative_value = 999999;
		double max_negative_value = -999999;

		for (int j=0; j<neg_num; j++)
		{
			int idx = j * feature_num + i;
			double value = ptr_neg_haar_values[idx].value;
			if (value < min_negative_value) 
				min_negative_value = value;

			if (value > max_negative_value) 
				max_negative_value = value;
		}				
		
		//-------------------------------------
		info.bin_max = ceil(min(max_positive_value, max_negative_value));
		info.bin_min = floor(max(min_positive_value, min_negative_value));
		info.bin_width = (info.bin_max - info.bin_min) / double(info.bin_num - 2);
		info.inv_bin_width = 1.0 / double(info.bin_width);
	
		for (int j=0; j<pos_num; j++)
		{
			int idx_i = i * pos_num + j;
			int idx_v = j * feature_num + i;
			ptr_pos_haar_values[idx_i].index = haar.computeFeatureIndex(ptr_pos_haar_values[idx_v]);
		}

		for (int j=0; j<neg_num; j++)
		{
			int idx_i = i * neg_num + j;
			int idx_v = j * feature_num + i;
			ptr_neg_haar_values[idx_i].index = haar.computeFeatureIndex(ptr_neg_haar_values[idx_v]);
		}
	}
	return 1;
}


int HaarFeatureHub::extractFeatures(int sample_num, const string &data_path, FeatureValueT *ptr_features)
{
	HaarFeatureValueT *ptr_haar_features = (HaarFeatureValueT *)ptr_features;
	FILE *fp = fopen(data_path.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}
	for (int i=0; i<sample_num; i++)
	{
		printf("%d -- %d\r", sample_num, i+1);
		int rst = extractAllFeatures(fp, ptr_haar_features + i * feature_num);
		if (rst == 0)
		{
			break;
		}
	}
	fclose(fp);

	printf("\n");
	return 1;
}


FeatureValueT *HaarFeatureHub::createTrainingMemory(int num)
{
	HaarFeatureValueT *pt_featureValue = new HaarFeatureValueT[num * feature_num];
	return (FeatureValueT *)pt_featureValue;
}


int HaarFeatureHub::releaseTrainingMemory(FeatureValueT *&pt_featureValue)
{
	if (pt_featureValue == NULL) 
	{
		return 0;
	}

	delete [](HaarFeatureValueT *)pt_featureValue;
	return 1;
}


int HaarFeatureHub::getPosFeatureIdx(int sampleIdx, int featureIdx)
{
	int idx = featureIdx * pos_num + sampleIdx;
	return ((HaarFeatureValueT *)p_pos_features)[idx].index;
}


int HaarFeatureHub::getNegFeatureIdx(int sampleIdx, int featureIdx)
{
	int idx = featureIdx * neg_num + sampleIdx;
	return ((HaarFeatureValueT *)p_neg_features)[idx].index;
}


Feature *HaarFeatureHub::getFeature(int idx)
{
	return &(p_haars[idx]);
}