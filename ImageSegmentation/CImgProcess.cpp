#include "CImageProcess.h"
#include <fstream>
#include <Windows.h>
CImgProcess::CImgProcess(string img_name)
	:threshold(200),threshold_2(220),area(30000),padding(20),facecount(3),scaleFactor(2.0),rate(0.35)
{
	srcImage = imread(img_name); //����ԭʼͼ���ұ����Զ�ֵͼģʽ����
	if(!srcImage.data)  //����ʧ��
	{
		//�׳��쳣
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
	srcImage = imread(image_path);  //����ԭͼƬ
	if (srcImage.empty())
		return false;
	//cvtColor(srcImage, imgGray, CV_RGB2GRAY);  //תΪ�Ҷ�ͼ
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
	//Ԥ����
	Mat pre_process_img = preProcess(srcImage);
	if(faces.size()<=0)
	{
		ofstream output;
		output.open(result_path+"dim.txt");
		output<<"err: û���ҵ���Ƭ"<<endl;
		output.close();
		return false;
	}
	//�ҵ�ÿ����Ƭ����߽�
	vector<Rect> rects = findAngularPoint(pre_process_img);
	//��������
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

	//������
	Rect tmp = rects[rects.size()/2];
	int width = tmp.width;
	int height = tmp.height;
	n_row = srcImage.rows/height;
	n_col = srcImage.cols/width;

	//��ȡ��Ƭ
	//for(int i=0; i<faces.size(); i++)
	//{
	//	//·��������Ƭ���
	//	//�жϸ�����Ƭ����������
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
	//	//�洢ͼƬ
	//	//imwrite("E:\\test\\",img);
	//}
	for(int i=0; i<rects.size(); i++)
	{
		
		ImgData img_data = cutImage(rects[i]);
		bool find_in_faces = findInFaces(rects[i]);
		/*AllocConsole();
		freopen("CONOUT$", "a+", stdout);
		cout<<"rate...."<<rate<<endl;*/

		//������Ƭ
		if((img_data.rate>=rate) || (find_in_faces==true))
		{
			//string name = to_string((long long)(i+1));
			//imshow(name, img_data.mat);

			string tmp_path = result_path;

			//·��������Ƭ���
			//�жϸ�����Ƭ����������
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
			//�洢ͼƬ
			imwrite(tmp_path,img_data.mat);
		}
		//�洢ͼƬ
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
	//תΪ�Ҷ�ͼ
	Mat pre_process_img;
	cvtColor(srcImage, pre_process_img, CV_RGB2GRAY);

	//�˲� ��ֹ�߿�ϱ�
	blur(pre_process_img, pre_process_img, Size(3,3));  
	//resize(pre_process_img, pre_process_img, Size(pre_process_img.cols/2, pre_process_img.rows / 2));
	
	pre_process_img = pre_process_img > threshold;//srcImageȡ������ֵ119���ǲ���
	
	//�������
	setClassifier("haarcascade_frontalface_alt2.xml");
	detectionFace();

	//��ʾԤ������
	//imshow("Ԥ����Ч��", pre_process_img);
	return pre_process_img;

	
}

std::vector<Rect> CImgProcess::findAngularPoint(const Mat &pre_process_img)
{
	vector<Rect> rects;

	//��������
	vector<vector<Point> > contours; //���������Ͳ�νṹ
	vector<Vec4i> hierarchy;
	findContours(pre_process_img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	//���������������
	for (int index = 0; index >= 0; index = hierarchy[index][0])
	{
		//������������
		if(hierarchy[index][0]<0)
			break;

		//�����������
		vector<Point> contour = contours[index];
		double s =  fabs(contourArea(contour, true));
		if(s<area)
		{
			continue;
		}
		else
		{
			//������������߽�
			Rect tmp = boundingRect(contours[index]);//boundingRect(contour);
			rects.push_back(tmp);
		}

	}


	//��7����ʾ��������ͼ
	//Mat dstImage = Mat::zeros(pre_process_img.rows, pre_process_img.cols, CV_8UC3); //��ʼ�����ͼ
	//for(int k=0; k<rects.size(); k++)
	//{
	//	rectangle(dstImage,rects[k], Scalar(rand() & 255, rand() & 255, rand() & 255));
	//}
	//imshow("����ͼ", dstImage);

	return rects;
}



ImgData CImgProcess::cutImage(Rect rect)
{
	Rect rect_pad = Rect(rect.x+padding, rect.y+padding, rect.width-2*padding, rect.height-2*padding);
	Mat RoiImage; //�ڰ�
	Mat RoiImage_color;//��ͼ
	srcImage(rect_pad).copyTo(RoiImage);
	srcImage(rect_pad).copyTo(RoiImage_color);
	cvtColor(RoiImage,RoiImage,CV_RGB2GRAY);
	//imshow("Roi",RoiImage);
	//imwrite("Roi.jpg", RoiImage);
	

	//����ͶӰ
	Mat dstImage_1 = Mat(RoiImage.rows, RoiImage.cols, CV_8UC1, Scalar::all(255)); //��ʼ�����ͼ
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
	//����������Ƭ��Χ
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


	//����ͶӰ
	Mat dstImage_2 = Mat(RoiImage.rows, RoiImage.cols, CV_8UC1, Scalar::all(255)); //��ʼ�����ͼ
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
	//�����������Ƭ��Χ
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


	//�����ɫ���صı����ж��Ƿ������Ƭ
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
	faceCascade.detectMultiScale(imgGray, faces, scaleFactor, facecount, 0, Size(0, 0));   //�������
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
	
	//Ԥ����
	Mat pre_process_img = preProcess(srcImage);
	//�ҵ�ÿ����Ƭ����߽�
	vector<Rect> rects = findAngularPoint(pre_process_img);
	//��7����ʾ��������ͼ
	Mat dstImage;
	srcImage.copyTo(dstImage);
	for(int k=0; k<rects.size(); k++)
	{
		rectangle(dstImage,rects[k], Scalar(0,0,255),4);
	}
	//imshow("����ͼ", dstImage);
	//return dstImage;

	//��������
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

	//������
	Rect tmp = rects[rects.size()/2];
	int width = tmp.width;
	int height = tmp.height;
	n_row = srcImage.rows/height;
	n_col = srcImage.cols/width;

	//��ȡ��Ƭ
	
	for(int i=0; i<rects.size(); i++)
	{
		
		ImgData img_data = cutImage(rects[i]);
		bool find_in_faces = findInFaces(rects[i]);
		//������Ƭ
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
