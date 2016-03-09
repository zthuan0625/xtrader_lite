#pragma once

class CSliderEx : public CSliderCtrl
{
public:
	DECLARE_DYNAMIC(CSliderEx)
	CSliderEx() : m_bDragging(false), m_bDragChanged(false) , m_bFocus(false)
	{}
protected:
	//{{AFX_MSG(CRoundSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool SetThumb(const CPoint& pt);
	void PostMessageToParent(const int nTBCode) const;
	bool m_bDragging;
	bool m_bDragChanged;
	bool m_bFocus;

};