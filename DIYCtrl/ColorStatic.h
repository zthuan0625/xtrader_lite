#pragma once

#include "color.h"
/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
public:
	void SetBkColor(COLORREF crColor); // This Function is to set the BackGround Color for the Text.
	CColorStatic();

	CWnd* GetWnd(){ return static_cast<CWnd*>(this); }
	void SetText(LPCTSTR lpszString,COLORREF crColor=WHITE,UINT uAlign=DT_RIGHT);
	void SetDouble(double dVal,COLORREF crColor=WHITE,UINT uAlign=DT_RIGHT);
	void SetInt(int iVal,COLORREF crColor=WHITE,UINT uAlign=DT_RIGHT);
	CString GetText() const { return m_szText;}
	double GetDouble() const { return _tcstod(m_szText,NULL);}
	BOOL SetFont(LPCTSTR szFont,int iHeight=15,int iWeight=FW_SEMIBOLD);
	int GetFont(LOGFONT* pLogFont);
// Overrides
	virtual ~CColorStatic();
	virtual BOOL OnEraseBkgnd(CDC* pDC);

protected:
	CString m_szText;
	CFont	m_cFont;
	
	CBrush m_brBkgnd; 
	COLORREF m_crBkColor; 
	COLORREF m_crTextColor;
	UINT	m_uAlign;
	double	m_dScale;

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
/*
private:
	CRITICAL_SECTION   m_cs ;
	void Lock() { EnterCriticalSection(&m_cs); }
    void UnLock() { LeaveCriticalSection(&m_cs); }
*/
};

