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
	CRect rect;
	GetDlgItem(IDC_PICTURE_SHOW)->GetClientRect(&rect);         //获取显示区    

	IplImage iplImg = IplImage(image);
	CvvImage cvvImg;											//创建一个CvvImage对象  
	cvvImg.CopyOf(&iplImg);
	cvvImg.DrawToHDC(hDC, &rect);
	cvvImg.Destroy();

	ShowWindow(SW_SHOW);
	ReleaseDC(pDC);
}

void CPictureDlg::ChangeSize(CWnd *pWnd, int cx, int cy)
{
	if (pWnd)  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建   
	{
		CRect rect;   //获取控件变化前的大小    
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标  

		//    cx/m_rect.Width()为对话框在横向的变化比例  
		rect.left = rect.left * cx / m_rect.Width();//调整控件大小  
		rect.right = rect.right * cx / m_rect.Width();
		rect.top = rect.top * cy / m_rect.Height();
		rect.bottom = rect.bottom * cy / m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小  
	}
}

void CPictureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (nType == 1) return;//最小化则什么都不做 

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_PICTURE_SHOW);
	ChangeSize(pWnd, cx, cy);
	GetClientRect(&m_rect);// 将变化后的对话框大小设为旧大小   
}

void CPictureDlg::OnClose()
{
	//CSplitScreenApp *app = (CSplitScreenApp *)AfxGetApp(); //生成指向应用程序类的指针 
	openExpanseDlg = false;		//修改变量openExpanseDlg
	CDialogEx::OnClose();
}
