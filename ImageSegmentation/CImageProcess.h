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
	double rate; //图片区域中黑色像素的比例，越大则约有可能贴有照片
	Rect rect;
}ImgData;

class CImgProcess
{
public:
	//构造
	CImgProcess(string img_name);
	CImgProcess(int m_threshold,int m_threshold_2,double m_area,int m_padding,double m_scaleFactor,int m_facecount, double rate=0.35);
	//加载图片
	bool loadImage(string image_path);
	//设置人脸识别CNN模型
	bool setClassifier(string classifier_path);
	//主处理函数
	bool processImage(string result_path);

	//test
	Mat CImgProcess::contourtest();
	

private:
	//预处理,返回处理后的图片
	Mat preProcess(const Mat &srcImage);

	//查找矩形轮廓,返回轮廓集合
	vector<Rect> findAngularPoint(const Mat &pre_process_img);

	//截取照片
	ImgData cutImage(Rect rect);

	//人脸识别
	void detectionFace();
	bool compare(cv::Rect a, cv::Rect b);
	bool findInFaces(Rect rect); //检查rect区域内是否检测到人脸

private:
	Mat srcImage;
	int threshold; //阈值
	int threshold_2; //人像阈值
	int padding;
	double area; //每个人的贴照面积
	double rate;

	CascadeClassifier faceCascade; //人脸检测的类
	vector<Rect> faces;  //人脸检测结果
	int facecount;
	double scaleFactor;
	int n_row;  //图片行数
	int n_col;  //图片列数
};




#endif
