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
	// 标准构造函数
	CPictureDlg(CWnd* pParent = NULL);  

	virtual ~CPictureDlg();

	//初始化图片对话框的函数
	BOOL OnInitDialog();

	//在图片控件上显示图片image
	void showImage(Mat& image);			

	//在控制界面的缩略图控件上显示图片frame
	void ShowSmallPic(CDC* pDC, Mat frame, CRect rect);

	//记录当前对话框的大小
	CRect m_rect;						

	//用于动态改变控件的大小
	void ChangeSize(CWnd *pWnd, int cx, int cy);

	//用于判断当前是否全屏显示某个屏幕
	bool openExpanseDlg;


// 对话框数据

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD__PICTURE_SHOW };
#endif


protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
