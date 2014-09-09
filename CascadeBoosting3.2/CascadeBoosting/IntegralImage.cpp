#include "stdafx.h"
#include "IntegralImage.h"


IntegralImage::IntegralImage(void)
{
	s = NULL;
	pt_intg_0 = NULL;
	pt_sq_intg_0 = NULL;

	s1 = NULL;
	s2 = NULL;
	s1_temp = NULL;
	s2_temp = NULL;
	pt_intg_45 = NULL;
	pt_sq_intg_45 = NULL;
}


IntegralImage::~IntegralImage(void)
{
	clearUp();
}


void IntegralImage::clearUp()
{
	if (s != NULL)
	{
		delete []s;
		s = NULL;
	}
	if (pt_intg_0 != NULL)
	{
		delete []pt_intg_0;
		pt_intg_0 = NULL;
	}
	if (pt_sq_intg_0 != NULL)
	{
		delete []pt_sq_intg_0;
		pt_sq_intg_0 = NULL;
	}

	if (s1 != NULL)
	{
		delete []s1;
		s1 = NULL;
	}
	if (s2 != NULL)
	{
		delete []s2;
		s2 = NULL;
	}
	if (s1_temp != NULL)
	{
		delete []s1_temp;
		s1_temp = NULL;
	}
	if (s2_temp != NULL)
	{
		delete []s2_temp;
		s2_temp = NULL;
	}
	if (pt_intg_45 != NULL)
	{
		delete []pt_intg_45;
		pt_intg_45 = NULL;
	}
	if (pt_sq_intg_45 != NULL)
	{
		delete []pt_sq_intg_45;
		pt_sq_intg_45 = NULL;
	}
}


int IntegralImage::init(int w, int h, int type)
{
	clearUp();
	width = w;
	height = h;
	this->type = type;

	int len = w * h;
	s = new double[len];
	pt_intg_0 = new double[len];
	pt_sq_intg_0 = new double[len];

	s1 = new double[height];
	s2 = new double[height];
	s1_temp = new double[height];
	s2_temp = new double[height];
	pt_intg_45 = new double[len];
	pt_sq_intg_45 = new double[len];

	return 1;
}


const double *IntegralImage::computeIntegralImage_0(const uchar *pt_data)
{
	if (pt_intg_0 == NULL)
	{
		return NULL;
	}

	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			if (i == 0)
				s[j] = pt_data[j];
			else
				s[i*width+j] = s[(i-1)*width+j] + pt_data[i*width+j];

			if (j == 0) 
				pt_intg_0[i*width] = s[i*width];
			else
				pt_intg_0[i*width+j] = pt_intg_0[i*width+(j-1)] + s[i*width+j];
		}
	}
	return pt_intg_0;
}


const double *IntegralImage::computeSquareIntegralImage_0(const uchar *pt_data)
{
	if (pt_sq_intg_0 == NULL)
	{
		return NULL;
	}

	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			if (i == 0)
				s[j] = pt_data[j] * pt_data[j];
			else
				s[i*width+j] = s[(i-1)*width+j] + pt_data[i*width+j] * pt_data[i*width+j];

			if (j == 0) 
				pt_sq_intg_0[i*width] = s[i*width];
			else
				pt_sq_intg_0[i*width+j] = pt_sq_intg_0[i*width+(j-1)] + s[i*width+j];
		}
	}
	return pt_sq_intg_0;
}


const double *IntegralImage::computeIntegralImage_45(const uchar *pt_data)
{
	if (pt_intg_45 == NULL)
	{
		return NULL;
	}

	//colomn 0
	for (int i=0; i<height; i++)
	{
		int idx = i * width;
		s1[i] = pt_data[idx];
		s2[i] = pt_data[idx];
		pt_intg_45[idx] = pt_data[idx];
	}

	for (int j=1; j<width; j++)
	{
		for (int i=0; i<height; i++)
		{
			int idx = i * width + j;
			int idx1 = idx - 1;
			int s1_idx = i - 1;
			int s2_idx = i + 1;
			double s_1;
			double s_2;

			if (s1_idx < 0)
				s_1 = 0;
			else
				s_1 = s1[s1_idx];
			if (s2_idx > height - 1)
				s_2 = 0;
			else
				s_2 = s2[s2_idx];
			pt_intg_45[idx] = pt_intg_45[idx1] + s_1 + s_2 + pt_data[idx];
		}
		memcpy(s1_temp, s1, height * sizeof(s1[0]));
		memcpy(s2_temp, s2, height * sizeof(s2[0]));
		//update s1 and s2
		s1_temp[0] = pt_data[j];
		for (int i=1; i<height-1; i++)
		{
			int idx = i * width + j;
			int s1_idx = i - 1;
			int s2_idx = i + 1;
			s1_temp[i] = s1[s1_idx] + pt_data[idx];
			s2_temp[i] = s2[s2_idx] + pt_data[idx];
		}
		s2_temp[height-1] = pt_data[(height-1) * width + j];
		memcpy(s1, s1_temp, height * sizeof(s1[0]));
		memcpy(s2, s2_temp, height * sizeof(s2[0]));
	}
	return pt_intg_45;
}


const double *IntegralImage::computeSquareIntegralImage_45(const uchar *pt_data)
{
	if (pt_sq_intg_45 == NULL)
	{
		return NULL;
	}

	//colomn 0
	for (int i=0; i<height; i++)
	{
		int idx = i * width;
		double temp = pt_data[idx] * pt_data[idx];
		s1[i] = temp;
		s2[i] = temp;
		pt_sq_intg_45[idx] = temp;
	}

	for (int j=1; j<width; j++)
	{
		for (int i=0; i<height; i++)
		{
			int idx = i * width + j;
			int idx1 = idx - 1;
			int s1_idx = i - 1;
			int s2_idx = i + 1;
			double s_1;
			double s_2;

			if (s1_idx < 0)
				s_1 = 0;
			else
				s_1 = s1[s1_idx];
			if (s2_idx > height - 1)
				s_2 = 0;
			else
				s_2 = s2[s2_idx];
			pt_sq_intg_45[idx] = pt_sq_intg_45[idx1] + s_1 + s_2 + pt_data[idx] * pt_data[idx];
		}
		memcpy(s1_temp, s1, height * sizeof(s1[0]));
		memcpy(s2_temp, s2, height * sizeof(s2[0]));
		//update s1 and s2
		s1_temp[0] = pt_data[j] * pt_data[j];
		for (int i=1; i<height-1; i++)
		{
			int idx = i * width + j;
			int s1_idx = i - 1;
			int s2_idx = i + 1;
			double temp = pt_data[idx] * pt_data[idx];
			s1_temp[i] = s1[s1_idx] + temp;
			s2_temp[i] = s2[s2_idx] + temp;
		}
		int idx = (height-1) * width + j;
		s2_temp[height-1] = pt_data[idx] * pt_data[idx];
		memcpy(s1, s1_temp, height * sizeof(s1[0]));
		memcpy(s2, s2_temp, height * sizeof(s2[0]));
	}
	return pt_sq_intg_45;
}


int IntegralImage::compute(const uchar *pdata)
{
	computeIntegralImage_0(pdata);
	computeSquareIntegralImage_0(pdata);

	if ((type & SLANT_HAAR) > 0)
	{
		computeIntegralImage_45(pdata);
		computeSquareIntegralImage_45(pdata);
	}

	return 1;
}


int IntegralImage::load(FILE *fp)
{
	int count = fread(&width, sizeof(width), 1, fp);
	if (count <= 0)
	{
		return 0;
	}
	count = fread(&height, sizeof(height), 1, fp);
	count = fread(&type, sizeof(type), 1, fp);

	init(width, height, type);

	int len = width * height;

	count = fread(pt_intg_0, sizeof(pt_intg_0[0]), len, fp);
	count = fread(pt_sq_intg_0, sizeof(pt_sq_intg_0[0]), len, fp);

	if ((type & SLANT_HAAR) > 0)
	{
		count = fread(pt_intg_45, sizeof(pt_intg_45[0]), len, fp);
		count = fread(pt_sq_intg_45, sizeof(pt_sq_intg_45[0]), len, fp);
	}
	return 1;
}


int IntegralImage::save(const string &path)
{
	FILE *fp = fopen(path.c_str(), "ab");
	save(fp);
	fclose(fp);

	return 1;
}


int IntegralImage::save(FILE *fp)
{
	fwrite(&width, sizeof(width), 1, fp);
	fwrite(&height, sizeof(height), 1, fp);
	fwrite(&type, sizeof(type), 1, fp);

	int len = width * height;

	fwrite(pt_intg_0, sizeof(pt_intg_0[0]), len, fp);
	fwrite(pt_sq_intg_0, sizeof(pt_sq_intg_0[0]), len, fp);

	if ((type & SLANT_HAAR) > 0)
	{
		fwrite(pt_intg_45, sizeof(pt_intg_45[0]), len, fp);
		fwrite(pt_sq_intg_45, sizeof(pt_sq_intg_45[0]), len, fp);
	}
	return 1;
}


int IntegralImage::computeSubwinMeanVar(SubwinInfoT &subwin)
{
	int left = subwin.win_pos.x;
	int right = left + subwin.win_size.x - 1;
	int top = subwin.win_pos.y;
	int bottom = top + subwin.win_size.y - 1;

	CB_RectT rect = {left, right, top, bottom};

	double inv_area = 1.0 / (subwin.win_size.x * subwin.win_size.y);
	subwin.mean = getRectValue_0(rect);
	subwin.mean *= inv_area;

	subwin.var = getRectSqValue_0(rect);
	subwin.var *= inv_area;
	subwin.var -= subwin.mean * subwin.mean;
	if (subwin.var < 1)
	{
		subwin.var = 1;
	}
	subwin.var = sqrt(subwin.var);

	return 1;
}


inline double IntegralImage::getValue_0(const CB_PointT &pos)
{
	if (pos.x < 0 || pos.y < 0)
		return 0;

	int idx = width * pos.y + pos.x;
	double value = pt_intg_0[idx];
	return value;
}


inline double IntegralImage::getSqureValue_0(const CB_PointT &pos)
{
	if (pos.x < 0 || pos.y < 0)
		return 0;

	int idx = width * pos.y + pos.x;
	double value = pt_sq_intg_0[idx];
	return value;
}


inline double IntegralImage::getValue_45(const CB_PointT &pos)
{
	if (pos.x < 0 || pos.y < 0)
		return 0;

	int idx = width * pos.y + pos.x;
	double value = pt_intg_45[idx];
	return value;
}


inline double IntegralImage::getSqureValue_45(const CB_PointT &pos)
{
	if (pos.x < 0 || pos.y < 0)
		return 0;

	int idx = width * pos.y + pos.x;
	double value = pt_sq_intg_45[idx];
	return value;
}


double IntegralImage::getRectValue_0(const CB_RectT &rect)
{
	CB_PointT left_top = {rect.left-1, rect.top-1};
	CB_PointT right_top = {rect.right, rect.top-1};
	CB_PointT left_bottom = {rect.left-1, rect.bottom};
	CB_PointT right_bottom = {rect.right, rect.bottom};

	double value = getValue_0(right_bottom) 
		         + getValue_0(left_top)
				 - getValue_0(right_top)
				 - getValue_0(left_bottom);
	return value;
}


double IntegralImage::getRectSqValue_0(const CB_RectT &rect)
{
	CB_PointT left_top = {rect.left-1, rect.top-1};
	CB_PointT right_top = {rect.right, rect.top-1};
	CB_PointT left_bottom = {rect.left-1, rect.bottom};
	CB_PointT right_bottom = {rect.right, rect.bottom};

	double value = getSqureValue_0(right_bottom) 
		         + getSqureValue_0(left_top)
				 - getSqureValue_0(right_top)
				 - getSqureValue_0(left_bottom);
	return value;
}


double IntegralImage::getRectValue_45(const CB_RectangleT &rect)
{
	double value = getValue_45(rect.right) 
				 + getValue_45(rect.left)
				 - getValue_45(rect.top)
				 - getValue_45(rect.bottom);
	return value;
}


double IntegralImage::getRectSqValue_45(const CB_RectangleT &rect)
{
	double value = getSqureValue_45(rect.right) 
				 + getSqureValue_45(rect.left)
				 - getSqureValue_45(rect.top)
				 - getSqureValue_45(rect.bottom);
	return value;
}
