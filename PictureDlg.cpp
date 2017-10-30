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


// CPictureDlg 消息处理程序
void CPictureDlg::showImage(Mat& image)
{
	CDC* pDC = GetDlgItem(IDC_PICTURE_SHOW)->GetDC();           //根据ID获得窗口指针再获取与该窗口关联的上下文指针  
	HDC hDC = pDC->GetSafeHdc();								// 获取设备上下文句柄  
	CRect winRect;
	GetDlgItem(IDC_PICTURE_SHOW)->GetClientRect(&winRect);         //获取显示区    

	IplImage iplImg;
	//resize后的frame
	Mat newImg;
	int newWidth, newHeight;
	bool is_resize = false;
	if (winRect.Width() < image.cols)		//如果图片的分辨率比屏幕的分辨率大，则resize
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
		resize(image, newImg, Size(winRect.Width(), winRect.Height()), 0, 0, CV_INTER_LINEAR);
		iplImg = IplImage(image);
	}
	else
	{
		iplImg = IplImage(image);
	}
	CvvImage cvvImg;											//创建一个CvvImage对象  
	cvvImg.CopyOf(&iplImg);
	cvvImg.DrawToHDC(hDC, &winRect);
	cvvImg.Destroy();

	ShowWindow(SW_SHOW);
	ReleaseDC(pDC);
}

void CPictureDlg::ShowSmallPic(CDC* pDC, Mat frame, CRect rect)
{
	HDC hDC = pDC->GetSafeHdc();							 // 获取设备上下文句柄  	
															 //resize后的frame
	Mat newImg;
	//将原图像resize为要显示的客户区（这里是缩略框的大小）的分辨率大小，这里使用的是内插值的方法
	resize(frame, newImg, Size(rect.Width(), rect.Height()), 0, 0, CV_INTER_LINEAR);

	IplImage iplImg = IplImage(newImg);
	CvvImage cvvImg;										//创建一个CvvImage对象  
	cvvImg.CopyOf(&iplImg);
	cvvImg.DrawToHDC(hDC, &rect);
	cvvImg.Destroy();
}

void CPictureDlg::ChangeSize(CWnd *pWnd, int cx, int cy)
{
	if (pWnd)  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建   
	{
		CRect rect;					  
		pWnd->GetWindowRect(&rect);		//获取整个对话框窗体变化前的大小并保存到rect中  
		ScreenToClient(&rect);			//将控件大小转换为在对话框中的区域坐标  

		//cx/m_rect.Width()为对话框在横向的变化比例  
		rect.left = rect.left * cx / m_rect.Width();//调整控件大小  
		rect.right = rect.right * cx / m_rect.Width();
		rect.top = rect.top * cy / m_rect.Height();
		rect.bottom = rect.bottom * cy / m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小  
	}
}

void CPictureDlg::OnSize(UINT nType, int cx, int cy)
{
	if (nType == 1) return;//最小化则什么都不做 

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_PICTURE_SHOW);
	ChangeSize(pWnd, cx, cy);
	GetClientRect(&m_rect);// 将变化后的对话框大小设为旧大小   

	CDialogEx::OnSize(nType, cx, cy);
}

void CPictureDlg::OnClose()
{
	openExpanseDlg = false;		//修改变量openExpanseDlg，即将本地全屏显示关闭了

	CDialogEx::OnClose();
}
