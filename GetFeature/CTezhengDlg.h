#pragma once
#include<list>

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
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnSize(UINT nType, int cx, int cy); //当窗口大小调整时调用
	void get_control_original_proportion();
	CRect m_rect;
	typedef struct Rect {
	public:
		int Id;
		double scale[4];

		Rect() {
			Id = -2;
			scale[0] = 0;
			scale[1] = 0;
			scale[2] = 0;
			scale[3] = 0;
		}

		Rect(const Rect& c) {
			*this = c;
		}
	}control;
	std::list<control*> m_con_list;
};
