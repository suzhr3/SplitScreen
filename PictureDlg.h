#pragma once

#include "stdafx.h"

#include "CvvImage.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class CPictureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPictureDlg)

public:
	CPictureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPictureDlg();

	BOOL OnInitDialog();

	//在图片控件上显示图片image
	void showImage(Mat& image);			
	//在缩略图控件上显示图片frame
	void ShowSmallPic(CDC* pDC, Mat frame, CRect rect);

	CRect m_rect;						//记录当前对话框的大小
	void ChangeSize(CWnd *pWnd, int cx, int cy);


	bool openExpanseDlg;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD__PICTURE_SHOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
