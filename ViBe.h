#pragma once  
#include <iostream>  
#include "opencv2/opencv.hpp"  

using namespace cv;
using namespace std;

#define NUM_SAMPLES 10      //每个像素点的样本个数  
#define MIN_MATCHES 3       //#min指数  
#define RADIUS 40       //Sqthere半径  
#define SUBSAMPLE_FACTOR 3 //子采样概率  


class ViBe_BGS
{
public:
	ViBe_BGS(void);
	~ViBe_BGS(void);

	void init(const Mat _image);   //初始化  
	void processFirstFrame(const Mat _image);
	void testAndUpdate(const Mat _image);  //更新  
	Mat getMask(void){ return m_mask; };

private:
	Mat m_samples[NUM_SAMPLES];
	Mat m_foregroundMatchCount;
	Mat m_mask;
};