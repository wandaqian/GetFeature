
// GetFeatureDlg.h: 头文件
//

#pragma once
#include "CJiaohuDlg.h"
#include "CPiliangDlg.h"
#include "CTezhengDlg.h"
#include "CChachongDlg.h"

// CGetFeatureDlg 对话框
class CGetFeatureDlg : public CDialogEx
{
// 构造
public:
	CGetFeatureDlg(CWnd* pParent = nullptr);	// 标准构造函数

	//Tab控件的成员变量
	CJiaohuDlg m_jiaohuDlg;
	CPiliangDlg m_piliangDlg;
	CTezhengDlg m_tezhengDlg;
	CChachongDlg m_chachongDlg;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GETFEATURE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	CTabCtrl m_tab;
};
