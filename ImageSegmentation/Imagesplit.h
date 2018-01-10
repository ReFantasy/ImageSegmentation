#include <iostream>
#include <opencv.hpp>
#include <algorithm>
#include <fstream>
using namespace std;
using namespace cv;
class Imagesplit
{
public:
	//构造
	Imagesplit()
	{
		threshold = 160;
		facecount = 8;
		minimumsize = 200;
	};
	Imagesplit(uchar m_threshold = 160, int m_facecount = 8,int m_minimumsize = 200)
		:threshold(m_threshold), facecount(m_facecount), minimumsize(m_minimumsize),m_top(0),m_bottom(0),m_left(0), m_right(0){}

	//加载图片
	bool loadImage(string image_path)
	{
		srcimg = imread(image_path);  //加载原图片
		if (srcimg.empty())
			return false;
		cvtColor(srcimg, imgGray, CV_RGB2GRAY);  //转为灰度图
		return true;
	}
	//设置人脸识别CNN模型
	bool setClassifier(string classifier_path)
	{
		bool res = faceCascade.load(classifier_path);
		if (!res)
			return false;
		else
			return true;
	}
	
	//主处理程序
	bool processImage(string result_path)
	{
		//人脸识别
		detectionFace();
		if(faces.size()<=0)
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<"err: 没有找到照片"<<endl;
			output.close();
			return false;
		}
		//计算行列数
		vector<int> num_row_col = compute_row_and_col();
		if(num_row_col.size()<2)
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<"err: 无法识别行数和列数"<<endl;
			output.close();
			return false;
		}
		int n_row = num_row_col[0];
		int n_col = num_row_col[1];
		/*CString rowcol;
		rowcol.Format(_T("row:%d col%d"), n_row, n_col);
		AfxMessageBox(rowcol);*/

		//截取证件照片
		for (size_t i = 0; i < faces.size(); i++)
		{
			auto res_rect = cutImage(faces[i]);
			//设置感兴趣区域
			Mat imageROI = srcimg(res_rect);
			string tmp_path = result_path;

			//路径加上照片编号
			//判断该人照片所处的行列
			int img_row = res_rect.y/reseau_height+1;
			int img_col = res_rect.x/reseau_width+1;

			/*char num[10];
			sprintf(num, "%d", img_row);
			tmp_path += num;
			sprintf(num, "%d", img_col);
			tmp_path += num;*/
			tmp_path += getia(img_row);
			tmp_path += getia(img_col);
			tmp_path += ".jpg";

			//存储图片
			imwrite(tmp_path,imageROI);
		}

		//写入dim.txt
		Sleep(300);
		if(n_row*n_col < faces.size())
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<n_row<<","<<n_col<<endl;
			output<<"err: 行列数识别错误"<<endl;
			output.close();
		}
		else
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<n_row<<","<<n_col;
			output.close();
		}
		
		return true;
	}
	//计算行数和列数
	vector<int> compute_row_and_col()
	{
		return compute_row_col(srcimg);
	}


private:
	//检测人脸,并将检测到的结果保存在faces向量中
	void detectionFace()
	{
		faceCascade.detectMultiScale(imgGray, faces, 1.2, facecount, 0, Size(0, 0));   //检测人脸
		//结果排序-插入排序
		for(int i=1; i<faces.size(); i++)
		{
			auto tmp = faces[i];
			int k = i-1;
			for(;k>=0; k--)
			{
				if(compare(tmp,faces[k]))
					faces[k+1] = faces[k];
				else
					break;
			}
			faces[k+1] = tmp;
		}
	}

	//此函数用来找到每一张证件照的边缘
	//face_rect 表示人脸的区域
	cv::Rect cutImage(cv::Rect face_rect)
	{
		int top = face_rect.y;
		int left = face_rect.x;
		int right = face_rect.x + face_rect.width;
		int bottom = face_rect.y + face_rect.height;

		//寻找上边界
		while (top>0 && imgGray.at<uchar>(--top, face_rect.x) < threshold) {}
		//寻找左边界
		while (left>0 && imgGray.at<uchar>(face_rect.y, --left) < threshold) {}
		//寻找右边界
		while ((right<imgGray.cols-1) && (right-left)<=reseau_width && (imgGray.at<uchar>(face_rect.y, ++right) < threshold)) {}
		//寻找下边界
		while ((bottom<imgGray.rows - 1) && (bottom-top)<=reseau_height &&(imgGray.at<uchar>(++bottom, face_rect.x+face_rect.width) < threshold)) {}

		face_rect.x = left;
		face_rect.y = top;
		face_rect.width = right - left;
		face_rect.height = (bottom - top)<minimumsize?minimumsize:(bottom - top);
		return face_rect;

	}

	//计算行列
	vector<int> compute_row_col(const Mat &srcImage)
	{
		Mat grayImage,edge, dstImage;
		cvtColor(srcImage, grayImage, COLOR_BGR2GRAY); //转为灰度图

		//检测人脸
		if(faces.empty())
		{
			CascadeClassifier faceCascade;
			faceCascade.load("haarcascade_frontalface_alt2.xml");
			Mat imgGray;
			cvtColor(srcImage, imgGray, CV_RGB2GRAY);  //转为灰度图
			faceCascade.detectMultiScale(imgGray, faces, 1.2, 8, 0, Size(0, 0));
		}

		//寻找人脸尺寸的最大值用于霍夫检测
		int max_size_face = 0;
		for (int i=0; i<faces.size(); i++)
		{
			cv::Rect f = faces[i];
			int tmp_size = max(f.width, f.height);
			max_size_face = max_size_face > tmp_size ? max_size_face : tmp_size;
		}

		blur(grayImage, grayImage, Size(3,3));//滤波
		Canny(grayImage, edge, 200,100, 3);//边缘检测

		dstImage = Mat(edge.rows, edge.cols, CV_8UC1);
		dstImage = Scalar::all(255);
		vector<Vec4i> lines;

		blur(edge, edge, Size(3, 3));//滤波
		HoughLinesP(edge, lines, 3, CV_PI / 180, 220, max_size_face, 0); //霍夫检测
		//检测到的直线画在dstimage
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), 0, 1, 4);
		}

	

		//dstImage投影寻找每张照片框的宽度
		//y方向投影
		vector<int> projection_Y(dstImage.cols, 0);
		for (int col = 0; col < dstImage.cols; col++)
		{
			for (int row = 0; row < dstImage.rows; row++)
			{
				if (dstImage.at<uchar>(row, col) == 0)
				{
					projection_Y[col]++;
				}
			}
		}
		//求中位数
		auto median = projection_Y;
		sort(median.begin(), median.end());
		double m_sum = 0;
		for (auto n = median.size()-50; n<median.size(); n++)
			m_sum += median[n];
		auto mid_value = m_sum/50;

		//可视化
		Mat visual_dst(dstImage.rows, dstImage.cols, CV_8UC1);
		visual_dst = Scalar::all(255);
		for (int col = 0; col < dstImage.cols; col++)
		{
			line(visual_dst, Point(col, 0), Point(col, projection_Y[col]), 0, 1, 4);
		}

		//line(visual_dst, Point(0, mid_value), Point(dstImage.cols-1, mid_value), 0, 1, LINE_4);
		//imshow("visual", visual_dst);
		//waitKey(0);

		int width = 0;
		int first_pt = 0;
		for (int end_pt = 0; end_pt < visual_dst.cols; end_pt++)
		{
			if (visual_dst.at<uchar>(mid_value, end_pt) == 0 && visual_dst.at<uchar>(mid_value, first_pt) == 0)
			{
				width = width > (end_pt - first_pt) ? width : (end_pt - first_pt);
				first_pt = end_pt;
				
			}
			else if (visual_dst.at<uchar>(mid_value, end_pt) == 0 && visual_dst.at<uchar>(mid_value, first_pt) != 0)
			{
				first_pt = end_pt;
				
			}
		}
		//求左右边界
		for(int col=0; col<visual_dst.cols; col++)
		{
			//左边界
			if(visual_dst.at<uchar>(0,col) == 0 && visual_dst.at<uchar>(0,m_left) != 0)
			{
				m_left = col;
			}
			//右边界
			if(visual_dst.at<uchar>(0,col) == 0)
			{
				m_right = col;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//X方向投影
		vector<int> projection_X(dstImage.rows, 0);
		for (int row = 0; row < dstImage.rows; row++)
		{
			for (int col = 0; col < dstImage.cols; col++)
			{
				if (dstImage.at<uchar>(row, col) == 0)
				{
					projection_X[row]++;
				}
			}
		}
		//求中位数
		auto median_X = projection_X;
		sort(median_X.begin(), median_X.end());
		double m_sum_X = 0;
		for (auto n = median_X.size() - 50; n < median_X.size(); n++)
			m_sum_X += median_X[n];
		auto mid_value_X = m_sum_X / 50;

		//可视化
		Mat visual_dst_X(dstImage.rows, dstImage.cols, CV_8UC1);
		visual_dst_X = Scalar::all(255);
		for (int row = 0; row < dstImage.rows; row++)
		{
			line(visual_dst_X, Point(0, row), Point(projection_X[row],row), 0, 1, 4);
		}

		//line(visual_dst_X, Point(mid_value_X,0), Point(mid_value_X, visual_dst_X.rows-1), 0, 1, LINE_4);
		//imshow("visual", visual_dst_X);



		int height = 1;
		int first_pt_X = 0;
		for (int end_pt = 0; end_pt < visual_dst_X.rows; end_pt++)
		{
			if (visual_dst_X.at<uchar>(end_pt, mid_value_X) == 0 && visual_dst_X.at<uchar>(first_pt_X, mid_value_X) == 0)
			{
				height = height > (end_pt - first_pt_X) ? height : (end_pt - first_pt_X);
				first_pt_X = end_pt;
				
			}
			else if (visual_dst_X.at<uchar>(end_pt, mid_value_X) == 0 && visual_dst_X.at<uchar>(first_pt_X, mid_value_X) != 0)
			{
				first_pt_X = end_pt;
				
			}
		}
		//求上下边界
		for(int row=0; row<visual_dst_X.rows; row++)
		{
			//上边界
			if(visual_dst_X.at<uchar>(row,0) == 0 && visual_dst_X.at<uchar>(m_top,0) != 0)
			{
				m_top = row;
			}
			//下边界
			if(visual_dst_X.at<uchar>(row,0) == 0)
			{
				m_bottom = row;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		vector<int> row_col;
		/*row_col.push_back(dstImage.rows/height);
		row_col.push_back(dstImage.cols/width);*/
		reseau_width = width;
		reseau_height = height;
		//识别的宽和高大小限制
		if(width>5000 || height>5000)
		{
			return vector<int>();
		}
		row_col.push_back((m_bottom-m_top)/height);
		row_col.push_back((m_right-m_left)/width);
		return row_col;
	}

	bool compare(cv::Rect a, cv::Rect b)
	{
		int tmp_threshold = 30;
		if((a.y<b.y) && abs(a.y-b.y)>tmp_threshold)
		{
			return true;
		}
		else if(abs(a.y-b.y)<tmp_threshold)
		{
			if(a.x<=b.x)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		return false;
	}
	inline string getia(int n)
	{
		//1-9返回数字，大于9返回a-z
		string table = "0123456789abcdefghijklmnopqistuvwxyzABCDEFGHIJKLMNOPQISTUVWXYZ";

		if(n>table.size())
			return string("out of range");

		char s[] = "0";
		s[0] = table[n];
		return string(s);
	}

private:
	Mat srcimg;  //原图片
	Mat imgGray;  //灰度图
	CascadeClassifier faceCascade; //人脸检测的类
	string classifier_path; //分类器路径
	vector<Rect> faces;  //检测结果
	uchar threshold; //阈值
	int facecount;
	int minimumsize;
	int m_top; //图片的上边界
	int m_bottom;//下边界
	int m_left;//左边界
	int m_right;//右边界
	int reseau_width; //每格宽度
	int reseau_height; //每格高度
};