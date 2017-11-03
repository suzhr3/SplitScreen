#pragma once


// CSYNDlg 对话框

class CSYNDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSYNDlg)

public:
	// 标准构造函数
	CSYNDlg(CWnd* pParent = NULL);  
	//析构函数
	virtual ~CSYNDlg();

	//用于判断当前一键拼接界面是否正在播放视频
	bool is_play;

	BOOL OnInitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYNDLG };
#endif

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSynLode();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedSynPlay();
	afx_msg void OnBnClickedSynReturn();
};
