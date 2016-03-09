#include "stdafx.h"
#include "xTrader.h"
#include "DlgUnLock.h"

IMPLEMENT_DYNAMIC(DlgUnLock, CDialog)

DlgUnLock::DlgUnLock(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UNLOCK, pParent)
{
	m_szPwd = _T("");
}

DlgUnLock::~DlgUnLock()
{
}

void DlgUnLock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UNLOCKPWD, m_szPwd);
}


BEGIN_MESSAGE_MAP(DlgUnLock, CDialog)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_UNLOCKPWD, OnEnChgPwd)
	ON_BN_CLICKED(ID_UNLOCKOK, OnUnLockOK)
	ON_BN_CLICKED(IDC_ULQUIT, OnClkQuit)
END_MESSAGE_MAP()

BOOL DlgUnLock::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	EnableClose(this,FALSE);
	GetDlgItem(IDC_UNLOCKPWD)->SetFocus();
	
	return FALSE;
}

void DlgUnLock::OnDestroy()
{
	CDialog::OnDestroy();
	
	delete this;
}

void DlgUnLock::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgUnLock::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

void DlgUnLock::OnEnChgPwd()
{
	UpdateData();
}

void DlgUnLock::OnUnLockOK()
{
	TThostFtdcPasswordType sPwd;
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szPwd,sPwd);
	if (!strcmp(g_s.m_sPwd,sPwd))
	{
        CXTraderDlg* p = g_Dlg;
		p->m_bLocked = false;
		p->ShowWindow(TRUE);
		p->SetIdleTm();

		OnOK();
	}
	else
	{
		m_szPwd.Empty();
		UpdateData(FALSE);
		
		SetDlgItemText(IDC_STAULMSG,LoadStr(IDS_UNLPWDERR));
		GetDlgItem(IDC_UNLOCKPWD)->SetFocus();
	}
}

void DlgUnLock::OnClkQuit()
{
	g_Dlg->PostMessage(WM_QUIT);
}
