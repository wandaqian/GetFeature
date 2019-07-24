#pragma once


// CChachongDlg 对话框

class CChachongDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChachongDlg)

public:
	CChachongDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChachongDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4_CHACHONG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
