
// ImageSegmentation.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CImageSegmentationApp:
// �йش����ʵ�֣������ ImageSegmentation.cpp
//

class CImageSegmentationApp : public CWinApp
{
public:
	CImageSegmentationApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CImageSegmentationApp theApp;