#pragma once
#include "TrainParams.h"
#include "IntegralImage.h"

const int HAAR_SHIFT_STEP_X = 2;
const int HAAR_SHIFT_STEP_Y = 2;
const int HAAR_SCALE_STEP = 2;
const int FEATURE_MARGIN = 1;


const double INV_AREA_R = 500.0;

const int MAX_FEATURE_TYPE_NUM = 20;

typedef enum {
	HFT_X_AB = 0,
	HFT_Y_AB = 1,
	HFT_X_ABA = 2,
	HFT_Y_ABA = 3,
	HFT_X_ABBA = 4,
	HFT_Y_ABBA = 5,
	HFT_XY_ABA = 6,
	HFT_XY_ABBA = 7,
	HFT_L_AB = 8,
	HFT_R_AB = 9,
	HFT_L_ABA = 10,
	HFT_R_ABA = 11,
	HFT_L_ABBA = 12,
	HFT_R_ABBA = 13,
	HFT_A_B = 14
} FeatureTypeT;

typedef struct
{
	FeatureTypeT type;
	int is_abs;
	CB_PointT pos1;
	CB_PointT pos2;
	CB_PointT size;
	CB_PointT tpl_size;
	double inv_area;
}HaarFeatureInfoT;


class HaarFeature
{
public:
	HaarFeature(void);
	~HaarFeature(void);

	int init(int w, int h, int type, int is_abs);
	int getFeatureNum();
	const float *extractBatchFeatures(FILE *fp);
	int saveTrainingData(const unsigned char *pdata, const string &file_path);
	static float computeFeature(IntegralImage &intg, const SubwinInfoT &subwin, const HaarFeatureInfoT &haar);

private:
	void clearUp();
	int getAllFeatureInfos(int is_extract_feature = 0, FILE *fp = NULL, float *pt_feature = NULL);
	float extractFeature(const HaarFeatureInfoT &info);
	int extractFeature(const IntegralImage &intg, SubwinInfoT &subwin_info, const HaarFeatureInfoT &info);
	int extractOneTypeFeatures(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature);
	int extractOneTypeFeatures45(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature);
	int extractOneTypeFeaturesAB(int is_extract_feature, HaarFeatureInfoT &info, float *pt_feature);
	static int slantToRect(const CB_SlantT &slant, CB_RectangleT &rect, const CB_PointT &image_size);
	static inline int isPointValid(const CB_PointT &point, const CB_PointT &image_size);

private:
	IntegralImage intg;

	float *pt_features;
	int *pt_flags;

	int template_w;
	int template_h;

	int feature_num;
	int sample_num;

	int feature_types;
	int is_abs;

	CB_PointT feature_sizes[MAX_FEATURE_TYPE_NUM];
	int feature_inv_ratio[MAX_FEATURE_TYPE_NUM];

	int feature_count;

public:	
	HaarFeatureInfoT *pt_feature_infos;
};

