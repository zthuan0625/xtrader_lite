#include "stdafx.h"
#include "xTrader.h"
#include "BkrNtDlg.h"

IMPLEMENT_DYNAMIC(BkrNtDlg, CDialog)

BkrNtDlg::BkrNtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BkrNtDlg::IDD, pParent)
{
	m_szTitle = LoadStr(IDS_BKRNT_TITLE);
	m_bMemory = FALSE;
	m_sContent.Empty();
}

BkrNtDlg::~BkrNtDlg()
{

}

void BkrNtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BkrNtDlg, CDialog)
ON_WM_DESTROY()
END_MESSAGE_MAP()

void BkrNtDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	delete this;
}

void BkrNtDlg::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void BkrNtDlg::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

BOOL BkrNtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_szTitle);
	if (m_Page.CreateFromStatic(IDC_HTMLVIEW, this))
	{ 
		if (m_bMemory)
		{
			m_Page.NavigateMemory();
			m_Page.PutBodyContent(m_sContent);
			COleVariant ovBgColor(_T("#f0f0f0"));
			m_Page.SetBodyAttributes(_T("BGCOLOR"), ovBgColor);
		}
		else
		{
			m_Page.NavigateNormal();
			m_Page.Navigate(m_szUrl); 
		}
	}

	return TRUE; 
}
