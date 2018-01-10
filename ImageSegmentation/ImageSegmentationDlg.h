
// ImageSegmentationDlg.h : 头文件
//

#pragma once


// CImageSegmentationDlg 对话框
class CImageSegmentationDlg : public CDialogEx
{
// 构造
public:
	CImageSegmentationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IMAGESEGMENTATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
private:
	void readIniFile();
	CString m_file_path;
	CRect show_rect;  //图像显示窗口的大小
	int m_threshold; //阈值
	int m_threshold_2; //阈值
	double m_area;
	int m_padding;
	double m_scaleFactor;
	double m_rate;

	int m_facecount;
	int m_minimumsize;
public:
	afx_msg void OnBnClickedButton2();
};
