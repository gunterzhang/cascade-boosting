#include "stdafx.h"
#include "HaarFeature.h"


HaarParamT::~HaarParamT()
{
}


int HaarParamT::saveToModel(FILE *fp)
{
	fprintf(fp, "%d %d\n", tpl_size.x, tpl_size.y);
	fprintf(fp, "%d\n", feature_types);

	return 1;
}


int HaarParamT::loadFromModel(FILE *fp)
{
	fscanf(fp, "%d %d\n", &tpl_size.x, &tpl_size.y);
	fscanf(fp, "%d\n", &feature_types);

	return 1;
}


int HaarParamT::initFromConfig(FILE *fp)
{
	char tmp_str[1000];

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d", &bin_num);
	printf("HaarBinNum:      %d\n", bin_num);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d", &tpl_size.x);
	printf("TemplateWidth:   %d\n", tpl_size.x);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d", &tpl_size.y);
	printf("TemplateHeight:  %d\n", tpl_size.y);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d", &feature_types);
	printf("FeatureType:     %d\n", feature_types);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d", &is_abs);
	printf("FeatureAbs:      %d\n", is_abs);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d\n", &is_candid);
	printf("IsCandid:        %d\n", is_candid);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%s\n", tmp_str);
	candid_path = tmp_str;\
	if (is_candid > 0)
	{
		printf("CandidPath:      %s\n", candid_path.c_str());
	}

	return 1;
}


CB_PointT HaarParamT::getTemplateSize()
{
	return this->tpl_size;
}


int HaarParamT::getFeatureTypes()
{
	return feature_types;
}



HaarFeature::HaarFeature(void)
{
}


HaarFeature::~HaarFeature(void)
{
}


int HaarFeature::loadCandid(FILE *fp)
{
	char line[1000];
	fgets(line, sizeof(line), (FILE *)fp);
	sscanf(line, "%d %d %d %d %d %d %d %d %d %d %lf",
			&info.tpl_size.x, &info.tpl_size.y,
		    &info.type, &info.abs,
		    &info.pos1.x, &info.pos1.y,
		    &info.pos2.x, &info.pos2.y,	
			&info.size.x, &info.size.y, 
			&info.inv_area);
	return 1;
}


int HaarFeature::loadFromModel(FILE *fp)
{
	fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %lf ",
		   &info.tpl_size.x, &info.tpl_size.y,
		   &info.type, &info.abs,
		   &info.pos1.x, &info.pos1.y,
		   &info.pos2.x, &info.pos2.y,
		   &info.size.x, &info.size.y,
		   &info.inv_area);

	fscanf(fp, "%d %lf %lf %lf\n", &info.bin_num, &info.bin_min, &info.bin_max, &info.bin_width);
	return 1;
}


int HaarFeature::saveToModel(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "at");
	fprintf(fp, "%d %d %d %d %d %d %d %d %d %d %lf ",
			info.tpl_size.x, info.tpl_size.y,
			info.type, info.abs,
		    info.pos1.x, info.pos1.y,
		    info.pos2.x, info.pos2.y,
			info.size.x, info.size.y,
			info.inv_area);

	fprintf(fp, "%d %lf %lf %lf\n", info.bin_num, info.bin_min, info.bin_max, info.bin_width);
	fclose(fp);
	return 1;
}


int HaarFeature::computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const
{
	int cur_scan_pos_x = subwin.win_pos.x;
	int cur_scan_pos_y = subwin.win_pos.y;

	double cur_scan_scale = (double)subwin.win_size.x / info.tpl_size.x;
	HaarFeatureInfoT haar_feature = info;

	haar_feature.pos1.x = haar_feature.pos1.x * cur_scan_scale;
	haar_feature.pos1.y = haar_feature.pos1.y * cur_scan_scale;
	haar_feature.pos2.x = haar_feature.pos2.x * cur_scan_scale;
	haar_feature.pos2.y = haar_feature.pos2.y * cur_scan_scale;
	haar_feature.size.x *= cur_scan_scale;
	haar_feature.size.y *= cur_scan_scale;

	double real_scale_square = (double)(haar_feature.size.x * haar_feature.size.y) / (info.size.x * info.size.y);

	double result;
	switch (haar_feature.type)
	{
	case HFT_X_AB:
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
			break;
		}
	case HFT_Y_AB:
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
			break;
		}
	case HFT_X_ABA:
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
			break;
		}
	case HFT_Y_ABA:
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
			break;
		}
	case HFT_X_ABBA:
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
			break;
		}
	case HFT_Y_ABBA:
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
			break;
		}
	case HFT_XY_ABA:
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
			break;
		}
	case HFT_XY_ABBA:
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
			break;
		}
	case HFT_L_AB:
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
			break;
		}
	case HFT_R_AB:
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
			break;
		}
	case HFT_L_ABA:
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
			break;
		}
	case HFT_R_ABA:
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
			break;
		}
	case HFT_L_ABBA:
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
			break;
		}
	case HFT_R_ABBA:
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
			break;
		}
	case HFT_A_B:
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
			break;
		}
	case HFT_SQ_X_AB:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_middle = x_left + haar_feature.size.x - 1;
			int x_right = x_middle + haar_feature.size.x;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_bottom = y_top + haar_feature.size.y - 1;

			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_middle+1, x_right, y_top, y_bottom};

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 2 * result2;
			break;
		}
	case HFT_SQ_Y_AB:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_right = x_left + haar_feature.size.x - 1;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_middle = y_top + haar_feature.size.y - 1;
			int y_bottom = y_middle + haar_feature.size.y;
		
			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_left, x_right, y_middle+1, y_bottom};

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 2 * result2;
			break;
		}
	case HFT_SQ_X_ABA:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_middleL = x_left + haar_feature.size.x - 1;
			int x_middleR = x_middleL + haar_feature.size.x;
			int x_right = x_middleR + haar_feature.size.x;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_bottom = y_top + haar_feature.size.y - 1;

			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_middleL + 1, x_middleR, y_top, y_bottom};

			double result1 = intg.getRectSqValue_0(rect1); 
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 3 * result2;
			break;
		}
	case HFT_SQ_Y_ABA:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_right = x_left + haar_feature.size.x - 1;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_middleT = y_top + haar_feature.size.y - 1;
			int y_middleB = y_middleT + haar_feature.size.y;
			int y_bottom = y_middleB + haar_feature.size.y;

			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_left, x_right, y_middleT + 1, y_middleB};

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 3 * result2;
			break;
		}
	case HFT_SQ_X_ABBA:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_middleL = x_left + haar_feature.size.x - 1;
			int x_middleR = x_middleL + haar_feature.size.x * 2;
			int x_right = x_middleR + haar_feature.size.x;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_bottom = y_top + haar_feature.size.y - 1;

			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_middleL + 1, x_middleR, y_top, y_bottom};

			double result1 = intg.getRectSqValue_0(rect1); 
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 2 * result2;
			break;
		}
	case HFT_SQ_Y_ABBA:
		{
			int x_left = cur_scan_pos_x + haar_feature.pos1.x;
			int x_right = x_left + haar_feature.size.x - 1;
			int y_top = cur_scan_pos_y + haar_feature.pos1.y;
			int y_middleT = y_top + haar_feature.size.y - 1;
			int y_middleB = y_middleT + haar_feature.size.y * 2;
			int y_bottom = y_middleB + haar_feature.size.y;

			CB_RectT rect1 = {x_left, x_right, y_top, y_bottom};
			CB_RectT rect2 = {x_left, x_right, y_middleT + 1, y_middleB};

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 2 * result2;
			break;
		}
	case HFT_SQ_XY_ABA:
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

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 9 * result2;
			break;
		}
	case HFT_SQ_XY_ABBA:
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

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - 4 * result2;
			break;
		}
	case HFT_SQ_A_B:
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

			double result1 = intg.getRectSqValue_0(rect1);
			double result2 = intg.getRectSqValue_0(rect2);
			result = result1 - result2;
			break;
		}
	default:
		break;
	}
	result *= haar_feature.inv_area;
	result = result / subwin.var;
	result /= real_scale_square;

	if (haar_feature.type <HFT_STEP1 && haar_feature.abs == 1)
	{
		result = fabs(result);
	}

	HaarFeatureValueT &feature_value = (HaarFeatureValueT &)value;
	feature_value.value = result;
	return 1;
}


int HaarFeature::computeFeature(const IntegralImage &intg, const SubwinInfoT &subwin) const
{
	HaarFeatureValueT haar_feature_value;
	computeFeatureValue(intg, subwin, haar_feature_value);
	int index = computeFeatureIndex(haar_feature_value);
	return index;
}


int HaarFeature::computeFeatureIndex(const FeatureValueT &src_feature_value) const
{
	HaarFeatureValueT &haar_feature_value = (HaarFeatureValueT &)src_feature_value;
	double value = haar_feature_value.value;

	int bin_index;
	if (value < info.bin_min)
		bin_index = 0;
	else if (value > info.bin_max)
		bin_index = info.bin_num - 1;
	else 
		bin_index = (value - info.bin_min) * info.inv_bin_width + 1;

	return bin_index;
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


int HaarFeature::setParam(FeatureParamT *ptr_param)
{
	HaarParamT *ptr_haar_param = (HaarParamT *)ptr_param;
	info.bin_num = ptr_haar_param->bin_num;
	return 1;
}