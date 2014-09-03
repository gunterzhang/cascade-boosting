#include "stdafx.h"
#include "Tester.h"


Tester::Tester(void)
{
}


Tester::~Tester(void)
{
}


void Tester::testIntg()
{
	IntegralImage intg;
	int w = 10;
	int h = 10;
	intg.init(w, h, 3);
	uchar *pdata = new uchar[w * h];
	memset(pdata, 2, w * h);
	const double *pt_intg_0 = intg.computeIntegralImage_0(pdata);
	const double *pt_sq_intg_0 = intg.computeSquareIntegralImage_0(pdata);

	delete []pdata;

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int idx = i * w + j;
			if (int(pt_intg_0[idx]) != (i + 1)*(j + 1) * 2)
			{
				printf("error\n");
			}
			if (int(pt_sq_intg_0[idx]) != (i + 1)*(j + 1) * 4)
			{
				printf("error\n");
			}
		}
	}

	CB_RectT rect = {0, 9, 0, 9};
	double value = intg.getRectValue_0(rect);
	if ((int)value != w * h * 2)
	{
		printf("error\n");
	}
	value = intg.getRectSqValue_0(rect);
	if ((int)value != w * h * 4)
	{
		printf("error\n");
	}

	
	CB_RectT rect1 = {3, 9, 1, 4};
	value = intg.getRectValue_0(rect1);
	if ((int)value != 7 * 4 * 2)
	{
		printf("error\n");
	}
	value = intg.getRectSqValue_0(rect1);
	if ((int)value != 7 * 4 * 4)
	{
		printf("error\n");
	}
}


void Tester::testIntg45()
{
	IntegralImage intg;
	int w = 10;
	int h = 10;
	intg.init(w, h, 2);
	uchar *pdata = new uchar[w * h];
	memset(pdata, 2, w * h);
	const double *pt_intg_45 = intg.computeIntegralImage_45(pdata);
	const double *pt_sq_intg_45 = intg.computeSquareIntegralImage_45(pdata);

	delete []pdata;

	CB_RectangleT rect;
	double value = intg.getRectValue_45(rect);
	if ((int)value != w * h * 2)
	{
		printf("error\n");
	}
	value = intg.getRectSqValue_45(rect);
	if ((int)value != w * h * 4)
	{
		printf("error\n");
	}

	
	CB_RectT rect1 = {3, 9, 1, 4};
	value = intg.getRectValue_0(rect1);
	if ((int)value != 7 * 4 * 2)
	{
		printf("error\n");
	}
	value = intg.getRectSqValue_0(rect1);
	if ((int)value != 7 * 4 * 4)
	{
		printf("error\n");
	}
}