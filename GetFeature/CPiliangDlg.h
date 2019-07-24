#pragma once


// CPiliangDlg 对话框

class CPiliangDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPiliangDlg)

public:
	CPiliangDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPiliangDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2_PILIANG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
