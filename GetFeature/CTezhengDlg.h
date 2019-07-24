#pragma once


// CTezhengDlg 对话框

class CTezhengDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTezhengDlg)

public:
	CTezhengDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTezhengDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3_TEZHENG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
