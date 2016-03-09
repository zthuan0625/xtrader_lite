#include "stdafx.h"
#include "xTrader.h"
#include "DlgQryHiSet.h"

extern HANDLE g_hEvent;
IMPLEMENT_DYNAMIC(DlgQryHiSet, CDialog)

DlgQryHiSet::DlgQryHiSet(CWnd* pParent /*=NULL*/)
	: CDialog(DlgQryHiSet::IDD, pParent)
{
	m_szDate = _T("");
	m_szHiSet =_T("");
	m_pQry = NULL;
	m_pQpra = new QRYPARAM;
}

DlgQryHiSet::~DlgQryHiSet()
{
	DEL(m_pQpra);
}

void DlgQryHiSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DTPICK, m_Date);
	DDX_Text(pDX, IDC_EDIT_HISOD, m_szHiSet);
}

BEGIN_MESSAGE_MAP(DlgQryHiSet, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_QRYDAY, OnClkQryDay)
	ON_BN_CLICKED(IDC_BT_QRYMONTH, OnClkQryMonth)
	ON_BN_CLICKED(IDC_BT_SAVEFILE, OnClkSave2file)
	ON_BN_CLICKED(ID_FILE_PRINT, OnPrintSet)
	ON_BN_CLICKED(IDC_BTCLOSE, OnClkClose)
	ON_REGISTERED_MESSAGE(WM_QRYSMI_MSG,QrySmiMsg)
END_MESSAGE_MAP()

void DlgQryHiSet::OnDestroy()
{
	CDialog::OnDestroy();

    ClearVec(gc_Td->m_StmiVec);
	delete this;
}

BOOL DlgQryHiSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  
}

LRESULT DlgQryHiSet::QrySmiMsg(WPARAM wParam,LPARAM lParam)
{
	AfxBeginThread((AFX_THREADPROC)Refresh,this);

	return 0;
}

void DlgQryHiSet::SetResult()
{
	CtpTdSpi* td = gc_Td;
	
	int iSize = static_cast<int>(td->m_StmiVec.size());
	int iBufSize = iSize*sizeof(TThostFtdcContentType);
	char* szMsg = new char[iBufSize];
	ZeroMemory(szMsg,sizeof(szMsg));
	
	for (int i=0;i<iSize;i++)
	{ strcat(szMsg,(const char*)td->m_StmiVec[i].Content); }
	
	iBufSize = static_cast<int>(strlen(szMsg));
	ansi2uni(CP_ACP,szMsg,m_szHiSet.GetBuffer(3*iBufSize));
	m_szHiSet.ReleaseBuffer();
	DELX(szMsg);
	
	UpdateData(FALSE);
}

UINT DlgQryHiSet::Refresh(LPVOID pParam)
{
	DlgQryHiSet*  pDlg = static_cast<DlgQryHiSet*>(pParam);
	
	pDlg->SetResult();

	return 0;
}

UINT DlgQryHiSet::QrySmi(LPVOID pParam)
{
	QRYPARAM*  pQryPara = static_cast<QRYPARAM*>(pParam);
	DlgQryHiSet* pDlg = static_cast<DlgQryHiSet*>(pQryPara->pDlg);
	CtpTdSpi* td = gc_Td;

	ClearVec(td->m_StmiVec);
	pDlg->m_szHiSet.Empty();

	td->ReqQrySetInf(pQryPara->TdDay);

	pDlg->m_pQry = NULL;
	return 0;
}

void DlgQryHiSet::QryDate(QRYSMTYPE qType)
{
	CTime tm;
	DWORD dwResult = m_Date.GetTime(tm);
	if (dwResult == GDT_VALID)
	{
		switch (qType)
		{
		case Q_MONTH:	//查询月
			m_szDate = tm.Format(_T("%Y%m"));
			break;
		case Q_DAY:	//查询日
			m_szDate = tm.Format(_T("%Y%m%d"));
			break;
		default:
			m_szDate = _T("");
			break;
		}
		
		ZeroMemory(m_pQpra,sizeof(QRYPARAM));
		m_pQpra->pDlg = this;
		uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szDate,m_pQpra->TdDay);
		
		if (m_pQry==NULL) { m_pQry = AfxBeginThread((AFX_THREADPROC)QrySmi,m_pQpra); }
	}	
}

void DlgQryHiSet::OnClkQryDay()
{
	QryDate(Q_DAY);
}

void DlgQryHiSet::OnClkQryMonth()
{
	QryDate(Q_MONTH);
}

void DlgQryHiSet::OnPrintSet()
{
	//CPrintDialog dlg(FALSE);

	//dlg.DoModal();
}

void DlgQryHiSet::OnClkSave2file()
{
	CString  strFilter = _T("文本文件(*.log;*.txt)|(*.*;*.log;*.txt;)|所有文件 |*.*||");
	
	CFileDialog* dlgSave = new CFileDialog(false, _T("*.txt"),  GenDef(_T("结算单"),_T("txt")), OFN_PATHMUSTEXIST | OFN_EXPLORER, strFilter, this);
	dlgSave->m_ofn.lStructSize=sizeof(OPENFILENAME);		//use the 2k+ open file dialog
	
	CString szFile;
	if (IDOK == dlgSave->DoModal())
	{
		szFile = dlgSave->GetPathName();
		UpdateData(TRUE);

		if (!m_szHiSet.IsEmpty())
		{
			CFile fLog(szFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeText);

			int iLen = m_szHiSet.GetLength();
			char* szLog = new char[4*iLen];
			
			uni2ansi(CP_UTF8,(LPTSTR)(LPCTSTR)m_szHiSet,szLog);
			
			BYTE bBom[3]={0xEF,0xBB,0xBF};
			fLog.Write(&bBom,3);
			
			fLog.Write(szLog,static_cast<int>(strlen(szLog)));
			fLog.Close();
			
			DELX(szLog);
		}	
	}
	
	DEL(dlgSave);	
}

void DlgQryHiSet::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgQryHiSet::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

void DlgQryHiSet::OnClkClose()
{
	OnOK();
}
