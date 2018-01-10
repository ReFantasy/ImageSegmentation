#ifndef __CImageProcess_h__
#define __CImageProcess_h__

#include <opencv.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include <algorithm>

#include <vector>
using namespace cv;
using namespace std;
typedef struct ImgData
{
	Mat mat;
	double rate; //ͼƬ�����к�ɫ���صı�����Խ����Լ�п���������Ƭ
	Rect rect;
}ImgData;

class CImgProcess
{
public:
	//����
	CImgProcess(string img_name);
	CImgProcess(int m_threshold,int m_threshold_2,double m_area,int m_padding,double m_scaleFactor,int m_facecount, double rate=0.35);
	//����ͼƬ
	bool loadImage(string image_path);
	//��������ʶ��CNNģ��
	bool setClassifier(string classifier_path);
	//��������
	bool processImage(string result_path);

	//test
	Mat CImgProcess::contourtest();
	

private:
	//Ԥ����,���ش�����ͼƬ
	Mat preProcess(const Mat &srcImage);

	//���Ҿ�������,������������
	vector<Rect> findAngularPoint(const Mat &pre_process_img);

	//��ȡ��Ƭ
	ImgData cutImage(Rect rect);

	//����ʶ��
	void detectionFace();
	bool compare(cv::Rect a, cv::Rect b);
	bool findInFaces(Rect rect); //���rect�������Ƿ��⵽����

private:
	Mat srcImage;
	int threshold; //��ֵ
	int threshold_2; //������ֵ
	int padding;
	double area; //ÿ���˵��������
	double rate;

	CascadeClassifier faceCascade; //����������
	vector<Rect> faces;  //���������
	int facecount;
	double scaleFactor;
	int n_row;  //ͼƬ����
	int n_col;  //ͼƬ����
};




#endif
