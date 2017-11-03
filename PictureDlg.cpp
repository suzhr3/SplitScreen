// PictureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "PictureDlg.h"
#include "afxdialogex.h"

using namespace std;
using namespace cv;

IMPLEMENT_DYNAMIC(CPictureDlg, CDialogEx)

CPictureDlg::CPictureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD__PICTURE_SHOW, pParent)
{
	openExpanseDlg = false;
}

CPictureDlg::~CPictureDlg()
{
	
}

void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPictureDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CPictureDlg::OnInitDialog()
{
	//获取初始化时的对话框大小
	GetClientRect(&m_rect);  

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// CPictureDlg 消息处理程序

//将视频的每一帧图片输出
void CPictureDlg::showImage(Mat& image)
{
	//根据ID获得窗口指针再获取与该窗口关联的上下文指针  
	CDC* pDC = GetDlgItem(IDC_PICTURE_SHOW)->GetDC();    
	// 获取设备上下文句柄  
	HDC hDC = pDC->GetSafeHdc();		

	//获取显示区  
	CRect winRect;	  
	GetDlgItem(IDC_PICTURE_SHOW)->GetClientRect(&winRect);        

	IplImage iplImg;

	//resize后的frame
	Mat newImg;
	int newWidth;
	int newHeight;
	bool is_resize = false;

	//如果图片的分辨率比屏幕的分辨率大，则resize
	if (winRect.Width() < image.cols)		
	{
		is_resize = true;
		newWidth = image.cols;
	}
	if (winRect.Height() < image.rows)
	{
		is_resize = true;
		newHeight = image.rows;
	}
	if (is_resize == true)
	{
		//将原图像resize为要显示的客户区的分辨率大小，这里使用的是内插值的方法
		resize(image, newImg, Size(winRect.Width(), winRect.Height()), 0, 0, INTER_CUBIC);
		iplImg = IplImage(image);
	}
	else
	{
		iplImg = IplImage(image);
	}

	//创建一个CvvImage对象  
	CvvImage cvvImg;											
	cvvImg.CopyOf(&iplImg);
	cvvImg.DrawToHDC(hDC, &winRect);
	cvvImg.Destroy();

	//将图片画到图片控件后显示该图片对话框
	ShowWindow(SW_SHOW);

	ReleaseDC(pDC);
}


//将视频的每一帧图片输出
void CPictureDlg::ShowSmallPic(CDC* pDC, Mat frame, CRect rect)
{
	// 获取设备上下文句柄
	HDC hDC = pDC->GetSafeHdc();

	//resize后的frame
	Mat newImg;
	int newWidth = rect.Width();
	int newHeight = rect.Height();

	//将原图像resize为要显示的客户区（这里是缩略框的大小）的分辨率大小，这里使用的是插值的方法
	resize(frame, newImg, Size(newWidth, newHeight), 0, 0, INTER_CUBIC);

	IplImage iplImg = IplImage(newImg);

	//创建一个CvvImage对象 
	CvvImage cvvImg;										 
	cvvImg.CopyOf(&iplImg);
	cvvImg.DrawToHDC(hDC, &rect);
	cvvImg.Destroy();
}

//在对话框的大小改变时动态改变控件的位置和大小
void CPictureDlg::ChangeSize(CWnd *pWnd, int cx, int cy)
{
	//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
	if (pWnd)    
	{
		//获取整个对话框窗体变化前的大小并保存到rect中
		CRect rect;					  
		pWnd->GetWindowRect(&rect);	

		//将控件大小转换为在对话框中的区域坐标  
		ScreenToClient(&rect);			

		// cx / m_rect.Width()为对话框在横向的变化比例 
		//调整控件大小  
		rect.left = rect.left * cx / m_rect.Width();
		rect.right = rect.right * cx / m_rect.Width();
		rect.top = rect.top * cy / m_rect.Height();
		rect.bottom = rect.bottom * cy / m_rect.Height();

		//设置控件的位置和大小  
		pWnd->MoveWindow(rect);
	}

}

//将对话框输出到不同屏幕时动态修改对话框的大小
void CPictureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//最小化对话框时什么都不做 
	if (nType == SIZE_MINIMIZED) return;

	//根据ID获得该图片控件的窗口指针
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_PICTURE_SHOW);

	//调用函数修改该图片控件动态随对话框的改变而改变位置和大小
	ChangeSize(pWnd, cx, cy);

	//更新变化后的对话框大小   
	GetClientRect(&m_rect);
}

//点击右上角关闭时
void CPictureDlg::OnClose()
{
	//修改变量openExpanseDlg，表示将本地全屏显示的对话框关闭了
	openExpanseDlg = false;		

	CDialogEx::OnClose();
}
