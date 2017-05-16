#include <opencv2/opencv.hpp>  
#include <iostream>  
#include "ViBe.h"  

using namespace std;
using namespace cv;

int c_xoff[9] = { -1, 0, 1, -1, 1, -1, 0, 1, 0 };  //x���ھӵ�  
int c_yoff[9] = { -1, 0, 1, -1, 1, -1, 0, 1, 0 };  //y���ھӵ�  

ViBe_BGS::ViBe_BGS(void)
{

}
ViBe_BGS::~ViBe_BGS(void)
{

}

/**************** Assign space and init ***************************/
void ViBe_BGS::init(const Mat _image)
{
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		m_samples[i] = Mat::zeros(_image.size(), CV_8UC1);
	}
	m_mask = Mat::zeros(_image.size(), CV_8UC1);
	m_foregroundMatchCount = Mat::zeros(_image.size(), CV_8UC1);
}

/**************** Init model from first frame ********************/
void ViBe_BGS::processFirstFrame(const Mat _image)
{
	RNG rng;
	int row, col;

	for (int i = 0; i < _image.rows; i++)
	{
		for (int j = 0; j < _image.cols; j++)
		{
			for (int k = 0; k < NUM_SAMPLES; k++)
			{
				// Random pick up NUM_SAMPLES pixel in neighbourhood to construct the model  
				int random = rng.uniform(0, 9);

				row = i + c_yoff[random];
				if (row < 0)
					row = 0;
				if (row >= _image.rows)
					row = _image.rows - 1;

				col = j + c_xoff[random];
				if (col < 0)
					col = 0;
				if (col >= _image.cols)
					col = _image.cols - 1;

				m_samples[k].at<uchar>(i, j) = _image.at<uchar>(row, col);
			}
		}
	}
}

/**************** Test a new frame and update model ********************/
void ViBe_BGS::testAndUpdate(const Mat _image)
{
	RNG rng;

	for (int i = 0; i < _image.rows; i++)
	{
		for (int j = 0; j < _image.cols; j++)
		{
			int matches(0), count(0);
			float dist;

			while (matches < MIN_MATCHES && count < NUM_SAMPLES)
			{
				dist = abs(m_samples[count].at<uchar>(i, j) - _image.at<uchar>(i, j));
				if (dist < RADIUS)
					matches++;
				count++;
			}

			if (matches >= MIN_MATCHES)
			{
				// It is a background pixel  
				m_foregroundMatchCount.at<uchar>(i, j) = 0;

				// Set background pixel to 0  
				m_mask.at<uchar>(i, j) = 0;

				// ���һ�������Ǳ����㣬��ô���� 1 / defaultSubsamplingFactor �ĸ���ȥ�����Լ���ģ������ֵ  
				int random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					random = rng.uniform(0, NUM_SAMPLES);
					m_samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
				}

				// ͬʱҲ�� 1 / defaultSubsamplingFactor �ĸ���ȥ���������ھӵ��ģ������ֵ  
				random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					int row, col;
					random = rng.uniform(0, 9);
					row = i + c_yoff[random];
					if (row < 0)
						row = 0;
					if (row >= _image.rows)
						row = _image.rows - 1;

					random = rng.uniform(0, 9);
					col = j + c_xoff[random];
					if (col < 0)
						col = 0;
					if (col >= _image.cols)
						col = _image.cols - 1;

					random = rng.uniform(0, NUM_SAMPLES);
					m_samples[random].at<uchar>(row, col) = _image.at<uchar>(i, j);
				}
			}
			else
			{
				// It is a foreground pixel  
				m_foregroundMatchCount.at<uchar>(i, j)++;

				// Set background pixel to 255  
				m_mask.at<uchar>(i, j) = 255;

				//���ĳ�����ص�����N�α����Ϊǰ��������Ϊһ�龲ֹ��������Ϊ�˶����������Ϊ������  
				if (m_foregroundMatchCount.at<uchar>(i, j) > 50)
				{
					int random = rng.uniform(0, SUBSAMPLE_FACTOR);
					if (random == 0)
					{
						random = rng.uniform(0, NUM_SAMPLES);
						m_samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
					}
				}
			}
		}
	}
}