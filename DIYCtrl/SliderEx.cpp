#include "stdafx.h"
#include "SliderEx.h"

IMPLEMENT_DYNAMIC(CSliderEx, CSliderCtrl)

BEGIN_MESSAGE_MAP(CSliderEx, CSliderCtrl)
	//{{AFX_MSG_MAP(CSliderEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSliderEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDragging = true;
	m_bFocus = true;

	SetCapture();
	//SetFocus();

	if (SetThumb(point))
	{
		PostMessageToParent(TB_THUMBTRACK);
	}
}
 
void CSliderEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		if (SetThumb(point))
		{
			PostMessageToParent(TB_THUMBTRACK);
		}
	}
	else 
	{
		CSliderCtrl::OnMouseMove(nFlags, point);
	}
}

void CSliderEx::OnLButtonUp(UINT nFlags, CPoint point) 
{ 
	if (m_bDragging)
	{
		m_bDragging = false;
		m_bFocus = false;

		::ReleaseCapture();

		if (SetThumb(point))
		{ 
			PostMessageToParent(TB_THUMBPOSITION);
		}

		PostMessageToParent(TB_ENDTRACK);
	}
	else
	{
		CSliderCtrl::OnLButtonUp(nFlags, point);
	}
}



bool CSliderEx::SetThumb(const CPoint& point)
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;
	CRect rc;
	GetChannelRect(rc);
	double dPos;
	double dCorrectionFactor = 0.0;
	if (GetStyle() & TBS_VERT) 
	{
		// note: there is a bug in GetChannelRect, it gets the orientation of the rectangle mixed up
		dPos = (double)(point.y - rc.left)/(rc.right - rc.left);
	}
	else
	{
		dPos = (double)(point.x - rc.left)/(rc.right - rc.left);
	}
	// This correction factor is needed when you click inbetween tick marks
	// so that the thumb will move to the nearest one
	dCorrectionFactor = 0.5 *(1-dPos) - 0.5 *dPos;
	int nNewPos = (int)(nMin + (nMax-nMin)*dPos + dCorrectionFactor);
	const bool bChanged = (nNewPos != GetPos());
	if(bChanged)
	{
		SetPos(nNewPos);
	}
	return bChanged;
}

void CSliderEx::PostMessageToParent(const int nTBCode) const
{
	CWnd* pWnd = GetParent();
	if(pWnd) pWnd->PostMessage(WM_HSCROLL, (WPARAM)((GetPos() << 16) | nTBCode), (LPARAM)GetSafeHwnd());
}

void CSliderEx::OnSetFocus(CWnd* pOldWnd)
{
	CSliderCtrl::OnSetFocus(pOldWnd);

	m_bFocus = true;
	Invalidate(FALSE);
}

void CSliderEx::OnKillFocus(CWnd* pNewWnd)
{
	CSliderCtrl::OnKillFocus(pNewWnd);

	m_bFocus = false;
	Invalidate(FALSE);
}
