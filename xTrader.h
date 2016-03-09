#pragma once
#include "resource.h"		// main symbols
#include "mdspi.h"
#include "traderspi.h"
#include "xTraderDlg.h"
#include "AppCfgs.h"

#define g_App	static_cast<CXTraderApp*>(AfxGetApp())
#define g_Dlg	static_cast<CXTraderDlg*>(AfxGetApp()->m_pMainWnd)
#define g_xLst	g_Dlg->m_xLst
#define g_s	g_App->m_cfg
#define gc_Md	g_App->m_cQ
#define gc_Td	g_App->m_cT
#define gm_Md	gc_Td->m_DepthMdMap
#define gv_Ord	gc_Td->m_orderVec
#define gv_Pk	gc_Td->m_pkExVec
#define gv_Td	gc_Td->m_tradeVec
#define gv_oR	gc_Td->m_onRoadVec
#define gv_AccReg	gc_Td->m_AccRegVec
#define gv_TdCode	gc_Td->m_TdCodeVec
#define gv_InvPos	gc_Td->m_InvPosVec
#define gm_Insinf	gc_Td->m_InsinfMap
#define gv_Acc g_s.m_pInfVec
#define gv_Bkr g_s.m_BkrParaVec
#define gvk_Ord g_s.m_ordvks
#define gvk_Inst g_s.m_vkInst
#define gvk_Misc g_s.m_vkMisc
#define gv_Vol g_s.m_vecVol
#define gv_inf g_s.m_InsinfVec
#define gv_minf g_s.m_InsMainVec

class CXTraderApp : public CWinApp
{
public:
	CXTraderApp();
	~CXTraderApp();

public:
	CThostFtdcMdApi* m_MApi;
	CThostFtdcTraderApi* m_TApi;
	CtpMdSpi* m_cQ;
	CtpTdSpi* m_cT;
	CAppCfgs m_cfg;

public:
	void CreateApi();
	void ReleaseApi();
	void LoadGlobCfg();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTraderApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTraderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
