#include <iostream>
#include <opencv.hpp>
#include <algorithm>
#include <fstream>
using namespace std;
using namespace cv;
class Imagesplit
{
public:
	//����
	Imagesplit()
	{
		threshold = 160;
		facecount = 8;
		minimumsize = 200;
	};
	Imagesplit(uchar m_threshold = 160, int m_facecount = 8,int m_minimumsize = 200)
		:threshold(m_threshold), facecount(m_facecount), minimumsize(m_minimumsize),m_top(0),m_bottom(0),m_left(0), m_right(0){}

	//����ͼƬ
	bool loadImage(string image_path)
	{
		srcimg = imread(image_path);  //����ԭͼƬ
		if (srcimg.empty())
			return false;
		cvtColor(srcimg, imgGray, CV_RGB2GRAY);  //תΪ�Ҷ�ͼ
		return true;
	}
	//��������ʶ��CNNģ��
	bool setClassifier(string classifier_path)
	{
		bool res = faceCascade.load(classifier_path);
		if (!res)
			return false;
		else
			return true;
	}
	
	//���������
	bool processImage(string result_path)
	{
		//����ʶ��
		detectionFace();
		if(faces.size()<=0)
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<"err: û���ҵ���Ƭ"<<endl;
			output.close();
			return false;
		}
		//����������
		vector<int> num_row_col = compute_row_and_col();
		if(num_row_col.size()<2)
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<"err: �޷�ʶ������������"<<endl;
			output.close();
			return false;
		}
		int n_row = num_row_col[0];
		int n_col = num_row_col[1];
		/*CString rowcol;
		rowcol.Format(_T("row:%d col%d"), n_row, n_col);
		AfxMessageBox(rowcol);*/

		//��ȡ֤����Ƭ
		for (size_t i = 0; i < faces.size(); i++)
		{
			auto res_rect = cutImage(faces[i]);
			//���ø���Ȥ����
			Mat imageROI = srcimg(res_rect);
			string tmp_path = result_path;

			//·��������Ƭ���
			//�жϸ�����Ƭ����������
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

			//�洢ͼƬ
			imwrite(tmp_path,imageROI);
		}

		//д��dim.txt
		Sleep(300);
		if(n_row*n_col < faces.size())
		{
			ofstream output;
			output.open(result_path+"dim.txt");
			output<<n_row<<","<<n_col<<endl;
			output<<"err: ������ʶ�����"<<endl;
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
	//��������������
	vector<int> compute_row_and_col()
	{
		return compute_row_col(srcimg);
	}


private:
	//�������,������⵽�Ľ��������faces������
	void detectionFace()
	{
		faceCascade.detectMultiScale(imgGray, faces, 1.2, facecount, 0, Size(0, 0));   //�������
		//�������-��������
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

	//�˺��������ҵ�ÿһ��֤���յı�Ե
	//face_rect ��ʾ����������
	cv::Rect cutImage(cv::Rect face_rect)
	{
		int top = face_rect.y;
		int left = face_rect.x;
		int right = face_rect.x + face_rect.width;
		int bottom = face_rect.y + face_rect.height;

		//Ѱ���ϱ߽�
		while (top>0 && imgGray.at<uchar>(--top, face_rect.x) < threshold) {}
		//Ѱ����߽�
		while (left>0 && imgGray.at<uchar>(face_rect.y, --left) < threshold) {}
		//Ѱ���ұ߽�
		while ((right<imgGray.cols-1) && (right-left)<=reseau_width && (imgGray.at<uchar>(face_rect.y, ++right) < threshold)) {}
		//Ѱ���±߽�
		while ((bottom<imgGray.rows - 1) && (bottom-top)<=reseau_height &&(imgGray.at<uchar>(++bottom, face_rect.x+face_rect.width) < threshold)) {}

		face_rect.x = left;
		face_rect.y = top;
		face_rect.width = right - left;
		face_rect.height = (bottom - top)<minimumsize?minimumsize:(bottom - top);
		return face_rect;

	}

	//��������
	vector<int> compute_row_col(const Mat &srcImage)
	{
		Mat grayImage,edge, dstImage;
		cvtColor(srcImage, grayImage, COLOR_BGR2GRAY); //תΪ�Ҷ�ͼ

		//�������
		if(faces.empty())
		{
			CascadeClassifier faceCascade;
			faceCascade.load("haarcascade_frontalface_alt2.xml");
			Mat imgGray;
			cvtColor(srcImage, imgGray, CV_RGB2GRAY);  //תΪ�Ҷ�ͼ
			faceCascade.detectMultiScale(imgGray, faces, 1.2, 8, 0, Size(0, 0));
		}

		//Ѱ�������ߴ�����ֵ���ڻ�����
		int max_size_face = 0;
		for (int i=0; i<faces.size(); i++)
		{
			cv::Rect f = faces[i];
			int tmp_size = max(f.width, f.height);
			max_size_face = max_size_face > tmp_size ? max_size_face : tmp_size;
		}

		blur(grayImage, grayImage, Size(3,3));//�˲�
		Canny(grayImage, edge, 200,100, 3);//��Ե���

		dstImage = Mat(edge.rows, edge.cols, CV_8UC1);
		dstImage = Scalar::all(255);
		vector<Vec4i> lines;

		blur(edge, edge, Size(3, 3));//�˲�
		HoughLinesP(edge, lines, 3, CV_PI / 180, 220, max_size_face, 0); //������
		//��⵽��ֱ�߻���dstimage
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), 0, 1, 4);
		}

	

		//dstImageͶӰѰ��ÿ����Ƭ��Ŀ��
		//y����ͶӰ
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
		//����λ��
		auto median = projection_Y;
		sort(median.begin(), median.end());
		double m_sum = 0;
		for (auto n = median.size()-50; n<median.size(); n++)
			m_sum += median[n];
		auto mid_value = m_sum/50;

		//���ӻ�
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
		//�����ұ߽�
		for(int col=0; col<visual_dst.cols; col++)
		{
			//��߽�
			if(visual_dst.at<uchar>(0,col) == 0 && visual_dst.at<uchar>(0,m_left) != 0)
			{
				m_left = col;
			}
			//�ұ߽�
			if(visual_dst.at<uchar>(0,col) == 0)
			{
				m_right = col;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//X����ͶӰ
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
		//����λ��
		auto median_X = projection_X;
		sort(median_X.begin(), median_X.end());
		double m_sum_X = 0;
		for (auto n = median_X.size() - 50; n < median_X.size(); n++)
			m_sum_X += median_X[n];
		auto mid_value_X = m_sum_X / 50;

		//���ӻ�
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
		//�����±߽�
		for(int row=0; row<visual_dst_X.rows; row++)
		{
			//�ϱ߽�
			if(visual_dst_X.at<uchar>(row,0) == 0 && visual_dst_X.at<uchar>(m_top,0) != 0)
			{
				m_top = row;
			}
			//�±߽�
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
		//ʶ��Ŀ�͸ߴ�С����
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
		//1-9�������֣�����9����a-z
		string table = "0123456789abcdefghijklmnopqistuvwxyzABCDEFGHIJKLMNOPQISTUVWXYZ";

		if(n>table.size())
			return string("out of range");

		char s[] = "0";
		s[0] = table[n];
		return string(s);
	}

private:
	Mat srcimg;  //ԭͼƬ
	Mat imgGray;  //�Ҷ�ͼ
	CascadeClassifier faceCascade; //����������
	string classifier_path; //������·��
	vector<Rect> faces;  //�����
	uchar threshold; //��ֵ
	int facecount;
	int minimumsize;
	int m_top; //ͼƬ���ϱ߽�
	int m_bottom;//�±߽�
	int m_left;//��߽�
	int m_right;//�ұ߽�
	int reseau_width; //ÿ����
	int reseau_height; //ÿ��߶�
};