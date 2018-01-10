
// ImageSegmentationDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CImageSegmentationDlg �Ի���




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


// CImageSegmentationDlg ��Ϣ�������

BOOL CImageSegmentationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	readIniFile();
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CImageSegmentationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CImageSegmentationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




//��ͼƬ
void CImageSegmentationDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog file_dialog(true, NULL, NULL);
	//CString strFilePath;
	//��ʾ�򿪵��ļ��Ի���
	if(IDOK == file_dialog.DoModal())
	{
		m_file_path = file_dialog.GetPathName(); 
		SetDlgItemText(IDC_STATIC, m_file_path); //IDC_STATIC_State

		//��ʾ
		string filepath_to_show = m_file_path.GetBuffer(0);
		Mat mat = imread(filepath_to_show);
		if(mat.data == NULL)
		{
			AfxMessageBox("��֧�ָ�����ͼƬ");
			m_file_path = CString();//·���ÿ�
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
		cimg.Destroy();  //ע���ͷſռ�
		ReleaseDC(pDC);     //�ͷ�

		//img.resize(200, 200);
		//imshow(SHOW_WINDOW, img);
	}
}


void CImageSegmentationDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	//�����ļ��������С����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDlgItemText(IDC_STATIC_State, "Processing..."); //IDC_STATIC_State

	if(m_file_path.IsEmpty())
	{
		SetDlgItemText(IDC_STATIC_State, "No Image!"); //IDC_STATIC_State
		return;
	}

	string filepath = m_file_path.GetBuffer(0);//ͼƬ·��
	CImgProcess plitimg(m_threshold, m_threshold_2, m_area, m_padding, m_scaleFactor, m_facecount,m_rate);
	plitimg.loadImage(filepath);//����ͼƬ
	plitimg.setClassifier("haarcascade_frontalface_alt2.xml");//����ѵ���õ�����ʶ��������
	//�����ļ���
	vector<string> filename = mkdir(filepath);
	string dir = filename[0];
	string name = filename[1];
	dir = dir+"\\"+name;
	//CString Cdir = dir.c_str();
	bool isCreateDir = CreateDirectory(dir.c_str(), NULL);
	if(!isCreateDir)
	{
		AfxMessageBox("�����ļ���ʧ��");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//SetDlgItemText(IDC_STATIC_State, "Processing..."); //IDC_STATIC_State

	//if(m_file_path.IsEmpty())
	//{
	//	SetDlgItemText(IDC_STATIC_State, "No Image!"); //IDC_STATIC_State
	//	return;
	//}
	readIniFile();
	string filepath = m_file_path.GetBuffer(0);//ͼƬ·��
	CImgProcess plitimg(m_threshold, m_threshold_2, m_area, m_padding, m_scaleFactor, m_facecount,m_rate);
	plitimg.loadImage(filepath);//����ͼƬ
	plitimg.setClassifier("haarcascade_frontalface_alt2.xml");//����ѵ���õ�����ʶ��������
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
	cimg.Destroy();  //ע���ͷſռ�
	ReleaseDC(pDC);     //�ͷ�
}
