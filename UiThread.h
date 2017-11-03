#pragma once
#include "afxwin.h"
#include "PictureDlg.h"

#define WM_SPLASH_NOTIFY (WM_USER + 1)

//UI线程
class CUiThread : public CWinThread
{
public:
	CUiThread();
	virtual ~CUiThread();

	//重载初始化和退出，以及初始化后自动运行线程这三个函数
	virtual BOOL InitInstance();

	virtual int ExitInstance();

	virtual int Run();

public:
	String path;
	CPictureDlg* dlg;
	VideoCapture capture;
	Mat frame;

	CRect rect;

	long totalFrameNumber;
	long currentFrame;


protected:

	//接收主线程传来的信息
	BOOL PreTranslateMessage(MSG* pMsg);

	//循环遍历消息队列函数
	void message();
};

