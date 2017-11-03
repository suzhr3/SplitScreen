// HelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitScreen.h"
#include "HelpDlg.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CHelpDlg 对话框

IMPLEMENT_DYNAMIC(CHelpDlg, CDialogEx)

CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HELPDLG, pParent)
{

}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_HELP_RETURN, &CHelpDlg::OnBnClickedHelpReturn)
END_MESSAGE_MAP()


// CHelpDlg 消息处理程序


void CHelpDlg::OnBnClickedHelpReturn()
{
	// TODO: 在此添加控件通知处理程序代码

	//返回
	CDialogEx::OnCancel();
}
