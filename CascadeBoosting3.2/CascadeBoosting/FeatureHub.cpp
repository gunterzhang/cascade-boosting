#include "stdafx.h"
#include "FeatureHub.h"


FeatureHub::FeatureHub(void)
{
	feature_num = 0;
	p_pos_features = NULL;
	p_neg_features = NULL;
}


FeatureHub::~FeatureHub(void)
{
	cleanUp();
}


int FeatureHub::initTrainingMem(int pos_num, int neg_num)
{
	cleanUp();
	p_pos_features = createTrainingMemory(pos_num);
	p_neg_features = createTrainingMemory(neg_num);
	return 1;
}

int FeatureHub::cleanUp()
{
	releaseTrainingMemory(p_pos_features);
	releaseTrainingMemory(p_neg_features);
	return 1;
}


int FeatureHub::train(int pos_num, const string &pos_data_path, 
		              int neg_num, const string &neg_data_path)
{
	this->pos_num = pos_num;
	this->neg_num = neg_num;
	extractFeatures(pos_num, pos_data_path, p_pos_features);
	extractFeatures(neg_num, neg_data_path, p_neg_features);
	trainFeatures(pos_num, p_pos_features, neg_num, p_neg_features);
	return 1;
}
