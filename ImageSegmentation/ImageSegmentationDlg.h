
// ImageSegmentationDlg.h : ͷ�ļ�
//

#pragma once


// CImageSegmentationDlg �Ի���
class CImageSegmentationDlg : public CDialogEx
{
// ����
public:
	CImageSegmentationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IMAGESEGMENTATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	CRect show_rect;  //ͼ����ʾ���ڵĴ�С
	int m_threshold; //��ֵ
	int m_threshold_2; //��ֵ
	double m_area;
	int m_padding;
	double m_scaleFactor;
	double m_rate;

	int m_facecount;
	int m_minimumsize;
public:
	afx_msg void OnBnClickedButton2();
};
