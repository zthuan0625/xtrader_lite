
#pragma once

#define DTS_YEAR	0
#define DTS_MONTH	5
#define DTS_DAY		8
#define DTS_HOUR	11
#define DTS_MINUTE	14
#define DTS_SECOND	17

/////////////////////////////////////////////////////////////////////////////
// CDateTimeEdit window

class CDateTimeEdit : public CEdit
{
// Construction
public:
	CDateTimeEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateTimeEdit)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetDateTime(const COleDateTime& dateSrc);
	BOOL GetDateTime(COleDateTime& dateSrc);
	BOOL EnableWindow(BOOL bEnable = TRUE);
	void ClearDateTime();
	BOOL LessThanDate(COleDateTime& dt);
	virtual ~CDateTimeEdit();

	// Generated message map functions
protected:
	int SmartSel(int nPos);
	//{{AFX_MSG(CDateTimeEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	int m_nPos;
	BOOL m_bEnable;
	DECLARE_MESSAGE_MAP()
};
