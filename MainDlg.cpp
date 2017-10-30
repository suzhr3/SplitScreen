// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "SplitScreenDlg.h"
#include "MainDlg.h"
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
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

//一键拼接分屏界面
void CMainDlg::OnBnClickedSplitscreen()
{
	this->ShowWindow(SW_HIDE);	//当前主界面隐藏
	CSplitScreenDlg dlg;		
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
}

//单独播放界面
void CMainDlg::OnBnClickedAlone()
{
	this->ShowWindow(SW_HIDE);	//当前主界面隐藏
	CSplitScreenDlg dlg;
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
}
