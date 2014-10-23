#include "stdafx.h"
#include "FernFeatureHub.h"


FernFeatureHub::FernFeatureHub(void)
{
	p_ferns = NULL;
}


FernFeatureHub::~FernFeatureHub(void)
{
	cleanUp();
}


void FernFeatureHub::cleanUp()
{
	if (p_ferns != NULL)
	{
		delete []p_ferns;
		p_ferns = NULL;
	}
}


int FernFeatureHub::newTrainingMem(int pos_num, int neg_num)
{
	FeatureHub::newTrainingMem(pos_num, neg_num);
	haar_hub.newTrainingMem(pos_num, neg_num);
	return 1;
}


int FernFeatureHub::init(const FeatureParamT &param)
{
	cleanUp();
	fern_param = (FernParamT &)param;
	haar_hub.init(fern_param.haar_param);

	if (fern_param.is_candid == 1)
	{
		int rst = loadCandid();
		return rst;
	}

	feature_num = getAllFeatureInfos();
	p_ferns = new FernFeature[feature_num];
	feature_num = getAllFeatureInfos(1);
	for (int i=0; i<feature_num; i++)
	{
		p_ferns[i].init(param);
	}
	return 1;
}


int FernFeatureHub::loadCandid()
{
	//todo: load fern candidates from file
	return 1;
}


int FernFeatureHub::initFromConfig(const string &path)
{
	FILE *fp = fopen(path.c_str(), "rt");
	fern_param.initFromConfig(fp);
	fclose(fp);

	bin_num = fern_param.bin_num;

	init(fern_param);

	return 1;
}


int FernFeatureHub::getFeatureNum()
{
	return feature_num;
}


int FernFeatureHub::train(int pos_num, const string &pos_data_path, 
		                  int neg_num, const string &neg_data_path)
{
	haar_hub.train(pos_num, pos_data_path, neg_num, neg_data_path);
	trainFeatures(pos_num, neg_num);

	return 1;
}


int FernFeatureHub::trainFeatures(int pos_num, int neg_num)
{
	this->pos_num = pos_num;
	this->neg_num = neg_num;

	FernFeatureValueT *pt_pos_fern_values = (FernFeatureValueT*)p_pos_features;
	FernFeatureValueT *pt_neg_fern_values = (FernFeatureValueT*)p_neg_features;

	for (int i=0; i<feature_num; i++)
	{
		printf("Fern Feature = %d -- %d\r", feature_num, i+1);
		FernFeature &fern = p_ferns[i];
		for (int j=0; j<pos_num; j++)
		{
			int idx_i = i * pos_num + j;
			pt_pos_fern_values[idx_i].index = computePosFeatureIdx(j, i);
		}

		for (int j=0; j<neg_num; j++)
		{
			int idx_i = i * neg_num + j;
			pt_neg_fern_values[idx_i].index = computeNegFeatureIdx(j, i);
		}
	}
	return 1;
}


int FernFeatureHub::computePosFeatureIdx(int sampleIdx, int featureIdx)
{
	FernFeature &fern = p_ferns[featureIdx];
	int index = 0;
	int exp = 1;
	for (int i=0; i<fern_param.fern_cell_num; i++)
	{
		int haar_idx = fern.info.haars_idx[i];
		int bin_index = haar_hub.getPosFeatureIdx(sampleIdx, haar_idx);
		index += bin_index * exp;
		exp *= fern_param.haar_param.bin_num;
	}
	return index;
}


int FernFeatureHub::computeNegFeatureIdx(int sampleIdx, int featureIdx)
{
	FernFeature &fern = p_ferns[featureIdx];
	int index = 0;
	int exp = 1;
	for (int i=0; i<fern_param.fern_cell_num; i++)
	{
		int haar_idx = fern.info.haars_idx[i];
		int bin_index = haar_hub.getNegFeatureIdx(sampleIdx, haar_idx);
		index += bin_index * exp;
		exp *= fern_param.haar_param.bin_num;
	}
	return index;
}


int FernFeatureHub::getPosFeatureIdx(int sampleIdx, int featureIdx)
{
	FernFeatureValueT *pt_pos_fern_values = (FernFeatureValueT*)p_pos_features;
	int idx_i = featureIdx * pos_num + sampleIdx;
	return pt_pos_fern_values[idx_i].index;
}


int FernFeatureHub::getNegFeatureIdx(int sampleIdx, int featureIdx)
{
	FernFeatureValueT *pt_neg_fern_values = (FernFeatureValueT*)p_neg_features;
	int idx_i = featureIdx * neg_num + sampleIdx;
	return pt_neg_fern_values[idx_i].index;
}


int FernFeatureHub::getAllFeatureInfos(int is_extract_feature)
{
	int count = 0;
	int haar_num = haar_hub.getFeatureNum();

	for (int i=0; i<haar_num; i++)
	{
		//for (int j=i+1; j<haar_num; j++)
		{
			if (is_extract_feature == 1)
			{
				p_ferns[count].info.ptr_haars[0] = &(haar_hub.p_haars[i]);
				p_ferns[count].info.haars_idx[0] = i;
				//p_ferns[count].info.ptr_haars[1] = &(haar_hub.p_haars[j]);
				//p_ferns[count].info.haars_idx[1] = j;
			}
			count++;
		}
	}
	return count;
}


FeatureValueT *FernFeatureHub::createTrainingMemory(int num)
{
	FernFeatureValueT *pt_featureValue = new FernFeatureValueT[num * feature_num];
	return (FeatureValueT *)pt_featureValue;
}


int FernFeatureHub::releaseTrainingMemory(FeatureValueT *&pt_featureValue)
{
	if (pt_featureValue == NULL) 
	{
		return 0;
	}

	delete [](FernFeatureValueT *)pt_featureValue;
	return 1;
}


Feature *FernFeatureHub::getFeature(int idx)
{
	return &(p_ferns[idx]);
}