#pragma once
#include "Utils.h"

class IntegralImage
{
public:
	IntegralImage(void);
	~IntegralImage(void);

	int init(int w, int h, int type);

	int compute(const uchar *pdata);
	const double *computeIntegralImage_0(const uchar *pt_data);
	const double *computeIntegralImage_45(const uchar *pt_data);
	const double *computeSquareIntegralImage_0(const uchar *pt_data);
	const double *computeSquareIntegralImage_45(const uchar *pt_data);

	double getRectValue_0(const CB_RectT &rect) const;
	double getRectSqValue_0(const CB_RectT &rect) const;
	double getRectValue_45(const CB_RectangleT &rect) const;
	double getRectSqValue_45(const CB_RectangleT &rect) const;

	int load(FILE *fp);
	int save(FILE *fp);
	int save(const string &path);
	int computeSubwinMeanVar(SubwinInfoT &subwin);

private:
	void cleanUp();
	inline double getValue_0(const CB_PointT &pos) const;
	inline double getValue_45(const CB_PointT &pos) const;
	inline double getSqureValue_0(const CB_PointT &pos) const;
	inline double getSqureValue_45(const CB_PointT &pos) const;

public:
	double *pt_intg_0;
	double *pt_intg_45;

	double *pt_sq_intg_0;
	double *pt_sq_intg_45;

	int width;
	int height;
	int type;

private:
	double *s;
	double *s1;
	double *s2;
	double *s1_temp;
	double *s2_temp;

	int data_size;
};

