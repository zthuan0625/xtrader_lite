#include "stdafx.h"
#include "xTrader.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE g_hEvent;
BEGIN_MESSAGE_MAP(CXTraderApp, CWinApp)
	//{{AFX_MSG_MAP(CXTraderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

CXTraderApp::CXTraderApp()
{
	m_MApi = NULL;
	m_TApi = NULL;
	m_cQ = NULL;
	m_cT = NULL;
	g_hEvent=CreateEvent(NULL, true, false, NULL); 
}

CXTraderApp::~CXTraderApp()
{

}

void CXTraderApp::CreateApi()
{	
	char szFilePath[MAX_PATH]={0};
	GetCurDir(szFilePath,"log\\");

	m_TApi = CThostFtdcTraderApi::CreateFtdcTraderApi(szFilePath);
	m_cT = new CtpTdSpi(m_TApi,m_cfg.m_sBkrId,m_cfg.m_sUid,
		m_cfg.m_sPwd,m_cfg.m_sDymPwd,m_cfg.m_sAtCode);
	m_TApi->RegisterSpi(m_cT);
	m_TApi->SubscribePublicTopic(THOST_TERT_QUICK);
	m_TApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	
	m_MApi = CThostFtdcMdApi::CreateFtdcMdApi(szFilePath,m_cfg.m_bIsUdp,m_cfg.m_bIsMulcast);
	m_cQ = new CtpMdSpi(m_MApi,m_cfg.m_sBkrId);
	m_MApi->RegisterSpi(m_cQ);
}

void CXTraderApp::ReleaseApi()
{
	if(m_cT) { m_cT->ReqUserLogout(); }
	RELZ(m_TApi);
	DEL(m_cT);
	
	RELZ(m_MApi);
	DEL(m_cQ);
}

void CXTraderApp::LoadGlobCfg()
{
	CString szPath = GetSpecFilePath(_T("log\\"));
	
	if (!PathIsDirectory(szPath))
	{ CreateDirectory(szPath,NULL); }

	if(!PathFileExists(LoadStr(IDS_CFG_FILE)))
	{ res2file(MAKEINTRESOURCE(IDR_CONFBAK),_T("STUFF"),LoadStr(IDS_CFG_FILE)); }

    m_cfg.ProcConfigs();
}

CXTraderApp theApp;

BOOL CXTraderApp::InitInstance()
{
	SetHighPriority();

	if (!AfxSocketInit()){ return FALSE; }

	LoadGlobCfg(); 

	LoginDlg login;
	if (login.DoModal() != IDOK) { return FALSE; }
	
	CXTraderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	
	return FALSE;
}

int CXTraderApp::ExitInstance()
{
	ReleaseApi();

	return CWinApp::ExitInstance();
} 
