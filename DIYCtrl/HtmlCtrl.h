#pragma once

#include "FixHtmlView.h"

typedef struct { CString sName; COleVariant vValue; } CBodyAttr;

typedef CTypedPtrList<CPtrList, CBodyAttr*> CBodyAttrList;

class CHtmlCtrl : public CFixHtmlView
{
protected:
	CHtmlCtrl();        
	DECLARE_DYNCREATE(CHtmlCtrl)

// html Data
public:
	//{{AFX_DATA(CHtmlCtrl)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void NavigateMemory();
	void NavigateNormal();
	BOOL SetCharset(LPCTSTR lpstrCharset);
	BOOL SetTitle(LPCTSTR sTitle);
	BOOL SetScript(LPCTSTR lpstrScript);
	BOOL SetBodyAttributes(CString sName, COleVariant vValue);
	BOOL PutBodyContent(LPCTSTR lpszContent);
	BOOL CreateFromStatic(UINT nID, CWnd* pParent);
	HWND GetBrowserWindow();

	virtual void PostNcDestroy() { }
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,DWORD nFlags,LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData,LPCTSTR lpszHeaders,BOOL* pbCancel );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlCtrl)
	public:
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnDownloadComplete();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHtmlCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CHtmlCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	LPTSTR m_lpstrCharset;
	LPTSTR m_lpstrTitle;
	BOOL m_bMemoryMode;
	IHTMLDocument2* m_pHtmlDoc2;
	void ShowMemoryHtml();
	CBodyAttrList m_BodyAttributes;
	LPTSTR m_lpstrScript;
	LPTSTR m_lpBodyContent;
};


class CMyIEWnd : public CWnd {
public:
	CMyIEWnd() { }
	~CMyIEWnd() { }
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC(CMyIEWnd)
};

class CMyHtmlCtrl : public CHtmlCtrl {
protected:
	CMyIEWnd m_myIEWnd;
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	DECLARE_DYNAMIC(CMyHtmlCtrl)
};

