#pragma once
#include <afxcmn.h>
class MySlider :
	public CSliderCtrl
{
	

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

