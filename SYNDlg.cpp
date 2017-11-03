// SYNDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "SYNDlg.h"
#include "afxdialogex.h"

#include "MainDlg.h"

#include "CvvImage.h"
#include "Pic.h"
#include "PictureDlg.h"

#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;


//输出到四个屏幕的对话框
CPictureDlg* syn_dlg1 = NULL;

CPictureDlg* syn_dlg2 = NULL;

CPictureDlg* syn_dlg3 = NULL;

CPictureDlg* syn_dlg4 = NULL;


//读取进来的视频的相关信息
CString syn_videoPath;
VideoCapture syn_capture;
long totalframe;
long currentframe = 0;

//每一帧完整的图片
Mat syn_frame;

//将每一帧图片切成4份的图片
Mat imageROI1;
Mat imageROI2;
Mat imageROI3;
Mat imageROI4;

//用于判断当前播放状态的信号量
bool syn_isOpen = false;

bool syn_isOpenAndDBClick = false;

bool syn_video_play = false;

//播放每一帧图片的延迟
int syn_delay = 1;

//总屏幕数
int syn_total_num_of_screen = GetSystemMetrics(SM_CMONITORS);		

//副屏个数
int syn_num_without_PrimaryScreen = syn_total_num_of_screen - 1;

//存储屏幕的信息
CRect * syn_rect = new CRect[syn_total_num_of_screen];						


// CSYNDlg 对话框
IMPLEMENT_DYNAMIC(CSYNDlg, CDialogEx)


//检测屏幕的回调函数
BOOL CALLBACK SYN_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);


CSYNDlg::CSYNDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYNDLG, pParent)
{
	//初始化当前播放状态为假
	is_play = false;

	//获取连接到电脑的屏幕相关信息
	EnumDisplayMonitors(NULL, NULL, &SYN_MonitorEnumProc, 0);
}


//析构函数
CSYNDlg::~CSYNDlg()
{
}


BOOL CSYNDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//初始化对话框时使能暂停按钮不可用
	CButton *pBtn = (CButton *)GetDlgItem(IDC_SYN_PLAY);
	if (pBtn != NULL)
	{
		pBtn->EnableWindow(FALSE);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//获取屏幕信息的回调函数
BOOL CALLBACK SYN_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	//用于记录屏幕信息
	DISPLAY_DEVICE ddDisplay;
	ZeroMemory(&ddDisplay, sizeof(ddDisplay));
	ddDisplay.cb = sizeof(ddDisplay);

	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	int m = 1;
	//首先获取总屏幕个数，然后进行遍历，当遍历到主屏幕时标记为rect[0]，然后跳过 
	for (int i = 0; i < syn_total_num_of_screen; i++, m++)
	{
		//获得每个显示器的名字  
		EnumDisplayDevices(NULL, i, &ddDisplay, 0);

		//将各个显示器的相关信息依次存储到rect数组中
		EnumDisplaySettings(ddDisplay.DeviceName, ENUM_CURRENT_SETTINGS, &dm);

		//保存显示器信息
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);

		//获得显示器信息，将信息保存到monitorinfo中
		GetMonitorInfo(hMonitor, &monitorInfo);

		//如果当前检测到的第i个屏幕是主屏幕
		if (monitorInfo.dwFlags == MONITORINFOF_PRIMARY)	
		{
			syn_rect[0].left = dm.dmPosition.x;
			syn_rect[0].top = dm.dmPosition.y;
			syn_rect[0].right = dm.dmPelsWidth;
			syn_rect[0].bottom = dm.dmPelsHeight;
			m--;
		}
		else	 //不是主屏幕
		{
			syn_rect[m].left = dm.dmPosition.x;
			syn_rect[m].top = dm.dmPosition.y;
			syn_rect[m].right = dm.dmPelsWidth;
			syn_rect[m].bottom = dm.dmPelsHeight;
		}
	}
	return TRUE;
}


void CSYNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSYNDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SYN_LODE, &CSYNDlg::OnBnClickedSynLode)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SYN_PLAY, &CSYNDlg::OnBnClickedSynPlay)
	ON_BN_CLICKED(IDC_SYN_RETURN, &CSYNDlg::OnBnClickedSynReturn)
END_MESSAGE_MAP()


// CSYNDlg 消息处理程序

//点击导入视频按钮
void CSYNDlg::OnBnClickedSynLode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);

	if (filedlg.DoModal() == IDOK)
	{
		if(syn_num_without_PrimaryScreen <= 0)
		{
			MessageBox(L"未检测到显示器", L"报告");
		}
		else
		{
			syn_videoPath = filedlg.GetPathName();
			String path(CW2A(syn_videoPath.GetString()));

			//先释放资源，再重新打开capture类
			syn_capture.release();
			syn_capture.open(path);
			if (!syn_capture.isOpened())
				MessageBox(L"打开资源失败!");

			//获取整个视频的帧数
			totalframe = static_cast<long>(syn_capture.get(CV_CAP_PROP_FRAME_COUNT));

			if (syn_dlg1 != NULL)
			{
				delete syn_dlg1;
				syn_dlg1 = NULL;
			}
			if (syn_dlg2 != NULL)
			{
				delete syn_dlg2;
				syn_dlg2 = NULL;
			}
			if (syn_dlg3 != NULL)
			{
				delete syn_dlg3;
				syn_dlg3 = NULL;
			}
			if (syn_dlg4 != NULL)
			{
				delete syn_dlg4;
				syn_dlg4 = NULL;
			}

			//创建4个对话框
			if (syn_num_without_PrimaryScreen >= 1)
			{
				syn_dlg1 = new CPictureDlg();
				syn_dlg1->Create(IDD__PICTURE_SHOW, this);
			}
			if (syn_num_without_PrimaryScreen >= 2)
			{
				syn_dlg2 = new CPictureDlg();
				syn_dlg2->Create(IDD__PICTURE_SHOW, this);
			}
			if (syn_num_without_PrimaryScreen >= 3)
			{
				syn_dlg3 = new CPictureDlg();
				syn_dlg3->Create(IDD__PICTURE_SHOW, this);
			}
			if (syn_num_without_PrimaryScreen >= 4)
			{
				syn_dlg4 = new CPictureDlg();
				syn_dlg4->Create(IDD__PICTURE_SHOW, this);
			}
			is_play = true;

			if (syn_num_without_PrimaryScreen == 1)
			{
				MessageBox(L"未检测到显示器2, 3, 4", L"报告");
			}
			else if (syn_num_without_PrimaryScreen == 2)
			{
				MessageBox(L"未检测到显示器3, 4", L"报告");
			}
			else if (syn_num_without_PrimaryScreen == 3)
			{
				MessageBox(L"未检测到显示器4", L"报告");
			}
				
			//用于响应同步的定时器，定时时间和设置的延迟时间一致
			SetTimer(1, syn_delay, NULL);

			//改变按钮的使能状态
			GetDlgItem(IDC_SYN_LODE)->SetWindowText(L"切换视频源");

			GetDlgItem(IDC_SYN_PLAY)->SetWindowText(L"暂停");

			//使能按钮
			CButton *pBtn = (CButton *)GetDlgItem(IDC_SYN_PLAY);
			if (pBtn != NULL)
			{
				pBtn->EnableWindow(TRUE);
			}
		}
	}
}

//定时播放器
void CSYNDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加控件通知处理程序代码
	//一键同步拼接显示
	if (1 == nIDEvent)
	{
		//循环读取视频的每一帧
		if (syn_capture.read(syn_frame))		
		{
			if (currentframe <= totalframe)
			{
				imageROI1 = syn_frame(Range(0, syn_frame.rows / 2), Range(0, syn_frame.cols / 2));
				imageROI2 = syn_frame(Range(0, syn_frame.rows / 2), Range((syn_frame.cols / 2) + 1, syn_frame.cols));
				imageROI3 = syn_frame(Range((syn_frame.rows / 2) + 1, syn_frame.rows), Range(0, syn_frame.cols / 2));
				imageROI4 = syn_frame(Range((syn_frame.rows / 2) + 1, syn_frame.rows), Range((syn_frame.cols / 2) + 1, syn_frame.cols));

				if (syn_num_without_PrimaryScreen >= 1)
				{
					syn_dlg1->MoveWindow(syn_rect[1].left, syn_rect[1].top, syn_rect[1].right, syn_rect[1].bottom, true);
					syn_dlg1->showImage(imageROI1);

					//在图片缩略框1中显示图片
					CDC* pDC1 = GetDlgItem(IDC_SYN_SMALL1)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
					HDC hDC1 = pDC1->GetSafeHdc();							  // 获取设备上下文句柄  
					CRect rect1;
					GetDlgItem(IDC_SYN_SMALL1)->GetClientRect(&rect1);		  //获取IDC_SMALL_PICTURE4控件的显示区    

					IplImage iplImg1 = IplImage(imageROI1);
					CvvImage cvvImg1;										  //创建一个CvvImage对象  
					cvvImg1.CopyOf(&iplImg1);
					cvvImg1.DrawToHDC(hDC1, &rect1);
					cvvImg1.Destroy();
					ReleaseDC(pDC1);
				}

				if (syn_num_without_PrimaryScreen >= 2)
				{
					syn_dlg2->MoveWindow(syn_rect[2].left, syn_rect[2].top, syn_rect[2].right, syn_rect[2].bottom, true);
					syn_dlg2->showImage(imageROI2);

					//在图片缩略框2中显示图片
					CDC* pDC2 = GetDlgItem(IDC_SYN_SMALL2)->GetDC();				 //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
					HDC hDC2 = pDC2->GetSafeHdc();								 // 获取设备上下文句柄  
					CRect rect2;
					GetDlgItem(IDC_SYN_SMALL2)->GetClientRect(&rect2);		 //获取IDC_SMALL_PICTURE4控件的显示区    

					IplImage iplImg2 = IplImage(imageROI2);
					CvvImage cvvImg2;										 //创建一个CvvImage对象  
					cvvImg2.CopyOf(&iplImg2);
					cvvImg2.DrawToHDC(hDC2, &rect2);
					cvvImg2.Destroy();
					ReleaseDC(pDC2);
				}

				if (syn_num_without_PrimaryScreen >= 3)
				{
					syn_dlg3->MoveWindow(syn_rect[3].left, syn_rect[3].top, syn_rect[3].right, syn_rect[3].bottom, true);
					syn_dlg3->showImage(imageROI3);

					//在图片缩略框3中显示图片
					CDC* pDC3 = GetDlgItem(IDC_SYN_SMALL3)->GetDC();				//获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
					HDC hDC3 = pDC3->GetSafeHdc();								 // 获取设备上下文句柄  
					CRect rect3;
					GetDlgItem(IDC_SYN_SMALL3)->GetClientRect(&rect3);		//获取IDC_SMALL_PICTURE4控件的显示区    

					IplImage iplImg3 = IplImage(imageROI3);
					CvvImage cvvImg3;										//创建一个CvvImage对象  
					cvvImg3.CopyOf(&iplImg3);
					cvvImg3.DrawToHDC(hDC3, &rect3);
					cvvImg3.Destroy();
					ReleaseDC(pDC3);
				}

				if (syn_num_without_PrimaryScreen >= 4)
				{
					syn_dlg4->MoveWindow(syn_rect[4].left, syn_rect[4].top, syn_rect[4].right, syn_rect[4].bottom, true);
					syn_dlg4->showImage(imageROI4);

					//在图片缩略框3中显示图片
					CDC* pDC4 = GetDlgItem(IDC_SYN_SMALL4)->GetDC();				 //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
					HDC hDC4 = pDC4->GetSafeHdc();								 // 获取设备上下文句柄  
					CRect rect4;
					GetDlgItem(IDC_SYN_SMALL4)->GetClientRect(&rect4);		//获取IDC_SMALL_PICTURE4控件的显示区    

					IplImage iplImg4 = IplImage(imageROI4);
					CvvImage cvvImg4;										//创建一个CvvImage对象  
					cvvImg4.CopyOf(&iplImg4);
					cvvImg4.DrawToHDC(hDC4, &rect4);
					cvvImg4.Destroy();
					ReleaseDC(pDC4);
				}
				currentframe++;
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


//点击暂停按钮
void CSYNDlg::OnBnClickedSynPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	//单击按钮一次时，如果视频正在播放，则暂停，反之则播放
	if (is_play == true)
	{
		//单击暂停
		KillTimer(1);				
		is_play = false;
		GetDlgItem(IDC_SYN_PLAY)->SetWindowText(L"播放");
	}
	else
	{
		//单击继续播放
		SetTimer(1, syn_delay, NULL);	
		is_play = true;
		GetDlgItem(IDC_SYN_PLAY)->SetWindowText(L"暂停");
	}
}


//点击退出按钮
void CSYNDlg::OnBnClickedSynReturn()
{
	// TODO: 在此添加控件通知处理程序代码
	//回收资源
	if (syn_dlg1 != NULL)
	{
		KillTimer(1);
		syn_dlg1->CloseWindow();
		delete syn_dlg1;
		syn_dlg1 = NULL;
	}
	if (syn_dlg2 != NULL)
	{
		KillTimer(1);
		syn_dlg2->CloseWindow();
		delete syn_dlg2;
		syn_dlg2 = NULL;
	}
	if (syn_dlg3 != NULL)
	{
		KillTimer(1);
		syn_dlg3->CloseWindow();
		delete syn_dlg3;
		syn_dlg3 = NULL;
	}
	if (syn_dlg4 != NULL)
	{
		KillTimer(1);
		syn_dlg4->CloseWindow();
		delete syn_dlg4;
		syn_dlg4 = NULL;
	}
	
	is_play = false;
	CDialogEx::OnCancel();
}


//点击右上角关闭按钮
void CSYNDlg::OnClose()
{
	// TODO: 在此添加控件通知处理程序代码
	//回收资源
	if (syn_dlg1 != NULL)
	{
		KillTimer(1);
		syn_dlg1->CloseWindow();
		delete syn_dlg1;
		syn_dlg1 = NULL;
	}
	if (syn_dlg2 != NULL)
	{
		KillTimer(1);
		syn_dlg2->CloseWindow();
		delete syn_dlg2;
		syn_dlg2 = NULL;
	}
	if (syn_dlg3 != NULL)
	{
		KillTimer(1);
		syn_dlg3->CloseWindow();
		delete syn_dlg3;
		syn_dlg3 = NULL;
	}
	if (syn_dlg4 != NULL)
	{
		KillTimer(1);
		syn_dlg4->CloseWindow();
		delete syn_dlg4;
		syn_dlg4 = NULL;
	}

	is_play = false;

	//关闭当前窗口
	CDialogEx::OnClose();	

	//关闭主界面窗口
	AfxGetMainWnd()->SendMessage(WM_CLOSE);		
}