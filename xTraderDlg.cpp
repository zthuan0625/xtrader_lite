#include "stdafx.h"
#include "xTrader.h"
#include "xTraderDlg.h"
#include "NoticeDlg.h"
#include "DlgModPass.h"
#include "GenMfDlg.h"
#include "DlgQryHiSet.h"
#include "ParkOrd.h"
#include "BfTransfer.h"
#include "LoginDlg.h"
#include "BkrNtDlg.h"
#include "DlgOrdEx.h"
#include "DlgUnLock.h"
#include "DlgSiLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
	
extern HANDLE g_hEvent;
CXTraderDlg::CXTraderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXTraderDlg::IDD, pParent)
{
	//InitializeCriticalSection(&m_cs);
    CAppCfgs& s = g_s;
	ansi2uni(CP_ACP,s.m_szInst,m_szInst.GetBuffer(MAX_PATH));
	m_szInst.ReleaseBuffer();

    m_dScale = LMT(getMasterScale(),1.0,5.0);
	m_rcBak = CRect(0,0,0,0);
	m_OldSize = CSize(-1,-1);
	m_szInstOld=_T("IF1302");
	
	m_bUpdateOp=TRUE;
	m_bTrans=FALSE;
	m_bQryPk = true;
	m_InstInf = new INSINFEX();
	m_pDepthMd = new CThostFtdcDepthMarketDataField();
	m_pTdAcc = new CThostFtdcTradingAccountField();
	m_pCliInfo = new CLINFO();
	m_plpMMI = new MINMAXINFO;
	m_plpMMI->ptMinTrackSize.y = static_cast<LONG>(359*m_dScale);  
	m_plpMMI->ptMaxTrackSize.y = static_cast<LONG>(359*m_dScale);
	m_plpMMI->ptMinTrackSize.x = static_cast<LONG>(498*m_dScale);
	m_plpMMI->ptMaxTrackSize.x = static_cast<LONG>(1200*m_dScale);

    m_bMinGui = s.m_bMinGui;
    m_bTop = s.m_bTop;
    m_bTdClosed=m_bLocked=false;
	m_Timer=m_uIdle=m_uSync=m_uShowBar=NULL;
	m_pQryAcc=m_pOrder=m_pModOrd=m_pReConQry=m_pQryBkrNt=m_pClrPosThd=
	m_pSubMd=m_pQryTdNt=m_pMdThread=m_pUnSubMd=m_pQryUinf=m_pQryTdTok=NULL;
	m_dOrdPx = m_dOldPrice = 1.0;
	m_iActIdx = -1;
	m_bLastPx = s.m_bLastPx;

	m_szExpDef = _T("");
	m_eLogMod = NORMAL_LOG;
	ZeroMemory(m_tDisStart,sizeof(TThostFtdcTimeType));
	ZeroMemory(m_tDisEnd,sizeof(TThostFtdcTimeType));
}

CXTraderDlg::~CXTraderDlg()
{
	//DeleteCriticalSection(&m_cs);
	ClearMemory();

	DEL(m_InstInf);
	DEL(m_pDepthMd);
	DEL(m_pTdAcc);
	DEL(m_pCliInfo);
	DEL(m_plpMMI);
}

void CXTraderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTraderDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_S1P, m_csS1P);
	DDX_Control(pDX, IDC_S1V, m_csS1V);
	DDX_Control(pDX, IDC_LASTP, m_csLastP);
	DDX_Control(pDX, IDC_LASTV, m_csLastV);
	DDX_Control(pDX, IDC_B1P, m_csB1P);
	DDX_Control(pDX, IDC_B1V, m_csB1V);
	DDX_Text(pDX, IDC_INST, m_szInst);
	DDV_MaxChars(pDX, m_szInst, 30);
	DDX_Control(pDX, IDC_SUPDOWN, m_csSUpDown);
	DDX_Control(pDX, IDC_DUPDOWN, m_csDUpDown);
	DDX_Control(pDX, IDC_SHIGHEST, m_csSHighest);
	DDX_Control(pDX, IDC_DHIGHEST, m_csDHighest);
	DDX_Control(pDX, IDC_SOPT, m_csSOpt);
	DDX_Control(pDX, IDC_DOPT, m_csDOpt);
	DDX_Control(pDX, IDC_SLOWEST, m_csSLowest);
	DDX_Control(pDX, IDC_DLOWEST, m_csDLowest);
	DDX_Control(pDX, IDC_STOTAL, m_csSTotal);
	DDX_Control(pDX, IDC_VTOTAL, m_csVTotal);
	DDX_Control(pDX, IDC_SOPEN, m_csSHold);
	DDX_Control(pDX, IDC_VOPEN, m_csVHold);
	DDX_Control(pDX, IDC_SSMP, m_csSSmp);
	DDX_Control(pDX, IDC_DSMP, m_csDSmp);
	DDX_Control(pDX, IDC_UPTIME, m_csSUptime);
	DDX_Control(pDX, IDC_SS1, m_csSS1);
	DDX_Control(pDX, IDC_SLAST, m_csSLast);
	DDX_Control(pDX, IDC_SB1, m_csSB1);
	DDX_Control(pDX, IDC_MDPAN, m_GroupMd);
	DDX_Control(pDX, IDC_GRPACC, m_GroupAcc);
	DDX_Text(pDX, IDC_EDVOL, m_iOrdVol);
	DDX_Text(pDX, IDC_EDPRICE, m_dOrdPx);
	DDX_Control(pDX, IDC_SPINPRICE, m_SpinPrice);
	DDX_Control(pDX, IDC_SPINVOL, m_SpinVol);
	DDX_Control(pDX, IDC_COMB_OC, m_CombOC);
	DDX_Control(pDX, IDC_COMB_BS, m_CombBS);
	DDX_Check(pDX, IDC_CHK_NEWP, m_bLastPx);
	DDX_Control(pDX, IDC_LST_ONROAD, m_xLst[ONROAD_IDX]);
	DDX_Control(pDX, IDC_LST_ODINF, m_xLst[ORDER_IDX]);
	DDX_Control(pDX, IDC_LST_TRADE, m_xLst[TRADE_IDX]);
	DDX_Control(pDX, IDC_LST_INVPOS, m_xLst[INVPOS_IDX]);
	DDX_Control(pDX, IDC_LST_ALLINST, m_xLst[INSLST_IDX]);
	DDX_Control(pDX, IDC_LST_PARKORD, m_xLst[PKCON_IDX]);
	DDX_Control(pDX, IDC_TABPAGE, m_TabOption);
	DDX_Control(pDX, IDC_CPYK, m_csCpProf);
	DDX_Control(pDX, IDC_HPYK, m_csHpProf);
	DDX_Control(pDX, IDC_TDFEE, m_csTdFee);
}

BEGIN_MESSAGE_MAP(CXTraderDlg, CDialog)
	//{{AFX_MSG_MAP(CXTraderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_EXIT, OnExit)
	ON_EN_CHANGE(IDC_EDVOL, OnChgEditVol)
	ON_EN_CHANGE(IDC_EDPRICE, OnChgEditPx)
	ON_COMMAND(ID_HELP_ABOUT, OnAbout)
	ON_COMMAND(ID_TIPS, OnTips)
	ON_COMMAND(ID_MODPASS, OnModifyPass)
	ON_COMMAND(ID_TRANSBF,OnTransBf)
	ON_COMMAND(ID_VIEW_TOP, OnViewTop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOP, OnUpdateViewTop)
	ON_COMMAND(ID_VIEW_TOP, OnViewTop)
	ON_BN_CLICKED(IDC_BTORDER, OnBtOrder)
	ON_BN_CLICKED(IDC_CHK_NEWP, OnBnClkChkLastPx)
	ON_CBN_SELCHANGE(IDC_COMB_OC, OnSelKpp)
	ON_CBN_SELCHANGE(IDC_COMB_BS, OnSelDir)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABPAGE, OnTabSelchange)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_ONROAD, OnNMDblclkOnroad)
	ON_NOTIFY(NM_CLICK, IDC_LST_ONROAD, OnNMClkLstOnroad)
	ON_NOTIFY(NM_RCLICK, IDC_LST_ONROAD, OnNMRClkLstOnroad)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_ONROAD, OnNMDblclkOnroad)
	ON_NOTIFY(NM_CLICK, IDC_LST_ONROAD, OnNMClkLstOnroad)
	ON_NOTIFY(NM_RCLICK, IDC_LST_ONROAD, OnNMRClkLstOnroad)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_ODINF, OnNMDblclkOrdInf)
	ON_NOTIFY(NM_CLICK, IDC_LST_ODINF, OnNMClkLstOrdInf)
	ON_NOTIFY(NM_RCLICK, IDC_LST_ODINF, OnNMRClkLstOrdInf)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_TRADE, OnNMDblclkTdInf)
	ON_NOTIFY(NM_CLICK, IDC_LST_TRADE, OnNMClkLstTdInf)
	ON_NOTIFY(NM_RCLICK, IDC_LST_TRADE, OnNMRClkLstTdInf)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_INVPOS, OnNMDblclkInvPInf)
	ON_NOTIFY(NM_CLICK, IDC_LST_INVPOS, OnNMClkLstInvPInf)
	ON_NOTIFY(NM_RCLICK, IDC_LST_INVPOS, OnNMRClkLstInvPInf)
	ON_NOTIFY(NM_CLICK, IDC_LST_ALLINST, OnNMClkLstInsts)
	ON_NOTIFY(NM_RCLICK, IDC_LST_ALLINST, OnNMRClkLstInsts)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_PARKORD, OnNMDblclkParkOrd)
	ON_NOTIFY(NM_CLICK, IDC_LST_PARKORD, OnNMClkParkOrd)
	ON_NOTIFY(NM_RCLICK, IDC_LST_PARKORD, OnNMRClkParkOrd)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_ONROAD, OnDispOnRoad)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_ONROAD, OnfindOnRoad)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_ODINF, OnDispOrdInf)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_ODINF, OnfindOrdInf)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_INVPOS, OnDispInvPos)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_INVPOS, OnfindInvPos)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_TRADE, OnDispTrade)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_TRADE, OnfindTrade)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_ALLINST, OnDispInsts)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_ALLINST, OnfindInsts)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_PARKORD, OnDispParked)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LST_PARKORD, OnfindParked)
	ON_BN_CLICKED(IDC_BTQRYACC,OnClkQryAcc)
	ON_EN_UPDATE(IDC_INST, OnEditchgInst)
	ON_COMMAND(ID_CANCEL_ORD, OnCancelOrd)
	ON_COMMAND(ID_CANCEL_ALL, OnCancelAll)
	ON_COMMAND(ID_CSV_EXPORT, OnCsvExport)
	ON_COMMAND(ID_MOD_DSJ, OnModDsj)
	ON_COMMAND(ID_REVORD, OnRevOrd)
	ON_COMMAND(ID_REV_ANYPX, OnBackHand)
	ON_COMMAND(ID_USERINFO, OnUserInf)
	ON_COMMAND(ID_CFMMC, OnCfmmc)
	ON_COMMAND(ID_GENMDFEE, OnGenMdFee)
	ON_COMMAND(ID_HISETTINF, OnHiSettInf)
	ON_COMMAND(ID_HKEY, OnHkeySet)
	ON_COMMAND(ID_LOCAL_SEND, OnLocalSend)
	ON_COMMAND(ID_LOCAL_REMV, OnLocalRemv)
	ON_COMMAND(ID_PK_CANCEL, OnPkAct)
	ON_COMMAND(ID_PK_REMV, OnPkRemv)
	ON_COMMAND(ID_PK_DELCANCEL, OnDelPkAct)
	ON_COMMAND(ID_COND_ACT,OnCondAct)
	ON_COMMAND(ID_MINI_MENU,OnMiniMode)
	ON_COMMAND(ID_MINI_TASK,OnMiniTask)
	ON_COMMAND(ID_COMMCFG,OnCommCfg)
	ON_MESSAGE(WM_QRYACC_MSG,QryAccMsg)
	ON_MESSAGE(WM_QRYUSER_MSG,QryUserMsg)
	ON_MESSAGE(WM_QRYBKYE_MSG,QryBkYe)
	ON_MESSAGE(WM_UPDATEMD_MSG,UpdateMdMsg)
	ON_MESSAGE(WM_TDRECON_MSG,TdReConnMsg)
	ON_MESSAGE(WM_MDRECON_MSG,MdReConnMsg)
	ON_MESSAGE(WM_TDISCON_MSG,TdDisConnMsg)
	ON_MESSAGE(WM_MDISCON_MSG,MdDisConnMsg)
	ON_MESSAGE(WM_EXHSTAT_MSG,ExhStatusMsg)
	ON_MESSAGE(WM_NOTICE_MSG,TdNoticeMsg)
	ON_MESSAGE(WM_OPENACC_MSG,OpenAccMsg)
	ON_MESSAGE(WM_DELACC_MSG,DelAccMsg)
	ON_MESSAGE(WM_MODACC_MSG,ModAccMsg)
	ON_MESSAGE(WM_QRYBKRNT_MSG,BkrNtMsg)
	ON_MESSAGE(WM_QRYTDNT_MSG,TdNtMsg)
	ON_MESSAGE(WM_QRYACCTOK_MSG,QryTokMsg)
	ON_MESSAGE_VOID(WM_DISPLAYCHANGE,OnDisplayChg)
	ON_MESSAGE(WM_BK2F_MSG,Bk2FMsg)
	ON_MESSAGE(WM_F2BK_MSG,F2BkMsg)
	ON_MESSAGE(WM_RELOGINOK,ReLogOkMsg)
	ON_MESSAGE(WM_SAVECFGS,SaveCfgsMsg)
	ON_COMMAND(ID_TOOL_PARKED, OnToolParked)
	ON_COMMAND(ID_RELOGIN, OnReLogin)
	ON_COMMAND(ID_TDNOTICE, OnTdNotice)
	ON_COMMAND(ID_BKRNOTICE, OnBkrNotice)
	ON_COMMAND(ID_ORDER_EXT, OnOrderExt)
	ON_COMMAND(ID_VIEWLOG, OnViewLog)
	ON_COMMAND(ID_LOCK, OnLockAcc)
END_MESSAGE_MAP()

void CXTraderDlg::InitAllVecs()
{
	ClearMemory();
	//////////////////////////
	CtpTdSpi* td = gc_Td;

	m_pCliInfo->iFrtId = td->m_iFrtId;
	m_pCliInfo->iSesId = td->m_iSesId;
	_tcscpy(m_pCliInfo->ProdInf,_T(PROD_INFO));

	 memcpy(m_pTdAcc,&td->m_TdAcc,sizeof(CThostFtdcTradingAccountField)); 
	 memcpy(&m_BkrTdPara,&td->m_BkrTdPara,sizeof(CThostFtdcBrokerTradingParamsField)); 

	UpdateMdList();
}

void CXTraderDlg::ClearMemory()
{
	ClearVec(m_SubList);
}

void CXTraderDlg::OnDestroy() 
{
	DelFolder(_T("temp"));
	SaveConfig();

	if (m_pSubMd != NULL)
	{
		TermThread(m_pSubMd->m_hThread);
		m_pSubMd= NULL;
	}
	
	if (m_pClrPosThd != NULL)
	{
		TermThread(m_pClrPosThd->m_hThread);
		m_pClrPosThd= NULL;
	}

	ClearTmOrdThd();

	KillTimer(REFRESH_TIMER); m_Timer = NULL;
	KillIdleTm();

	m_Notify.uFlags=NULL;
	Shell_NotifyIcon(NIM_DELETE,&m_Notify);

	//LOGI("程序退出(" << this << ")");
	//////////////////////////////////////////
	CDialog::OnDestroy();
}

/*
void CXTraderDlg::StartLog(const char* logpath)
{
	//ILog4zManager::getInstance()->setLoggerMonthdir(LOG4Z_MAIN_LOGGER_ID, true);
	//ILog4zManager::getInstance()->setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
	//ILog4zManager::getInstance()->setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
	//ILog4zManager::getInstance()->setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID,false);
	//ILog4zManager::getInstance()->setLoggerLimitsize(LOG4Z_MAIN_LOGGER_ID, 100);
	//if (logpath) {
	//	ILog4zManager::getInstance()->setLoggerPath(LOG4Z_MAIN_LOGGER_ID, logpath);
	//}
	ILog4zManager::getInstance()->start();
}

void CXTraderDlg::StopLog()
{
	ILog4zManager::getInstance()->stop();
}
*/

void CXTraderDlg::SaveConfig()
{
    CAppCfgs& s = g_s;
	s.m_bLastPx = !!m_bLastPx;
	s.m_bMinGui = m_bMinGui;
    s.m_bTop = m_bTop;
	//////////////////////////////////	
	CRect rect;
	GetWindowRect(rect);
	if (!IsWindowVisible() && (rect.left<0 || rect.top<0)){ rect = m_rcBak; }
	s.m_ixPos = rect.left;
	s.m_iyPos = rect.top;
	s.m_iWidth = rect.Width();
	s.m_iHeight = rect.Height();

	s.ProcConfigs(SAVE);
}

void CXTraderDlg::InitProfit(const CThostFtdcTradingAccountField* pAcc)
{
	/////////////////显示盈亏情况//////////////////////////////////
	CString szCPft,szPPft,szTdFee;

	szTdFee.Format(_T("%lld"),D2Int(pAcc->Commission)); outStrAs4(szTdFee);
	szCPft.Format(_T("%lld"),D2Int(pAcc->CloseProfit)); outStrAs4(szCPft);
	szPPft.Format(_T("%lld"),D2Int(pAcc->PositionProfit)); outStrAs4(szPPft);
	
	m_csCpProf.SetText(szCPft,WHITE,DT_CENTER);
	m_csHpProf.SetText(szPPft,WHITE,DT_CENTER);
	m_csTdFee.SetText(szTdFee,WHITE,DT_CENTER);	
    /////////////////////////////////////////////////////////////
}

void CXTraderDlg::InitCtrs()
{
	CRect rect;
	GetClientRect(rect);
	m_OldSize = CSize(rect.Width(), rect.Height());

	m_GroupMd.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW).SetBackgroundColor(BLACK, BLACK).SetBorderColor(BLACK);
	m_GroupAcc.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW).SetBackgroundColor(ACC_BG, ACC_BG).SetCatptionTextColor(WHITE).SetBorderColor(ACC_BG);

	m_csCpProf.SetFont(_T("Arial"),19,FW_BOLD);
	m_csCpProf.SetBkColor(ACC_BG);
	m_csHpProf.SetFont(_T("Arial"),19,FW_BOLD);
	m_csHpProf.SetBkColor(ACC_BG);
	m_csTdFee.SetFont(_T("Arial"),19,FW_BOLD);
	m_csTdFee.SetBkColor(ACC_BG);

	InitTabs();
	InitAllHdrs();

	int i = 0;
    for(i=1;i<=PKCON_IDX;i++){ m_xLst[i].ShowWindow(SW_HIDE); }

	m_SpinVol.SetBuddy(GetDlgItem(IDC_EDVOL));	//m_VolCtl.GetWnd()
	m_SpinPrice.SetBuddy(GetDlgItem(IDC_EDPRICE));
	
	m_SpinVol.SetDecimalPlaces(0);
	m_SpinVol.SetTrimTrailingZeros (TRUE);
	m_SpinVol.SetPos(10.0);
	m_SpinPrice.SetTrimTrailingZeros (FALSE);

	m_csSS1.SetText((LPCTSTR)LoadStr(IDS_STRASK1));
	m_csS1P.SetText(_T("2279.2"),GREEN);
	m_csS1V.SetText(_T("2"),YELLOW);
	m_csSLast.SetText(_T("最新"));
	m_csLastV.SetText(_T(" "),YELLOW);
	m_csLastP.SetText(_T("2279.2"),RED);
	m_csSB1.SetText((LPCTSTR)LoadStr(IDS_STRBID1));
	m_csB1P.SetText(_T("2279.0"),GREEN);
	m_csB1V.SetText(_T("6"),YELLOW);
	m_csSUpDown.SetText(_T("涨跌"));
	m_csDUpDown.SetText(_T("-27.8"),GREEN);

	m_csSHighest.SetText(_T("最高"));
	m_csDHighest.SetText(_T("2328.6"),RED);
	m_csSLowest.SetText(_T("最低"));
	m_csDLowest.SetText(_T("2243.6"),GREEN);
	m_csSTotal.SetText(_T("总手"));
	m_csVTotal.SetText(_T("884883"),YELLOW);
	m_csSHold.SetText(_T("持仓"));
	m_csVHold.SetText(_T("88383"),YELLOW);
	m_csSSmp.SetText(_T("幅度"));
	m_csDSmp.SetText(_T("-1.21%"),WHITE);
	m_csSOpt.SetText(_T("今开"));
	m_csDOpt.SetText(_T("2323.0"),RED);
	
	m_csSUptime.SetText(_T("15:15:00.000"));
	m_csSUptime.SetFont(_T("Arial"),16,FW_NORMAL);
	//////////////////////////////////////////////////////////////////
	LPCTSTR strBS[2] ={DIR_BUY,DIR_SELL};
	LPCTSTR strOC[2] ={ORD_O,ORD_C/*,ORD_CT*/};
	for (i=0;i<2;i++) { m_CombBS.AddString(strBS[i]); }
	m_CombBS.SetCurSel(0);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_COMB_BS,CBN_SELCHANGE),(LPARAM)m_CombBS.GetSafeHwnd());

	for (i=0;i<2;i++){ m_CombOC.AddString(strOC[i]); }
	m_CombOC.SetCurSel(0);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_COMB_OC,CBN_SELCHANGE),(LPARAM)m_CombOC.GetSafeHwnd());
}

void CXTraderDlg::InitData()
{
    CAppCfgs& s = g_s;
	CtpTdSpi* td = gc_Td;
	m_iRef_tm=0;
	m_iRef_pk=0;
	m_iRef_tj=0;
	//可能重新登录!!
	CString str;
	if(m_bMinGui)
	{
		str.Format(_T("盈亏:%lld 费用:%lld"),D2Int(m_pTdAcc->CloseProfit+m_pTdAcc->PositionProfit),
			D2Int(m_pTdAcc->Commission));
		SetWindowText(str);
	}
	else
	{
		str.Format(_T("%s"),s.m_szTitle);
        SetWindowText(str);
	}
	
	if (m_eLogMod == CHGACC_LOG)
	{ 
		CString szTmp = s.m_szTitle;
		int iPos = szTmp.Find((LPCTSTR)LoadStr(IDS_TITLE));
		CString szTips;
		szTips.Format(_T("账户%s 切换登录成功!"),szTmp.Mid(0,iPos-1));
		ShowNotifyIcon(LoadStr(IDS_MY_TIPS),szTips,NIM_MODIFY); 
	}

	InitProfit(m_pTdAcc);
	m_xLst[ONROAD_IDX].SetItemCountEx(static_cast<int>(td->m_onRoadVec.size()));
	m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(td->m_orderVec.size()));
	m_xLst[INVPOS_IDX].SetItemCountEx(static_cast<int>(td->m_InvPosVec.size()));
	m_xLst[TRADE_IDX].SetItemCountEx(static_cast<int>(td->m_tradeVec.size()));
	m_xLst[INSLST_IDX].SetItemCountEx(static_cast<int>(s.m_InsinfVec.size()));
	m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(td->m_pkExVec.size()));
	
	if(m_xLst[ONROAD_IDX].IsWindowVisible()) { m_xLst[ONROAD_IDX].Invalidate(); }
	if(m_xLst[ORDER_IDX].IsWindowVisible()) { m_xLst[ORDER_IDX].Invalidate(); }
	if(m_xLst[INVPOS_IDX].IsWindowVisible()) { m_xLst[INVPOS_IDX].Invalidate(); }
	if(m_xLst[TRADE_IDX].IsWindowVisible()) { m_xLst[TRADE_IDX].Invalidate(); }
	if(m_xLst[INSLST_IDX].IsWindowVisible()) { m_xLst[INSLST_IDX].Invalidate(); }
	if(m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX].Invalidate(); }

/////////////////////////////////////////////////////////////////////////////
	BOOL bRes = td->IsValidInst(m_szInst,m_InstInf);
	if (m_eLogMod == NORMAL_LOG)
	{	
		CTime tm(CTime::GetCurrentTime());	
		CString	szT;
		szT.Format(_T("%d:%02d:%02d CTP登录成功"), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
		SetStatusTxt(szT,2);
	}

	int iMaxVol=1000000;
	double dPriceTick=1;
	int iDig = 1;
	if (bRes)
	{
		dPriceTick = m_InstInf->iinf.PriceTick;
		iMaxVol = m_InstInf->iinf.MaxLimitOrderVolume;
		iDig = JudgeDigit(dPriceTick);
	}
	
	m_SpinVol.SetRangeAndDelta (1, iMaxVol, 1);
	m_SpinPrice.SetDecimalPlaces (iDig);

	if (!m_pSubMd) { m_pSubMd = AfxBeginThread((AFX_THREADPROC)SubscribeMD, this); }
}

void CXTraderDlg::InitAllHdrs()
{
	LPCTSTR lpHdrs0[ONROAD_ITMES] = {_T("单号"),_T("合约"),_T("买卖"),_T("开平"),_T("未成"),_T("价格"),_T("时间"),_T("冻结保证金")};
	int iWidths0[ONROAD_ITMES] = {1,48,36,36,36,48,60,80};
	int i=0;
	
	for (i = 0;i<ONROAD_ITMES ; i++)
	{
		m_xLst[ONROAD_IDX].InsertColumn(i, lpHdrs0[i],LVCFMT_RIGHT,static_cast<int>(iWidths0[i] * m_dScale));
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	LPCTSTR lpHdrs1[ORDER_ITMES] = {_T("单号"),_T("合约"),_T("买卖"),_T("开平"),_T("状态"),_T("价格"),_T("报量"),_T("未成"),
		_T("已成"),_T("均价"),_T("时间"),_T("冻结保证金"),_T("冻结手续费"),_T("详细状态")};
	int iWidths1[ORDER_ITMES] = {1,48,36,36,60,48,36,36,36,60,60,80,80,180};
	
	for (i = 0;i<ORDER_ITMES ; i++)
	{
		m_xLst[ORDER_IDX].InsertColumn(i, lpHdrs1[i],LVCFMT_RIGHT,static_cast<int>(iWidths1[i] * m_dScale));
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	LPCTSTR lpHdrs2[TRADE_ITMES] = {_T("合约"),_T("买卖"),_T("开平"),_T("价格"),_T("手数"),_T("时间"),_T("手续费"),
		_T("投保"),_T("成交类型"),_T("交易所"),_T("成交编号"),_T("报单编号")};
	int iWidths2[TRADE_ITMES] = {48,36,36,48,36,60,48,36,64,48,64,64};
	
	for (i = 0;i<TRADE_ITMES ; i++)
	{
		m_xLst[TRADE_IDX].InsertColumn(i, lpHdrs2[i],LVCFMT_RIGHT,static_cast<int>(iWidths2[i] * m_dScale));
	}
	
	///////////////////////////////////////////////////////////////////////////////
	LPCTSTR lpHdrs3[INVPOS_ITMES] = {_T("合约"),_T("买卖"),_T("总持仓"),_T("可平量"),_T("持仓均价"),_T("持仓盈亏"),_T("占保证金"),_T("总盈亏")};
	int iWidths3[INVPOS_ITMES] = {48,36,48,48,64,70,70,60};
	
	for (i = 0;i<INVPOS_ITMES ; i++)
	{
		m_xLst[INVPOS_IDX].InsertColumn(i, lpHdrs3[i],LVCFMT_RIGHT,static_cast<int>(iWidths3[i] * m_dScale));
	}
	///////////////////////////////////////////////////////////////////////////////////////
	LPCTSTR lpHdrs4[ALLINST_ITMES] = {_T("代码"),_T("合约"),_T("合约名"),_T("交易所"),_T("乘数"),
		_T("点差"),_T("类型"),_T("最后日期"),_T("执行价格"),_T("保证金率"),_T("手续费率")};
	int iWidths4[ALLINST_ITMES] = {26,48,80,48,36,36,36,64,64,64,120};

	for (i = 0;i<ALLINST_ITMES ; i++)
	{
		m_xLst[INSLST_IDX].InsertColumn(i, lpHdrs4[i],LVCFMT_RIGHT,static_cast<int>(iWidths4[i] * m_dScale));
	}
	
	LPCTSTR lpHdrs5[PARKORD_ITMES] = {_T("报单编号"),_T("类型"),_T("状态"),_T("触发条件"),
		_T("合约"),_T("买卖"),	_T("开平"),_T("价格"),_T("报量"),_T("投保"),_T("交易所"),_T("预埋时间"),_T("状态信息")};

	int iWidths5[PARKORD_ITMES] = {2,60,48,92,48,36,36,48,36,36,48,64,120};
	
	for (i = 0;i<PARKORD_ITMES ; i++)
	{
		m_xLst[PKCON_IDX].InsertColumn(i, lpHdrs5[i],LVCFMT_RIGHT,static_cast<int>(iWidths5[i] * m_dScale));
	}
	///////////////////////////////////////////////////////////////////////////////////////	
}

void CXTraderDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	
	ASSERT(pPopupMenu != NULL);

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu; // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
	
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}
	
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.
		
		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;    // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE); // Popups are never auto disabled.
		}
		else
		{
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}
		
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CXTraderDlg::SetDlgIcon(UINT uId)
{
	m_hIcon = AfxGetApp()->LoadIcon(uId);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
}

BOOL CXTraderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetDlgIcon(IDR_MAINFRAME);

	#ifndef _REAL_CTP_
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TRANSBF,MF_BYCOMMAND|MF_GRAYED);
	#endif
	
    CAppCfgs& s = g_s;

	InitAllVecs();
	InitCtrs();
	InitData();

	if(s.m_bChkTrans){ SetWndAlpha(m_hWnd,s.m_uAlpha); }
	if (m_bTop)
	{ 
		if (m_bMinGui)
		{
			SetWindowPos(&wndTopMost,s.m_ixPos,s.m_iyPos,0,0,SWP_NOSIZE); 
			Go2MiniGui();
		}
		else
		{ SetWindowPos(&wndTopMost,s.m_ixPos,s.m_iyPos,s.m_iWidth,s.m_iHeight,SWP_SHOWWINDOW); }
		
	} 
	else
	{ 
		if (m_bMinGui)
		{
			SetWindowPos(&wndNoTopMost,s.m_ixPos,s.m_iyPos,0,0,SWP_NOSIZE);
			Go2MiniGui();
		}
		else
		{ SetWindowPos(&wndNoTopMost,s.m_ixPos,s.m_iyPos,s.m_iWidth,s.m_iHeight,SWP_SHOWWINDOW);}	 
	}

	SetForegroundWindow();
    ShowNotifyIcon(_T(""),_T(""),NIM_ADD);

	if (m_Timer == 0) { m_Timer = static_cast<UINT>(SetTimer(REFRESH_TIMER, 1000, NULL)); }
	SetIdleTm();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXTraderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		OnAbout();
	}
	else
	{
		switch( nID )
		{
		case SC_MINIMIZE:
			OnMinimize();
			break;
			
		default:
			CDialog::OnSysCommand(nID, lParam);
		}
	}		
}

void CXTraderDlg::OnMinimize()
{
	GetWindowRect(m_rcBak);
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_HIDE);
}

void CXTraderDlg::ClearInvPos()
{
	CXTraderApp* pApp = g_App;
	CAppCfgs& s = g_App->m_cfg;
	CtpTdSpi* td = pApp->m_cT;

	VIT_pos vpos = td->m_InvPosVec.begin();
	for (vpos;vpos!=td->m_InvPosVec.end();vpos++)
	{
		if (vpos->Position){ GenOrdByInvPos(&(*vpos)); }
	}
}

void CXTraderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == REFRESH_TIMER)
	{
		SYSTEMTIME curTime;
		::GetLocalTime(&curTime);
		TCHAR szT[5][10];
			
		CTime t[5];
		for(int i=0;i<5;i++)
		{
			t[i] = CTime(curTime);
			t[i] += gc_Td->m_tsEXnLocal[i];
			_stprintf(szT[i],_T("%d:%02d:%02d"), t[i].GetHour(), t[i].GetMinute(), t[i].GetSecond());
				
			SetStatusTxt(szT[i],i+3);
		}
	}

	if (nIDEvent == IDLE_TIMER){ if(!m_bLocked) OnLockAcc(); }

	CDialog::OnTimer(nIDEvent);
}

CString CXTraderDlg::GetIpAndMac()
{
	char szStr[MAX_PATH];
	CString szTemp = _T("");

    CAppCfgs& s = g_s;
	sprintf(szStr,"IP:%s,MAC:%s",s.m_sLocIp,strlen(s.m_sLocMac)?s.m_sLocMac:"N/A");
	ansi2uni(CP_ACP,szStr,szTemp.GetBuffer(2*MAX_PATH));
	szTemp.ReleaseBuffer();
	
	return szTemp;
}

void CXTraderDlg::OnAbout()
{
	CString sTitle = LoadStr(IDS_TITLE);
	CString sDevInfo = _T(""),szTemp = _T("");
	
	#ifdef _WIN64
	sTitle += _T(" x64@");
	#else
	sTitle += _T(" x86@");
	#endif
	
	sTitle += AUTHOR;

	sDevInfo += _T("CORE:");
	sDevInfo += __DATE__;
	sDevInfo += _T(",");
	sDevInfo += __TIME__;
	sDevInfo += _T(",API:");

	ansi2uni(CP_ACP,(char*)CThostFtdcTraderApi::GetApiVersion(),szTemp.GetBuffer(MAX_PATH));
	szTemp.ReleaseBuffer();
	sDevInfo += szTemp;
	sDevInfo += _T("\n");
	sDevInfo += GetIpAndMac();

	ShellAbout(m_hWnd, sTitle , sDevInfo ,AfxGetApp()->LoadIcon(IDR_MAINFRAME));
}

void CXTraderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CXTraderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXTraderDlg::OnExit()
{
	PostMessage(WM_CLOSE);
}

void CXTraderDlg::OnRestore() 
{
    ShowWindow( SW_RESTORE );
    SetForegroundWindow();
}

BOOL CXTraderDlg::PopTrayMenu()
{
	CPoint pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.CreatePopupMenu();
	DWORD flags = (m_bLocked? (MF_POPUP|MF_BYPOSITION|MF_GRAYED):(MF_POPUP|MF_BYPOSITION));

	menu.InsertMenu(0, flags, IDSHELL_RESTORE,_T("恢复窗口(&R)"));
	menu.InsertMenu(1, flags, ID_MINI_TASK,_T("普通界面(&N)"));
	menu.InsertMenu(2, MF_BYPOSITION | MF_SEPARATOR);
	menu.InsertMenu(3, MF_POPUP|MF_BYPOSITION, ID_COMMCFG,_T("选项配置(&O)"));
	menu.InsertMenu(4, flags, ID_RELOGIN,_T("切换账户(&W)"));
	menu.InsertMenu(6, MF_BYPOSITION | MF_SEPARATOR);
	menu.InsertMenu(7, MF_POPUP|MF_BYPOSITION, IDSHELL_EXIT,_T("退出终端(&E)"));

	return menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
}

BOOL CXTraderDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
	case IDSHELL_RESTORE:
		OnRestore();
		break;
	case IDSHELL_ABOUT:
		OnAbout();
		break;
	case IDSHELL_EXIT:
		OnExit();
		break;
	case 0:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
			if(!m_bLocked) { OnRestore(); }
			break;
		case WM_RBUTTONUP:
			PopTrayMenu();
			break;	
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CXTraderDlg::SwitchTab(int nIndex)
{
	NMHDR nmhdr; 
	nmhdr.code = TCN_SELCHANGE; 
	nmhdr.hwndFrom = m_TabOption.GetSafeHwnd();  
	nmhdr.idFrom= IDC_TABPAGE;  
	
	m_TabOption.SetCurSel(nIndex);
	::SendMessage(m_TabOption.GetSafeHwnd(),WM_NOTIFY,MAKELONG(TCN_SELCHANGE,nIndex),(LPARAM)(&nmhdr));
}

BOOL CXTraderDlg::PreTranslateMessage(MSG* pMsg) 
{ 
	int nVirtKey = (int) pMsg->wParam;
	if((pMsg->message == WM_KEYDOWN)||(pMsg->message >= WM_MOUSEFIRST && pMsg->message <=WM_MOUSELAST) ) 
	{
		KillIdleTm();
		switch(nVirtKey)
		{
			case VK_F2:
				SwitchTab(ONROAD_IDX);
				break;
			case VK_F3:
				SwitchTab(ORDER_IDX);
				break;
			case VK_F4:
				SwitchTab(INVPOS_IDX);
				break;
			case VK_F5:
				SwitchTab(TRADE_IDX);
				break;
			case VK_F6:
				SwitchTab(INSLST_IDX);
				break;
			case VK_F8:
				SwitchTab(PKCON_IDX);
				break;
			default:
				break;
		}
	} 
	else
	{ SetIdleTm(); }
	
	if(nVirtKey == VK_ESCAPE){	return 0; }
	
	return CDialog::PreTranslateMessage(pMsg); 
}

LRESULT CXTraderDlg::DefWindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	if (Msg == WM_TASKBARCREATED) { ShowNotifyIcon(_T(""),_T(""),NIM_ADD); }

	if (Msg == WM_DPICHANGED)
	{
		//在更换了DPI的显示器上时会接收到
		m_dScale = LMT(getMasterScale(),1.0,5.0);
	}

	if(Msg == WM_CLOSE)
	{
		if (!m_bLocked &&(IDYES != MessageBox(LoadStr(IDS_CFMQUIT),LoadStr(IDS_MY_TIPS),MB_YESNO|MB_ICONINFORMATION))) { return 0; }
	}
	
	return CDialog::DefWindowProc(Msg, wParam, lParam);
}

void CXTraderDlg::OnChgEditVol() 
{
	VerifyEdit(this,IDC_EDVOL,false);
	
	ChgStatByBsOc(m_cDir,m_cKpp,m_iOrdVol);
}

void CXTraderDlg::OnChgEditPx() 
{
	VerifyEdit(this,IDC_EDPRICE,true);
	
	//同时检查开平仓条件并刷新状态显示
}

void CXTraderDlg::SetStatusTxt(UINT uID, int nPane)
{
	m_StatusBar.SetPaneText(nPane,LoadStr(uID),TRUE);
}

void CXTraderDlg::SetStatusTxt(LPCTSTR pMsg, int nPane)
{
	m_StatusBar.SetPaneText(nPane, pMsg,TRUE);
}

void CXTraderDlg::SetTipTxt(LPCTSTR pMsg,int nTool)
{
	m_StatusBar.UpdateTipText(pMsg,nTool);
}

void CXTraderDlg::SetPaneTxtColor(int nPane,COLORREF cr)
{
	m_StatusBar.SetPaneTextColor(nPane,cr);
}

static UINT BASED_CODE indicators[] =
{
	IDS_MD_STAT,
	IDS_TRADE_STAT,
	IDS_RESPINFO,
	IDS_SFETIME,
	IDS_DCETIME,
	IDS_ZCETIME,
	IDS_CFXTIME,
	IDS_INETIME
};

static UINT tooltipIndicators[] =
{
	IDS_MD_TIPS,
	IDS_TRADE_TIPS,
	0,
	IDS_SHFE,
	IDS_DCE,
	IDS_CZCE,
	IDS_CFFEX,
	IDS_CINE
};

int CXTraderDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CreateStatusBar();

	return 0;
}

void CXTraderDlg::CreateStatusBar()
{
	m_StatusBar.CreateEx(this);
	m_StatusBar.SetIndicators(indicators,8,tooltipIndicators,NULL);
	
	m_StatusBar.SetPaneInfo(0,IDS_MD_STAT,SBPS_NORMAL,static_cast<int>(30*m_dScale));
	m_StatusBar.SetPaneInfo(1,IDS_TRADE_STAT,SBPS_NORMAL, static_cast<int>(30*m_dScale));
	m_StatusBar.SetPaneInfo(2,IDS_RESPINFO,SBPS_NORMAL, static_cast<int>(240*m_dScale));
	m_StatusBar.SetPaneInfo(3,IDS_SFETIME,SBPS_NORMAL, static_cast<int>(48*m_dScale));
	m_StatusBar.SetPaneInfo(4,IDS_DCETIME,SBPS_NORMAL, static_cast<int>(48*m_dScale));
	m_StatusBar.SetPaneInfo(5,IDS_ZCETIME,SBPS_NORMAL, static_cast<int>(48*m_dScale));
	m_StatusBar.SetPaneInfo(6,IDS_CFXTIME,SBPS_NORMAL, static_cast<int>(48*m_dScale));
	m_StatusBar.SetPaneInfo(7,IDS_INETIME,SBPS_NORMAL, static_cast<int>(48*m_dScale));

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,IDS_INETIME);
	m_StatusBar.SetDelayTime(300);

	SetPaneTxtColor(0,RED);
	SetPaneTxtColor(1,RED);
}

void CXTraderDlg::OnGlobKey(BOOL bCtl)
{
	if(bCtl){ RegisterHotKey(m_hWnd,HKEY_BEGINID-1,0,192); }
	else
	{ UnregisterHotKey(m_hWnd,HKEY_BEGINID-1); }
}

void CXTraderDlg::OnViewTop()
{
	m_bTop = !m_bTop;
	if (m_bTop){ SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); }
	else 
	{ SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); }
}

void CXTraderDlg::OnUpdateViewTop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTop);
}

void CXTraderDlg::SubMdData(bool bSub)
{
	CXTraderApp* pApp = g_App;
	CtpMdSpi* md = pApp->m_cQ;

	if (bSub)
	{
		char szOldInst[MAX_PATH];
		uni2ansi(CP_ACP, (LPTSTR)(LPCTSTR)m_szInstOld, szOldInst);
	
		if (m_szInstOld.Compare(m_szInst))
		{
			//int iCnt = IsTdPeriod()?1:2;
			char** pInst = new char*[2];
			pInst[0] = szOldInst;
			pInst[1] = pApp->m_cfg.m_szInst; 
			//非盘中时持仓合约退订才可刷新一帧行情
			//if(!IsTdPeriod()) { pInst[1] = pApp->m_cfg.m_szInst; }
			md->UnSubscribeMarketData(pInst,2);
			
			DELX(pInst);
		}
	}
	
	int iLen = static_cast<int>(m_SubList.size());
	char** pInsLst = new char*[iLen];
	
	for(int i=0; i<iLen;i++) { pInsLst[i] = m_SubList[i].instId; }
	
	if (bSub){ md->SubscribeMarketData(pInsLst,iLen); }
	else
	{
		md->UnSubscribeMarketData(pInsLst,iLen);
		//ClearVec(m_SubList);
	}
	m_szInstOld = m_szInst;

	DELX(pInsLst);
}

UINT CXTraderDlg::SubscribeMD(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	pDlg->SubMdData();

	pDlg->m_pSubMd =NULL;

	return 0;
}

UINT CXTraderDlg::UnSubscribeMD(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	pDlg->SubMdData(false);

	pDlg->m_pUnSubMd = NULL;
	return 0;
}

void CXTraderDlg::OnBnClkChkLastPx()
{
    m_bLastPx = !m_bLastPx;
}

void CXTraderDlg::InitTabs()
{
	LPCTSTR lpStr[PKCON_IDX+1] = {_T("挂单F2"),_T("委托F3"),_T("持仓F4"),_T("成交F5"),_T("合约F6"),_T("条件单F8")};
	for (int i=ONROAD_IDX;i<=PKCON_IDX;i++) { m_TabOption.InsertItem( i, lpStr[i] ); }
}

///////////////////////////////////////////////

void CXTraderDlg::OnTabSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	int idx = m_TabOption.GetCurSel();
	for (int i=0;i<=PKCON_IDX;i++) 
	{ 
		if(i==idx){ m_xLst[i].ShowWindow( SW_SHOW ); }
		else
		{ m_xLst[i].ShowWindow( SW_HIDE ); }	
	}

	*pResult = 0;
}

void CXTraderDlg::ShowNotifyIcon(/*CWnd* pWnd,*/CString sInfoTitle,CString sInfo,DWORD dwMsg)
{
	ZeroMemory( &m_Notify, sizeof( NOTIFYICONDATA ) );
	
	m_Notify.cbSize		      = sizeof( NOTIFYICONDATA );
	m_Notify.hWnd			  = m_hWnd;
	m_Notify.uID			  = 0;
	
	m_Notify.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	m_Notify.dwInfoFlags      = NIIF_INFO; // add an icon to a balloon ToolTip
	
	m_Notify.uCallbackMessage = WM_COMMAND;  
	m_Notify.uTimeout         = 2000;
	m_Notify.hIcon		   	  = (HICON)LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_MAINFRAME),IMAGE_ICON,16,16,NULL);
	
	_tcscpy( m_Notify.szInfoTitle, sInfoTitle);
	_tcscpy( m_Notify.szInfo, sInfo);
	_tcscpy( m_Notify.szTip, (LPCTSTR)g_s.m_szTitle/*LoadStr(m_bRegStatus ? IDS_SHELL_RUN : IDS_SHELL_STOP)*/);
	
	Shell_NotifyIcon(dwMsg, &m_Notify ); // add to the taskbar's status area
}

LRESULT CXTraderDlg::QryUserMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcInvestorField* pInv = (CThostFtdcInvestorField*)lParam;
	if(pInv)
	{
		PopupPrivInf(pInv);

		DEL(pInv);
	}
	return TRUE;
}

LRESULT CXTraderDlg::QryAccMsg(WPARAM wParam,LPARAM lParam)
{
	memcpy(m_pTdAcc,&(gc_Td->m_TdAcc),sizeof(CThostFtdcTradingAccountField));
	PopupAccDlg(m_pTdAcc);
		
	return TRUE;
}

UINT CXTraderDlg::OrderThread(LPVOID pParam)
{
	ORDTHREADP* pOrd = static_cast<ORDTHREADP*>(pParam);
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pOrd->pDlg);
	CXTraderApp* pApp = g_App;
	CtpTdSpi* td = pApp->m_cT;

	BOOL bAny = JgAnyPxOrd(pOrd->ordReq.OrderPriceType,pOrd->ordReq.TimeCondition);
	int iOrdVol = max(pOrd->ordReq.VolumeTotalOriginal,1);
	////////////////////////////////////////////////////////
	int iTdPos = pOrd->iTdPos;
	int iYdPos = pOrd->iYdPos;
	TThostFtdcVolumeType iLmt=1, iMkt=1;
	td->GetLmtMktVol(pOrd->ordReq.InstrumentID,iLmt,iMkt);
	
	int iMaxVol = bAny?iMkt:iLmt;
	int iTimes = iOrdVol/iMaxVol;
	int iLeft = iOrdVol%iMaxVol;
	int i=0;

	double dUp=0,dLo=0;
	td->GetUpLoLmt(pOrd->ordReq.InstrumentID,dUp,dLo);
	if (pOrd->ordReq.OrderPriceType == THOST_FTDC_OPT_LimitPrice)
	{ pOrd->ordReq.LimitPrice = LMT(pOrd->ordReq.LimitPrice,dLo,dUp); }

	// 不能直接用上日持仓,要用总仓-今仓,YdPos = Pos - TdPos
	BOOL bRet = td->JgExhByInst(pOrd->ordReq.InstrumentID,_SFE);
	//上期所且市价单,改成合法的指令
	if (bRet && bAny)
	{
		pOrd->ordReq.LimitPrice = (pOrd->ordReq.Direction==THOST_FTDC_D_Buy)?dUp:dLo;
		pOrd->ordReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		pOrd->ordReq.TimeCondition = THOST_FTDC_TC_GFD;
	}
	
	if (pOrd->ordReq.CombOffsetFlag[0] > THOST_FTDC_OF_Open && bRet)
	{
		if (iOrdVol <= iYdPos) //仅平昨
		{
			pOrd->ordReq.CombOffsetFlag[0]=THOST_FTDC_OF_CloseYesterday;
			if (iTimes>=1)
			{
				for (i=0;i<iTimes;i++)
				{
					pOrd->ordReq.VolumeTotalOriginal = iMaxVol;
					td->ReqOrds(&pOrd->ordReq);
				}			
			}
			if (iLeft>0)
			{
				pOrd->ordReq.VolumeTotalOriginal = iLeft;
				td->ReqOrds(&pOrd->ordReq);			
			}
		}
		else //平昨,再平今
		{
			int iVolTd = iOrdVol-iYdPos;

			iTimes = iYdPos/iMaxVol;
			iLeft = iYdPos%iMaxVol;
			int iTimes2 = iVolTd/iMaxVol;
			int iLeft2 = iVolTd%iMaxVol;

			/////////////////////平昨///////////////////
			if (iTimes>=1)
			{
				//平昨
				pOrd->ordReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday; 
				for (i=0;i<iTimes;i++)
				{
					pOrd->ordReq.VolumeTotalOriginal = iMaxVol;
					td->ReqOrds(&pOrd->ordReq);
				}			
			}
			if (iLeft>0)
			{
				//平昨
				pOrd->ordReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday; 
				pOrd->ordReq.VolumeTotalOriginal = iLeft;
				td->ReqOrds(&pOrd->ordReq);			
			}

			/////////////////////平今///////////////////
			if (iTimes2>=1)
			{
				//平今仓
				pOrd->ordReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday; 
				for (i=0;i<iTimes2;i++)
				{
					pOrd->ordReq.VolumeTotalOriginal = iMaxVol;
					td->ReqOrds(&pOrd->ordReq);
				}			
			}
			if (iLeft2>0)
			{
				//平今仓
				pOrd->ordReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday; 
				pOrd->ordReq.VolumeTotalOriginal = iLeft2;
				td->ReqOrds(&pOrd->ordReq);			
			}
		}
	}
	else
	{
		if (iTimes>=1)
		{
			for (i=0;i<iTimes;i++)
			{
				pOrd->ordReq.VolumeTotalOriginal = iMaxVol;
				td->ReqOrds(&pOrd->ordReq);
			}			
		}
		if (iLeft>0)
		{
			pOrd->ordReq.VolumeTotalOriginal = iLeft;
			td->ReqOrds(&pOrd->ordReq);			
		}
	}	
	
	if (pOrd->ordReq.StopPrice<NEARZERO && !pDlg->m_xLst[ORDER_IDX].IsWindowVisible())
	{ pDlg->SwitchTab(ORDER_IDX); }

	DEL(pOrd);
	pDlg->m_pOrder = NULL;

	return 0;
}

void CXTraderDlg::OnSelKpp()
{
	int iOC = m_CombOC.GetCurSel();
	m_cKpp = (iOC+THOST_FTDC_OF_Open);

	ChgStatByBsOc(m_cDir,m_cKpp,m_iOrdVol);
}

void CXTraderDlg::OnSelDir()
{
	int iDir= m_CombBS.GetCurSel();
	m_cDir = (iDir+THOST_FTDC_D_Buy);

	ChgStatByBsOc(m_cDir,m_cKpp,m_iOrdVol);
}

void CXTraderDlg::OnBtOrder()
{
	CXTraderApp* pApp = g_App;
	BOOL bRet = pApp->m_cT->IsValidInst(m_szInst,m_InstInf);
	if (bRet){ GenOrdFromPara(pApp->m_cfg.m_szInst,m_cDir,m_cKpp,m_iOrdVol,m_dOrdPx); }
}

void CXTraderDlg::GenOrdFromPara(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType cDir,TThostFtdcOffsetFlagType cOff,
		TThostFtdcVolumeType iVol,TThostFtdcPriceType dPx,bool bLmt)
{
	if (!m_pOrder)
	{
		ORDTHREADP *pOrdPara = new ORDTHREADP;
		pOrdPara->pDlg = this;
		ZeroMemory(&pOrdPara->ordReq,sizeof(CThostFtdcInputOrderField));
		strcpy(pOrdPara->ordReq.InstrumentID,instId);
		
		pOrdPara->ordReq.Direction=cDir;
		pOrdPara->ordReq.CombOffsetFlag[0]=cOff;
		
		int iValidVol = GetValidVol(pOrdPara->ordReq.InstrumentID,cDir,
			cOff,iVol,pOrdPara->iTdPos,pOrdPara->iYdPos);
		if (iValidVol<1 ){ return; }

		pOrdPara->ordReq.VolumeTotalOriginal = iValidVol;
		
		pOrdPara->ordReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		pOrdPara->ordReq.VolumeCondition = THOST_FTDC_VC_AV;
		pOrdPara->ordReq.ContingentCondition = THOST_FTDC_CC_Immediately;	

		if (bLmt)
		{
			pOrdPara->ordReq.LimitPrice = dPx;
			pOrdPara->ordReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			pOrdPara->ordReq.TimeCondition = THOST_FTDC_TC_GFD;
		}
		else
		{
			pOrdPara->ordReq.LimitPrice = 0;
			pOrdPara->ordReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			pOrdPara->ordReq.TimeCondition = THOST_FTDC_TC_IOC;
		}
		m_pOrder = AfxBeginThread((AFX_THREADPROC)OrderThread,pOrdPara);
	}
}

int CXTraderDlg::GetVolByPos(const CThostFtdcInvestorPositionField* pInvPos,int iVol,int& iTdPos,int& iYdPos)
{
	int iRet=0,iHoldPos=GetHoldPos(pInvPos);
	iTdPos = pInvPos->TodayPosition;
	iYdPos = pInvPos->Position - pInvPos->TodayPosition;
			
	iRet = min(iVol,iHoldPos); 
	
	return iRet;
}

int CXTraderDlg::GetValidVol(TThostFtdcInstrumentIDType instId,char cDir,char cOffFlag,int iVol,int& iTdPos,int& iYdPos)
{
	iTdPos = 0;
	iYdPos = 0;

	if (cOffFlag==THOST_FTDC_OF_Open) { return iVol; }

	TThostFtdcDirectionType pDir;
	CThostFtdcInvestorPositionField* pInvPos;
	int iSize = static_cast<int>(gv_InvPos.size());
	for(int i=0;i<iSize;i++)
	{
		pInvPos = &gv_InvPos[i];
		pDir = pInvPos->PosiDirection -2;

		if(!strcmp(instId,pInvPos->InstrumentID)&& (cDir ==(THOST_FTDC_D_Buy+THOST_FTDC_D_Sell-pDir)) &&
			(cOffFlag>THOST_FTDC_OF_Open))
		{ return GetVolByPos(pInvPos,iVol,iTdPos,iYdPos); }
	}
	
	return 0;
}

UINT CXTraderDlg::QryTdAcc(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	gc_Td->ReqQryTdAcc();
	
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }	

	pDlg->m_pQryAcc=NULL;

	return 0;
}

void CXTraderDlg::PopupAccDlg(CThostFtdcTradingAccountField* pAcc)
{
	InitProfit(pAcc);	

	if (m_eLogMod == NORMAL_LOG)
	{
		CNoticeDlg* pDlg = new CNoticeDlg;
		pDlg->m_szTitle = LoadStr(IDS_ACC_DETAILS);
		
		BOOL res=pDlg->Create(IDD_DLG_NOTICE,NULL);
		VERIFY( res==TRUE );
		pDlg->CenterWindow();
		pDlg->ShowWindow(SW_SHOW);
	
		CString szLine = _T(""),szTemp = _T("");
		
		szLine += FormatLine(_T(""),_T(""),_T("="),42);
		
		szTemp.Format(_T("%.2f"),pAcc->PreBalance); outStrAs4(szTemp);
		szLine += FormatLine(_T("  上次结算准备金:"),szTemp,_T(" "),40);

		szTemp.Format(_T("%.2f"),pAcc->PreCredit); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 上次信用额度:"),szTemp,_T(" "),40);

		szTemp.Format(_T("%.2f"),pAcc->PreMortgage); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 上次质押金额:"),szTemp,_T(" "),40);

		szTemp.Format(_T("%.2f"),pAcc->Mortgage); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 质押金额:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->Withdraw); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 今日出金:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->Deposit); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 今日入金:"),szTemp,_T(" "),41);

		szLine += FormatLine(_T(""),_T(""),_T("-"),42);

		double dStatProf = pAcc->PreBalance-pAcc->PreCredit-pAcc->PreMortgage+pAcc->Mortgage-pAcc->Withdraw+pAcc->Deposit;
		szTemp.Format(_T("%.2f"),dStatProf); outStrAs4(szTemp);
		szLine += FormatLine(_T("= 静态权益:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),FiltDb(pAcc->CloseProfit)); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 平仓盈亏:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),FiltDb(pAcc->PositionProfit)); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 持仓盈亏:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->CashIn); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 权利金:"),szTemp,_T(" "),42);
		
		szTemp.Format(_T("%.2f"),pAcc->Commission); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 手续费:"),szTemp,_T(" "),42);

		szLine += FormatLine(_T(""),_T(""),_T("-"),42);

		double dDymProf = dStatProf+pAcc->CloseProfit+pAcc->PositionProfit+pAcc->CashIn-pAcc->Commission;
		szTemp.Format(_T("%.2f"),dDymProf); outStrAs4(szTemp);
		szLine += FormatLine(_T("= 动态权益:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->CurrMargin); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 占用保证金:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->FrozenMargin); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 冻结保证金:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->FrozenCommission); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 冻结手续费:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->DeliveryMargin); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 交割保证金:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->FrozenCash); outStrAs4(szTemp);
		szLine += FormatLine(_T("- 冻结权利金:"),szTemp,_T(" "),41);
		
		szTemp.Format(_T("%.2f"),pAcc->Credit); outStrAs4(szTemp);
		szLine += FormatLine(_T("+ 信用金额:"),szTemp,_T(" "),41);

		szLine += FormatLine(_T(""),_T(""),_T("-"),42);

		double dValidProf = dDymProf-pAcc->CurrMargin-pAcc->FrozenMargin-pAcc->FrozenCommission-pAcc->FrozenCash-pAcc->DeliveryMargin+pAcc->Credit;
		szTemp.Format(_T("%.2f"),dValidProf); outStrAs4(szTemp);
		szLine += FormatLine(_T("= 可用金额:"),szTemp,_T(" "),41);

		szLine += FormatLine(_T(""),_T(""),_T("="),42);

		szTemp.Format(_T("%.2f"),pAcc->Reserve); outStrAs4(szTemp);
		szLine += FormatLine(_T("  保底资金:"),szTemp,_T(" "),41);

		szTemp.Format(_T("%.2f"),pAcc->WithdrawQuota); outStrAs4(szTemp);
		szLine += FormatLine(_T("  可取资金:"),szTemp,_T(" "),41);

		szLine += FormatLine(_T(""),_T(""),_T("="),42);

		pDlg->GetDlgItem(IDC_NTMSG)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szLine);
	}
}

LRESULT CXTraderDlg::Bk2FMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcRspTransferField* pTrans = (CThostFtdcRspTransferField*)lParam;

	if (pTrans)
	{
		m_pTdAcc->Deposit += pTrans->TradeAmount;
		CString strMsg;
		strMsg.Format(LoadStr(IDS_BK2FMSG),pTrans->TradeAmount,pTrans->FutureFetchAmount);
		ShowErroTips(strMsg,LoadStr(IDS_BFTRANS_OK));
		DEL(pTrans);
	}
	
	return 0;
}

LRESULT CXTraderDlg::F2BkMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcRspTransferField* pTrans = (CThostFtdcRspTransferField*)lParam;
	
	if (pTrans)
	{
		m_pTdAcc->Withdraw += pTrans->TradeAmount;
		CString strMsg;
		strMsg.Format(LoadStr(IDS_F2BKMSG),pTrans->TradeAmount,pTrans->FutureFetchAmount);
		ShowErroTips(strMsg,LoadStr(IDS_BFTRANS_OK));
		DEL(pTrans);
	}
	
	return 0;
}

void CXTraderDlg::OnClkQryAcc()
{
	m_eLogMod = NORMAL_LOG;
	if (!m_pQryAcc){ m_pQryAcc = AfxBeginThread((AFX_THREADPROC)QryTdAcc, this); }
}

double CXTraderDlg::GetDsjByInst(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType cDir)
{
	double dPx = 0;
	string strkey(instId);
	MIT_md vmd = gm_Md.find(strkey);
	if (vmd != gm_Md.end()) 
	{ 
		if (cDir==THOST_FTDC_D_Buy)
		{ dPx = vmd->second.AskPrice1; }
		else if(cDir==THOST_FTDC_D_Sell)
		{ dPx = vmd->second.BidPrice1; }	
	}
	return dPx;
}

void CXTraderDlg::GenOrdFromPk(PARKEDEX pkReq)
{
	ORDTHREADP *pOrdPara = new ORDTHREADP;
	pOrdPara->pDlg = this;
	ZeroMemory(&pOrdPara->ordReq,sizeof(CThostFtdcInputOrderField));

	OrdConvter(pOrdPara->ordReq,pkReq,(pkReq.ordType==COND_LOCAL)?PK_LOCCOND2INP:PARKED2INP);

	if(pOrdPara->ordReq.LimitPrice <-NEARZERO) 
	{ pOrdPara->ordReq.LimitPrice = GetDsjByInst(pkReq.park.InstrumentID,pkReq.park.Direction); }

	int iValidVol = GetValidVol(pOrdPara->ordReq.InstrumentID,pOrdPara->ordReq.Direction,
		pOrdPara->ordReq.CombOffsetFlag[0],pkReq.park.VolumeTotalOriginal,pOrdPara->iTdPos,pOrdPara->iYdPos);
	if (iValidVol<1 ){ return; }

		pOrdPara->ordReq.VolumeTotalOriginal = iValidVol;
	m_pOrder = AfxBeginThread((AFX_THREADPROC)OrderThread,pOrdPara);
}

UINT CXTraderDlg::SendTmOrd(LPVOID pParam)
{
	TMORDPARAM*  pPa = static_cast<TMORDPARAM*>(pParam);
	CXTraderDlg *pDlg = static_cast<CXTraderDlg*>(pPa->pDlg);

	COleDateTime dt(pPa->dt.nYear,pPa->dt.nMonth,pPa->dt.nDay,pPa->dt.nHour,pPa->dt.nMin,pPa->dt.nSec);
	COleDateTimeSpan ts(NULL);
	while(true)
	{
		Sleep(1);
		ts = COleDateTime::GetCurrentTime()-dt;
		if (ts.GetTotalSeconds()>= OLEDT_HALFSECOND)
		{
			pDlg->GenOrdFromPk(pPa->pkReq);

			VIT_pk vpk=gv_Pk.begin();
			for(vpk;vpk!=gv_Pk.end();vpk++)
			{
				//线程HANDLE修改过
				if (!memcmp(&pPa->pkReq,&(*vpk),sizeof(PARKEDEX)-2*sizeof(HANDLE)))
				{
					//LOGI("定时单:(" << vpk->park.InstrumentID << ")");
					vpk->park.Status = THOST_FTDC_PAOS_Send;

					pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(gv_Pk.size())); 
					if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
					break;
				}
			}

			break;
		}
	}
	
	DEL(pPa);
	return 0;
}

void CXTraderDlg::SendLocalCondOrd(const CThostFtdcDepthMarketDataField* pMd)
{
	CtpTdSpi* td = gc_Td;
	VIT_pk vpk = td->m_pkExVec.begin();
	for (vpk;vpk!=td->m_pkExVec.end();vpk++)
	{
		if (vpk->park.Status ==THOST_FTDC_OST_NotTouched && vpk->ordType==COND_LOCAL && 
		CondCompare(pMd,vpk->park.InstrumentID,vpk->park.StopPrice,vpk->park.ContingentCondition))
		{ 
			GenOrdFromPk(*vpk);
			vpk->park.Status = THOST_FTDC_OST_Touched;

			m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(td->m_pkExVec.size()));
			if(m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
}

LRESULT CXTraderDlg::UpdateMdMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcDepthMarketDataField* pMd = (CThostFtdcDepthMarketDataField*)lParam;
	if(pMd)
	{
		CtpTdSpi* td = gc_Td;
		int iMul = td->FindInstMul(pMd->InstrumentID);
		//调整成正常值
		if (!strcmp(pMd->ExchangeID,_CZCE)) { pMd->Turnover *= iMul; }
		else
		{ pMd->AveragePrice /= iMul; }
		
        if(pMd->Volume<1||pMd->LastPrice==DBL_MAX) { pMd->LastPrice = pMd->PreSettlementPrice; }

		string strkey(pMd->InstrumentID);
		MIT_md vmd = td->m_DepthMdMap.find(strkey);
		if (vmd != td->m_DepthMdMap.end()) { /*Lock();*/ vmd->second = *pMd; /*UnLock();*/ }

		if (!strcmp(pMd->InstrumentID,g_s.m_szInst))
		{
			memcpy(m_pDepthMd,pMd,sizeof(CThostFtdcDepthMarketDataField));
			if (!m_pMdThread) { m_pMdThread = AfxBeginThread((AFX_THREADPROC)UpdateMdThread,this); }
		}

		//if (IsTdPeriod()) { UpdatePosProf(pMd); }
		UpdatePosProf(pMd);
		SendLocalCondOrd(pMd);
	
		DEL(pMd);
	}
	return 0;
}
void CXTraderDlg::UpdatePosProf(CThostFtdcDepthMarketDataField* pMd)
{
	CtpTdSpi* td = gc_Td;

	double dPosProf = 0.0;
	string strkey;
	int iPos = 0,iYdPos=0,iFlag=0,iMul=0,iRet=1;
	CThostFtdcDepthMarketDataField* theMd=NULL;
	MIT_md vmd;
	VIT_pos vp=td->m_InvPosVec.begin();
	for (vp;vp!=td->m_InvPosVec.end();vp++)
	{
		iPos = vp->Position;
		iFlag = (vp->PosiDirection == THOST_FTDC_PD_Long)?1:(-1);
		iMul = td->FindInstMul(vp->InstrumentID);	
		iRet = strcmp(pMd->InstrumentID,vp->InstrumentID);
		if (iRet==0){ theMd = pMd;	}
		else
		{ 
			strkey = vp->InstrumentID;
			vmd = td->m_DepthMdMap.find(strkey);
			if (vmd != td->m_DepthMdMap.end()){ theMd = &vmd->second; }
		}
		
		vp->PositionProfit = FiltDb(iFlag*(theMd->LastPrice*iPos*iMul - vp->PositionCost));
		dPosProf += vp->PositionProfit;
		//此处可以优化
		td->GetInstMgr(vp->InstrumentID,vp->PosiDirection,vp->MarginRateByMoney,vp->MarginRateByVolume);
		vp->UseMargin = vp->PositionCost*vp->MarginRateByMoney + iPos*vp->MarginRateByVolume;
		if (iRet==0) 
		{  		
			m_xLst[INVPOS_IDX].SetItemCountEx(static_cast<int>(td->m_InvPosVec.size()));
			if(m_xLst[INVPOS_IDX].IsWindowVisible()) { m_xLst[INVPOS_IDX].Invalidate(); }  
		}
	}
	
	m_pTdAcc->PositionProfit = dPosProf;
	InitProfit(m_pTdAcc);
	if (m_bMinGui)
	{
		CString str;
		str.Format(_T("盈亏:%lld 费用:%lld"),D2Int(m_pTdAcc->CloseProfit+m_pTdAcc->PositionProfit),
			D2Int(m_pTdAcc->Commission));
		SetWindowText(str);
	}
}

double CXTraderDlg::GetFreeMoney()
{
	double dStatProf = m_pTdAcc->PreBalance-m_pTdAcc->PreCredit-m_pTdAcc->PreMortgage+
		m_pTdAcc->Mortgage-m_pTdAcc->Withdraw+m_pTdAcc->Deposit;

	//有期权交易时权利金CashIn需要动态计算
	double dDymProf = dStatProf+m_pTdAcc->CloseProfit+m_pTdAcc->PositionProfit+
		m_pTdAcc->CashIn-m_pTdAcc->Commission;

	//冻结保证金,冻结手续费,冻结资金,信用金额都需要动态计算
	//要考虑大边保证金
	double dValidProf = dDymProf-m_pTdAcc->CurrMargin-m_pTdAcc->FrozenMargin-
		m_pTdAcc->FrozenCommission-m_pTdAcc->FrozenCash-m_pTdAcc->DeliveryMargin+m_pTdAcc->Credit;

	return dValidProf;
}

void CXTraderDlg::UpdateMdList()
{
    CAppCfgs& s = g_s;
	InsIt vecf;
	ClearVec(m_SubList);
	VIT_pos vp = gv_InvPos.begin();
	for (vp;vp!=gv_InvPos.end();vp++)
	{
		vecf = find(m_SubList.begin(), m_SubList.end(), INSTMENT(vp->InstrumentID));
		if(vecf == m_SubList.end()) { m_SubList.push_back(INSTMENT(vp->InstrumentID)); }
	}

	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,s.m_szInst);
	vecf = find(m_SubList.begin(), m_SubList.end(), INSTMENT(s.m_szInst));
	if(vecf == m_SubList.end()) { m_SubList.push_back(INSTMENT(s.m_szInst));}

}

UINT CXTraderDlg::UpdateMdThread(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	pDlg->UpdateMdPane();
	pDlg->m_pMdThread = NULL;

	return 0;
}

void CXTraderDlg::UpdateMdPane()
{
	TCHAR szUpTm[40],szuni[20];
	ansi2uni(CP_ACP,m_pDepthMd->UpdateTime,szuni);
	
	_stprintf(szUpTm,_T("%s.%03d"),szuni,m_pDepthMd->UpdateMillisec);
	m_csSUptime.SetText(szUpTm);
	
	double dPresp=m_pDepthMd->PreSettlementPrice;
	double dUpD = m_pDepthMd->LastPrice-dPresp;

	CString szAmp;
	szAmp.Format(_T("%.2f%%"),100*dUpD/dPresp);
	m_csDSmp.SetText(szAmp,CmpPriceColor(dUpD,0));
	
	m_csS1P.SetDouble(m_pDepthMd->AskPrice1,CmpPriceColor(m_pDepthMd->AskPrice1,dPresp));
	m_csS1V.SetInt(m_pDepthMd->AskVolume1,YELLOW);
	
	if (m_pDepthMd->LastPrice > m_dOldPrice)
	{ m_csLastV.SetText(_T("↑"),RED); }
	else if (m_pDepthMd->LastPrice < m_dOldPrice)
	{ m_csLastV.SetText(_T("↓"),GREEN); }
	else
	{ m_csLastV.SetText(_T("―"),WHITE); }
	
	m_csLastP.SetDouble(m_pDepthMd->LastPrice,CmpPriceColor(m_pDepthMd->LastPrice,m_dOldPrice));
	m_csB1P.SetDouble(m_pDepthMd->BidPrice1,CmpPriceColor(m_pDepthMd->BidPrice1,dPresp));
	m_csB1V.SetInt(m_pDepthMd->BidVolume1,YELLOW);
	
	m_csDUpDown.SetDouble(dUpD,CmpPriceColor(m_pDepthMd->LastPrice,dPresp));
	m_csDHighest.SetDouble(m_pDepthMd->HighestPrice,CmpPriceColor(m_pDepthMd->HighestPrice,dPresp));
	
	m_csDLowest.SetDouble(m_pDepthMd->LowestPrice,CmpPriceColor(m_pDepthMd->LowestPrice,dPresp));
	m_csVTotal.SetInt(m_pDepthMd->Volume,YELLOW);
	m_csVHold.SetDouble(m_pDepthMd->OpenInterest,YELLOW);
	
	m_csDOpt.SetDouble(m_pDepthMd->OpenPrice,CmpPriceColor(m_pDepthMd->OpenPrice,dPresp));
	
	m_dOldPrice = m_pDepthMd->LastPrice;
	
	m_SpinPrice.SetRangeAndDelta(m_pDepthMd->LowerLimitPrice, m_pDepthMd->UpperLimitPrice, m_InstInf->iinf.PriceTick);
	if(m_bUpdateOp||m_bLastPx) {m_SpinPrice.SetPos((m_pDepthMd->LastPrice<NEARZERO)?m_InstInf->iinf.PriceTick:m_pDepthMd->LastPrice);}
	
	 m_bUpdateOp = FALSE;
}

void CXTraderDlg::Go2InstMd()
{
	if (gc_Td->IsValidInst(m_szInst,m_InstInf))
	{
		double dPriceTick = m_InstInf->iinf.PriceTick;
		int iMaxVol = m_InstInf->iinf.MaxLimitOrderVolume;
		
		m_SpinVol.SetRangeAndDelta(1, iMaxVol, 1);
		m_SpinPrice.SetDecimalPlaces(JudgeDigit(dPriceTick));
		
		if (!m_pSubMd){ m_pSubMd = AfxBeginThread((AFX_THREADPROC)SubscribeMD, this); }
	}
}

void CXTraderDlg::OnEditchgInst()
{
	m_bUpdateOp = TRUE;

	GetDlgItem(IDC_INST)->GetWindowText(m_szInst);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,g_s.m_szInst);
	UpdateMdList();

	Go2InstMd();
}

void CXTraderDlg::OnTips()
{

}

////////////////////////////////////////////////////////////////////////
void CXTraderDlg::OnModifyPass()
{
	DlgModPass* dlgMop = new DlgModPass;
	
	BOOL res=dlgMop->Create(IDD_DLG_MODPASS,NULL);
	VERIFY( res==TRUE );
	dlgMop->CenterWindow();
	dlgMop->ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////
void CXTraderDlg::OnTransBf()
{
	BfTransfer* dlgBft = new BfTransfer;

	BOOL res=dlgBft->Create(IDD_BFTRANS,NULL);
	VERIFY( res==TRUE );
	dlgBft->CenterWindow();
	dlgBft->ShowWindow(SW_SHOW);
}

void CXTraderDlg::PopupBkAccDlg(CThostFtdcNotifyQueryAccountField* pNt)
{
	/////////////////////////////////////////////////
	CNoticeDlg* pDlg = new CNoticeDlg;
	pDlg->m_szTitle = LoadStr(IDS_BKACC_LEFT);
	
	BOOL res=pDlg->Create(IDD_DLG_NOTICE,this);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
	
	CString szLine = _T(""),szTemp = _T("");
	
	szLine += FormatLine(_T(""),_T(""),_T("="),42);
	
	ansi2uni(CP_ACP,pNt->BankAccount,szTemp.GetBuffer(MAX_PATH));
	szTemp.ReleaseBuffer();
	szLine += FormatLine(_T("  银行账号:"),szTemp,_T(" "),40);
	
	szTemp.Format(_T("%.2f"),pNt->BankUseAmount); 
	outStrAs4(szTemp);
	szLine += FormatLine(_T("  银行可用金额:"),szTemp,_T(" "),39);
	
	szTemp.Format(_T("%.2f"),pNt->BankFetchAmount); 
	outStrAs4(szTemp);
	szLine += FormatLine(_T("  银行可取金额:"),szTemp,_T(" "),39);
	
	ansi2uni(CP_ACP,pNt->CurrencyID,szTemp.GetBuffer(MAX_PATH));
	szTemp.ReleaseBuffer();
	szLine += FormatLine(_T("  币种:"),szTemp,_T(" "),41);
	
	szLine += FormatLine(_T(""),_T(""),_T("="),42);
	
	pDlg->GetDlgItem(IDC_NTMSG)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szLine);
}

LRESULT CXTraderDlg::QryBkYe(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcNotifyQueryAccountField* pQryAcc = (CThostFtdcNotifyQueryAccountField*)lParam;
	if(pQryAcc)
	{
		PopupBkAccDlg(pQryAcc);
		DEL(pQryAcc);
	}

	return 0;
}

LRESULT CXTraderDlg::TdReConnMsg(WPARAM wParam,LPARAM lParam)
{
	m_pCliInfo->iFrtId = gc_Td->m_iFrtId;
	m_pCliInfo->iSesId = gc_Td->m_iSesId;
	
	SetTipTxt(_T("交易在线"),IDS_TRADE_TIPS);
	SetPaneTxtColor(1,RED);
	
	CString	szT;
	CTime t0(CTime::GetCurrentTime());
	CTimeSpan ts(0,0,0,5);
	CTime t1 = t0 + ts;
	
	sprintf(m_tDisEnd,"%02d:%02d:%02d",t1.GetHour(), t1.GetMinute(), t1.GetSecond());
	
	if (m_eLogMod == RECONN_LOG)
	{
		if (!m_pUnSubMd) { m_pUnSubMd = AfxBeginThread((AFX_THREADPROC)UnSubscribeMD, this); }
		if (!m_pReConQry){ m_pReConQry = AfxBeginThread((AFX_THREADPROC)ReConnTdQry, this); }

		szT.Format(_T("%d:%02d:%02d CTP重登录成功"), t0.GetHour(), t0.GetMinute(), t0.GetSecond());
		SetStatusTxt(szT,2);
	}

	if (m_eLogMod == CHGACC_LOG)
	{
		szT.Format(_T("%d:%02d:%02d 切换用户成功"), t0.GetHour(), t0.GetMinute(), t0.GetSecond());
		SetStatusTxt(szT,2);
	}

	return 0;
}

LRESULT CXTraderDlg::MdReConnMsg(WPARAM wParam,LPARAM lParam)
{
	SetTipTxt(_T("行情在线"),IDS_MD_TIPS);
	SetPaneTxtColor(0,RED);

	return 0;
}

// 如果结算后断开就重置
LRESULT CXTraderDlg::TdDisConnMsg(WPARAM wParam,LPARAM lParam)
{
	SetTipTxt(_T("交易断开"),IDS_TRADE_TIPS);
	SetPaneTxtColor(1,BLUE);

	///////////////线程清理////////////////
	ClearTmOrdThd();
	/////////////////////////////////////////

	CString	szT;
	CTime t0(CTime::GetCurrentTime());
	CTimeSpan ts(0,0,0,60);
	CTime t1 = t0 - ts;
	
	sprintf(m_tDisStart,"%02d:%02d:%02d",t1.GetHour(), t1.GetMinute(), t1.GetSecond());
	szT.Format(_T("%d:%02d:%02d CTP中断等待重连"),t0.GetHour(), t0.GetMinute(), t0.GetSecond());
	SetStatusTxt(szT,2);
	ShowErroTips(IDS_DISCONTIPS,IDS_STRTIPS);

	m_eLogMod = RECONN_LOG;

	return 0;
}

void CXTraderDlg::SilentReLogin()
{
    DlgSiLog* pDlg = new DlgSiLog;
    
    pDlg->Create(IDD_SILOGIN,this);
    pDlg->CenterWindow();
    pDlg->ShowWindow(SW_SHOW);
}

UINT CXTraderDlg::ReConnTdQry(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	if (pDlg->m_bTdClosed) { pDlg->SilentReLogin(); }
	else
	{
        CtpTdSpi* td = gc_Td;
		ClearVec(td->m_InvPosVec); 
		//ClearVec(td->m_pkExVec);
		
		ZeroMemory(&td->m_TdAcc,sizeof(CThostFtdcTradingAccountField));
		ZeroMemory(pDlg->m_pTdAcc,sizeof(CThostFtdcTradingAccountField));
		
		td->ReqQryTd(NULL,NULL,NULL,pDlg->m_tDisStart,pDlg->m_tDisEnd);
		DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
		if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }	
		
		td->ReqQryOrd(NULL,NULL,NULL,pDlg->m_tDisStart,pDlg->m_tDisEnd);
		dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
		if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }	
		
		td->ReqQryInvPos(NULL);
		dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
		if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }	
		
		td->ReqQryTdAcc();
		dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
		if (dwRet==WAIT_OBJECT_0) { ResetEvent(g_hEvent); }	
		
		//td->ReqQryParkedOrd(NULL,NULL);
		//dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
		//if (dwRet==WAIT_OBJECT_0) { ResetEvent(g_hEvent); }	
		
		//因为只查询了掉线时间段的order和td数据
		//sort(td->m_orderVec.begin(),td->m_orderVec.end(),cmpOrder);
		//sort(td->m_tradeVec.begin(),td->m_tradeVec.end(),cmpTrade);
		
		pDlg->m_xLst[ONROAD_IDX].SetItemCountEx(static_cast<int>(td->m_onRoadVec.size()));
		pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(td->m_orderVec.size()));
		pDlg->m_xLst[INVPOS_IDX].SetItemCountEx(static_cast<int>(td->m_InvPosVec.size()));
		pDlg->m_xLst[TRADE_IDX].SetItemCountEx(static_cast<int>(td->m_tradeVec.size()));
		
		if(pDlg->m_xLst[ONROAD_IDX].IsWindowVisible()) { pDlg->m_xLst[ONROAD_IDX].Invalidate(); }
		if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
		if(pDlg->m_xLst[INVPOS_IDX].IsWindowVisible()) { pDlg->m_xLst[INVPOS_IDX].Invalidate(); }
		if(pDlg->m_xLst[TRADE_IDX].IsWindowVisible()) { pDlg->m_xLst[TRADE_IDX].Invalidate(); }

		if (!pDlg->m_pSubMd) { pDlg->m_pSubMd = AfxBeginThread((AFX_THREADPROC)CXTraderDlg::SubscribeMD, pDlg); }
	}
	
	pDlg->m_pReConQry = NULL;
	return 0;
}

LRESULT CXTraderDlg::MdDisConnMsg(WPARAM wParam,LPARAM lParam)
{
	SetTipTxt( _T("行情断开"),IDS_MD_TIPS);
	SetPaneTxtColor(0,BLUE);

	return 0;
}

void CXTraderDlg::SendLocalPkOrd()
{
    vector<PARKEDEX>& pkv = gv_Pk;
	VIT_pk vpk = pkv.begin();
	for (vpk;vpk!=pkv.end();vpk++)
	{
		if (vpk->ordType==ENTERTD_LOCAL && vpk->park.Status==THOST_FTDC_PAOS_NotSend)
		{ 
			GenOrdFromPk(*vpk);
			vpk->park.Status = THOST_FTDC_PAOS_Send;

			m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(pkv.size()));
			if(m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
}

UINT CXTraderDlg::EnterTdQryPk(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	//延迟两秒
	Sleep(2000);
	gc_Td->ReqQryParkedOrd(NULL,NULL);
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{ 
		pDlg->m_bQryPk = false;
		ResetEvent(g_hEvent); 
	}	

	return 0;
}

LRESULT CXTraderDlg::ExhStatusMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcInstrumentStatusField* pStat = (CThostFtdcInstrumentStatusField*)lParam;
	CString szStat,szMsg,szExh;
	TCHAR szTm[30];
	ansi2uni(CP_ACP,pStat->EnterTime,szTm);
	szStat = JgTdStatus(pStat->InstrumentStatus);
	szExh = JgExchage(pStat->ExchangeID);
	
	szMsg.Format(_T("%s %s:%s"),szTm,szExh,szStat);
	SetStatusTxt(szMsg,2);

	//开盘时发本地预埋单,并查询已经预埋的是否发送成功
	switch (pStat->InstrumentStatus)
	{
		case THOST_FTDC_IS_Continous:
		case THOST_FTDC_IS_AuctionOrdering:
		case THOST_FTDC_IS_AuctionBalance:
		case THOST_FTDC_IS_AuctionMatch:
		{ 
			m_bTdClosed = false;
			SendLocalPkOrd(); 
			if (m_bQryPk){ AfxBeginThread((AFX_THREADPROC)CXTraderDlg::EnterTdQryPk, this); }
			PcBeep();
		}
			break;

		case THOST_FTDC_IS_BeforeTrading:
			m_bQryPk = true;
			m_bTdClosed = false;
			break;
		case THOST_FTDC_IS_Closed:
			m_bTdClosed = true;
			PcBeep();
			break;
	}
	
	DEL(pStat);

	return 0;
}

void CXTraderDlg::PopupNoticeDlg(CThostFtdcTradingNoticeInfoField* pMsg)
{
	/////////////////////////////////////////////////
	if (strlen(pMsg->FieldContent) && g_s.m_bRiskNt)
	{
		CNoticeDlg* pDlg = new CNoticeDlg;
		pDlg->m_szTitle = LoadStr(IDS_TDNT_TITLE);
		
		BOOL res=pDlg->Create(IDD_DLG_NOTICE,this);
		VERIFY( res==TRUE );
		pDlg->CenterWindow();
		pDlg->ShowWindow(SW_SHOW);
		
		CString szLine = _T(""),szTemp = _T("");
		TCHAR szCont[1024];

		szTemp = FormatLine(_T(""),_T(""),_T("="),60);
		szLine += szTemp;

		ansi2uni(CP_ACP,pMsg->SendTime,szCont);
		szTemp.Format(_T("信息时间:  %s\r\n"),szCont);
		szLine += szTemp;
		
		ansi2uni(CP_ACP,pMsg->FieldContent,szCont);
		szTemp.Format(_T("信息内容:\r\n\r\n\t%s\r\n"),szCont);
		szLine += szTemp;

		szTemp = FormatLine(_T(""),_T(""),_T("="),60);
		szLine += szTemp;
		szLine += _T("\r\n");
		
		pDlg->GetDlgItem(IDC_NTMSG)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szLine);
	}
}

LRESULT CXTraderDlg::TdNoticeMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcTradingNoticeInfoField* pNotice = (CThostFtdcTradingNoticeInfoField*)lParam;
	PopupNoticeDlg(pNotice);
	
	DEL(pNotice);
	return 0;
}

LRESULT CXTraderDlg::OpenAccMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcOpenAccountField* pMsg = (CThostFtdcOpenAccountField*)lParam;
	//PopupNoticeDlg(pNotice);
	
	DEL(pMsg);
	return 0;
}

LRESULT CXTraderDlg::DelAccMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcCancelAccountField* pMsg = (CThostFtdcCancelAccountField*)lParam;
	//PopupNoticeDlg(pNotice);
	
	DEL(pMsg);
	return 0;
}

LRESULT CXTraderDlg::ModAccMsg(WPARAM wParam,LPARAM lParam)
{
	CThostFtdcChangeAccountField* pMsg = (CThostFtdcChangeAccountField*)lParam;
	//PopupNoticeDlg(pNotice);
	
	DEL(pMsg);
	return 0;
}

void CXTraderDlg::CancelOrd(PCORDFEx pOrd)
{
	gc_Td->CancelOrd(pOrd);
}

void CXTraderDlg::OnCancelOrd()
{
	if (m_xLst[ONROAD_IDX].IsWindowVisible()) { CancelOrd(&gv_oR[m_iActIdx]); }
	if (m_xLst[ORDER_IDX].IsWindowVisible()) { CancelOrd(&gv_Ord[m_iActIdx]); }
}

void CXTraderDlg::OnCancelAll()
{
	gc_Td->CancelAll();
}

void CXTraderDlg::GenOrdByInvPos(const CThostFtdcInvestorPositionField *pInv,TINVPCLOSE mType)
{
	//if (!m_pOrder)
	{
		ORDTHREADP *pOrdPara = new ORDTHREADP;
		pOrdPara->pDlg = this;
		ZeroMemory(&pOrdPara->ordReq,sizeof(CThostFtdcInputOrderField));
		strcpy(pOrdPara->ordReq.InstrumentID,pInv->InstrumentID);
		
		pOrdPara->ordReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		pOrdPara->ordReq.Direction=THOST_FTDC_D_Buy+THOST_FTDC_D_Sell+2-pInv->PosiDirection;
		
		pOrdPara->ordReq.VolumeCondition = THOST_FTDC_VC_AV;
		pOrdPara->ordReq.ContingentCondition = THOST_FTDC_CC_Immediately;	
		
		pOrdPara->ordReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		pOrdPara->ordReq.CombOffsetFlag[0]=THOST_FTDC_OF_Close;
		pOrdPara->ordReq.TimeCondition = THOST_FTDC_TC_IOC;
	
		pOrdPara->ordReq.VolumeTotalOriginal = GetVolByPos(pInv,pInv->Position,pOrdPara->iTdPos,pOrdPara->iYdPos);
		//m_pOrder = 
		AfxBeginThread((AFX_THREADPROC)OrderThread,pOrdPara);
	}

	if (mType==TBKHAND)
	{
		ORDTHREADP *pOrdPara = new ORDTHREADP;
		pOrdPara->pDlg = this;
		ZeroMemory(&pOrdPara->ordReq,sizeof(CThostFtdcInputOrderField));
		strcpy(pOrdPara->ordReq.InstrumentID,pInv->InstrumentID);
		pOrdPara->ordReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		
		pOrdPara->ordReq.Direction=THOST_FTDC_D_Buy+THOST_FTDC_D_Sell+2-pInv->PosiDirection;
		pOrdPara->ordReq.CombOffsetFlag[0]=THOST_FTDC_OF_Open;
		pOrdPara->ordReq.VolumeTotalOriginal = pInv->Position;
		
		pOrdPara->ordReq.VolumeCondition = THOST_FTDC_VC_AV;
		pOrdPara->ordReq.ContingentCondition = THOST_FTDC_CC_Immediately;	
		pOrdPara->ordReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		pOrdPara->ordReq.TimeCondition = THOST_FTDC_TC_IOC;

		pOrdPara->ordReq.VolumeTotalOriginal = GetVolByPos(pInv,pInv->Position,pOrdPara->iTdPos,pOrdPara->iYdPos);//pInv->Position;
		AfxBeginThread((AFX_THREADPROC)OrderThread,pOrdPara);
	}
}

void CXTraderDlg::OnBackHand()
{
	if (m_xLst[INVPOS_IDX].IsWindowVisible()) {  GenOrdByInvPos(&gv_InvPos[m_iActIdx],TBKHAND); }	
}

void CXTraderDlg::InvPosClose()
{
	if (m_xLst[INVPOS_IDX].IsWindowVisible()) {  GenOrdByInvPos(&gv_InvPos[m_iActIdx]); }
}

void CXTraderDlg::GenFromOrd(CThostFtdcOrderFieldEx *pOrd)
{
	if (!m_pOrder)
	{
		ORDTHREADP *pOrdPara = new ORDTHREADP;
		pOrdPara->pDlg = this;
		ZeroMemory(&pOrdPara->ordReq,sizeof(CThostFtdcInputOrderField));
		strcpy(pOrdPara->ordReq.InstrumentID,pOrd->Ord.InstrumentID);
		
		pOrdPara->ordReq.Direction=pOrd->Ord.Direction;
		pOrdPara->ordReq.CombOffsetFlag[0]=pOrd->Ord.CombOffsetFlag[0];
		
		pOrdPara->ordReq.LimitPrice = pOrd->Ord.LimitPrice;
		pOrdPara->ordReq.VolumeTotalOriginal = pOrd->Ord.VolumeTotal;
		
		pOrdPara->ordReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		pOrdPara->ordReq.OrderPriceType = pOrd->Ord.OrderPriceType;
		pOrdPara->ordReq.VolumeCondition = pOrd->Ord.VolumeCondition;
		pOrdPara->ordReq.ContingentCondition = pOrd->Ord.ContingentCondition;	
		pOrdPara->ordReq.TimeCondition = pOrd->Ord.TimeCondition;
		
		m_pOrder = AfxBeginThread((AFX_THREADPROC)OrderThread,pOrdPara);
	}
}

UINT CXTraderDlg::ModOrdThread(LPVOID pParam)
{
	MODPARAM*  pPa = static_cast<MODPARAM*>(pParam);
	CXTraderDlg *pDlg = static_cast<CXTraderDlg*>(pPa->pDlg);
	pDlg->ModOrd(pPa->mType);
		
	pDlg->m_pModOrd = NULL;

	DEL(pPa);
	return 0;
}

void CXTraderDlg::ModOrd(MODTYPE mType)
{
	CXTraderApp* pApp = g_App;
	CtpTdSpi* td = g_App->m_cT;

	CThostFtdcOrderFieldEx* pOrd= new CThostFtdcOrderFieldEx; 
	ZeroMemory(pOrd,sizeof(CThostFtdcOrderFieldEx));
	if (m_xLst[ONROAD_IDX].IsWindowVisible()) {  memcpy(pOrd,&td->m_onRoadVec[m_iActIdx],sizeof(CThostFtdcOrderFieldEx));}
	if (m_xLst[ORDER_IDX].IsWindowVisible()) { memcpy(pOrd,&td->m_orderVec[m_iActIdx],sizeof(CThostFtdcOrderFieldEx)); }
	
	CancelOrd(pOrd); 

	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{ 
		ResetEvent(g_hEvent); 

		if(mType==MOD_DIR) { pOrd->Ord.Direction = THOST_FTDC_D_Buy+THOST_FTDC_D_Sell-pOrd->Ord.Direction; }
		if (pOrd->Ord.OrderPriceType == THOST_FTDC_OPT_LimitPrice)
		{
			CThostFtdcDepthMarketDataField pMd;
			ZeroMemory(&pMd,sizeof(CThostFtdcDepthMarketDataField));
			td->GetMdByInst(pOrd->Ord.InstrumentID,&pMd);

			if (pOrd->Ord.Direction == THOST_FTDC_D_Buy)
			{ pOrd->Ord.LimitPrice = pMd.AskPrice1; }
			else
			{ pOrd->Ord.LimitPrice = pMd.BidPrice1; }
		}
		 
		GenFromOrd(pOrd);
	}
	DEL(pOrd);
}

void CXTraderDlg::RunMod(MODTYPE mType)
{
	MODPARAM* pPara=new MODPARAM;
	ZeroMemory(pPara,sizeof(MODPARAM));
	pPara->pDlg = this; pPara->mType=mType;
		
	if (!m_pModOrd) { m_pModOrd = AfxBeginThread((AFX_THREADPROC)ModOrdThread,pPara); }
}

void CXTraderDlg::OnModDsj()
{
	RunMod(MOD_PX);
}

void CXTraderDlg::OnRevOrd()
{
	RunMod(MOD_DIR);
}

void CXTraderDlg::OnCsvExport()
{
	CString  strFilter = _T("文本文件(*.csv;*.txt)|*.csv;*.txt|所有文件(*.*)|*.*||");

	CFileDialog* dlgSave = new CFileDialog(FALSE, _T("*.csv"),  m_szExpDef, OFN_PATHMUSTEXIST | OFN_EXPLORER, strFilter, this);
	dlgSave->m_ofn.lStructSize=sizeof(OPENFILENAME);		//use the 2k+ open file dialog

	CString szFile;
	if (IDOK == dlgSave->DoModal())
	{
		szFile = dlgSave->GetPathName();
		for (int i=0;i<=PKCON_IDX;i++)
		{
			if (m_xLst[i].IsWindowVisible())
			{ 
				List2Csv(&m_xLst[i],(LPCTSTR)szFile); 
				break;
			}
		}
	}
	DEL(dlgSave);
}

void CXTraderDlg::PopupPrivInf(CThostFtdcInvestorField* pInf)
{
	CNoticeDlg* pDlg = new CNoticeDlg;
	pDlg->m_szTitle = LoadStr(IDS_ACC_FBINFO);
	
	BOOL res=pDlg->Create(IDD_DLG_NOTICE,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
	
	CString szLine = _T("");
	TCHAR szTemp[MAX_PATH] = {0};
	CXTraderApp* app = g_App;
	CtpTdSpi* td = app->m_cT;
	CAppCfgs& s= app->m_cfg;
	
	szLine += FormatLine(_T(""),_T(""),_T("="),48);
	
	szLine += FormatLine(_T(" 期货经纪:"),s.GetBkrById(pInf->BrokerID),_T(" "),42);
	
	ansi2uni(CP_ACP,pInf->InvestorID,szTemp);
	szLine += FormatLine(_T(" 期货账户:"),szTemp,_T(" "),46);
	
	ansi2uni(CP_ACP,pInf->OpenDate,szTemp);
	szLine += FormatLine(_T(" 开户日期:"),szTemp,_T(" "),46);
	
	szLine += FormatLine(_T(""),_T(""),_T("="),48);
	ansi2uni(CP_ACP,pInf->InvestorName,szTemp);
	szLine += FormatLine(_T(" 客户姓名:"),szTemp,_T(" "),44);
	
	_tcscpy(szTemp, (LPCTSTR)JgCardType(pInf->IdentifiedCardType));
	szLine += FormatLine(_T(" 证件类型:"),szTemp,_T(" "),43);
	
	ansi2uni(CP_ACP,pInf->IdentifiedCardNo,szTemp);
	szLine += FormatLine(_T(" 证件号码:"),szTemp,_T(" "),46);
	
	if (pInf->IsActive){ _tcscpy(szTemp,_T("是")); }
	else
	{ _tcscpy(szTemp,_T("否")); }
	
	szLine += FormatLine(_T(" 是否活跃:"),szTemp,_T(" "),45);
	
	if (strlen(pInf->Telephone))
	{
		ansi2uni(CP_ACP,pInf->Telephone,szTemp);
		szLine += FormatLine(_T(" 联系电话:"),szTemp,_T(" "),46);
	}

	if (strlen(pInf->Mobile))
	{
		ansi2uni(CP_ACP,pInf->Mobile,szTemp);
		szLine += FormatLine(_T(" 联系手机:"),szTemp,_T(" "),46);
	}

	if (strlen(pInf->Address))
	{
		ansi2uni(CP_ACP,pInf->Address,szTemp);
		szLine += FormatLine(_T(" 通讯地址:"),szTemp,_T(" "),31);
	}
	
	szLine += FormatLine(_T(""),_T(""),_T("="),48);
	CString szItem = _T("");
	for (UINT i=0;i<td->m_TdCodeVec.size();i++)
	{
		ansi2uni(CP_ACP,td->m_TdCodeVec[i].ClientID,szTemp);
		szItem.Format(_T(" %s%s编码:"),JgExchage(td->m_TdCodeVec[i].ExchangeID),
			JgTdCodeType(td->m_TdCodeVec[i].ClientIDType));
		szLine += FormatLine((LPCTSTR)szItem, szTemp, _T(" "), 44);
	}
	
	szLine += FormatLine(_T(""),_T(""),_T("="),48);
	
	pDlg->GetDlgItem(IDC_NTMSG)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szLine);	
}

UINT CXTraderDlg::QryUserInf(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	
	gc_Td->ReqQryInvest();
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{ ResetEvent(g_hEvent); }	

	pDlg->m_pQryUinf = NULL;

	return 0;
}

void CXTraderDlg::OnUserInf()
{
	if (!m_pQryUinf)
	{ m_pQryUinf = AfxBeginThread((AFX_THREADPROC)QryUserInf, this); }
}

void CXTraderDlg::OnNMClkLstOnroad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ONROAD_IDX].GetItemCount();
		if ((nItem>=0) && (iCount>0))
		{
			m_xLst[ONROAD_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_szInst=m_xLst[ONROAD_IDX].GetItemText(nItem,1); //合约
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
		}

	}
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////
void CXTraderDlg::OnNMRClkLstOnroad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ONROAD_IDX].GetItemCount();
		
		m_xLst[ONROAD_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		
		if ((nItem>=0) && (iCount>0))
		{
			m_szExpDef = GenDef(_T("挂单列表"),_T("csv"));
			m_iActIdx = iCount-1-nItem;

			CMenu PopMenu,*pSubMenu;
			CPoint pt;
			PopMenu.LoadMenu(IDR_LSTRMENU1);
			GetCursorPos(&pt);
			pSubMenu = PopMenu.GetSubMenu(0);	
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);	
		}
	}
	
	*pResult = 0;
}

void CXTraderDlg::OnNMDblclkOnroad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int iCount = -1;
	int nItem = -1;
	int nSubItem = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ONROAD_IDX].GetItemCount();
		
		m_xLst[ONROAD_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		
		if ((nItem>=0) && (iCount>0))
		{
			m_iActIdx = iCount-1-nItem;
			CancelOrd(&gv_oR[m_iActIdx]);
		}
	}
	*pResult = 0;
		
}

void CXTraderDlg::OnNMDblclkOrdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ORDER_IDX].GetItemCount();
		
		m_xLst[ORDER_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		if ((nItem>=0) && (iCount>0))
		{
			m_iActIdx = iCount-1-nItem;

			if(JgCancelStat(&gv_Ord[m_iActIdx])){ CancelOrd(&gv_Ord[m_iActIdx]); }
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMClkLstOrdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ORDER_IDX].GetItemCount();
		if ((nItem>=0) && (iCount>0))
		{
			m_xLst[ORDER_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_szInst=m_xLst[ORDER_IDX].GetItemText(nItem,1); //合约
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMRClkLstOrdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[ORDER_IDX].GetItemCount();
		
		m_xLst[ORDER_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		
		if ((nItem>=0) && (iCount>0))
		{
			m_iActIdx = iCount-1-nItem;
			m_szExpDef = GenDef(_T("委托列表"),_T("csv"));

			CPoint pt;
			GetCursorPos(&pt);
			
			CMenu menu;
			menu.CreatePopupMenu();

			DWORD flags = (JgCancelStat(&gv_Ord[m_iActIdx]))? (MF_POPUP|MF_BYPOSITION):(MF_POPUP|MF_BYPOSITION|MF_GRAYED);
			menu.InsertMenu(0, flags, ID_CANCEL_ORD,_T("撤单"));
			menu.InsertMenu(1, flags, ID_PK_CANCEL,_T("撤单预埋"));
			menu.InsertMenu(2, MF_BYPOSITION | MF_SEPARATOR);
			menu.InsertMenu(3, flags, ID_MOD_DSJ,_T("对价改单"));
			menu.InsertMenu(4, flags, ID_REVORD,_T("反向改单"));
			menu.InsertMenu(5, MF_BYPOSITION | MF_SEPARATOR);
			menu.InsertMenu(6, MF_POPUP|MF_BYPOSITION, ID_CSV_EXPORT,_T("导出列表"));
			menu.InsertMenu(7, MF_BYPOSITION | MF_SEPARATOR);
				
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMDblclkTdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[TRADE_IDX].GetItemCount();
		
		m_xLst[TRADE_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	*pResult = 0;
}

//单击订阅行情
void CXTraderDlg::OnNMClkLstTdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[TRADE_IDX].GetItemCount();
		
		if ((nItem>=0) && (iCount>0))
		{
			m_xLst[TRADE_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_szInst=m_xLst[TRADE_IDX].GetItemText(nItem,0); //合约
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMRClkLstTdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[TRADE_IDX].GetItemCount();
		
		m_xLst[TRADE_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		iCount = m_xLst[TRADE_IDX].GetItemCount();

		if (iCount>0)
		{
			m_szExpDef = GenDef(_T("成交列表"),_T("csv"));

			CPoint pt;
			GetCursorPos(&pt);
			
			CMenu menu;
			menu.CreatePopupMenu();
			menu.InsertMenu(0, MF_POPUP|MF_BYPOSITION, ID_CSV_EXPORT,_T("导出列表"));
			menu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*pResult = 0;
}

//双击持仓条目 平仓
void CXTraderDlg::OnNMDblclkInvPInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[INVPOS_IDX].GetItemCount();
		
		if ((nItem>=0) && (iCount>0))
		{
			m_xLst[INVPOS_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_iActIdx = iCount-1-nItem;
			InvPosClose();
		}
	}
	*pResult = 0;
}

void CXTraderDlg::ChgStatByBsOc(TThostFtdcDirectionType cDir,TThostFtdcOffsetFlagType cKp,int iVol)
{
	//此处调整开平仓选中时状态文字变化
	CString szDetail;
	szDetail.Format(_T("%s%s:%d手"),(cDir==THOST_FTDC_D_Buy)?_T("买"):_T("卖"),
		(cKp==THOST_FTDC_OF_Open)?_T("开"):_T("平"),iVol); 
	
	SetDlgItemText(IDC_STAVOL,szDetail);
}

void CXTraderDlg::ChgStatByInvPos(const CThostFtdcInvestorPositionField* pInvPos)
{
	BOOL bRet = gc_Td->JgExhByInst(pInvPos->InstrumentID,_SFE);

	int iHoldPos=GetHoldPos(pInvPos);

	m_SpinVol.SetPos(iHoldPos);
	//仅仅区分上期所,需要修改
	CString szDetail;
	int iTdPos = pInvPos->TodayPosition;	//今仓
	int iYdPos = pInvPos->Position-pInvPos->TodayPosition;	//昨仓
	if (bRet)
	{
		if (iTdPos==0){ szDetail.Format(_T("昨:%d"),iYdPos); }

		if (iYdPos==0){ szDetail.Format(_T("今:%d"),iTdPos); }

		if (iTdPos>0 && iYdPos>0){ szDetail.Format(_T("今:%d,昨:%d"),iTdPos,iYdPos); }
	}
	else
	{ szDetail.Format(_T("平仓:%d"),iHoldPos); }

	SetDlgItemText(IDC_STAVOL,szDetail);

	int iDir = THOST_FTDC_PD_Short-pInvPos->PosiDirection;

	m_CombBS.SetCurSel(iDir);
	m_cDir = (iDir+THOST_FTDC_D_Buy);
	m_CombOC.SetCurSel(1); //平仓
	m_cKpp = THOST_FTDC_OF_Close;
}

//单击持仓条目订阅行情
void CXTraderDlg::OnNMClkLstInvPInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[INVPOS_IDX].GetItemCount();
		
		if (nItem>=0 && iCount)
		{
			m_xLst[INVPOS_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

			m_iActIdx = iCount-1-nItem;
			m_szInst=m_xLst[INVPOS_IDX].GetItemText(nItem,0); //合约
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);

			ChgStatByInvPos(&gv_InvPos[m_iActIdx]);
		}
	}
	*pResult = 0;
}

//右击持仓条目市价(涨跌停板)反手
void CXTraderDlg::OnNMRClkLstInvPInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[INVPOS_IDX].GetItemCount();
		
		m_xLst[INVPOS_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		
		iCount =m_xLst[INVPOS_IDX].GetItemCount();
		if (nItem>=0 && iCount)
		{
			m_iActIdx = iCount-1-nItem;
			m_szExpDef = GenDef(_T("持仓列表"),_T("csv"));

			CPoint pt;
			GetCursorPos(&pt);
			
			CMenu menu;
			menu.CreatePopupMenu();
			menu.InsertMenu(0, MF_POPUP|MF_BYPOSITION, ID_REV_ANYPX,_T("市价反手"));
			menu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);
			menu.InsertMenu(2, MF_POPUP|MF_BYPOSITION, ID_CSV_EXPORT,_T("导出列表"));
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMClkLstInsts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[INSLST_IDX].GetItemCount();
		
		if (nItem>=0 && iCount)
		{
			m_xLst[INSLST_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_szInst=m_xLst[INSLST_IDX].GetItemText(nItem,1); //合约	
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMRClkLstInsts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[INSLST_IDX].GetItemCount();
		
		m_xLst[INSLST_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		iCount = m_xLst[INSLST_IDX].GetItemCount();
		
		if (iCount)
		{
			m_szExpDef = GenDef(_T("合约列表"),_T("csv"));

			CPoint pt;
			GetCursorPos(&pt);
			CMenu menu;
			menu.CreatePopupMenu();
			menu.InsertMenu(0, MF_POPUP|MF_BYPOSITION, ID_CSV_EXPORT,_T("导出列表"));
			menu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMDblclkParkOrd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[PKCON_IDX].GetItemCount();
		
		if (nItem>=0 && iCount)
		{
			m_xLst[PKCON_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}	
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMClkParkOrd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[PKCON_IDX].GetItemCount();

		if ((nItem>=0) && (iCount>0))
		{
			m_xLst[PKCON_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_szInst=m_xLst[PKCON_IDX].GetItemText(nItem,4); //合约	
			
			GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
		}
	}
	*pResult = 0;
}

void CXTraderDlg::OnNMRClkParkOrd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	
	int iCount = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
		iCount = m_xLst[PKCON_IDX].GetItemCount();
		
		m_xLst[PKCON_IDX].SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		iCount = m_xLst[PKCON_IDX].GetItemCount();
		if (nItem>=0 && iCount)
		{
			m_iActIdx = iCount-1-nItem;
			m_szExpDef = GenDef(_T("条件单列表"),_T("csv"));
			
			DWORD dwPkRmv = (JgPkRmvStat(&gv_Pk[m_iActIdx]))? (MF_POPUP|MF_BYPOSITION):(MF_POPUP|MF_BYPOSITION|MF_GRAYED);
			DWORD dwCanSend = (JgPkCanSend(&gv_Pk[m_iActIdx]))? (MF_POPUP|MF_BYPOSITION):(MF_POPUP|MF_BYPOSITION|MF_GRAYED);
			DWORD dwCondAct = (JgCondActStat(&gv_Pk[m_iActIdx]))? (MF_POPUP|MF_BYPOSITION):(MF_POPUP|MF_BYPOSITION|MF_GRAYED);
			
			CPoint pt;
			GetCursorPos(&pt);
			CMenu menu;
			menu.CreatePopupMenu();
			menu.InsertMenu(0, dwCanSend, ID_LOCAL_SEND,_T("发送报单"));
			menu.InsertMenu(1, dwCanSend, ID_LOCAL_REMV,_T("删除报单"));
			menu.InsertMenu(2, dwCondAct, ID_COND_ACT,_T("撤条件单"));
			menu.InsertMenu(3, MF_BYPOSITION | MF_SEPARATOR);
			menu.InsertMenu(4, dwPkRmv, ID_PK_REMV,_T("删除预埋"));
			menu.InsertMenu(5, dwPkRmv, ID_PK_DELCANCEL,_T("删除撤单"));
			menu.InsertMenu(6, MF_BYPOSITION | MF_SEPARATOR);
			menu.InsertMenu(7, MF_POPUP|MF_BYPOSITION, ID_CSV_EXPORT,_T("导出列表"));
			menu.InsertMenu(8, MF_BYPOSITION | MF_SEPARATOR);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}	
	}
	*pResult = 0;
}

void CXTraderDlg::OnGenMdFee()
{
	GenMfDlg* pDlg = new GenMfDlg;

	BOOL res=pDlg->Create(IDD_DLG_GENMRFEE,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
}

void CXTraderDlg::OnHiSettInf()
{
	DlgQryHiSet* pDlg = new DlgQryHiSet;

	BOOL res=pDlg->Create(IDD_DLG_HISETM,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
}

void CXTraderDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	if(cx<= 10 || cy <= 10)   return; 
	
	CSize Translate(cx - m_OldSize.cx, cy - m_OldSize.cy);
	::EnumChildWindows(m_hWnd, CXTraderDlg::EnumProc, (LPARAM)&Translate);
	
	m_OldSize = CSize(cx,cy); 
}

BOOL CALLBACK CXTraderDlg::EnumProc(HWND hw, LPARAM lParam)
{
	CWnd* pWnd = CWnd::FromHandle(hw);
	CSize* pTranslate = (CSize*) lParam;
	
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pWnd->GetParent());
	if (!pDlg) return FALSE;
	
	CRect rc,rc1;
	pWnd->GetWindowRect(rc1);
	pDlg->ScreenToClient(rc1);

	pDlg->GetWindowRect(rc);
	pDlg->ScreenToClient(rc);

	if (rc1.left-rc.left<30)
	{
		if(!(*pWnd==pDlg->m_csCpProf))
		{ ::MoveWindow(hw,rc1.left, rc1.top,rc1.Width()+pTranslate->cx, rc1.Height(), FALSE); }
	}
	else
	{
		if (!(*pWnd==pDlg->m_csHpProf||*pWnd==pDlg->m_csTdFee))
		{ 
			::MoveWindow(hw,rc1.left+pTranslate->cx, rc1.top, rc1.Width(), rc1.Height(), FALSE); 
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	::InvalidateRect(hw,NULL,TRUE);
	pDlg->Invalidate();
	return TRUE;
}

void CXTraderDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)   
{   
	memcpy(lpMMI,m_plpMMI,sizeof(MINMAXINFO));

    CDialog::OnGetMinMaxInfo(lpMMI);  
} 

void CXTraderDlg::Go2MiniGui()
{ 
	GetWindowRect(m_rcBak);

	SetMenu(NULL);
	DrawMenuBar();

	m_plpMMI->ptMinTrackSize.y = static_cast<LONG>(140*m_dScale);
	m_plpMMI->ptMaxTrackSize.y = static_cast<LONG>(140*m_dScale);
	m_plpMMI->ptMinTrackSize.x = static_cast<LONG>(390*m_dScale);
	m_plpMMI->ptMaxTrackSize.x = static_cast<LONG>(390*m_dScale);

	CRect rcMd,rcAcc,rcChild,rc0(m_rcBak);
	rc0.right -= static_cast<LONG>(100*m_dScale);
	rc0.bottom -= static_cast<LONG>(280*m_dScale);

	m_GroupMd.GetWindowRect(rcMd);
	m_GroupAcc.GetWindowRect(rcAcc);

	CWnd* pChild = NULL;
	for (pChild = GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		pChild->GetWindowRect(rcChild);
		if ((rcMd.PtInRect(rcChild.TopLeft()) && rcMd.PtInRect(rcChild.BottomRight()))||
			(rcAcc.PtInRect(rcChild.TopLeft()) && rcAcc.PtInRect(rcChild.BottomRight()))||
			m_GroupMd==*pChild || m_GroupAcc==*pChild ||m_StatusBar==*pChild)
		{ pChild->ShowWindow(SW_HIDE); }
	}

	MoveWindow(rc0);
	for (pChild = GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		pChild->GetWindowRect(rcChild);
		if (m_xLst[ONROAD_IDX]==*pChild||m_xLst[ORDER_IDX]==*pChild||m_xLst[INVPOS_IDX]==*pChild||
			m_xLst[TRADE_IDX]==*pChild||m_xLst[INSLST_IDX]==*pChild||m_xLst[PKCON_IDX]==*pChild||
			m_TabOption==*pChild)
		{
			rcChild.top -= static_cast<LONG>(54*m_dScale);
			rcChild.bottom -= static_cast<LONG>(180*m_dScale);
			ScreenToClient(&rcChild);
			pChild->MoveWindow(rcChild);
		}
		else if (GetDlgItem(IDC_EDVOL)==pChild || m_SpinVol==*pChild||GetDlgItem(IDC_BTORDER)==pChild||
			GetDlgItem(IDC_INST)==pChild ||m_CombBS==*pChild||m_CombOC==*pChild||GetDlgItem(IDC_EDPRICE)==pChild||
			m_SpinPrice==*pChild ||GetDlgItem(IDC_CHK_NEWP)==pChild||GetDlgItem(IDC_STAVOL)==pChild)
		{
			rcChild.top -= static_cast<LONG>(180*m_dScale);
			rcChild.bottom -= static_cast<LONG>(180*m_dScale);
			ScreenToClient(&rcChild);
			pChild->MoveWindow(rcChild);
		}
	}

	m_bMinGui = true;
	CString str;
	str.Format(_T("盈亏:%lld 费用:%lld"),D2Int(m_pTdAcc->CloseProfit+m_pTdAcc->PositionProfit),
		D2Int(m_pTdAcc->Commission));
	SetWindowText(str);
}

void CXTraderDlg::Scroll2Mid()
{
	/*
	CSize cs(0,0);

	if (m_xLst[ONROAD_IDX].IsWindowVisible()) { m_xLst[ONROAD_IDX].GetWindowRect(); }
	if (m_xLst[ORDER_IDX].IsWindowVisible()) { m_xLst[ORDER_IDX]; }
	if (m_xLst[TRADE_IDX].IsWindowVisible()) { m_xLst[TRADE_IDX]; }
	if (m_xLst[INVPOS_IDX].IsWindowVisible()) { m_xLst[INVPOS_IDX]; }
	if (m_xLst[INSLST_IDX].IsWindowVisible()) { m_xLst[INSLST_IDX]; }
	if (m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX]; }
	*/
}

void CXTraderDlg::RestoreGui()
{ 
	CMenu menu;
	m_plpMMI->ptMinTrackSize.y = static_cast<LONG>(359*m_dScale);
	m_plpMMI->ptMaxTrackSize.y = static_cast<LONG>(359*m_dScale);
	m_plpMMI->ptMinTrackSize.x = static_cast<LONG>(498*m_dScale);
	m_plpMMI->ptMaxTrackSize.x = static_cast<LONG>(1200*m_dScale);

	menu.LoadMenu(IDR_MAINMENU);
	SetMenu(&menu);
	DrawMenuBar();
	menu.Detach();

	CRect rc0,rcMd,rcAcc,rcChild;
	GetWindowRect(&rc0);
	rc0 = CRect(rc0.left,rc0.top,rc0.left+m_rcBak.Width(),rc0.top+m_rcBak.Height());

	m_GroupMd.GetWindowRect(rcMd);
	m_GroupAcc.GetWindowRect(rcAcc);
	
	CWnd* pChild=NULL;
	for (pChild = GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		pChild->GetWindowRect(rcChild);
		if ((rcMd.PtInRect(rcChild.TopLeft()) && rcMd.PtInRect(rcChild.BottomRight()))||
			(rcAcc.PtInRect(rcChild.TopLeft()) && rcAcc.PtInRect(rcChild.BottomRight()))||
			m_GroupMd==*pChild || m_GroupAcc==*pChild ||m_StatusBar==*pChild|| 
			m_csHpProf==*pChild||m_csTdFee==*pChild)
		{ pChild->ShowWindow(SW_SHOW); }
	}

	MoveWindow(rc0);
	for (pChild = GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		pChild->GetWindowRect(rcChild);
		if (m_xLst[ONROAD_IDX]==*pChild||m_xLst[ORDER_IDX]==*pChild||m_xLst[INVPOS_IDX]==*pChild||
			m_xLst[TRADE_IDX]==*pChild||m_xLst[INSLST_IDX]==*pChild||m_xLst[PKCON_IDX]==*pChild||
			m_TabOption==*pChild)
		{
			rcChild.top += static_cast<LONG>(54*m_dScale);
			rcChild.bottom += static_cast<LONG>(180*m_dScale);
			ScreenToClient(&rcChild);
			pChild->MoveWindow(rcChild);
		}
		else if (GetDlgItem(IDC_EDVOL)==pChild || m_SpinVol==*pChild||GetDlgItem(IDC_BTORDER)==pChild||
			GetDlgItem(IDC_INST)==pChild ||m_CombBS==*pChild||m_CombOC==*pChild||GetDlgItem(IDC_EDPRICE)==pChild||
			m_SpinPrice==*pChild ||GetDlgItem(IDC_CHK_NEWP)==pChild||GetDlgItem(IDC_STAVOL)==pChild)
		{
			rcChild.top += static_cast<LONG>(180*m_dScale);
			rcChild.bottom += static_cast<LONG>(180*m_dScale);
			ScreenToClient(&rcChild);
			pChild->MoveWindow(rcChild);
		}
	}

	m_bMinGui = false;
	CString str;
	str.Format(_T("%s"),g_s.m_szTitle);
	SetWindowText(str);
}

void CXTraderDlg::OnMiniMode()
{
	if (!m_bMinGui){ Go2MiniGui(); }
}

void CXTraderDlg::OnMiniTask()
{
	if (m_bMinGui){ RestoreGui(); }
}

void CXTraderDlg::OnDisplayChg(WPARAM wParam, LPARAM lParam)
{
	Invalidate();
}

void CXTraderDlg::OnHkeySet()
{

}

void CXTraderDlg::SwithMd2Inst(TThostFtdcInstrumentIDType instId)
{
	ansi2uni(CP_ACP,instId,m_szInst.GetBuffer(MAX_PATH));
	m_szInst.ReleaseBuffer();

	GetDlgItem(IDC_INST)->SetWindowText(m_szInst);
}

void CXTraderDlg::SetMainAlpha()
{
    CAppCfgs& s = g_s;
	s.m_bChkTrans = !s.m_bChkTrans;
	if (s.m_bChkTrans){ SetWndAlpha(m_hWnd, 128); }
	else
	{ SetWndAlpha(m_hWnd, 0xff); }
}

void CXTraderDlg::OnToolParked()
{
	ParkOrdDlg* Dlg = new ParkOrdDlg;

	Dlg->m_szInst = m_szInst;
	Dlg->m_iVol = m_iOrdVol;
    Dlg->m_dStopPx = m_dOrdPx;
	Dlg->m_dLmtPx = m_dOrdPx;
	CRect rc0,rc1;
	GetWindowRect(rc0);
	Dlg->Create(IDD_PARKORD_DLG);
	Dlg->GetWindowRect(rc1);
	//超出屏幕则从左侧弹出
	int xPos = rc0.right,yPos = rc0.bottom-rc1.Height();
	if ((rc0.right+rc1.Width())>GetSystemMetrics(SM_CXSCREEN)) 
	{ 
		xPos = (rc0.left+rc0.right-rc1.Width())/2; 
	}
	Dlg->SetWindowPos(NULL,xPos,yPos,0,0,SWP_NOSIZE);
	Dlg->ShowWindow(SW_SHOW);

	SwitchTab(PKCON_IDX);
}

void CXTraderDlg::ClearTmOrdThd()
{
	VIT_pk vpk=gv_Pk.begin();
	for(vpk;vpk!=gv_Pk.end();vpk++)
	{
		if ((vpk->ordType == TMORD_LOCAL) && (vpk->park.Status ==THOST_FTDC_OST_NotTouched) && (vpk->hThd !=NULL))
		{
			TermThread(vpk->hThd);
			DEL(vpk->pTmOrd);
		}
	}
}

void CXTraderDlg::OnReLogin()
{
	if (!m_pUnSubMd) { m_pUnSubMd = AfxBeginThread((AFX_THREADPROC)UnSubscribeMD, this); }
	///////////////线程清理////////////////
	ClearTmOrdThd();
	/////////////////////////////////////////
	m_eLogMod = CHGACC_LOG;
	
	m_xLst[ONROAD_IDX].SetItemCountEx(0); m_xLst[ONROAD_IDX].Invalidate();
	m_xLst[ORDER_IDX].SetItemCountEx(0); m_xLst[ORDER_IDX].Invalidate();
	m_xLst[INVPOS_IDX].SetItemCountEx(0); m_xLst[INVPOS_IDX].Invalidate();
	m_xLst[TRADE_IDX].SetItemCountEx(0); m_xLst[TRADE_IDX].Invalidate();
	m_xLst[INSLST_IDX].SetItemCountEx(0); m_xLst[INSLST_IDX].Invalidate();
	m_xLst[PKCON_IDX].SetItemCountEx(0); m_xLst[PKCON_IDX].Invalidate();
    //////////////////////////////////////////
	LoginDlg* login = new LoginDlg();
	login->m_bModeLess = TRUE;

	login->Create(IDD_LOGIN);
	login->ShowWindow(SW_SHOW);
	login->CenterWindow();
}

LRESULT CXTraderDlg::ReLogOkMsg(WPARAM wParam,LPARAM lParam)
{
	InitAllVecs();
	InitData();

	m_eLogMod = NORMAL_LOG;

	return 0;
}

UINT CXTraderDlg::QryTdNtThread(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	CtpTdSpi* td = gc_Td;

	ClearVec(td->m_TdNoticeVec);
	td->ReqQryTdNotice();
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }

	pDlg->m_pQryTdNt = NULL;
	return 0;
}

LRESULT CXTraderDlg::TdNtMsg(WPARAM wParam,LPARAM lParam)
{
	CString szLine = _T(""),szTemp = _T("");
	TCHAR szCont[1024];

	szTemp = FormatLine(_T(""),_T(""),_T("="),60);
	szLine += szTemp;
	//每条信息都是完整的而不是分步
	CThostFtdcTradingNoticeField* pTdNt;
	CtpTdSpi* td = gc_Td;
	for (UINT i=0;i<td->m_TdNoticeVec.size();i++)
	{ 
		pTdNt = &td->m_TdNoticeVec[i];
		if (strlen(pTdNt->FieldContent))
		{
			ansi2uni(CP_ACP,pTdNt->SendTime,szCont);
			szTemp.Format(_T("信息时间:  %s\r\n"),szCont);
			szLine += szTemp;
			
			ansi2uni(CP_ACP,pTdNt->FieldContent,szCont);
			szTemp.Format(_T("信息内容:\r\n\r\n\t%s\r\n"),szCont);
			szLine += szTemp;
			
			szTemp = FormatLine(_T(""),_T(""),_T("="),60);
			szLine += szTemp;
			szLine += _T("\r\n");
		}
	}
	
	///////////////////////////////////
	CNoticeDlg* pDlg = new CNoticeDlg;
	pDlg->m_szTitle = LoadStr(IDS_TDNT_TITLE);
	
	BOOL res=pDlg->Create(IDD_DLG_NOTICE,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
	
	pDlg->GetDlgItem(IDC_NTMSG)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szLine);

	return 0;
}

void CXTraderDlg::OnTdNotice()
{
	if (!m_pQryTdNt) {m_pQryTdNt = AfxBeginThread((AFX_THREADPROC)QryTdNtThread, this); }
}

UINT CXTraderDlg::QryBkrNtThread(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	CtpTdSpi* td = gc_Td;
	
	ClearVec(td->m_BkrNtVec);

	td->ReqQryNotice();
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0){ ResetEvent(g_hEvent); }
	
	pDlg->m_pQryBkrNt = NULL;
	return 0;
}

LRESULT CXTraderDlg::BkrNtMsg(WPARAM wParam,LPARAM lParam)
{
	CtpTdSpi* td = gc_Td;
	int iSize = static_cast<int>(td->m_BkrNtVec.size());
	int iBufSize = iSize*sizeof(CThostFtdcNoticeField);

	///////////////////////////////////
	BkrNtDlg* pDlg = new BkrNtDlg;
	pDlg->m_bMemory = TRUE;
	if (iBufSize)
	{
		char* szMsg = new char[iBufSize];
		ZeroMemory(szMsg,sizeof(szMsg));
		
		for (int i=0;i<iSize;i++)
		{ strcat(szMsg,(const char*)td->m_BkrNtVec[i].Content); }

		ansi2uni(CP_ACP,szMsg,pDlg->m_sContent.GetBuffer(2*static_cast<int>(strlen(szMsg))+2));
		pDlg->m_sContent.ReleaseBuffer();

		DELX(szMsg);
	}
	else
	{
		pDlg->m_sContent.Format(_T("<p align=center><b><font size=20>无风险提示</font></b><BR><a href=\"file://%sdemo.htm\">demo.htm</a></p>"),GetProgramDir());
	}

	BOOL res=pDlg->Create(IDD_DLG_BKRNT,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);

	return 0;
}

void CXTraderDlg::OnBkrNotice()
{
	if (!m_pQryBkrNt) { m_pQryBkrNt = AfxBeginThread((AFX_THREADPROC)QryBkrNtThread, this); }
}

UINT CXTraderDlg::QryTdTokThread(LPVOID pParam)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	gc_Td->ReqQryCFMMCTdAccTok();

	pDlg->m_pQryTdTok = NULL;
	return 0;
}

LRESULT CXTraderDlg::QryTokMsg(WPARAM wParam,LPARAM lParam)
{
	LPTSTR pUrl = (LPTSTR)lParam;
	///////////////////////////////////////////
	CString szUrl((LPCTSTR)pUrl);
	BkrNtDlg* pDlg = new BkrNtDlg;
	pDlg->m_szUrl = szUrl;
	pDlg->m_szTitle = LoadStr(IDS_CFMMC_TITLE);
	
	BOOL res=pDlg->Create(IDD_DLG_BKRNT,NULL);
	VERIFY( res==TRUE );
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);

	DELX(pUrl);
	return 0;
}

//进入期货(保证金)监控中心
void CXTraderDlg::OnCfmmc()
{
	if (!m_pQryTdTok) { m_pQryTdTok = AfxBeginThread((AFX_THREADPROC)QryTdTokThread, this); }
}

CString CXTraderDlg::GetStatMsg(PCORDFEx pOrder)
{
	CString szOut=_T("");
	if (pOrder->Ord.OrderStatus==THOST_FTDC_OST_ErrOrd)
	{
		ansi2uni(CP_ACP,pOrder->Ord.StatusMsg,szOut.GetBuffer(MAX_PATH));
		szOut.ReleaseBuffer();
	}
	else
	{ 
		CString szInf=_T("");
		TCHAR szPinf[MAX_PATH];
		if ((m_pCliInfo->iFrtId != pOrder->Ord.FrontID) || (m_pCliInfo->iSesId != pOrder->Ord.SessionID))
		{
			ansi2uni(CP_ACP,pOrder->Ord.UserProductInfo,szPinf);
			szInf.Format(_T("重登录:%s"),szPinf);
		}
		else
		{ szInf.Format(_T("当前:%s"),m_pCliInfo->ProdInf); }
		
		szOut.Format(_T("%s,%s"),JgOrdSubmitStat(pOrder->Ord.OrderSubmitStatus),szInf);
	}

	return szOut;
}

void CXTraderDlg::OnDispOnRoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem= pItem->iItem,iSubItem=pItem->iSubItem;
	rBg = (iItem%2)?LITGRAY:WHITE;

	CtpTdSpi* td = gc_Td;
	VRIT_ord vod = td->m_onRoadVec.rbegin()+iItem;
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		switch(iSubItem)
		{
            case COL_SYSID1:
                ansi2uni(CP_ACP,vod->Ord.OrderSysID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                szTemp.TrimLeft();
                break;
            case COL_INST1: 
                ansi2uni(CP_ACP,vod->Ord.InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_DIR1:
                if (vod->Ord.Direction ==THOST_FTDC_D_Buy){ rTx = LITRED; }
                else
                { rTx = LITGREEN; }
                szTemp = JgBsType(vod->Ord.Direction);
                break;
            case COL_KPP1:
                szTemp=JgOcType(vod->Ord.CombOffsetFlag[0]);
                break;
            case COL_VOLLEFT1:
                szTemp.Format(_T("%d"),vod->Ord.VolumeTotal);
                break;
            case COL_ORDPX1:
                szTemp = FiltPx(vod->Ord.LimitPrice);
                break;
            case COL_ORDTM1:
                ansi2uni(CP_ACP,vod->Ord.InsertTime,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                TrimTime(szTemp);
                break;
            case COL_FRZMGR1:
                szTemp.Format(_T("%.2f"),td->GetFrozenMgr(&(*vod),m_BkrTdPara)); outStrAs4(szTemp);
                break;
            }
            m_xLst[ONROAD_IDX].SetColor(iItem,iSubItem,rTx,rBg);
            lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}
	
	*pResult = 0;
}

void CXTraderDlg::OnfindOnRoad(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[ONROAD_IDX].GetItemCount();
	if(startPos >= iCount){ startPos = 0; }
	
	int curPos=startPos;
	
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,gv_oR[iCount-1-curPos].Ord.InstrumentID,szInst);
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount){ curPos = 0; }
	}while(curPos != startPos);		
}

void CXTraderDlg::OnDispOrdInf(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	CtpTdSpi* td = gc_Td;
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem = pItem->iItem,iSubItem=pItem->iSubItem;
	VRIT_ord vod = td->m_orderVec.rbegin()+iItem;
	double dMgr=0.0,dFee=0.0; 
	rBg = (iItem%2)?LITGRAY:WHITE;
	
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		td->GetFrozMgrFee(&(*vod),m_BkrTdPara,dMgr,dFee);
		switch(iSubItem)
		{
            case COL_SYSID2:
                ansi2uni(CP_ACP,vod->Ord.OrderSysID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                szTemp.TrimLeft();
                break;
            case COL_INST2:
                ansi2uni(CP_ACP,vod->Ord.InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_DIR2: 
                if (vod->Ord.Direction ==THOST_FTDC_D_Buy){ rTx = LITRED; }
                else
                { rTx = LITGREEN; }
                szTemp = JgBsType(vod->Ord.Direction);
                break;
            case COL_KPP2:
                szTemp = JgOcType(vod->Ord.CombOffsetFlag[0]);
                break;
            case COL_ORDSTAT2:
                if (vod->Ord.OrderStatus==THOST_FTDC_OST_AllTraded)
                { rTx = LITGREEN; }
                else if (vod->Ord.OrderStatus<THOST_FTDC_OST_NoTradeQueueing)
                { rTx = MIDBLUE; }
                else if (vod->Ord.OrderStatus<=THOST_FTDC_OST_Canceled)
                { rTx = BG_CANCEL; }
                else if (vod->Ord.OrderStatus==THOST_FTDC_OST_ErrOrd)
                { rTx=RED; }
                else
                { rTx=BLACK; }	

                szTemp=JgOrdStatType(vod->Ord.OrderStatus);
                if (vod->Ord.OrderStatus==THOST_FTDC_OST_Canceled && (vod->Ord.VolumeTraded>=1))
                { szTemp=_T("已撤余单"); }
                break;
            case COL_ORDPX2:
                if (vod->Ord.OrderPriceType == THOST_FTDC_OPT_LimitPrice)
                { szTemp = FiltPx(vod->Ord.LimitPrice); }
                else
                { szTemp = JgAnyOrdType(vod->Ord.OrderPriceType,vod->Ord.TimeCondition); }
                break;
            case COL_VOLALL2:
                szTemp.Format(_T("%d"),vod->Ord.VolumeTotalOriginal);
                break;
            case COL_VOLLEFT2:
                szTemp.Format(_T("%d"),vod->Ord.VolumeTotal);
                break;
            case COL_VOLTD2:
                szTemp.Format(_T("%d"),vod->Ord.VolumeTraded);
                break;
            case COL_AVGPX2:
                if (vod->Ord.VolumeTraded<1){ szTemp = _T("-"); }
                else
                {
                    if (0.001>vod->dAvgPx){ szTemp =_T("0"); }
                    else
                    { szTemp.Format(_T("%.3f"),vod->dAvgPx); }
                }
                break;
            case COL_ORDTM2:
                ansi2uni(CP_ACP,vod->Ord.InsertTime,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                TrimTime(szTemp);
                break;
            case COL_FRZMGR2:
                szTemp.Format(_T("%.2f"),dMgr); outStrAs4(szTemp);
                break;
            case COL_FRZFEE2:
                szTemp.Format(_T("%.2f"),dFee); outStrAs4(szTemp);
                break;
            case COL_STATMSG2:
                szTemp = GetStatMsg(&(*vod));
                break;
		}
		m_xLst[ORDER_IDX].SetColor(iItem,iSubItem,rTx,rBg);
		lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}

	*pResult = 0;
}

void CXTraderDlg::OnfindOrdInf(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[ORDER_IDX].GetItemCount();
	if(startPos >= iCount){ startPos = 0; }
	
	int curPos=startPos;
	
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,gv_Ord[iCount-1-curPos].Ord.InstrumentID,szInst);	
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount) { curPos = 0; }
	}while(curPos != startPos);		
}

void CXTraderDlg::OnDispInvPos(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem= pItem->iItem,iSubItem=pItem->iSubItem;
	CtpTdSpi* td = gc_Td;
	VRIT_pos vps = td->m_InvPosVec.rbegin()+iItem;
	rBg = (iItem%2)?LITGRAY:WHITE;
	
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		double dAvPrice;
		switch(iSubItem)
		{
            case COL_INST3:
                ansi2uni(CP_ACP,vps->InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
        
                break;
            case COL_POSDIR3: 
                if (vps->PosiDirection ==THOST_FTDC_PD_Long){ rTx = LITRED; }
                else
                { rTx = LITGREEN; }
                szTemp = JgBsType(vps->PosiDirection-2);
                break;
            case COL_POS3:
                szTemp.Format(_T("%d"),vps->Position);
                break;
            case COL_VALIDPOS3:
                if (vps->PosiDirection == THOST_FTDC_PD_Long)
                { iLen = vps->Position-vps->ShortFrozen; }
                if (vps->PosiDirection == THOST_FTDC_PD_Short)
                { iLen = vps->Position-vps->LongFrozen; }
                szTemp.Format(_T("%d"),iLen);

                break;
            case COL_AVGPX3:
                dAvPrice = vps->PositionCost/(vps->Position)/(td->FindInstMul(vps->InstrumentID));
                szTemp.Format(_T("%.3f"),dAvPrice);
                break;
            case COL_PROFIT3:
                if (vps->PositionProfit<-NEARZERO){ rTx = LITGREEN; }
                else if (vps->PositionProfit>NEARZERO)
                { rTx = LITRED; }
                else
                { rTx = BLACK; }
                
                szTemp.Format(_T("%.2f"),FiltDb(vps->PositionProfit));
                outStrAs4(szTemp);
                break;
            case COL_USEMGR3:
                szTemp.Format(_T("%.2f"),vps->UseMargin);
                outStrAs4(szTemp);
                
                break;
            case COL_SUMPROFIT3:
                szTemp = UNCOMP;
                break;
		}
		m_xLst[INVPOS_IDX].SetColor(iItem,iSubItem,rTx,rBg);
		lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}
	
	*pResult = 0;
}

void CXTraderDlg::OnfindInvPos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[INVPOS_IDX].GetItemCount();
	if(startPos >= iCount) { startPos = 0; }
	
	int curPos=startPos;
	
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,gv_InvPos[iCount-1-curPos].InstrumentID,szInst);	
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount) { curPos = 0; }
	}while(curPos != startPos);		
}

void CXTraderDlg::OnDispTrade(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem = pItem->iItem,iSubItem=pItem->iSubItem;

	CtpTdSpi* td = gc_Td;
	VRIT_td vtd = td->m_tradeVec.rbegin()+iItem;
	rBg = (iItem%2)?LITGRAY:WHITE;
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		switch(iSubItem)
		{
            case COL_INST4:
                ansi2uni(CP_ACP,vtd->trade.InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_DIR4: 
                if (vtd->trade.Direction ==THOST_FTDC_D_Buy){ rTx = LITRED; }
                else
                { rTx = LITGREEN; }
                szTemp = JgBsType(vtd->trade.Direction);
                break;
            case COL_KPP4:
                szTemp = JgOcType(vtd->trade.OffsetFlag);
                break;
            case COL_TDPX4:
                szTemp = FiltPx(vtd->trade.Price);
                break;
            case COL_TDVOL4:
                szTemp.Format(_T("%d"),vtd->trade.Volume);
                break;
            case COL_TDTM4:
                ansi2uni(CP_ACP,vtd->trade.TradeTime,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                TrimTime(szTemp);
                break;
            case COL_TDFEE4:
                //td->GetTdFee(&(*vtd));
                if (vtd->dFee<NEARZERO){ szTemp = _T("0"); }
                else
                { szTemp.Format(_T("%.3f"),vtd->dFee); }
                break;
            case COL_HEDGE4:
                szTemp=JgTbType(vtd->trade.HedgeFlag);
                break;
            case COL_TDTYPE4:
                szTemp=JgTdType(vtd->trade.TradeType);
                break;
            case COL_EXHID4:
                szTemp=JgExchage(vtd->trade.ExchangeID);
                break;
            case COL_TDID4:
                ansi2uni(CP_ACP,vtd->trade.TradeID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                szTemp.TrimLeft();
                break;
            case COL_SYSID4:
                ansi2uni(CP_ACP,vtd->trade.OrderSysID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                szTemp.TrimLeft();
                break;
		}
		m_xLst[TRADE_IDX].SetColor(iItem,iSubItem,rTx,rBg);
		lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}
	
	*pResult = 0;
}

void CXTraderDlg::OnfindTrade(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[TRADE_IDX].GetItemCount();
	if(startPos >= iCount){ startPos = 0; }
	
	int curPos=startPos;
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,gv_Td[iCount-1-curPos].trade.InstrumentID,szInst);	
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount) { curPos = 0; }
	}while(curPos != startPos);		
}

void CXTraderDlg::OnDispInsts(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem = pItem->iItem,iSubItem=pItem->iSubItem;
    VIT_if vif = gv_inf.begin()+iItem;
    
	rBg = (iItem%2)?LITGRAY:WHITE;
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		switch(iSubItem)
		{
            case COL_PRODID5:
                ansi2uni(CP_ACP,vif->iinf.ProductID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_INST5: 
                ansi2uni(CP_ACP,vif->iinf.InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_INSTNAME5:
                ansi2uni(CP_ACP,vif->iinf.InstrumentName,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_EXHID5:
                szTemp = JgExchage(vif->iinf.ExchangeID);
                break;
            case COL_VOLMUL5:
                szTemp.Format(_T("%d"),vif->iinf.VolumeMultiple);
                break;
            case COL_PXTICK5:
                szTemp = FiltPx(vif->iinf.PriceTick);
                break;
            case COL_PRODCLS5:
                szTemp = JgProType(vif->iinf.ProductClass);
                break;
            case COL_EXPDATE5:
                ansi2uni(CP_ACP,vif->iinf.ExpireDate,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                break;
            case COL_STKPX5:
                szTemp = FiltPx(vif->iinf.StrikePrice);
                break;
            case COL_LONGMGR5:
                szTemp.Format(_T("%g%%"),vif->LongMgrByMoney*100.0);
                break;
            case COL_FEERATE5:
                Fee2String(szTemp,vif->fee.OpenRatioByMoney,vif->fee.OpenRatioByVolume,
                    vif->fee.CloseRatioByMoney,vif->fee.CloseRatioByVolume,
					vif->fee.CloseTodayRatioByMoney,vif->fee.CloseTodayRatioByVolume);
                break;
		}
		m_xLst[INSLST_IDX].SetColor(iItem,iSubItem,rTx,rBg);
		lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}
	
	*pResult = 0;
}

void CXTraderDlg::OnfindInsts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[INSLST_IDX].GetItemCount();
	if(startPos >= iCount) { startPos = 0; }
	
	int curPos=startPos;
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,gv_inf[curPos].iinf.InstrumentID,szInst);
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount) { curPos = 0; }
	}while(curPos != startPos);		
}

COLORREF CXTraderDlg::GetCondColor(TThostFtdcParkedOrderStatusType stat)
{
	COLORREF rTx=BLACK;

	switch (stat)
	{
        case THOST_FTDC_PAOS_NotSend:
            rTx = BLUE; 
            break;
        case THOST_FTDC_PAOS_Send:
		case THOST_FTDC_OST_Touched:
            rTx = LITGREEN;
            break;
        case THOST_FTDC_PAOS_Deleted:
		case THOST_FTDC_OST_NotTouched:
		case THOST_FTDC_OST_Canceled:
            rTx = BG_CANCEL; 
            break;
        case THOST_FTDC_OST_ErrOrd:
            rTx = RED; 
            break;
        default:
            rTx = BLACK;
            break;
	}

	return rTx;
}

CString CXTraderDlg::GetDiySysId(PPARKEDEX vpk)
{
	CString szTemp = _T(""),strId=_T("");
	switch (vpk->ordType)
	{
        case PARKED_ACT:
            ansi2uni(CP_ACP,vpk->pkAct.ParkedOrderActionID,strId.GetBuffer(MAX_PATH));
            strId.ReleaseBuffer();
            strId.TrimLeft();
            szTemp.Format(_T("PKACT_%s"),strId);
            break;
        case COND_SERVER:
        case TOUCH_ZY:
        case TOUCH_ZS:
            if (vpk->park.Status == THOST_FTDC_OST_ErrOrd)
            { 
                ansi2uni(CP_ACP,vpk->park.OrderRef,strId.GetBuffer(MAX_PATH));
                strId.ReleaseBuffer();
                strId.TrimLeft('0');
                szTemp.Format(_T("TJERR_%s"),strId);
            }
            else
            { 
                ansi2uni(CP_ACP,vpk->OrderSysID,szTemp.GetBuffer(MAX_PATH)); 
                szTemp.ReleaseBuffer();
                szTemp.TrimLeft();
            }
            break;
        case PARKED_SERVER:
            ansi2uni(CP_ACP,vpk->park.ParkedOrderID,strId.GetBuffer(MAX_PATH));
            strId.ReleaseBuffer();
            strId.TrimLeft();
            szTemp.Format(_T("PKSVR_%s"),strId);
            break;
        case ENTERTD_LOCAL:
            ansi2uni(CP_ACP,vpk->park.OrderRef,strId.GetBuffer(MAX_PATH));
            strId.ReleaseBuffer();
            strId.TrimLeft('0');
            szTemp.Format(_T("PKLOC_%s"),strId);
            break;
        case COND_LOCAL:
            ansi2uni(CP_ACP,vpk->park.OrderRef,strId.GetBuffer(MAX_PATH));
            strId.ReleaseBuffer();
            strId.TrimLeft('0');
            szTemp.Format(_T("TJLOC_%s"),strId);
            break;
        case TMORD_LOCAL:
            ansi2uni(CP_ACP,vpk->park.OrderRef,strId.GetBuffer(MAX_PATH));
            strId.ReleaseBuffer();
            strId.TrimLeft('0');
            szTemp.Format(_T("TIMER_%s"),strId);
            break;
	}

	return szTemp;
}

void CXTraderDlg::OnDispParked(NMHDR *pNMHDR, LRESULT *pResult)
{
	COLORREF rBg,rTx=BLACK;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItem = pItem->iItem,iSubItem=pItem->iSubItem;
	VRIT_pk vpk = gv_Pk.rbegin()+iItem;

	rBg = (iItem%2)?LITGRAY:WHITE;
	if(pItem->mask & LVIF_TEXT)
	{
		CString szTemp = _T("");
		int iLen=0;
		double dStopPx = 0.0;
		int iVol=0;
		COleDateTime tm;
		TThostFtdcOffsetFlagType cOff;
		TThostFtdcDirectionType cDir;
		TThostFtdcOrderPriceTypeType cPx;
		TThostFtdcHedgeFlagType cHedge;

		if (vpk->ordType==PARKED_ACT)
		{ gc_Td->GetValBySysId(vpk->pkAct.OrderSysID,iVol,cOff,cDir,cPx,cHedge); }

		switch(iSubItem)
		{
            case COL_DIYSYSID6:
                szTemp = GetDiySysId(&(*vpk));
                break;
            case COL_PKTYPE6: 
                szTemp = JgParkedType(vpk->ordType);
                break;
            case COL_PKSTAT6:
                rTx = (vpk->ordType==PARKED_ACT)?GetCondColor(vpk->pkAct.Status):GetCondColor(vpk->park.Status);
                szTemp = (vpk->ordType==PARKED_ACT)?JgParkOrdStat(vpk->pkAct.Status):JgParkOrdStat(vpk->park.Status);
                break;
            case COL_PKCOND6:
                if (vpk->ordType==PARKED_ACT){ szTemp = _T("—"); }
                else if (vpk->ordType==TMORD_LOCAL)
                {
                    ansi2uni(CP_ACP,vpk->park.ErrorMsg,szTemp.GetBuffer(MAX_PATH));
                    szTemp.ReleaseBuffer();
                    tm.ParseDateTime(szTemp);
                    szTemp = tm.Format(_T("%H:%M:%S"));
                    TrimTime(szTemp);
                }
				else if (vpk->ordType==ENTERTD_LOCAL || vpk->ordType==PARKED_SERVER)
				{ szTemp = _T("再次开盘"); }
                else
                {
                    dStopPx = vpk->park.StopPrice;
                    szTemp = JgOrdCondType(vpk->park.ContingentCondition,dStopPx);
                }
                break;
            case COL_INST6:
                ansi2uni(CP_ACP,(vpk->ordType==PARKED_ACT)?vpk->pkAct.InstrumentID:vpk->park.InstrumentID,szTemp.GetBuffer(MAX_PATH));
                szTemp.ReleaseBuffer();
                
                break;
            case COL_DIR6:
                if (vpk->ordType==PARKED_ACT)
                {
                    if (cDir ==THOST_FTDC_D_Buy) { rTx = LITRED; }
                    else
                    { rTx = LITGREEN; }

                    szTemp = JgBsType(cDir);
                }
                else
                {
                    if (vpk->park.Direction ==THOST_FTDC_D_Buy){ rTx = LITRED; }
                    else
                    { rTx = LITGREEN; }

                    szTemp = JgBsType(vpk->park.Direction);
                }
                
                break;
            case COL_KPP6:
                if (vpk->ordType==PARKED_ACT){ szTemp = JgOcType(cOff); }
                else
                { szTemp = JgOcType(vpk->park.CombOffsetFlag[0]); }
                
                break;
            case COL_PKPX6:
                if (vpk->ordType==PARKED_ACT)
                {
                    if (cPx == THOST_FTDC_OPT_AnyPrice){ szTemp = _T("市价"); }
                    else
                    { szTemp = FiltPx(vpk->pkAct.LimitPrice); }	
                }
                else
                {
                    if (vpk->park.OrderPriceType == THOST_FTDC_OPT_AnyPrice) { szTemp = _T("市价"); }
                    else
                    { 
                        if (vpk->park.LimitPrice<-NEARZERO){ szTemp = _T("对价"); }
                        else
                        { szTemp = FiltPx(vpk->park.LimitPrice); }
                    }	
                }

                break;
            case COL_PKVOL6:
                if (vpk->ordType==PARKED_ACT)
                { szTemp.Format(_T("%d/%d"),vpk->pkAct.VolumeChange,iVol); }
                else
                { szTemp.Format(_T("%d"),vpk->park.VolumeTotalOriginal); }
                
                break;
            case COL_HEDGE6:
                if (vpk->ordType==PARKED_ACT){ szTemp = JgTbType(cHedge); }
                else
                { szTemp = JgTbType(vpk->park.CombHedgeFlag[0]); }
                break;
            case COL_EXHID6:
                szTemp = JgExchage((vpk->ordType==PARKED_ACT)?vpk->pkAct.ExchangeID:vpk->park.ExchangeID);
                
                break;
            case COL_PKTM6:
                ansi2uni(CP_ACP,vpk->InsertTime,szTemp.GetBuffer(MAX_PATH));	
                szTemp.ReleaseBuffer();
                TrimTime(szTemp);
                break;
            case COL_STATMSG6:
                ansi2uni(CP_ACP,(vpk->ordType==PARKED_ACT)?vpk->pkAct.ErrorMsg:vpk->park.ErrorMsg,szTemp.GetBuffer(MAX_PATH));	
                szTemp.ReleaseBuffer();
                break;
		}
		m_xLst[PKCON_IDX].SetColor(iItem,iSubItem,rTx,rBg);
		lstrcpy(pItem->pszText,(LPCTSTR)szTemp);
	}
	*pResult = 0;
}

void CXTraderDlg::OnfindParked(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	*pResult = -1;
	
	if((pFindInfo->lvfi.flags & LVFI_STRING) == 0){ return; }
	
	vector<PARKEDEX>& pkv = gv_Pk;
	CString dstStr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;
	int iCount = m_xLst[PKCON_IDX].GetItemCount();
	if(startPos >= iCount) { startPos = 0; }
	
	int curPos=startPos;
	TCHAR szInst[MAX_PATH];
	do
	{	
		ansi2uni(CP_ACP,(pkv[iCount-1-curPos].ordType==PARKED_ACT)?pkv[iCount-1-curPos].pkAct.InstrumentID:pkv[iCount-1-curPos].park.InstrumentID,szInst);
		if( _tcsnicmp(szInst, dstStr, dstStr.GetLength()) == 0)
		{
			*pResult = curPos;
			break;
		}
		
		curPos++;
		if(curPos >= iCount) { curPos = 0; }
	}while(curPos != startPos);		
}

void CXTraderDlg::OnLocalSend()
{
	vector<PARKEDEX>& pkv = gv_Pk;
	if (pkv[m_iActIdx].ordType == COND_LOCAL ||pkv[m_iActIdx].ordType == ENTERTD_LOCAL||
		pkv[m_iActIdx].ordType == TMORD_LOCAL)
	{
		GenOrdFromPk(pkv[m_iActIdx]);
		
		pkv[m_iActIdx].park.Status = THOST_FTDC_PAOS_Send;

		if ((pkv[m_iActIdx].ordType == TMORD_LOCAL) && (pkv[m_iActIdx].hThd !=NULL))
		{
			TermThread(pkv[m_iActIdx].hThd);
			DEL(pkv[m_iActIdx].pTmOrd);
		}
		
		m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(pkv.size())); 
		if(m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX].Invalidate(); }
	}
}

void CXTraderDlg::OnLocalRemv()
{
	vector<PARKEDEX>& pkv = gv_Pk;
	if (pkv[m_iActIdx].ordType == COND_LOCAL || pkv[m_iActIdx].ordType == ENTERTD_LOCAL||
		pkv[m_iActIdx].ordType == TMORD_LOCAL)
	{
		if ((pkv[m_iActIdx].ordType == TMORD_LOCAL) && (pkv[m_iActIdx].hThd !=NULL))
		{
			TermThread(pkv[m_iActIdx].hThd);
			DEL(pkv[m_iActIdx].pTmOrd);
		}

		pkv.erase(pkv.begin()+m_iActIdx);
		
		m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(pkv.size())); 
		if(m_xLst[PKCON_IDX].IsWindowVisible()) { m_xLst[PKCON_IDX].Invalidate(); }
	}
}

void CXTraderDlg::OnPkRemv()
{
	CtpTdSpi* td = gc_Td;
	if(JgPkRmvStat(&td->m_pkExVec[m_iActIdx])){ td->ReqRmParkedOrd(td->m_pkExVec[m_iActIdx].park.ParkedOrderID); }
}

//撤单预埋
void CXTraderDlg::OnPkAct()
{
	CtpTdSpi* td = gc_Td;
	if (m_xLst[ONROAD_IDX].IsWindowVisible())
	{
		if(JgCancelStat(&td->m_onRoadVec[m_iActIdx])) { td->ReqParkedOrdAct2(&td->m_onRoadVec[m_iActIdx]); }
	}
	else if (m_xLst[ORDER_IDX].IsWindowVisible())
	{
		if(JgCancelStat(&td->m_orderVec[m_iActIdx])){ td->ReqParkedOrdAct2(&td->m_orderVec[m_iActIdx]); }
	}
}

//条件预埋单list里的各种撤单
void CXTraderDlg::OnCondAct()
{
	CtpTdSpi* td = gc_Td;
	if(JgCondActStat(&td->m_pkExVec[m_iActIdx]))
	{ td->ReqOrdCancel(td->m_pkExVec[m_iActIdx].park.ExchangeID,td->m_pkExVec[m_iActIdx].OrderSysID); }
}

void CXTraderDlg::OnDelPkAct()
{
	CtpTdSpi* td = gc_Td;
	if(JgPkRmvStat(&td->m_pkExVec[m_iActIdx]))
	{ td->ReqRmParkedOrdAct(td->m_pkExVec[m_iActIdx].pkAct.ParkedOrderActionID); }
}

void CXTraderDlg::OnOrderExt()
{
	DlgOrdEx* Dlg = new DlgOrdEx;
	Dlg->m_szInst = m_szInst;
	Dlg->m_dPrice = m_dOrdPx;
	Dlg->m_iVol = m_iOrdVol;

	Dlg->Create(IDD_DLG_ORDEX,this);
	Dlg->CenterWindow();
	Dlg->ShowWindow(SW_SHOW);
}

void CXTraderDlg::OnCommCfg()
{

}

void CXTraderDlg::OnViewLog()
{
	
}

void CXTraderDlg::SetIdleTm()
{
    CAppCfgs& s = g_s;
	if (m_uIdle == 0 && s.m_bChkIdle) 
	{ m_uIdle = static_cast<UINT>(SetTimer(IDLE_TIMER,s.m_uIdleSec*1000,NULL)); }
}

void CXTraderDlg::KillIdleTm()
{
	if (m_uIdle){ KillTimer(IDLE_TIMER); m_uIdle=NULL; }
}

void CXTraderDlg::OnLockAcc()
{
	ShowWindow(SW_HIDE);

	m_bLocked = true;
	KillIdleTm();

	DlgUnLock* Dlg = new DlgUnLock;
	
	Dlg->Create(IDD_UNLOCK);
	Dlg->CenterWindow();
	Dlg->ShowWindow(SW_SHOW);	
}

LRESULT CXTraderDlg::SaveCfgsMsg(WPARAM wParam,LPARAM lParam)
{
	g_s.ProcConfigs(SAVE);

	return 0;
}
