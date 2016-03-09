#include "stdafx.h"
#include "xtrader.h"
#include "BfTransfer.h"
#include "DlgBfLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HANDLE g_hEvent;

IMPLEMENT_DYNAMIC(BfTransfer, CDialog)
BfTransfer::BfTransfer(CWnd* pParent /*=NULL*/)
	: CDialog(BfTransfer::IDD, pParent)
{
	m_szAccpwd = _T("");
	m_szBkpwd = _T("1");
	m_pQryBf = NULL;
	m_dTrsAmt = 0.01;
}

BfTransfer::~BfTransfer()
{
}

void BfTransfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BfTransfer)
	DDX_Control(pDX, IDC_CBEXHTYPE, m_cbExhType);
	DDX_Control(pDX, IDC_CBBKLST, m_cbBkLst);
	DDX_Text(pDX, IDC_ACCPASSWD, m_szAccpwd);
	DDX_Text(pDX, IDC_BKPASSWD, m_szBkpwd);
	DDX_Text(pDX, IDC_TRANSAMT, m_dTrsAmt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(BfTransfer, CDialog)
	//{{AFX_MSG_MAP(BfTransfer)
	ON_BN_CLICKED(IDC_BTNQRYBK, OnBtnQryBk)
	ON_BN_CLICKED(IDC_BTNQRYFT, OnBtnQryFt)
	ON_BN_CLICKED(IDC_BTNFT2BK, OnBtnFt2Bk)
	ON_BN_CLICKED(IDC_BTNBK2FT, OnBtnBk2Ft)
	ON_BN_CLICKED(IDC_QRYDETAIL, OnQryDetail)
	ON_EN_CHANGE(IDC_ACCPASSWD, OnChgAccPwd)
	ON_EN_CHANGE(IDC_BKPASSWD, OnChgBkPwd)
	ON_EN_CHANGE(IDC_TRANSAMT, OnTransAmt)
	ON_REGISTERED_MESSAGE(WM_QRYBFLOG_MSG,QryDetailMsg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL BfTransfer::OnInitDialog()
{
	TCHAR szUser[MAX_PATH/2],szTitle[MAX_PATH];
	ansi2uni(CP_ACP,g_s.m_sUid,szUser);

	_stprintf(szTitle,LoadStr(IDS_BFTRANS_TITLE),szUser);
	SetWindowText(szTitle);

	InitCombo();
	UpdateData(FALSE);
	
	return TRUE;
}

void BfTransfer::InitCombo()
{
	m_cbBkLst.SubclassDlgItem(IDC_CBBKLST,this);
	m_cbExhType.SubclassDlgItem(IDC_CBEXHTYPE,this);
	
	CtpTdSpi* td = gc_Td;

	TCHAR strBkAcc[MAX_PATH];
	CString szBkInfo,szBkName;
	int nIndex = 0;
	size_t i=0;
	size_t uSize = td->m_AccRegVec.size();
	for (i=0;i<uSize;i++)
	{
		ansi2uni(CP_ACP,td->m_AccRegVec[i].BankAccount,strBkAcc);
		szBkName = JgBkName(td->m_AccRegVec[i].BankID[0]);

		szBkInfo.Format(_T("%s[%s]"),(LPCTSTR)szBkName,strBkAcc);
		nIndex = m_cbBkLst.AddString(szBkInfo);
		m_cbBkLst.SetItemData(nIndex,i);
	}

	if (uSize>=1) { m_cbBkLst.SetCurSel(0); }
	
	LPCTSTR strExhType[3] = {_T(_CNY),_T(_USD),_T(_HKD)};
	
	for (i=0;i<3;i++)
	{
		nIndex = m_cbExhType.AddString(strExhType[i]);
		m_cbExhType.SetItemData(nIndex,i);
	}

	if (uSize>=1){ m_cbExhType.SetCurSel(JgCurIdType(td->m_AccRegVec[0].CurrencyID)); }
	else{ m_cbExhType.SetCurSel(0); }
}

void BfTransfer::OnBtnQryFt() 
{
	CXTraderDlg* p = g_Dlg;
	if (!p->m_pQryAcc)
	{ p->m_pQryAcc = AfxBeginThread((AFX_THREADPROC)CXTraderDlg::QryTdAcc, p); }
}

void BfTransfer::Transfunc(BFTRANSTYPE tp)
{
	if (m_szAccpwd.IsEmpty()||m_szBkpwd.IsEmpty())
	{
		ShowErroTips(IDS_STREMPTY,IDS_STRTIPS);
		return;
	}
	
	char szAccPass[MAX_PATH],szBkPass[MAX_PATH];
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szAccpwd,szAccPass);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szBkpwd,szBkPass);
	
	int nIndex = m_cbBkLst.GetCurSel();
	if (nIndex<0) 
	{
		ShowErroTips(IDS_QRYBKERR,IDS_STRTIPS);
		return;
	}
	
	CtpTdSpi* td = gc_Td;
	if (tp==TRANS_F2B)
	{ td->ReqF2BkByF(td->m_AccRegVec[nIndex].BankID,szBkPass,szAccPass,m_dTrsAmt); }
	else if (tp==TRANS_B2F)
	{ td->ReqBk2FByF(td->m_AccRegVec[nIndex].BankID,szBkPass,szAccPass,m_dTrsAmt); }
	else if (tp==QRY_BKACC)
	{ td->ReqQryBkAccMoneyByF(td->m_AccRegVec[nIndex].BankID,szBkPass,szAccPass); }
}

void BfTransfer::OnBtnQryBk() 
{
	Transfunc(QRY_BKACC);	
}

void BfTransfer::OnBtnFt2Bk() 
{
	Transfunc(TRANS_F2B);
}

void BfTransfer::OnBtnBk2Ft() 
{
	Transfunc(TRANS_B2F);
}

LRESULT BfTransfer::QryDetailMsg(WPARAM wParam,LPARAM lParam)
{
	DlgBfLog* tdlg = new DlgBfLog;
	
	BOOL res=tdlg->Create(IDD_DLG_BFDETAILS,NULL);
	VERIFY( res==TRUE );
	tdlg->CenterWindow();
	tdlg->ShowWindow(SW_SHOW);
	
	return 0;
}

void BfTransfer::OnQryDetail() 
{
	if (!m_pQryBf){ m_pQryBf = AfxBeginThread((AFX_THREADPROC)BfTransfer::QryBfS,this); }
}

UINT BfTransfer::QryBfS(LPVOID pParam)
{
	BfTransfer* pDlg = static_cast<BfTransfer*>(pParam);
	int nIndex = pDlg->m_cbBkLst.GetCurSel();
	if (nIndex<0) 
	{
		ShowErroTips(IDS_QRYBKERR,IDS_STRTIPS);
		pDlg->m_pQryBf = NULL;
		return 0;
	}

    CtpTdSpi* td = gc_Td;
	ClearVec(td->m_BfTransVec);
	td->ReqQryTfSerial(td->m_AccRegVec[nIndex].BankID);

	pDlg->m_pQryBf = NULL;
	return 0;
}
	  
void BfTransfer::OnOK() 
{
	CDialog::OnOK();
	DestroyWindow();
}

void BfTransfer::OnCancel() 
{
	CDialog::OnCancel();
	DestroyWindow();
}

void BfTransfer::OnDestroy()
{
	CDialog::OnDestroy();
	
	delete this;
}

void BfTransfer::OnChgAccPwd()
{
	UpdateData();
}

void BfTransfer::OnChgBkPwd()
{
	UpdateData();
}

void BfTransfer::OnTransAmt()
{
	VerifyEdit(this,IDC_TRANSAMT,true);
}