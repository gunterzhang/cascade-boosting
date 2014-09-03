#pragma once
#include "TrainParams.h"

const int MAX_RECT_NUM = 500;

typedef struct
{
	int min_hit;
	double max_ratio_x;
	double max_ratio_y;
	double overlap_r_x;
	double overlap_r_y;
}MergerParamT;


typedef struct
{
	int is_valid;
	int num;
	CB_RectT rects[MAX_RECT_NUM];

	CB_RectT rect;
	int weight;
	int area;
}RectGroupT;


typedef struct
{
	CB_DBl_PointT overlap;
	CB_DBl_PointT overlap1;
	CB_DBl_PointT overlap2;
}OverlapT;


class ObjMerger
{
public:
	ObjMerger(void);
	~ObjMerger(void);

	int init(const MergerParamT &param);
	int merge(int num, CB_RectT *pt_rects);

private:
	void clearUp();
	int min(int a, int b);
	int max(int a, int b);
	int computeOverlap(CB_RectT &rect1, CB_RectT &rect2, OverlapT &overlap);
	int clusterRects(int num, CB_RectT *pt_rects);
	int	chooseRectGroup(int num, CB_RectT *pt_rects, int idx);
	int mergeTwoGroups(RectGroupT &group1, RectGroupT &group2);
	int processOverlapCluster();
	int computeGroupFeature(RectGroupT &group);

private:
	int final_rect_num;
	MergerParamT param;

	int rect_group_num;
	RectGroupT *rect_groups;
};

