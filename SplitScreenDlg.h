// SplitScreenDlg.h : 头文件

#pragma once
#include "PictureDlg.h"
#include "UiThread.h"

using namespace std;

//四个线程函数
UINT ThreadProc1(LPVOID pM);
UINT ThreadProc2(LPVOID pM);
UINT ThreadProc3(LPVOID pM);
UINT ThreadProc4(LPVOID pM);

// CSplitScreenDlg 对话框
class CSplitScreenDlg : public CDialogEx
{
// 构造
public:
	CSplitScreenDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSplitScreenDlg();						//析构函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPLITSCREEN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:

	CUiThread* pThread1;
	CUiThread* pThread2;
	CUiThread* pThread3;
	CUiThread* pThread4;

	//四个工作者线程
	CWinThread* pic_pthread1;
	CWinThread* pic_pthread2;
	CWinThread* pic_pthread3;
	CWinThread* pic_pthread4;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnDblclickSmallPicture1();
	afx_msg void OnStnDblclickSmallPicture2();
	afx_msg void OnStnDblclickSmallPicture3();
	afx_msg void OnStnDblclickSmallPicture4();
	afx_msg void OnStnClickedSmallPicture1();
	afx_msg void OnStnClickedSmallPicture2();
	afx_msg void OnStnClickedSmallPicture3();
	afx_msg void OnStnClickedSmallPicture4();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedClose1();
	afx_msg void OnBnClickedClose2();
	afx_msg void OnBnClickedClose3();
	afx_msg void OnBnClickedClose4();
	afx_msg void OnBnClickedSyn();
	afx_msg void OnClose();
};
