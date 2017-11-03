#include "stdafx.h"
#include "SplitScreen.h"

#include "UiThread.h"
#include "Pic.h"
#include "PictureDlg.h"

//构造函数

CUiThread::CUiThread()
{
	currentFrame = 0;
}

//析构函数

CUiThread::~CUiThread()
{
}

//UI线程对话框初始化函数

BOOL CUiThread::InitInstance()
{
	dlg = new CPictureDlg();
	dlg->Create(IDD__PICTURE_SHOW);			//创建该线程显示的对话框窗口

	m_pMainWnd = dlg;						//设置CWinThread类的m_pMainWnd成员，否则这个线程不会随着界面的关闭而退出。

	return true;
}

//UI线程对话框退出时自动执行的退出函数

int CUiThread::ExitInstance()
{
	dlg->DestroyWindow();//销毁窗口
	delete dlg;
	return 0;
}

//重写的线程Run函数

int CUiThread::Run()
{
	capture.open(path);
	if (!capture.isOpened())
		AfxMessageBox(L"打开资源失败!");

	//获取视频总帧数
	totalFrameNumber = static_cast<long>(capture.get(CV_CAP_PROP_FRAME_COUNT));
	dlg->MoveWindow(rect.left, rect.top, rect.right, rect.bottom, true);
	while (1)
	{
		if (capture.read(frame))	//循环读取视频的每一帧
		{
			if (currentFrame <= totalFrameNumber)
			{
				//在线程对应的屏幕上显示
				dlg->showImage(frame);
				currentFrame++;
			}
			else
			{
				break;
			}
			Sleep(20);
		}
		else
		{
			break;
		}
			
	}
	return 0;
}

//循环遍历消息队列函数
void CUiThread::message()
{
	MSG msg;

	//循环取出消息队列中的消息并分发下去
	while (1)
	{
		GetMessage(&msg, NULL, 0, 0);
		DispatchMessage(&msg);
	}
}

//接收到来自主线程的消息
BOOL CUiThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_SPLASH_NOTIFY)
	{
		message();
	}
	return CWinThread::PreTranslateMessage(pMsg);
}