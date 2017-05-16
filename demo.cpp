#include "opencv2/opencv.hpp" 
#include <time.h>
#include <conio.h>
#include "ViBe.h"  
#include <iostream>  
#include <cstdio>  

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	Mat frame, gray, mask;
	Mat grayImg;
	VideoCapture capture;
	capture.open(0);

	if (!capture.isOpened())
	{
		cout << "No camera or video input!\n" << endl;
		return -1;
	}

	ViBe_BGS Vibe_Bgs;
	capture >> frame;
	int count = 0;
	if (frame.empty())
	{
		exit(-1);
	}
	cvtColor(frame, gray, CV_RGB2GRAY);
	while (1)
	{
		capture >> frame;
		if (frame.empty())
		{
			break;
		}
		cvtColor(frame, grayImg, CV_RGB2GRAY);
		clock_t starttime = clock();

		Vibe_Bgs.init(gray);//模型的灰度图初始化
		Vibe_Bgs.processFirstFrame(gray);
		Vibe_Bgs.testAndUpdate(grayImg);//实时更新的灰度图
		mask = Vibe_Bgs.getMask();

		morphologyEx(mask, mask, MORPH_OPEN, Mat());
		imshow("mask", mask);
		imshow("input", frame);

		cout << "Total time:" << clock() - starttime << "ms" << endl;
		cvtColor(frame, gray, CV_RGB2GRAY);

		if (cvWaitKey(10) == 'q')
			break;
	}

	return 0;
}