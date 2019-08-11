// CChachongDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CChachongDlg.h"
#include "afxdialogex.h"


// CChachongDlg 对话框
// add for test

IMPLEMENT_DYNAMIC(CChachongDlg, CDialogEx)

CChachongDlg::CChachongDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4_CHACHONG, pParent)
{

}

CChachongDlg::~CChachongDlg()
{
}

void CChachongDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChachongDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChachongDlg 消息处理程序


BOOL CChachongDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//////////////////////////////////////////////////////////////////////////
	get_control_original_proportion();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CChachongDlg::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED)  //判断窗口是不是最小化了，因为窗口最小化之后 ，
			//窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
	{

		CRect rect;// 获取当前窗口大小
		for (std::list<control*>::iterator it = m_con_list.begin(); it != m_con_list.end(); it++) {
			CWnd* pWnd = GetDlgItem((*it)->Id);//获取ID为woc的空间的句柄
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
			rect.left = (*it)->scale[0] * cx;
			rect.right = (*it)->scale[1] * cx;
			rect.top = (*it)->scale[2] * cy;
			rect.bottom = (*it)->scale[3] * cy;
			pWnd->MoveWindow(rect);//设置控件大小
		}

	}
	GetClientRect(&m_rect);//将变化后的对话框大小设为旧大小
	CDialogEx::OnSize(nType, cx, cy);
}

void CChachongDlg::get_control_original_proportion() {
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild)
	{
		CRect rect;//获取当前窗口的大小
		control* tempcon = new control;
		CWnd* pWnd = GetDlgItem(::GetDlgCtrlID(hwndChild));//获取ID为woc的空间的句柄
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
		tempcon->Id = ::GetDlgCtrlID(hwndChild);//获得控件的ID;
		tempcon->scale[0] = (double)rect.left / m_rect.Width();//注意类型转换，不然保存成long型就直接为0了
		tempcon->scale[1] = (double)rect.right / m_rect.Width();
		tempcon->scale[2] = (double)rect.top / m_rect.Height();
		tempcon->scale[3] = (double)rect.bottom / m_rect.Height();
		m_con_list.push_back(tempcon);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}

