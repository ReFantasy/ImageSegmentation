#include "CImageProcess.h"
#include <fstream>
#include <Windows.h>
CImgProcess::CImgProcess(string img_name)
	:threshold(200),threshold_2(220),area(30000),padding(20),facecount(3),scaleFactor(2.0),rate(0.35)
{
	srcImage = imread(img_name); //载入原始图，且必须以二值图模式载入
	if(!srcImage.data)  //加载失败
	{
		//抛出异常
	}
}

CImgProcess::CImgProcess(int m_threshold,int m_threshold_2,double m_area,int m_padding,double m_scaleFactor,int m_facecount,double m_rate)
{
	threshold = m_threshold;
	threshold_2 = m_threshold_2;
	area = m_area;
	padding = m_padding;
	scaleFactor = m_scaleFactor;
	facecount = m_facecount;
	rate = m_rate;
}

bool CImgProcess::loadImage(string image_path)
{
	srcImage = imread(image_path);  //加载原图片
	if (srcImage.empty())
		return false;
	//cvtColor(srcImage, imgGray, CV_RGB2GRAY);  //转为灰度图
	return true;
}

bool CImgProcess::setClassifier(string classifier_path)
{
	bool res = faceCascade.load(classifier_path);
	if (!res)
	{
		return false;
	}
	else
		return true;
}

bool CImgProcess::processImage(string result_path)
{
	//预处理
	Mat pre_process_img = preProcess(srcImage);
	if(faces.size()<=0)
	{
		ofstream output;
		output.open(result_path+"dim.txt");
		output<<"err: 没有找到照片"<<endl;
		output.close();
		return false;
	}
	//找到每个照片区域边界
	vector<Rect> rects = findAngularPoint(pre_process_img);
	//插入排序
	for(int i=1; i<rects.size(); i++)
	{
		auto tmp = rects[i];
		int k = i-1;
		for(;k>=0; k--)
		{
			if(compare(tmp,rects[k]))
				rects[k+1] = rects[k];
			else
				break;
		}
		rects[k+1] = tmp;
	}

	//行列数
	Rect tmp = rects[rects.size()/2];
	int width = tmp.width;
	int height = tmp.height;
	n_row = srcImage.rows/height;
	n_col = srcImage.cols/width;

	//截取照片
	//for(int i=0; i<faces.size(); i++)
	//{
	//	//路径加上照片编号
	//	//判断该人照片所处的行列
	//	string tmp_path = result_path;
	//	int index_row = faces[i].y/height;
	//	int index_col = faces[i].x/width;
	//	
	//	/*tmp_path += to_string((long long)index_row);
	//	tmp_path += to_string((long long)index_col);
	//	tmp_path += ".jpg";*/

	//	int k = index_row*n_col + index_col;
	//	Mat img = cutImage(rects[k]);
	//	string name = to_string((long long)(k+1));
	//	imshow(name, img);
	//	//存储图片
	//	//imwrite("E:\\test\\",img);
	//}
	for(int i=0; i<rects.size(); i++)
	{
		
		ImgData img_data = cutImage(rects[i]);
		bool find_in_faces = findInFaces(rects[i]);
		/*AllocConsole();
		freopen("CONOUT$", "a+", stdout);
		cout<<"rate...."<<rate<<endl;*/

		//存在照片
		if((img_data.rate>=rate) || (find_in_faces==true))
		{
			//string name = to_string((long long)(i+1));
			//imshow(name, img_data.mat);

			string tmp_path = result_path;

			//路径加上照片编号
			//判断该人照片所处的行列
			int index_row = (i)/n_col+1;
			int index_col = (i)%n_col+1;

			/*char num[10];
			sprintf(num, "%d", img_row);
			tmp_path += num;
			sprintf(num, "%d", img_col);
			tmp_path += num;*/
			tmp_path += to_string((long long)index_row);
			tmp_path += to_string((long long)index_col);
			tmp_path += ".jpg";
			//存储图片
			imwrite(tmp_path,img_data.mat);
		}
		//存储图片
		//imwrite("E:\\test\\",img);
	}
	ofstream output;
	output.open(result_path+"dim.txt");
	output<<n_row<<","<<n_col;
	output.close();

	//waitKey(0);
}


Mat CImgProcess::preProcess(const Mat &srcImage)
{
	//转为灰度图
	Mat pre_process_img;
	cvtColor(srcImage, pre_process_img, CV_RGB2GRAY);

	//滤波 防止边框断边
	blur(pre_process_img, pre_process_img, Size(3,3));  
	//resize(pre_process_img, pre_process_img, Size(pre_process_img.cols/2, pre_process_img.rows / 2));
	
	pre_process_img = pre_process_img > threshold;//srcImage取大于阈值119的那部分
	
	//人脸检测
	setClassifier("haarcascade_frontalface_alt2.xml");
	detectionFace();

	//显示预处理结果
	//imshow("预处理效果", pre_process_img);
	return pre_process_img;

	
}

std::vector<Rect> CImgProcess::findAngularPoint(const Mat &pre_process_img)
{
	vector<Rect> rects;

	//查找轮廓
	vector<vector<Point> > contours; //定义轮廓和层次结构
	vector<Vec4i> hierarchy;
	findContours(pre_process_img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	//根据面积过滤轮廓
	for (int index = 0; index >= 0; index = hierarchy[index][0])
	{
		//舍弃顶层轮廓
		if(hierarchy[index][0]<0)
			break;

		//计算轮廓面积
		vector<Point> contour = contours[index];
		double s =  fabs(contourArea(contour, true));
		if(s<area)
		{
			continue;
		}
		else
		{
			//计算矩形轮廓边界
			Rect tmp = boundingRect(contours[index]);//boundingRect(contour);
			rects.push_back(tmp);
		}

	}


	//【7】显示最后的轮廓图
	//Mat dstImage = Mat::zeros(pre_process_img.rows, pre_process_img.cols, CV_8UC3); //初始化结果图
	//for(int k=0; k<rects.size(); k++)
	//{
	//	rectangle(dstImage,rects[k], Scalar(rand() & 255, rand() & 255, rand() & 255));
	//}
	//imshow("轮廓图", dstImage);

	return rects;
}



ImgData CImgProcess::cutImage(Rect rect)
{
	Rect rect_pad = Rect(rect.x+padding, rect.y+padding, rect.width-2*padding, rect.height-2*padding);
	Mat RoiImage; //黑白
	Mat RoiImage_color;//彩图
	srcImage(rect_pad).copyTo(RoiImage);
	srcImage(rect_pad).copyTo(RoiImage_color);
	cvtColor(RoiImage,RoiImage,CV_RGB2GRAY);
	//imshow("Roi",RoiImage);
	//imwrite("Roi.jpg", RoiImage);
	

	//横向投影
	Mat dstImage_1 = Mat(RoiImage.rows, RoiImage.cols, CV_8UC1, Scalar::all(255)); //初始化结果图
	double n_black=0.0;
	for(int row = 0; row<RoiImage.rows; row++)
	{
		int dstImage_col = 0;
		for(int col=0; col<RoiImage.cols;col++)
		{
			if(RoiImage.at<uchar>(row,col)<threshold_2)
			{
				dstImage_1.at<uchar>(row,dstImage_col++) = 0;
				n_black++;
			}
		}
	}
	//计算横向的照片范围
	int top=0,bottom =dstImage_1.rows-1;
	for(int i=0; i<dstImage_1.rows; i++)
	{
		if(dstImage_1.at<uchar>(i,dstImage_1.cols/2)==0)
		{
			top = i;
			break;
		}
	}
	for(int i=dstImage_1.rows-1; i>=0; i--)
	{
		if(dstImage_1.at<uchar>(i,dstImage_1.cols/2)==0)
		{
			bottom = i;
			break;
		}
	}
	/*cout<<"height "<<dstImage_1.rows<<endl;
	cout<<"top "<<top<<endl;
	cout<<"bottom "<<bottom<<endl;
	imshow("dstImage_1", dstImage_1);*/


	//纵向投影
	Mat dstImage_2 = Mat(RoiImage.rows, RoiImage.cols, CV_8UC1, Scalar::all(255)); //初始化结果图
	for(int col = 0; col<RoiImage.cols; col++)
	{
		int dstImage_row = RoiImage.rows-1;
		for(int row=0; row<RoiImage.rows;row++)
		{
			if(RoiImage.at<uchar>(row,col)<threshold_2)
			{
				dstImage_2.at<uchar>(dstImage_row--,col) = 0;
			}
		}
	}
	//计算纵向的照片范围
	int left=0,right =dstImage_2.cols-1;
	for(int i=0; i<dstImage_2.cols; i++)
	{
		if(dstImage_2.at<uchar>(dstImage_2.rows/2,i)==0)
		{
			left = i;
			break;
		}
	}
	for(int i=dstImage_2.cols-1; i>=0; i--)
	{
		if(dstImage_2.at<uchar>(dstImage_2.rows/2,i)==0)
		{
			right = i;
			break;
		}
	}
	/*cout<<"width "<<dstImage_2.cols<<endl;
	cout<<"left "<<left<<endl;
	cout<<"right "<<right<<endl;
	imshow("dstImage_2", dstImage_2);*/
	/*Mat res = RoiImage_color(Rect(left,top, right-left, bottom-top));
	return res;*/
	//AllocConsole();
	//freopen("CONOUT$", "a+", stdout);


	//计算黑色像素的比例判断是否存在照片
	double rate = n_black/(rect.width*rect.height);
	//cout<<"rate  "<<rate<<endl;
	Mat mat = RoiImage_color(Rect(left,top, right-left, bottom-top));
	ImgData imgdata;
	imgdata.mat = mat;
	imgdata.rate = rate;
	imgdata.rect = Rect(left,top, right-left, bottom-top);
	return imgdata;
}

void CImgProcess::detectionFace()
{
	Mat imgGray;
	cvtColor(srcImage,imgGray, CV_RGB2GRAY);
	faceCascade.detectMultiScale(imgGray, faces, scaleFactor, facecount, 0, Size(0, 0));   //检测人脸
}

bool CImgProcess::compare(cv::Rect a, cv::Rect b)
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


bool CImgProcess::findInFaces(Rect rect)
{
	bool isExit = false;
	for(int i=0; i<faces.size(); i++)
	{
		if(faces[i].x>=rect.x && faces[i].y>=rect.y)
		{
			if((faces[i].x+faces[i].width)<(rect.x+rect.width)  && (faces[i].y+faces[i].height)<(rect.y+rect.height))
			{
				isExit = true;
				break;
			}
		}
	}

	return isExit;
}


cv::Mat CImgProcess::contourtest()
{
	
	//预处理
	Mat pre_process_img = preProcess(srcImage);
	//找到每个照片区域边界
	vector<Rect> rects = findAngularPoint(pre_process_img);
	//【7】显示最后的轮廓图
	Mat dstImage;
	srcImage.copyTo(dstImage);
	for(int k=0; k<rects.size(); k++)
	{
		rectangle(dstImage,rects[k], Scalar(0,0,255),4);
	}
	//imshow("轮廓图", dstImage);
	//return dstImage;

	//插入排序
	for(int i=1; i<rects.size(); i++)
	{
		auto tmp = rects[i];
		int k = i-1;
		for(;k>=0; k--)
		{
			if(compare(tmp,rects[k]))
				rects[k+1] = rects[k];
			else
				break;
		}
		rects[k+1] = tmp;
	}

	//行列数
	Rect tmp = rects[rects.size()/2];
	int width = tmp.width;
	int height = tmp.height;
	n_row = srcImage.rows/height;
	n_col = srcImage.cols/width;

	//截取照片
	
	for(int i=0; i<rects.size(); i++)
	{
		
		ImgData img_data = cutImage(rects[i]);
		bool find_in_faces = findInFaces(rects[i]);
		//存在照片
		if((img_data.rate>=rate) || (find_in_faces==true))
		{
			Rect new_rect;// = Rect(rects[i].x+img_data.rect.x, rects[i].y+img_data.rect.y, img_data.rect.width, img_data.rect,height);
			new_rect.x = rects[i].x+img_data.rect.x+padding;
			new_rect.y = rects[i].y+img_data.rect.y+padding;
			new_rect.width = img_data.rect.width;
			new_rect.height = img_data.rect.height;
			rectangle(dstImage,new_rect, Scalar(255,0,0),4);
		}
		
	}

	return dstImage;
}
