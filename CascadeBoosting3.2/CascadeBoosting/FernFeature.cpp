#include "stdafx.h"
#include "FernFeature.h"


FernParamT::~FernParamT()
{
}


int FernParamT::saveToModel(FILE *fp)
{
	haar_param.saveToModel(fp);
	return 1;
}


int FernParamT::loadFromModel(FILE *fp)
{
	haar_param.loadFromModel(fp);
	return 1;
}


int FernParamT::initFromConfig(FILE *fp)
{
	char tmp_str[1000];
	
	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d\n", &fern_cell_num);
	printf("FernCellNum:     %d\n", fern_cell_num);
	
	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%d\n", &is_candid);
	printf("IsCandid:        %d\n", is_candid);

	fscanf(fp, "%s", tmp_str);
	fscanf(fp, "%s\n", tmp_str);
	candid_path = tmp_str;
	if (is_candid > 0)
	{
		printf("CandidPath:      %s\n", candid_path.c_str());
	}

	haar_param.initFromConfig(fp);

	bin_num = pow(haar_param.bin_num, fern_cell_num);
	return 1;
}


CB_PointT FernParamT::getTemplateSize()
{
	return haar_param.getTemplateSize();
}


int FernParamT::getFeatureTypes()
{
	return haar_param.getFeatureTypes();
}


FernFeature::FernFeature(void)
{
}


FernFeature::~FernFeature(void)
{
}



int FernFeature::init(const FeatureParamT &init_param)
{
	info.param = (FernParamT &)init_param;
	return 1;
}


int FernFeature::copy(Feature *ptr_feature)
{
	info = ((FernFeature *)ptr_feature)->info;
	return 1;
}


int FernFeature::loadFromModel(FILE *fp)
{
	fscanf(fp, "fern %d %d\n", &info.param.fern_cell_num, &info.param.bin_num);
	for (int i=0; i<info.param.fern_cell_num; i++)
	{
		info.ptr_haars[i]->loadFromModel(fp);
	}
	return 1;
}


int FernFeature::saveToModel(const string &file_path)
{
	FILE *fp = fopen(file_path.c_str(), "at");
	fprintf(fp, "fern %d %d\n", info.param.fern_cell_num, info.param.bin_num);
	fclose(fp);

	for (int i=0; i<info.param.fern_cell_num; i++)
	{
		info.ptr_haars[i]->saveToModel(file_path);
	}
	return 1;
}


int FernFeature::computeFeatureValue(const IntegralImage &intg, const SubwinInfoT &subwin, FeatureValueT &value) const
{
	FernFeatureValueT &fern_value = (FernFeatureValueT &)value;
	for (int i=0; i<info.param.fern_cell_num; i++)
	{
		info.ptr_haars[i]->computeFeatureValue(intg, subwin, fern_value.haar_value[i]);
	}
	return 1;
}


int FernFeature::computeFeature(const IntegralImage &intg, const SubwinInfoT &subwin) const
{
	int index = 0; 
	int exp = 1;
	for (int i=0; i<info.param.fern_cell_num; i++)
	{
		int bin_index = info.ptr_haars[i]->computeFeature(intg, subwin);
		index += bin_index * exp;
		exp *= info.ptr_haars[i]->info.bin_num;
	}
	return index;
}


int FernFeature::computeFeatureIndex(const FeatureValueT &src_value) const
{
	FernFeatureValueT &src_fern_value = (FernFeatureValueT &)src_value;

	int index = 0;
	int exp = 1;
	for (int i=0; i<info.param.fern_cell_num; i++)
	{
		int bin_index = info.ptr_haars[i]->computeFeatureIndex(src_fern_value.haar_value[i]);
		index += bin_index * exp;
		exp *= info.ptr_haars[i]->info.bin_num;
	}
	return index;
}


int FernFeature::setParam(FeatureParamT *ptr_param)
{
	info.param = *((FernParamT *)ptr_param);
	return 1;
}