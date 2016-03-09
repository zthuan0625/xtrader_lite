#pragma once

class CMyMsgBox : public CWnd
{
	DECLARE_DYNAMIC(CMyMsgBox)

public:
	CMyMsgBox(CWnd* pParent);

	int MessageBox(LPCTSTR lpszText,LPCTSTR lpTitle, int count, bool bclose = false, UINT uType = MB_ICONINFORMATION );	

protected:
	int m_count;
	bool m_autoclose;	
	HWND  m_hWndParent;

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSubclassedInit(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};
