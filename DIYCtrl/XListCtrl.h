#pragma once
#include "color.h"

class CXListCtrl : public CListCtrl
{
// Construction
public:
	CXListCtrl();

// Implementation
public:
	virtual void PreSubclassWindow();
	virtual ~CXListCtrl();
	virtual BOOL SetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL|LVSICF_NOSCROLL);

protected:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	void SetSubItemFont(LOGFONT font, COLORREF color, long lsize);
	BOOL GetHeaderItem(int nPos, HDITEM* pHeaderItem) { return GetHeaderCtrl()->GetItem(nPos, pHeaderItem); }
	void SetColor(int iRow, int iCol, COLORREF TextColor,COLORREF bkColor=WHITE);
	int m_iRow;
	int m_iCol;
	COLORREF m_Color;
	COLORREF m_TextColor;
	CFont *m_Font;
	long m_TextSize;
};


