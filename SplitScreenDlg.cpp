// SplitScreenDlg.cpp : 实现文件

#include "stdafx.h"
#include "SplitScreen.h"
#include "SplitScreenDlg.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#include "CvvImage.h"
#include "Pic.h"
#include "UiThread.h"

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

struct winPt 
{
	CDC* pDC;
	CRect rect;
};
winPt* pt = new winPt[4];		//传给四个线程使用的参数

CPictureDlg* dlg1 = NULL;
CPictureDlg* dlg2 = NULL;
CPictureDlg* dlg3 = NULL;
CPictureDlg* dlg4 = NULL;

bool close_thread1 = false;
bool close_thread2 = false;
bool close_thread3 = false;
bool close_thread4 = false;


int delay = 1;		//相当于每秒30帧
int total_num_of_screen = GetSystemMetrics(SM_CMONITORS);		//总屏幕数
int num_without_PrimaryScreen = total_num_of_screen - 1;		//副屏个数
CRect * rect = new CRect[total_num_of_screen];					//存储屏幕的信息									

//用于同步用
CString syn_videoPath;
VideoCapture syn_capture;
long totalframe;
long currentframe = 0;
Mat syn_frame;
Mat imageROI1, imageROI2, imageROI3, imageROI4;
bool syn_isOpen = false;
bool syn_isOpenAndDBClick = false;
bool syn_video_play = false;

CPictureDlg* expanse1 = NULL;			//点击4个缩略图对象之后弹出的4个全屏对话框
CPictureDlg* expanse2 = NULL;
CPictureDlg* expanse3 = NULL;
CPictureDlg* expanse4 = NULL;

Pic * pic = new Pic[4];					//对应4幅播放的视频或者图片的对象

//检测屏幕的回调函数
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//构造函数
CSplitScreenDlg::CSplitScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPLITSCREEN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}
//析构函数
CSplitScreenDlg::~CSplitScreenDlg()
{
	
}


void CSplitScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CSplitScreenDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_STN_DBLCLK(IDC_SMALL_PICTURE1, &CSplitScreenDlg::OnStnDblclickSmallPicture1)
	ON_STN_DBLCLK(IDC_SMALL_PICTURE2, &CSplitScreenDlg::OnStnDblclickSmallPicture2)
	ON_STN_DBLCLK(IDC_SMALL_PICTURE3, &CSplitScreenDlg::OnStnDblclickSmallPicture3)
	ON_STN_DBLCLK(IDC_SMALL_PICTURE4, &CSplitScreenDlg::OnStnDblclickSmallPicture4)
	ON_STN_CLICKED(IDC_SMALL_PICTURE1, &CSplitScreenDlg::OnStnClickedSmallPicture1)
	ON_STN_CLICKED(IDC_SMALL_PICTURE2, &CSplitScreenDlg::OnStnClickedSmallPicture2)
	ON_STN_CLICKED(IDC_SMALL_PICTURE3, &CSplitScreenDlg::OnStnClickedSmallPicture3)
	ON_STN_CLICKED(IDC_SMALL_PICTURE4, &CSplitScreenDlg::OnStnClickedSmallPicture4)
	ON_BN_CLICKED(IDCANCEL, &CSplitScreenDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLOSE1, &CSplitScreenDlg::OnBnClickedClose1)
	ON_BN_CLICKED(IDC_CLOSE2, &CSplitScreenDlg::OnBnClickedClose2)
	ON_BN_CLICKED(IDC_CLOSE3, &CSplitScreenDlg::OnBnClickedClose3)
	ON_BN_CLICKED(IDC_CLOSE4, &CSplitScreenDlg::OnBnClickedClose4)
	ON_BN_CLICKED(IDC_SYN, &CSplitScreenDlg::OnBnClickedSyn)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSplitScreenDlg 消息处理程序
BOOL CSplitScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
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
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	//获取显示器相关信息
	EnumDisplayMonitors(NULL, NULL, &MonitorEnumProc, 0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
//EnumDisplayMonitors的回调函数
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	DISPLAY_DEVICE ddDisplay;
	ZeroMemory(&ddDisplay, sizeof(ddDisplay));
	ddDisplay.cb = sizeof(ddDisplay);

	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	int m = 1;
	for (int i = 0; i < total_num_of_screen; i++, m++)//首先获取总屏幕个数，然后进行遍历，当遍历到主屏幕时标记为rect[0]，然后跳过 
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
		if (monitorInfo.dwFlags == MONITORINFOF_PRIMARY)	//如果当前检测到的第i个屏幕是主屏幕
		{
			rect[0].left = dm.dmPosition.x;
			rect[0].top = dm.dmPosition.y;
			rect[0].right = dm.dmPelsWidth;
			rect[0].bottom = dm.dmPelsHeight;
			m--;
		}
		else												//不是主屏幕
		{
			rect[m].left = dm.dmPosition.x;
			rect[m].top = dm.dmPosition.y;
			rect[m].right = dm.dmPelsWidth;
			rect[m].bottom = dm.dmPelsHeight;
		}	
	}
	return TRUE;
}

//BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
//{
//	//保存显示器信息
//	MONITORINFO monitorinfo;
//	monitorinfo.cbSize = sizeof(MONITORINFO);
//
//	//获得显示器信息，将信息保存到monitorinfo中
//	GetMonitorInfo(hMonitor, &monitorinfo);
//
//	//若检测到主屏
//	if (monitorinfo.dwFlags == MONITORINFOF_PRIMARY)
//	{
//		//将显示器的分辨率信息保存到rect[0]
//		rect[0] = monitorinfo.rcMonitor;
//	}
//	else //检测到其他屏幕
//	{
//		rect[m] = monitorinfo.rcMonitor;
//		m++;
//	}
//	return TRUE;
//}

void CSplitScreenDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSplitScreenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSplitScreenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



UINT ThreadProc1(LPVOID pM)
{
	winPt* pt = (winPt*)pM;			//获取线程传过来的参数

	pic[0].capture.open(pic[0].videoPath);
	if (!pic[0].capture.isOpened())
		AfxMessageBox(L"打开资源失败!");
	//获取视频总帧数
	pic[0].totalFrameNumber = static_cast<long>(pic[0].capture.get(CV_CAP_PROP_FRAME_COUNT));
	dlg1->MoveWindow(rect[1].left, rect[1].top, rect[1].right, rect[1].bottom, true);
	while (1)
	{
		if (pic[0].capture.read(pic[0].frame))	//循环读取视频的每一帧
		{
			//在屏幕1上显示
			dlg1->showImage(pic[0].frame);
			//在主窗口的第1个图片缩略框中显示
			dlg1->ShowSmallPic(pt->pDC, pic[0].frame, pt->rect);

			//双击了缩略图1则放大缩略图1
			if (expanse1 != NULL && expanse1->openExpanseDlg == true)
			{
				expanse1->showImage(pic[0].frame);
			}
		}
		else			//视频播放完毕或者读取帧出错
			break;
		if (close_thread1 == true)
		{
			return 0;
		}
		Sleep(1);		//视频播放每帧间隔（单位:ms）
	}
	return 0;
}
UINT ThreadProc2(LPVOID pM)
{
	winPt* pt = (winPt*)pM;			//获取线程传过来的参数

	pic[1].capture.open(pic[1].videoPath);
	if (!pic[1].capture.isOpened())
		AfxMessageBox(L"打开资源失败!");
	//获取视频总帧数
	pic[1].totalFrameNumber = static_cast<long>(pic[1].capture.get(CV_CAP_PROP_FRAME_COUNT));
	dlg2->MoveWindow(rect[2].left, rect[2].top, rect[2].right, rect[2].bottom, true);
	while (close_thread2 == false)
	{
		if (pic[1].capture.read(pic[1].frame))	//循环读取视频的每一帧
		{
			//在屏幕2上显示
			dlg2->showImage(pic[1].frame);
			//在主窗口的第2个图片缩略框中显示
			dlg2->ShowSmallPic(pt->pDC, pic[1].frame, pt->rect);

			//双击了缩略图2则放大缩略图2
			if (expanse2 != NULL && expanse2->openExpanseDlg == true)
			{
				expanse2->showImage(pic[1].frame);
			}
		}
		else			//视频播放完毕或者读取帧出错
			break;
		if (close_thread2 == true)
		{
			return 0;
		}
		Sleep(1);		//视频播放每帧间隔（单位:ms）
	}
	return 0;
}
UINT ThreadProc3(LPVOID pM)
{
	winPt* pt = (winPt*)pM;

	pic[2].capture.open(pic[2].videoPath);
	if (!pic[2].capture.isOpened())
		AfxMessageBox(L"打开资源失败!");
	//获取视频总帧数
	pic[2].totalFrameNumber = static_cast<long>(pic[2].capture.get(CV_CAP_PROP_FRAME_COUNT));
	dlg3->MoveWindow(rect[3].left, rect[3].top, rect[3].right, rect[3].bottom, true);

	while (close_thread3 == false)
	{
		if (pic[2].capture.read(pic[2].frame))	//循环读取视频的每一帧
		{
			//在屏幕3上显示
			dlg3->showImage(pic[2].frame);
			//在主窗口的第3个图片缩略框中显示
			dlg3->ShowSmallPic(pt->pDC, pic[2].frame, pt->rect);
			//双击了缩略图3则放大缩略图3
			if (expanse3 != NULL && expanse3->openExpanseDlg == true)
			{
				expanse3->showImage(pic[2].frame);
			}
		}
		else			//视频播放完毕或者读取帧出错
			break;
		if (close_thread3 == true)
		{
			return 0;
		}
		Sleep(1);		//视频播放每帧间隔（单位:ms）
	}
	return 0;
}
UINT ThreadProc4(LPVOID pM)
{
	winPt* pt = (winPt*)pM;

	pic[3].capture.open(pic[3].videoPath);
	if (!pic[3].capture.isOpened())
		AfxMessageBox(L"打开资源失败!");
	//获取视频总帧数
	pic[3].totalFrameNumber = static_cast<long>(pic[3].capture.get(CV_CAP_PROP_FRAME_COUNT));
	dlg4->MoveWindow(rect[0].left, rect[0].top, rect[0].right, rect[0].bottom, true);

	while (close_thread4 == false)
	{
		if (pic[3].capture.read(pic[3].frame))	//循环读取视频的每一帧
		{
			//在屏幕4上显示
			dlg4->showImage(pic[3].frame);
			//在主窗口的第4个图片缩略框中显示
			dlg4->ShowSmallPic(pt->pDC, pic[3].frame, pt->rect);
			//双击了缩略图4则放大缩略图4
			if (expanse4 != NULL && expanse4->openExpanseDlg == true)
			{
				expanse4->showImage(pic[3].frame);
			}
		}
		else			//视频播放完毕或者读取帧出错
			break;
		if (close_thread4 == true)
		{
			return 0;
		}
		Sleep(1);		//视频播放每帧间隔（单位:ms）
	}
	return 0;
}


//在缩略图1上双击
void CSplitScreenDlg::OnStnDblclickSmallPicture1()
{
	//如果缩略图1已经在播放视频了，则双击它之后会放大缩略图内容
	if (pic[0].isOpen == true)
	{
		if (expanse1 != NULL)
		{
			expanse1->CloseWindow();
			delete expanse1;
			expanse1 = NULL;
		}
		expanse1 = new CPictureDlg();
		expanse1->Create(IDD__PICTURE_SHOW, this);
		expanse1->SetWindowText(L"屏幕一的全屏");
		expanse1->MoveWindow(rect[0].left, rect[0].top, rect[0].right, rect[0].bottom, true);	//在主屏幕上全屏显示
		SetWindowLong(expanse1->GetSafeHwnd(), GWL_STYLE, GetWindowLong(expanse1->m_hWnd, GWL_STYLE) + WS_CAPTION);
		expanse1->openExpanseDlg = true;
	}
	//如果缩略图1没有在播放视频，则双击它之后会提示用户导入资源
	else if (num_without_PrimaryScreen >= 1 && pic[0].isOpen == false)
	{
		CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
		CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);
		if (filedlg.DoModal() == IDOK)
		{
			if (NULL == dlg1)
			{
				dlg1 = new CPictureDlg();
				dlg1->Create(IDD__PICTURE_SHOW, this);
			}
			CString cpath = filedlg.GetPathName();
			pic[0].videoPath = CW2A(cpath.GetString());					//获取屏幕1的视频播放路径
			pic[0].isOpen = true;										//屏幕1打开了
		    pic[0].video_play = true;									//屏幕1正在播放

			//获取主窗口上第1个缩略图的相关句柄和指针
			CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE1)->GetDC();         //获得缩略图1上控件的窗口指针，再获取与该窗口关联的上下文指针 
			CRect rect;
			GetDlgItem(IDC_SMALL_PICTURE1)->GetClientRect(&rect);		//获取缩略图1上控件的显示区 
			pt[0].pDC = pDC;
			pt[0].rect = rect;

			pic_pthread1 = AfxBeginThread(ThreadProc1, &pt[0]);			//启动线程1来播放视频1
		}
	}
	else
		MessageBox(L"未检测到显示器1", L"报告");
}

//在缩略图2上双击
void CSplitScreenDlg::OnStnDblclickSmallPicture2()
{
	//如果缩略图2已经在播放视频了，则双击它之后会放大缩略图内容
	if (pic[1].isOpen == true)
	{
		if (expanse2 != NULL)
		{
			expanse2->CloseWindow();
			delete expanse2;
			expanse2 = NULL;
		}
		expanse2 = new CPictureDlg();
		expanse2->Create(IDD__PICTURE_SHOW, this);
		expanse2->SetWindowText(L"屏幕二的全屏");
		expanse2->MoveWindow(rect[0].left, rect[0].top, rect[0].right, rect[0].bottom, true);	//在主屏幕上全屏显示
		SetWindowLong(expanse2->GetSafeHwnd(), GWL_STYLE, GetWindowLong(expanse2->m_hWnd, GWL_STYLE) + WS_CAPTION);
		expanse2->openExpanseDlg = true;
	}
	//如果缩略图2没有在播放视频，则双击它之后会提示用户导入资源
	else if (num_without_PrimaryScreen >= 2 && pic[1].isOpen == false)
	{
		CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
		CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);
		if (filedlg.DoModal() == IDOK)
		{
			if (NULL == dlg2)
			{
				dlg2 = new CPictureDlg();
				dlg2->Create(IDD__PICTURE_SHOW, this);
			}
			CString cpath = filedlg.GetPathName();
			pic[1].videoPath = CW2A(cpath.GetString());					//获取屏幕2的视频播放路径
			pic[1].isOpen = true;										//屏幕2打开了
			pic[1].video_play = true;									//屏幕2正在播放

			//获取主窗口上第2个缩略图的相关句柄和指针
			CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE2)->GetDC();         //获得缩略图2上控件的窗口指针，再获取与该窗口关联的上下文指针 
			CRect rect;
			GetDlgItem(IDC_SMALL_PICTURE2)->GetClientRect(&rect);		//获取缩略图2上控件的显示区 
			pt[1].pDC = pDC;
			pt[1].rect = rect;

			pic_pthread2 = AfxBeginThread(ThreadProc2, &pt[1]);			//启动线程2来播放视频2
		}
	}
	else
		MessageBox(L"未检测到显示器2", L"报告");
}

//在缩略图3上双击
void CSplitScreenDlg::OnStnDblclickSmallPicture3()
{
	//如果缩略图3已经在播放视频了，则双击它之后会放大缩略图内容
	if (pic[2].isOpen == true)
	{
		if (expanse3 != NULL)
		{
			expanse3->CloseWindow();
			delete expanse3;
			expanse3 = NULL;
		}
		expanse3 = new CPictureDlg();
		expanse3->Create(IDD__PICTURE_SHOW, this);
		expanse3->SetWindowText(L"屏幕三的全屏");
		expanse3->MoveWindow(rect[0].left, rect[0].top, rect[0].right, rect[0].bottom, true);	//在主屏幕上全屏显示
		SetWindowLong(expanse3->GetSafeHwnd(), GWL_STYLE, GetWindowLong(expanse3->m_hWnd, GWL_STYLE) + WS_CAPTION);
		expanse3->openExpanseDlg = true;
	}
	//如果缩略图3没有在播放视频，则双击它之后会提示用户导入资源
	else if (num_without_PrimaryScreen >= 3 && pic[2].isOpen == false)
	{
		CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
		CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);
		if (filedlg.DoModal() == IDOK)
		{		
			if (NULL == dlg3)
			{
				dlg3 = new CPictureDlg();
				dlg3->Create(IDD__PICTURE_SHOW, this);
			}
			CString cpath = filedlg.GetPathName();
			pic[2].videoPath = CW2A(cpath.GetString());					//获取屏幕3的视频播放路径
			pic[2].isOpen = true;										//屏幕3打开了
			pic[2].video_play = true;									//屏幕3正在播放

			//获取主窗口上第3个缩略图的相关句柄和指针
			CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE3)->GetDC();         //获得缩略图3上控件的窗口指针，再获取与该窗口关联的上下文指针 
			CRect rect;
			GetDlgItem(IDC_SMALL_PICTURE3)->GetClientRect(&rect);		//获取缩略图3上控件的显示区 
			pt[2].pDC = pDC;
			pt[2].rect = rect;

			pic_pthread3 = AfxBeginThread(ThreadProc3, &pt[2]);			//启动线程3来播放视频3
		}
	}
	else
		MessageBox(L"未检测到显示器3", L"报告");
}

//在缩略图4上双击
void CSplitScreenDlg::OnStnDblclickSmallPicture4()
{
	//如果缩略图4已经在播放视频了，则双击它之后会放大缩略图内容
	if (pic[3].isOpen == true)
	{
		if (expanse4 != NULL)
		{
			expanse4->CloseWindow();
			delete expanse4;
			expanse4 = NULL;
		}
		expanse4 = new CPictureDlg();
		expanse4->Create(IDD__PICTURE_SHOW, this);
		expanse4->SetWindowText(L"屏幕四的全屏");
		expanse4->MoveWindow(rect[0].left, rect[0].top, rect[0].right, rect[0].bottom, true);	//在主屏幕上全屏显示
		SetWindowLong(expanse4->GetSafeHwnd(), GWL_STYLE, GetWindowLong(expanse4->m_hWnd, GWL_STYLE) + WS_CAPTION);
		expanse4->openExpanseDlg = true;
	}
	//如果缩略图4没有在播放视频，则双击它之后会提示用户导入资源
	else if (num_without_PrimaryScreen >= 4 && pic[3].isOpen == false)
	{
		CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
		CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);
		if (filedlg.DoModal() == IDOK)
		{
			if (NULL == dlg4)
			{
				dlg4 = new CPictureDlg();
				dlg4->Create(IDD__PICTURE_SHOW, this);
			}
			CString cpath = filedlg.GetPathName();
			pic[3].videoPath = CW2A(cpath.GetString());					//获取屏幕4的视频播放路径
			pic[3].isOpen = true;										//屏幕4打开了
			pic[3].video_play = true;									//屏幕4正在播放

			//获取主窗口上第4个缩略图的相关句柄和指针
			CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE4)->GetDC();         //获得缩略图4上控件的窗口指针，再获取与该窗口关联的上下文指针 
			CRect rect;
			GetDlgItem(IDC_SMALL_PICTURE4)->GetClientRect(&rect);		//获取缩略图4上控件的显示区 
			pt[3].pDC = pDC;
			pt[3].rect = rect;

			pic_pthread4 = AfxBeginThread(ThreadProc4, &pt[3]);			//启动线程4来播放视频4
		}
	}
	else
		MessageBox(L"未检测到显示器4", L"报告");
}

//在缩略图1上单击
void CSplitScreenDlg::OnStnClickedSmallPicture1()
{
	MSG message;
	DWORD st = GetTickCount();
	while (1)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if (message.message == WM_LBUTTONDBLCLK)	//单机时如果是双击事件则返回执行双击事件
				return;
		}
		DWORD et = GetTickCount();
		if (et - st > 200)
			break;
	}
	//单击缩略图一次时，如果视频正在播放，则暂停，反之则播放
	if (pic[0].isOpen == true && pic[0].video_play == true)
	{
		pic_pthread1->SuspendThread();				//单击暂停
		pic[0].video_play = false;
	}
	else if (pic[0].isOpen == true && pic[0].video_play == false)
	{
		pic_pthread1->ResumeThread();				//单击继续播放
		pic[0].video_play = true;
	}
}

//在缩略图2上单击
void CSplitScreenDlg::OnStnClickedSmallPicture2()
{
	MSG message;
	DWORD st = GetTickCount();
	while (1)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if (message.message == WM_LBUTTONDBLCLK)	//单机时如果是双击事件则返回执行双击事件
				return;
		}
		DWORD et = GetTickCount();
		if (et - st > 200)
			break;
	}
	//单击缩略图一次时，如果视频正在播放，则暂停，反之则播放
	if (pic[1].isOpen == true && pic[1].video_play == true)
	{
		pic_pthread2->SuspendThread();					//单击暂停
		pic[1].video_play = false;
	}
	else if (pic[1].isOpen == true && pic[1].video_play == false)
	{
		pic_pthread2->ResumeThread();	//单击继续播放
		pic[1].video_play = true;
	}
}

//在缩略图3上单击
void CSplitScreenDlg::OnStnClickedSmallPicture3()
{
	MSG message;
	DWORD st = GetTickCount();
	while (1)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if (message.message == WM_LBUTTONDBLCLK)	//单机时如果是双击事件则返回执行双击事件
				return;
		}
		DWORD et = GetTickCount();
		if (et - st > 200)
			break;
	}
	//单击缩略图一次时，如果视频正在播放，则暂停，反之则播放
	if (pic[2].isOpen == true && pic[2].video_play == true)
	{
		pic_pthread3->SuspendThread();				//单击暂停
		pic[2].video_play = false;
	}
	else if (pic[2].isOpen == true && pic[2].video_play == false)
	{
		pic_pthread3->ResumeThread();	//单击继续播放
		pic[2].video_play = true;
	}
}

//在缩略图4上单击
void CSplitScreenDlg::OnStnClickedSmallPicture4()
{
	MSG message;
	DWORD st = GetTickCount();
	while (1)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if (message.message == WM_LBUTTONDBLCLK)	//单机时如果是双击事件则返回执行双击事件
				return;
		}
		DWORD et = GetTickCount();
		if (et - st > 200)
			break;
	}
	//单击缩略图一次时，如果视频正在播放，则暂停，反之则播放
	if (pic[3].isOpen == true && pic[3].video_play == true)
	{
		pic_pthread4->SuspendThread();				//单击暂停
		pic[3].video_play = false;
	}
	else if (pic[3].isOpen == true && pic[3].video_play == false)
	{
		pic_pthread4->ResumeThread();	//单击继续播放
		pic[3].video_play = true;
	}
}


void CSplitScreenDlg::OnBnClickedSyn()
{
	CString fileFilter = _T("媒体文件(*.wmv,*.mp3,*.avi,,*.rm,*.rmvb,*.mkv,*.mp4)|*.wmv;*.mp3;*.avi;*.rm;*.rmvb;*.mkv;*.mp4|");
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR, fileFilter);

	if (filedlg.DoModal() == IDOK)
	{
		syn_videoPath = filedlg.GetPathName();
		String path(CW2A(syn_videoPath.GetString()));

		syn_capture.open(path);
		if (!syn_capture.isOpened())
			MessageBox(L"打开资源失败!");

		//获取整个帧数
		totalframe = static_cast<long>(syn_capture.get(CV_CAP_PROP_FRAME_COUNT));
		OnBnClickedClose1();
		OnBnClickedClose2();
		OnBnClickedClose3();
		OnBnClickedClose4();

		dlg1 = new CPictureDlg();
		dlg2 = new CPictureDlg();
		dlg3 = new CPictureDlg();
		dlg4 = new CPictureDlg();
		dlg1->Create(IDD__PICTURE_SHOW, this);
		dlg2->Create(IDD__PICTURE_SHOW, this);
		dlg3->Create(IDD__PICTURE_SHOW, this);
		dlg4->Create(IDD__PICTURE_SHOW, this);
		SetTimer(5, delay, NULL);				//用于响应同步的定时器，定时时间和帧率一致

	}
	else
		MessageBox(L"未检测到显示器", L"报告");

}

//定时播放视频
void CSplitScreenDlg::OnTimer(UINT_PTR nIDEvent)
{
	//一键同步拼接显示
	if (5 == nIDEvent)
	{
		if (syn_capture.read(syn_frame))		//循环读取视频的每一帧
		{
			if (currentframe <= totalframe)
			{
				imageROI1 = syn_frame(Range(0, syn_frame.rows / 2), Range(0, syn_frame.cols / 2));
				imageROI2 = syn_frame(Range(0, syn_frame.rows / 2), Range((syn_frame.cols / 2) + 1, syn_frame.cols));
				imageROI3 = syn_frame(Range((syn_frame.rows / 2) + 1, syn_frame.rows), Range(0, syn_frame.cols / 2));
				imageROI4 = syn_frame(Range((syn_frame.rows / 2) + 1, syn_frame.rows), Range((syn_frame.cols / 2) + 1, syn_frame.cols));

				dlg1->MoveWindow(rect[1].left, rect[1].top, rect[1].right, rect[1].bottom, true);
				dlg1->showImage(imageROI1);
				dlg2->MoveWindow(rect[2].left, rect[2].top, rect[2].right, rect[2].bottom, true);
				dlg2->showImage(imageROI2);
				dlg3->MoveWindow(rect[3].left, rect[3].top, rect[3].right, rect[3].bottom, true);
				dlg3->showImage(imageROI3);
				dlg4->MoveWindow(rect[4].left, rect[4].top, rect[4].right, rect[4].bottom, true);
				dlg4->showImage(imageROI4);

				//在四个图片缩略框中显示图片
				CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE1)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
				HDC hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
				CRect rect1;
				GetDlgItem(IDC_SMALL_PICTURE1)->GetClientRect(&rect1);		//获取IDC_SMALL_PICTURE4控件的显示区    

				IplImage iplImg1 = IplImage(imageROI1);
				CvvImage cvvImg1;											//创建一个CvvImage对象  
				cvvImg1.CopyOf(&iplImg1);
				cvvImg1.DrawToHDC(hDC, &rect1);
				cvvImg1.Destroy();
				ReleaseDC(pDC);

				pDC = GetDlgItem(IDC_SMALL_PICTURE2)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
				hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
				CRect rect2;
				GetDlgItem(IDC_SMALL_PICTURE2)->GetClientRect(&rect2);		//获取IDC_SMALL_PICTURE4控件的显示区    

				IplImage iplImg2 = IplImage(imageROI2);
				CvvImage cvvImg2;											//创建一个CvvImage对象  
				cvvImg2.CopyOf(&iplImg2);
				cvvImg2.DrawToHDC(hDC, &rect2);
				cvvImg2.Destroy();
				ReleaseDC(pDC);

				pDC = GetDlgItem(IDC_SMALL_PICTURE3)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
				hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
				CRect rect3;
				GetDlgItem(IDC_SMALL_PICTURE3)->GetClientRect(&rect3);		//获取IDC_SMALL_PICTURE4控件的显示区    

				IplImage iplImg3 = IplImage(imageROI3);
				CvvImage cvvImg3;											//创建一个CvvImage对象  
				cvvImg3.CopyOf(&iplImg3);
				cvvImg3.DrawToHDC(hDC, &rect3);
				cvvImg3.Destroy();
				ReleaseDC(pDC);

				pDC = GetDlgItem(IDC_SMALL_PICTURE4)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
				hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
				CRect rect4;
				GetDlgItem(IDC_SMALL_PICTURE4)->GetClientRect(&rect4);		//获取IDC_SMALL_PICTURE4控件的显示区    

				IplImage iplImg4 = IplImage(imageROI4);
				CvvImage cvvImg4;											//创建一个CvvImage对象  
				cvvImg4.CopyOf(&iplImg4);
				cvvImg4.DrawToHDC(hDC, &rect4);
				cvvImg4.Destroy();
				ReleaseDC(pDC);
				currentframe++;
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

//关闭屏幕1
void CSplitScreenDlg::OnBnClickedClose1()
{
	if (dlg1 != NULL)
	{
		close_thread1 = true;
		Sleep(100);

		dlg1->CloseWindow();
		delete dlg1;
		dlg1 = NULL;
		pic[0].capture.release();
		pic[0].isOpen = false;
		pic[0].video_play = false;

		CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE1)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
		HDC hDC = pDC->GetSafeHdc();								  // 获取设备上下文句柄  
		CRect rect;
		GetDlgItem(IDC_SMALL_PICTURE1)->GetClientRect(&rect);		  //获取IDC_SMALL_PICTURE1控件的显示区   

		COLORREF clrBack = pDC->GetBkColor();
		CBrush br(clrBack);
		pDC->FillRect(rect, &br);
	}
}
//关闭屏幕2
void CSplitScreenDlg::OnBnClickedClose2()
{
	if (dlg2 != NULL)
	{
		close_thread2 = true;
		Sleep(100);

		dlg2->CloseWindow();
		delete dlg2;
		dlg2 = NULL;
		pic[1].capture.release();
		pic[1].isOpen = false;
		pic[1].video_play = false;

		CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE2)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
		HDC hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
		CRect rect;
		GetDlgItem(IDC_SMALL_PICTURE2)->GetClientRect(&rect);		//获取IDC_SMALL_PICTURE1控件的显示区   

		COLORREF clrBack = pDC->GetBkColor();
		CBrush br(clrBack);
		pDC->FillRect(rect, &br);
	}
}
//关闭屏幕3
void CSplitScreenDlg::OnBnClickedClose3()
{
	if (dlg3 != NULL)
	{
		close_thread3 = true;
		Sleep(100);

		dlg3->CloseWindow();
		delete dlg3;
		dlg3 = NULL;
		pic[2].capture.release();
		pic[2].isOpen = false;
		pic[2].video_play = false;

		CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE3)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
		HDC hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
		CRect rect;
		GetDlgItem(IDC_SMALL_PICTURE3)->GetClientRect(&rect);		//获取IDC_SMALL_PICTURE1控件的显示区   

		COLORREF clrBack = pDC->GetBkColor();
		CBrush br(clrBack);
		pDC->FillRect(rect, &br);
	}
}
//关闭屏幕4
void CSplitScreenDlg::OnBnClickedClose4()
{
	if (dlg4 != NULL)
	{
		close_thread4 = true;
		Sleep(100);

		dlg4->CloseWindow();
		delete dlg4;
		dlg4 = NULL;
		pic[3].capture.release();
		pic[3].isOpen = false;
		pic[3].video_play = false;

		CDC* pDC = GetDlgItem(IDC_SMALL_PICTURE4)->GetDC();           //获得IDC_SMALL_PICTURE1控件的窗口指针，再获取与该窗口关联的上下文指针  
		HDC hDC = pDC->GetSafeHdc();								 // 获取设备上下文句柄  
		CRect rect;
		GetDlgItem(IDC_SMALL_PICTURE4)->GetClientRect(&rect);		//获取IDC_SMALL_PICTURE1控件的显示区   

		COLORREF clrBack = pDC->GetBkColor();
		CBrush br(clrBack);
		pDC->FillRect(rect, &br);
	}
}

//点击退出按钮
void CSplitScreenDlg::OnBnClickedCancel()
{
	close_thread1 = true;
	close_thread2 = true;
	close_thread3 = true;
	close_thread4 = true;

	Sleep(200);			//等待200ms后才关闭资源，目的是让子线程有充足的时间去退出并释放资源

	if (dlg1 != NULL)
	{
		delete dlg1;
		dlg1 = NULL;
	}
	if (dlg2 != NULL)
	{
		delete dlg2;
		dlg2 = NULL;
	}
	if (dlg3 != NULL)
	{
		delete dlg3;
		dlg3 = NULL;
	}
	if (dlg4 != NULL)
	{
		delete dlg4;
		dlg4 = NULL;
	}
	if (pt != NULL)
	{
		delete[] pt;
		pt = NULL;
	}
	CDialogEx::OnCancel();							//关闭当前窗口
	AfxGetMainWnd()->SendMessage(WM_CLOSE);			//关闭主界面窗口
}
//点击右上角关闭按钮
void CSplitScreenDlg::OnClose()
{
	close_thread1 = true;
	close_thread2 = true;
	close_thread3 = true;
	close_thread4 = true;
	Sleep(200);			//等待200ms后才关闭资源，目的是让子线程有充足的时间去退出并释放资源

	if (dlg1 != NULL)
	{
		delete dlg1;
		dlg1 = NULL;
	}
	if (dlg2 != NULL)
	{
		delete dlg2;
		dlg2 = NULL;
	}
	if (dlg3 != NULL)
	{
		delete dlg3;
		dlg3 = NULL;
	}
	if (dlg4 != NULL)
	{
		delete dlg4;
		dlg4 = NULL;
	}
	if (pt != NULL)
	{
		delete[] pt;
		pt = NULL;
	}
	CDialogEx::OnClose();						//关闭当前窗口
	AfxGetMainWnd()->SendMessage(WM_CLOSE);		//关闭主界面窗口
}
