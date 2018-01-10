
// ImageSegmentationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageSegmentation.h"
#include "ImageSegmentationDlg.h"
#include "CImageProcess.h"
#include "afxdialogex.h"
#include <opencv.hpp>
#include "Imagesplit.h"
#include "CvvImage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define SHOW_WINDOW "showPic"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImageSegmentationDlg 对话框




CImageSegmentationDlg::CImageSegmentationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageSegmentationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageSegmentationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageSegmentationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CImageSegmentationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageSegmentationDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CImageSegmentationDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageSegmentationDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CImageSegmentationDlg 消息处理程序

BOOL CImageSegmentationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	readIniFile();
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImageSegmentationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImageSegmentationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageSegmentationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




//打开图片
void CImageSegmentationDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog file_dialog(true, NULL, NULL);
	//CString strFilePath;
	//显示打开的文件对话框
	if(IDOK == file_dialog.DoModal())
	{
		m_file_path = file_dialog.GetPathName(); 
		SetDlgItemText(IDC_STATIC, m_file_path); //IDC_STATIC_State

		//显示
		string filepath_to_show = m_file_path.GetBuffer(0);
		Mat mat = imread(filepath_to_show);
		if(mat.data == NULL)
		{
			AfxMessageBox("不支持该类型图片");
			m_file_path = CString();//路径置空
			return;
		}
		CDC* pDC = GetDlgItem(IDC_MainPic)->GetDC();
		HDC hDC = pDC->GetSafeHdc();
		IplImage img = mat;
		CvvImage cimg;
		cimg.CopyOf(&img);
		CRect rect;
		GetDlgItem( IDC_MainPic )->GetClientRect(&rect);
		cimg.DrawToHDC(hDC, &rect);
		cimg.Destroy();  //注意释放空间
		ReleaseDC(pDC);     //释放

		//img.resize(200, 200);
		//imshow(SHOW_WINDOW, img);
	}
}


void CImageSegmentationDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

void CImageSegmentationDlg::readIniFile()
{
	TCHAR PATH[MAX_PATH+1]={NULL};
	GetModuleFileName(NULL, PATH, MAX_PATH);
	PathRemoveFileSpec(PATH);
	PathAppend(PATH, "ImgSeg.ini");

	CString threshold,threshold_2,facecount,area,padding,scaleFactor, rate;
	GetPrivateProfileString("RECT","threshold","200",threshold.GetBuffer(MAX_PATH),MAX_PATH,PATH);
	GetPrivateProfileString("RECT","threshold_2","220",threshold_2.GetBuffer(MAX_PATH),MAX_PATH,PATH);
	GetPrivateProfileString("RECT", "facecount", "3", facecount.GetBuffer(MAX_PATH), MAX_PATH,PATH);
	GetPrivateProfileString("RECT", "area", "30000",area.GetBuffer(MAX_PATH),MAX_PATH,PATH);
	GetPrivateProfileString("RECT", "padding", "20",padding.GetBuffer(MAX_PATH),MAX_PATH,PATH);
	GetPrivateProfileString("RECT", "scaleFactor","2.0", scaleFactor.GetBuffer(MAX_PATH),MAX_PATH,PATH );
	GetPrivateProfileString("RECT", "rate","0.35", rate.GetBuffer(MAX_PATH),MAX_PATH,PATH );
	m_threshold = atoi(threshold);
	m_threshold_2 = atoi(threshold_2);
	m_facecount = atoi(facecount);
	m_area = atof(area);
	m_padding = atoi(padding);
	m_scaleFactor = atof(scaleFactor);
	m_rate = atof(rate);
}

vector<string> mkdir(string filename)
{
	int index = filename.length()-1;
	for(; index>0 && filename[index]!='\\'; index--){}
	string dir_name = filename.substr(0,index);
	string file_name = filename.substr(++index);

	//处理文件名称里的小数点
	for(string::iterator iter = file_name.begin(); iter!= file_name.end();)
	{
		if(*iter == '.')
		{
			file_name.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	vector<string> s;
	s.push_back(dir_name);
	s.push_back(file_name);
	return s;
}
void CImageSegmentationDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATIC_State, "Processing..."); //IDC_STATIC_State

	if(m_file_path.IsEmpty())
	{
		SetDlgItemText(IDC_STATIC_State, "No Image!"); //IDC_STATIC_State
		return;
	}

	string filepath = m_file_path.GetBuffer(0);//图片路径
	CImgProcess plitimg(m_threshold, m_threshold_2, m_area, m_padding, m_scaleFactor, m_facecount,m_rate);
	plitimg.loadImage(filepath);//加载图片
	plitimg.setClassifier("haarcascade_frontalface_alt2.xml");//加载训练好的人脸识别神经网络
	//创建文件夹
	vector<string> filename = mkdir(filepath);
	string dir = filename[0];
	string name = filename[1];
	dir = dir+"\\"+name;
	//CString Cdir = dir.c_str();
	bool isCreateDir = CreateDirectory(dir.c_str(), NULL);
	if(!isCreateDir)
	{
		AfxMessageBox("创建文件夹失败");
		return;
	}

	bool isProceed = plitimg.processImage(string(dir+'\\'));

	
	if(isProceed)
	{
		SetDlgItemText(IDC_STATIC_State, "Complete!"); //IDC_STATIC_State
	}
	else
	{
		SetDlgItemText(IDC_STATIC_State, "Failure!"); 
	}
	
}


void CImageSegmentationDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	//SetDlgItemText(IDC_STATIC_State, "Processing..."); //IDC_STATIC_State

	//if(m_file_path.IsEmpty())
	//{
	//	SetDlgItemText(IDC_STATIC_State, "No Image!"); //IDC_STATIC_State
	//	return;
	//}
	readIniFile();
	string filepath = m_file_path.GetBuffer(0);//图片路径
	CImgProcess plitimg(m_threshold, m_threshold_2, m_area, m_padding, m_scaleFactor, m_facecount,m_rate);
	plitimg.loadImage(filepath);//加载图片
	plitimg.setClassifier("haarcascade_frontalface_alt2.xml");//加载训练好的人脸识别神经网络
	Mat mat = plitimg.contourtest();
	//waitKey(0);

	CDC* pDC = GetDlgItem(IDC_MainPic)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	IplImage img = mat;
	CvvImage cimg;
	cimg.CopyOf(&img);
	CRect rect;
	GetDlgItem( IDC_MainPic )->GetClientRect(&rect);
	cimg.DrawToHDC(hDC, &rect);
	cimg.Destroy();  //注意释放空间
	ReleaseDC(pDC);     //释放
}
