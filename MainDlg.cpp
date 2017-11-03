// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "SplitScreenDlg.h"
#include "MainDlg.h"
#include "SYNDlg.h"
#include "HelpDlg.h"
#include "About.h"
#include "afxdialogex.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAINDLG, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SPLITSCREEN, &CMainDlg::OnBnClickedSplitscreen)
	ON_BN_CLICKED(IDC_ALONE, &CMainDlg::OnBnClickedAlone)
	ON_NOTIFY(NM_CLICK, IDC_HELP, &CMainDlg::OnNMClickHelp)
	ON_NOTIFY(NM_CLICK, IDC_ABOUT, &CMainDlg::OnNMClickAbout)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

//一键拼接分屏界面
void CMainDlg::OnBnClickedSplitscreen()
{

	//隐藏当前登录主界面
	this->ShowWindow(SW_HIDE);	

	CSYNDlg dlg;
	dlg.DoModal();

	//显示拼接播放的控制界面
	this->ShowWindow(SW_SHOW);
}

//单独播放界面
void CMainDlg::OnBnClickedAlone()
{
	// TODO: 在此添加控件通知处理程序代码
	//隐藏当前登录主界面
	this->ShowWindow(SW_HIDE);	

	CSplitScreenDlg dlg;
	dlg.DoModal();

	//显示单独播放的控制界面
	this->ShowWindow(SW_SHOW);
}


//点击帮助按钮
void CMainDlg::OnNMClickHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	//隐藏当前登录主界面
	this->ShowWindow(SW_HIDE);

	CHelpDlg dlg;
	dlg.DoModal();

	//显示单独播放的控制界面
	this->ShowWindow(SW_SHOW);

}


//点击关于按钮
void CMainDlg::OnNMClickAbout(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CAbout dlg;
	dlg.DoModal();

	//显示单独播放的控制界面
	this->ShowWindow(SW_SHOW);
}
