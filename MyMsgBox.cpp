#include "stdafx.h"
#include "MyMsgBox.h"

IMPLEMENT_DYNAMIC(CMyMsgBox, CWnd)
CMyMsgBox::CMyMsgBox(CWnd* pParent)
{
	m_hWndParent = pParent->GetSafeHwnd();    // can be NULL
	m_autoclose = NULL;
	m_count = 0;
}

BEGIN_MESSAGE_MAP(CMyMsgBox, CWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_MESSAGE(WM_INITDIALOG, OnSubclassedInit)
END_MESSAGE_MAP()

int CMyMsgBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AfxUnhookWindowCreate();
	return CWnd::OnCreate(lpCreateStruct);
}

LRESULT CMyMsgBox::OnSubclassedInit(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = Default();
	CWnd* pOk = GetDlgItem(IDCANCEL);
	if ( NULL != pOk ) { pOk->EnableWindow(FALSE); }
	SetTimer(100,2000,NULL);			
	return lRet;
}

void CMyMsgBox::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 100)
	{		
		if (m_count>0) { m_count--; }

		if (m_count == 0)
		{
			CWnd* pOk = GetDlgItem(IDCANCEL);
			if ( NULL != pOk )
			{
				pOk->EnableWindow(TRUE);
				pOk->SetFocus();
			}
			KillTimer(100);
			if (m_autoclose) { PostMessage(WM_CLOSE); }
		}
	}
}

int CMyMsgBox::MessageBox(LPCTSTR lpszText,LPCTSTR lpTitle, int count, bool bclose,UINT uType)
{	
	m_autoclose = bclose;
	m_count = count;
	AfxHookWindowCreate(this);
	return ::MessageBox(m_hWndParent, lpszText,lpTitle, uType);
}

LRESULT CMyMsgBox::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_COMMAND && m_count>0)
	{
		if(HIWORD(wParam)==BN_CLICKED) { return 0; }
	}
	return CWnd::DefWindowProc(message, wParam, lParam);
}
