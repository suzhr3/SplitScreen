// About.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "About.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CAbout 对话框

IMPLEMENT_DYNAMIC(CAbout, CDialogEx)

CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ABOUTDLG, pParent)
{

}

CAbout::~CAbout()
{
}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAbout, CDialogEx)
	ON_BN_CLICKED(IDC_QUEDING, &CAbout::OnBnClickedQueding)
END_MESSAGE_MAP()


// CAbout 消息处理程序


void CAbout::OnBnClickedQueding()
{
	// TODO: 在此添加控件通知处理程序代码

	//返回
	CDialogEx::OnCancel();
}
