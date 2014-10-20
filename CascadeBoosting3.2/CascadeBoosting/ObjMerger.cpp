#include "stdafx.h"
#include "ObjMerger.h"


ObjMerger::ObjMerger(void)
{
	rect_groups = NULL;
}


ObjMerger::~ObjMerger(void)
{
}


void ObjMerger::cleanUp()
{
	if (rect_groups != NULL)
	{
		delete []rect_groups;
		rect_groups = NULL;
	}
}


int ObjMerger::init(const MergerParamT &param)
{
	cleanUp();
	this->param = param;

	rect_groups = new RectGroupT[MAX_RECT_NUM];
	return 1;
}


int ObjMerger::merge(int num, CB_RectT *pt_rects)
{
	clusterRects(num, pt_rects);

	while (true)
	{
		int count = processOverlapCluster();
		if (count == 0)
		{
			break;
		}
	}
	
	final_rect_num = 0;
	for (int i=0; i<rect_group_num; i++)
	{
		if (rect_groups[i].is_valid == 0)
		{
			continue;
		}
		pt_rects[final_rect_num] = rect_groups[i].rect;
		final_rect_num++;
	}
	return final_rect_num;
}


int ObjMerger::computeOverlap(CB_RectT &rect1, CB_RectT &rect2, OverlapT &overlap)
{
	int w1 = rect1.right - rect1.left;
	int w2 = rect2.right - rect2.left;

	int h1 = rect1.bottom - rect1.top;
	int h2 = rect2.bottom - rect2.top;

	int left = min(rect1.left, rect2.left);
	int right = max(rect1.right, rect2.right);

	int top = min(rect1.top, rect2.top);
	int bottom = max(rect1.bottom, rect2.bottom);

	int dx = ((rect1.right - rect1.left) + (rect2.right - rect2.left)) - (right - left);
	int dy = ((rect1.bottom - rect1.top) + (rect2.bottom - rect2.top)) - (bottom - top);
	
	overlap.overlap.x = dx * 2.0 / ((rect1.right - rect1.left) + (rect2.right - rect2.left));
	overlap.overlap.y = dy * 2.0 / ((rect1.bottom - rect1.top) + (rect2.bottom - rect2.top));

	overlap.overlap1.x = dx / (rect1.right - rect1.left);
	overlap.overlap1.y = dy / (rect1.bottom - rect1.top);

	overlap.overlap2.x = dx / (rect2.right - rect2.left);
	overlap.overlap2.y = dy / (rect2.bottom - rect2.top);

	return 1;
}


int ObjMerger::clusterRects(int num, CB_RectT *pt_rects)
{
	rect_group_num = 0;
	for (int i=0; i<num; i++)
	{
		chooseRectGroup(num, pt_rects, i);
	}

	for (int i=0; i<rect_group_num; i++)
	{
		if (rect_groups[i].num < param.min_hit)
		{
			rect_groups[i].is_valid = 0;
		}
	}

	for (int i=0; i<rect_group_num; i++)
	{
		if (rect_groups[i].is_valid == 0)
		{
			continue;
		}
		computeGroupFeature(rect_groups[i]);
	}
	return 1;
}


int ObjMerger::chooseRectGroup(int num, CB_RectT *pt_rects, int idx)
{
	int is_found = 0;
	for (int i=0; i<rect_group_num; i++)
	{
		for (int j=0; j<rect_groups[i].num; j++)
		{
			OverlapT overlap;
			computeOverlap(pt_rects[idx], rect_groups[i].rects[j], overlap);
			if (overlap.overlap.x < param.overlap_r_x || overlap.overlap.y < param.overlap_r_y)
			{
				continue;
			}
			int cur_idx = rect_groups[i].num;
			rect_groups[i].rects[cur_idx] = pt_rects[idx];
			rect_groups[i].num++;
			is_found = 1;
			break;
		}
	}

	if (is_found != 0)
	{
		return 1;
	}

	rect_groups[rect_group_num].num = 1;
	rect_groups[rect_group_num].rects[0] = pt_rects[idx];
	rect_groups[rect_group_num].is_valid = 1;
	rect_group_num++;

	return 0;
}


int ObjMerger::processOverlapCluster()
{
	int count = 0;

	for (int i=0; i<rect_group_num; i++)
	{
		if (rect_groups[i].is_valid == 0)
		{
			continue;
		}
		for (int j=i+1; j<rect_group_num; j++)
		{
			if (rect_groups[j].is_valid == 0)
			{
				continue;
			}
			OverlapT overlap;
			computeOverlap(rect_groups[i].rect, rect_groups[j].rect, overlap);

			int w1 = abs(rect_groups[i].rect.right - rect_groups[i].rect.left);
			int w2 = abs(rect_groups[j].rect.right - rect_groups[j].rect.left);
			if ( min(overlap.overlap1.x, overlap.overlap1.y) > 0.3 &&
		         max(overlap.overlap1.x, overlap.overlap1.y) > 0.6 && 
				 w2 > w1 * 1.8 )
			{
				rect_groups[i].is_valid = 0;
				count++;
			}
			else if ( min(overlap.overlap2.x, overlap.overlap2.y) > 0.3 &&
		              max(overlap.overlap2.x, overlap.overlap2.y) > 0.6 && 
					  w1 > w2 * 1.8 )
			{
				rect_groups[j].is_valid = 0;
				count++;
			}
			
			if (overlap.overlap.x < MAX_OBJ_OVERLAP_X || overlap.overlap.y < MAX_OBJ_OVERLAP_Y)
			{
				continue;
			}

			if (overlap.overlap.x > 0.8 && overlap.overlap.y > 0.8)
			{
				mergeTwoGroups(rect_groups[i], rect_groups[j]);
				count++;
			}
			else if (rect_groups[i].weight > rect_groups[j].weight)
			{
				rect_groups[j].is_valid = 0;
				count++;
			}
			else if (rect_groups[j].weight > rect_groups[i].weight)
			{
				rect_groups[i].is_valid = 0;
				count++;
			}
			else if (rect_groups[i].rect.bottom > rect_groups[j].rect.bottom)
			{
				rect_groups[j].is_valid = 0;
				count++;
			}
			else
			{
				rect_groups[i].is_valid = 0;
				count++;
			}
		}
	}
	return count;
}


int ObjMerger::mergeTwoGroups(RectGroupT &group1, RectGroupT &group2)
{
	for (int i=0; i<group2.num; i++)
	{
		int idx = group1.num;
		group1.rects[idx] = group2.rects[i];
		group1.num++;
	}
	group2.is_valid = 0;
	computeGroupFeature(group1);
	return 1;
}


int ObjMerger::computeGroupFeature(RectGroupT &group)
{
		group.rect.left = 0;
		group.rect.right = 0;
		group.rect.top = 0;
		group.rect.bottom = 0;
		group.weight = 0;
		group.area = 0;

		for (int j=0; j<group.num; j++)
		{
			group.rect.left += group.rects[j].left;
			group.rect.right += group.rects[j].right;
			group.rect.top += group.rects[j].top;
			group.rect.bottom += group.rects[j].bottom;
		}

		group.weight = group.num;
		double inv_num = 1.0 / group.num;
		group.rect.left *= inv_num;
		group.rect.right *= inv_num;
		group.rect.top *= inv_num;
		group.rect.bottom *= inv_num;
		group.area = (group.rect.bottom - group.rect.top) * 
			         (group.rect.right - group.rect.left);
	return 1;
}


int ObjMerger::min(int a, int b)
{
	return (a < b) ? a : b;
}


int ObjMerger::max(int a, int b)
{
	return (a > b) ? a : b;
}