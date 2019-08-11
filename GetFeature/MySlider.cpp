#include "pch.h"
#include "MySlider.h"

BEGIN_MESSAGE_MAP(MySlider, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void MySlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CSliderCtrl::OnLButtonDown(nFlags, point);
	CRect   rectClient, rectChannel;
	GetClientRect(rectClient);
	GetChannelRect(rectChannel);
	int nMax = 0;
	int nMin = 0;
	GetRange(nMin, nMax);
	int nPos =
		(nMax - nMin) * (point.x - rectClient.left - rectChannel.left) / (rectChannel.right - rectChannel.left);
	SetPos(nPos);
	
}
