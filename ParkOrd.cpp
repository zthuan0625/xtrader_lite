#include "stdafx.h"
#include "xtrader.h"
#include "ParkOrd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(ParkOrdDlg, CDialog)
ParkOrdDlg::ParkOrdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ParkOrdDlg::IDD, pParent)
{
	m_szStat = _T("");
	m_dStopPx = 1000;
	m_iConds = PARKED_SERVER;
	m_iLmt = LMT_PX;
	m_cPxType = THOST_FTDC_OPT_LimitPrice;
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_iSelPx = 1;
	m_iSelCond = 1;
	m_bChkMain = TRUE;
	m_dLmtPx = 0;
	m_iVol = 0;
	m_InstInf = new INSINFEX;
	//{{AFX_DATA_INIT(ParkOrdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

ParkOrdDlg::~ParkOrdDlg()
{
	DEL(m_InstInf);
}

void ParkOrdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ParkOrdDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_PARK_LOCAL,m_iConds);
	DDX_Radio(pDX, IDC_RD_DSJ,m_iLmt);
	DDX_Control(pDX, IDC_CBBASEPX, m_CombBasePx);
	DDX_Control(pDX, IDC_CBCONDS, m_CombConds);
	DDX_Text(pDX, IDC_CBPKINST, m_szInst);
	DDV_MaxChars(pDX, m_szInst, 30);
	DDX_Text(pDX, IDC_STOPPX, m_dStopPx);
	DDX_Text(pDX, IDC_EDVOLPK, m_iVol);
	DDX_Text(pDX, IDC_EDPXPK, m_dLmtPx);
	DDV_MinMaxDouble(pDX, m_dStopPx, 0.0001, 10000000);
	DDX_Control(pDX, IDC_SPSTOPPX, m_SpStopPx);
	DDX_Control(pDX, IDC_SPPXPK, m_SpLmtPx);
	DDX_Control(pDX, IDC_SPVOLPK, m_SpVolPk);
	DDX_Control(pDX, IDC_CBBSPK, m_CbBs);
	DDX_Control(pDX, IDC_CBOCPK, m_CbOc);
	DDX_Check(pDX,IDC_CHK_MAIN,m_bChkMain);
	DDX_Control(pDX,IDC_TMEDIT,m_tmEdit);
}

BEGIN_MESSAGE_MAP(ParkOrdDlg, CDialog)
	ON_WM_DESTROY()
	//{{AFX_MSG_MAP(ParkOrdDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_STOPPX, OnChgStopPx)
	ON_EN_CHANGE(IDC_EDPXPK, OnChgLmtPx)
	ON_EN_CHANGE(IDC_EDVOLPK, OnChgVol)
	ON_EN_CHANGE(IDC_TMEDIT, OnChgTime)
	ON_CBN_SELCHANGE(IDC_CBBASEPX, OnSelPx)
	ON_CBN_SELCHANGE(IDC_CBCONDS, OnSelCond)
	ON_CBN_SELCHANGE(IDC_CBBSPK, OnSelBs)
	ON_CBN_SELCHANGE(IDC_CBOCPK, OnSelOc)
	ON_EN_UPDATE(IDC_CBPKINST, OnEditchgInst)
	ON_BN_CLICKED(IDC_PARK_TMORD,OnClkTmOrd)
	ON_BN_CLICKED(IDC_PARK_LOCAL, OnClkParkLocal)
	ON_BN_CLICKED(IDC_PARK_REMT, OnClkParkServer)
	ON_BN_CLICKED(IDC_CONDLOCAL, OnClkCondLocal)
	ON_BN_CLICKED(IDC_CONDORD, OnClkCondServer)
	ON_BN_CLICKED(IDC_TOUCH, OnClkTouch)
	ON_BN_CLICKED(IDC_TOUCHPROF, OnClkProfit)
	ON_BN_CLICKED(IDC_RD_DSJ, OnClkDsj)
	ON_BN_CLICKED(IDC_RDLMTPX, OnClkLmtPx)
	ON_BN_CLICKED(IDC_RDPKANY, OnClkAnyPx)
END_MESSAGE_MAP()

BOOL ParkOrdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitCtrls();

	CreateToolTips();
	UpdateData(FALSE);
	return TRUE;
}

void ParkOrdDlg::CreateToolTips()
{
	if (!m_ToolTip.Create(this,TTS_NOPREFIX|TTS_BALLOON /*|TTS_ALWAYSTIP*/))
		return;
	
	m_ToolTip.SetFont(GetFont());
	
	if (::SendMessage(m_ToolTip.m_hWnd,TTM_SETTITLE,TTI_INFO, (LPARAM)(LPCTSTR)LoadStr(IDS_MY_TIPS)))
	{ AddToolTips(this,&m_ToolTip); }
}

BOOL ParkOrdDlg::PreTranslateMessage(MSG * pMsg)
{
	if (IsWindow(m_ToolTip))
	{
		if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) 
		{
			MSG msg;
			memcpy(&msg, pMsg, sizeof(MSG));
			
			for (HWND hWndParent = ::GetParent(msg.hwnd);
			hWndParent && hWndParent != m_hWnd;
			hWndParent = ::GetParent(hWndParent)) 
			{ msg.hwnd = hWndParent; }
			
			if (msg.hwnd) { m_ToolTip.RelayEvent(&msg); }
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void ParkOrdDlg::InitCtrls()
{
	GetDlgItem(IDC_CBPKINST)->SetWindowText(m_szInst);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,m_instId);
	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);

	GetDlgItem(IDC_RDPKANY)->EnableWindow(FALSE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(FALSE);

	ChkDceAny();

	int i=0;
	
	LPCTSTR strBasePx[3] = {_T("卖一价"),_T("最新价"),_T("买一价")};
	LPCTSTR strConds[4] = {_T("＞"),_T("≥"),_T("＜"),_T("≤")};
	
	for (i=0;i<3;i++){ m_CombBasePx.AddString(strBasePx[i]); }
	m_CombBasePx.SetCurSel(m_iSelPx);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBBASEPX,CBN_SELCHANGE),(LPARAM)m_CombBasePx.GetSafeHwnd());
	
	for (i=0;i<4;i++){ m_CombConds.AddString(strConds[i]); }
	m_CombConds.SetCurSel(m_iSelCond);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBCONDS,CBN_SELCHANGE),(LPARAM)m_CombConds.GetSafeHwnd());

	LPCTSTR strBS[2] ={DIR_BUY,DIR_SELL};
	LPCTSTR strOC[2] ={ORD_O,ORD_C/*,ORD_CT*/};
	for (i=0;i<2;i++){ m_CbBs.AddString(strBS[i]); }
	m_CbBs.SetCurSel(0);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBBSPK,CBN_SELCHANGE),(LPARAM)m_CbBs.GetSafeHwnd());
	
	for (i=0;i<2;i++){ m_CbOc.AddString(strOC[i]); }
	m_CbOc.SetCurSel(0);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBOCPK,CBN_SELCHANGE),(LPARAM)m_CbOc.GetSafeHwnd());

	m_SpStopPx.SetBuddy(GetDlgItem(IDC_STOPPX));
	m_SpStopPx.SetTrimTrailingZeros (FALSE);

	m_SpLmtPx.SetBuddy(GetDlgItem(IDC_EDPXPK));
	m_SpLmtPx.SetTrimTrailingZeros (FALSE);
	m_SpVolPk.SetBuddy(GetDlgItem(IDC_EDVOLPK));
	m_SpVolPk.SetDecimalPlaces(0);
	m_SpVolPk.SetTrimTrailingZeros (TRUE);
	m_SpVolPk.SetPos(1.0*m_iVol);

	double dPxTick=1;
	int iDig = 1;
	BOOL bRet = gc_Td->IsValidInst(m_szInst,m_InstInf);
	if (bRet)
	{
		dPxTick = m_InstInf->iinf.PriceTick;
		iDig = JudgeDigit(dPxTick);
		m_SpStopPx.SetDecimalPlaces (iDig);
		m_SpStopPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, dPxTick);

		m_SpLmtPx.SetDecimalPlaces (iDig);
		m_SpLmtPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, dPxTick);
		m_SpVolPk.SetRangeAndDelta(1,m_InstInf->iinf.MaxLimitOrderVolume, 1);
	}

	m_conType = CondTyTable[4*m_iSelPx+m_iSelCond];
	
	if (m_iConds==COND_LOCAL || m_iConds==COND_SERVER )
	{
		m_CombBasePx.EnableWindow(TRUE);
		m_CombConds.EnableWindow(TRUE);
	}
	else
	{
		m_CombBasePx.EnableWindow(FALSE);
		m_CombConds.EnableWindow(FALSE);
	}

	if (m_iConds==ENTERTD_LOCAL||m_iConds==TMORD_LOCAL || m_iConds==PARKED_SERVER )
	{
		GetDlgItem(IDC_STOPPX)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STOPPX)->EnableWindow(TRUE);
	}

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::ChkDceAny()
{
	BOOL bRet = gc_Td->JgExhByInst(m_instId,_DCE);
	
	GetDlgItem(IDC_TOUCH)->EnableWindow(bRet);
	GetDlgItem(IDC_TOUCHPROF)->EnableWindow(bRet);
}

void ParkOrdDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}

void ParkOrdDlg::OnOK()
{
	GenOrd();
}

void ParkOrdDlg::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

CString ParkOrdDlg::FmtOrdPx(int iType)
{
	CString szStr;
	if (iType==LAST_DSJ)
	{ szStr = _T("对价"); }
	else if(iType==ANY_PX)
	{ szStr = _T("市价"); }
	else
	{ szStr.Format(_T("%g"),m_dLmtPx); }

	return szStr;	
}

CString ParkOrdDlg::GetStatTxt()
{
	CXTraderDlg* pDlg = g_Dlg;
	CString strMsg = _T("");

	COleDateTime dt;
	m_tmEdit.GetDateTime(dt);
	CString str = dt.Format(_T("%H:%M:%S"));
	switch (m_iConds)
	{
	case TMORD_LOCAL:
		strMsg.Format(_T("%s:%s,%s,%s,%d手,定时单于%s发出."),m_szInst,
		JgBsType(m_cDir),JgOcType(m_cKp),FmtOrdPx(m_iLmt),m_iVol,str);
		break;
	case ENTERTD_LOCAL:
		strMsg.Format(_T("%s:%s,%s,%s,%d手,%s"),m_szInst,
		JgBsType(m_cDir),JgOcType(m_cKp),FmtOrdPx(m_iLmt),m_iVol,JgParkedType(m_iConds));
		break;
	case PARKED_SERVER:
		strMsg.Format(_T("%s:%s,%s,%g,%d手,%s单."),m_szInst,
			JgBsType(m_cDir),JgOcType(m_cKp),m_dLmtPx,m_iVol,JgParkedType(m_iConds));
		break;
	case COND_LOCAL:
		strMsg.Format(_T("%s:%s,%s,%s,%d手,%s触发"),m_szInst,
			JgBsType(m_cDir),JgOcType(m_cKp),FmtOrdPx(m_iLmt),m_iVol,
			JgOrdCondType(CondTyTable[4*m_iSelPx+m_iSelCond],m_dStopPx));
		break;
	case COND_SERVER:
		strMsg.Format(_T("%s:%s,%s,%g,%d手,%s触发"),m_szInst,
			JgBsType(m_cDir),JgOcType(m_cKp),m_dLmtPx,m_iVol,
			JgOrdCondType(CondTyTable[4*m_iSelPx+m_iSelCond],m_dStopPx));
		break;
	case TOUCH_ZS:
			strMsg.Format(_T("%s:%s,%s,%s,%d手,%s"),m_szInst,
				JgBsType(m_cDir),JgOcType(m_cKp),FmtOrdPx(m_iLmt),m_iVol,
			JgOrdCondType(THOST_FTDC_CC_Touch,m_dStopPx));

		break;
	case TOUCH_ZY:
			strMsg.Format(_T("%s:%s,%s,%s,%d手,%s"),m_szInst,
				JgBsType(m_cDir),JgOcType(m_cKp),FmtOrdPx(m_iLmt),m_iVol,
				JgOrdCondType(THOST_FTDC_CC_TouchProfit,m_dStopPx));
		break;
	}

	return strMsg;
}

void ParkOrdDlg::OnChgStopPx()
{
	VerifyEdit(this,IDC_STOPPX,true);

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnChgLmtPx()
{
	VerifyEdit(this,IDC_EDPXPK,true);

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnChgVol()
{
	VerifyEdit(this,IDC_EDVOLPK,false);

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnChgTime()
{
	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnSelPx()
{
	m_iSelPx = m_CombBasePx.GetCurSel();

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnSelCond()
{
	m_iSelCond = m_CombConds.GetCurSel();

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::SetLastPx()
{
	CThostFtdcDepthMarketDataField LastMd;
	ZeroMemory(&LastMd,sizeof(CThostFtdcDepthMarketDataField));
	
	gc_Td->GetMdByInst(m_instId,&LastMd);
	m_SpLmtPx.SetPos(LMT(LastMd.LastPrice,m_InstInf->LowLmtPx,m_InstInf->UpLmtPx));
}

void ParkOrdDlg::SubMd(CString sInst)
{
	CXTraderApp* pApp = g_App;
	if (pApp->m_cT->IsValidInst(sInst,m_InstInf))
	{
		TThostFtdcInstrumentIDType instId;
		CThostFtdcDepthMarketDataField LastMd;
		ZeroMemory(&LastMd,sizeof(CThostFtdcDepthMarketDataField));

		strcpy(instId,m_InstInf->iinf.InstrumentID);

		double dPriceTick = m_InstInf->iinf.PriceTick;
		int iMaxVol = m_InstInf->iinf.MaxLimitOrderVolume;

		pApp->m_cT->GetMdByInst(instId,&LastMd);

		m_SpVolPk.SetRangeAndDelta(1, iMaxVol, 1);
		m_SpLmtPx.SetDecimalPlaces(JudgeDigit(dPriceTick));
		m_SpLmtPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, m_InstInf->iinf.PriceTick);
		m_SpLmtPx.SetPos(LMT(LastMd.LastPrice,m_InstInf->LowLmtPx,m_InstInf->UpLmtPx));
		m_SpStopPx.SetDecimalPlaces(JudgeDigit(dPriceTick));
		m_SpStopPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, m_InstInf->iinf.PriceTick);
		if(!JgAnyPxOrd(m_cPxType,m_cTmcType))
		{ m_SpStopPx.SetPos(LMT(LastMd.LastPrice,m_InstInf->LowLmtPx,m_InstInf->UpLmtPx)); }
		
		LPSTR* pInst = new LPSTR;
		pInst[0] = instId;
		pApp->m_cQ->SubscribeMarketData(pInst,1);

		DEL(pInst);
	}
}

void ParkOrdDlg::OnEditchgInst()
{ 
	GetDlgItem(IDC_CBPKINST)->GetWindowText(m_szInst);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,m_instId);

	ChkDceAny();

	SubMd(m_szInst);

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnSelBs()
{
	int iDir= m_CbBs.GetCurSel();
	m_cDir = (iDir==0)?THOST_FTDC_D_Buy:THOST_FTDC_D_Sell; 

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnSelOc()
{
	int iOC = m_CbOc.GetCurSel();
	m_cKp = (iOC==0)?THOST_FTDC_OF_Open:THOST_FTDC_OF_Close; 

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkDsj()
{
	m_iLmt = LAST_DSJ;

	if (m_iConds==COND_LOCAL || m_iConds==ENTERTD_LOCAL || 
		m_iConds==TMORD_LOCAL)
	{ 
		m_dLmtPx = LAST_DJ_ALIAS; 
		GetDlgItem(IDC_EDPXPK)->EnableWindow(FALSE);
	}
	else
	{ GetDlgItem(IDC_EDPXPK)->EnableWindow(TRUE); }

	m_cPxType = THOST_FTDC_OPT_LimitPrice;
	m_cTmcType = THOST_FTDC_TC_GFD;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkLmtPx()
{
	m_iLmt = LMT_PX;
	SetLastPx();

	m_cPxType = THOST_FTDC_OPT_LimitPrice;
	m_cTmcType = THOST_FTDC_TC_GFD;

	GetDlgItem(IDC_EDPXPK)->EnableWindow(TRUE);
	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkAnyPx()
{
	m_iLmt = ANY_PX;
    m_SpLmtPx.SetPos(0); 

	GetDlgItem(IDC_EDPXPK)->EnableWindow(FALSE);

    m_cPxType = THOST_FTDC_OPT_AnyPrice;
	m_cTmcType = THOST_FTDC_TC_IOC;
	
	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkParkLocal()
{
	m_iConds = ENTERTD_LOCAL;
	GetDlgItem(IDC_RDPKANY)->EnableWindow(TRUE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(TRUE);
	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);
	
	m_CombBasePx.EnableWindow(FALSE);
	m_CombConds.EnableWindow(FALSE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(FALSE);

	//本地预埋其实就是普通立即单
	m_conType = THOST_FTDC_CC_Immediately;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkTmOrd()
{
	m_iConds = TMORD_LOCAL;
	GetDlgItem(IDC_RDPKANY)->EnableWindow(TRUE);
	GetDlgItem(IDC_TMEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(TRUE);
	
	m_CombBasePx.EnableWindow(FALSE);
	m_CombConds.EnableWindow(FALSE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(FALSE);
	
	//本地预埋其实就是普通立即单
	m_conType = THOST_FTDC_CC_Immediately;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkParkServer()
{
	m_iConds = PARKED_SERVER;
	m_iLmt = LMT_PX;
	GetDlgItem(IDC_RDLMTPX)->SendMessage(BM_CLICK,1);
	SetLastPx();

	GetDlgItem(IDC_EDPXPK)->EnableWindow(TRUE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDPKANY)->EnableWindow(FALSE);
	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);
	m_CombBasePx.EnableWindow(FALSE);
	m_CombConds.EnableWindow(FALSE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(FALSE);

	m_conType = THOST_FTDC_CC_ParkedOrder;
	m_cPxType = THOST_FTDC_OPT_LimitPrice;
	m_cTmcType = THOST_FTDC_TC_GFD;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkCondLocal()
{
	m_iConds = COND_LOCAL;

	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(TRUE);
	GetDlgItem(IDC_RDPKANY)->EnableWindow(TRUE);
	m_CombBasePx.EnableWindow(TRUE);
	m_CombConds.EnableWindow(TRUE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(TRUE);

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkCondServer()
{
	m_iConds = COND_SERVER;
	m_iLmt = LMT_PX;
	GetDlgItem(IDC_RDLMTPX)->SendMessage(BM_CLICK,1);
	SetLastPx();

	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDPKANY)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(TRUE);
	m_CombBasePx.EnableWindow(TRUE);
	m_CombConds.EnableWindow(TRUE);

	m_cPxType = THOST_FTDC_OPT_LimitPrice;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkTouch()
{
	m_iConds = TOUCH_ZS;
	if (m_iLmt==LAST_DSJ)
	{ 
		m_iLmt = LMT_PX;
		GetDlgItem(IDC_RDLMTPX)->SendMessage(BM_SETCHECK,1);
		SetLastPx();
	}
	m_CombBasePx.SetCurSel(1);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBBASEPX,CBN_SELCHANGE),(LPARAM)m_CombBasePx.GetSafeHwnd());
	m_CombConds.SetCurSel(2);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBCONDS,CBN_SELCHANGE),(LPARAM)m_CombConds.GetSafeHwnd());
	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);

	GetDlgItem(IDC_RD_DSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDPKANY)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(TRUE);
	m_CombBasePx.EnableWindow(FALSE);
	m_CombConds.EnableWindow(FALSE);
	m_conType = THOST_FTDC_CC_Touch;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

void ParkOrdDlg::OnClkProfit()
{
	m_iConds = TOUCH_ZY;
	if (m_iLmt==LAST_DSJ)
	{ 
		m_iLmt = LMT_PX;
		GetDlgItem(IDC_RDLMTPX)->SendMessage(BM_SETCHECK,1);
		SetLastPx();
	}
	m_CombBasePx.SetCurSel(1);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBBASEPX,CBN_SELCHANGE),(LPARAM)m_CombBasePx.GetSafeHwnd());
	m_CombConds.SetCurSel(0);
	SendMessage(WM_COMMAND,MAKEWPARAM(IDC_CBCONDS,CBN_SELCHANGE),(LPARAM)m_CombConds.GetSafeHwnd());
	GetDlgItem(IDC_TMEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RD_DSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDPKANY)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOPPX)->EnableWindow(TRUE);
	m_CombBasePx.EnableWindow(FALSE);
	m_CombConds.EnableWindow(FALSE);
	m_conType = THOST_FTDC_CC_TouchProfit;

	m_szStat = GetStatTxt();
	GetDlgItem(IDC_PARKORDSTAT)->SetWindowText(m_szStat);
}

//本地条件单和预埋单都通过OrderRef来定位
void ParkOrdDlg::GenPkLocal()
{
	CXTraderDlg* pDlg = g_Dlg;
	CtpTdSpi* td = gc_Td;

	PARKEDEX pkReq;
	ZeroMemory(&pkReq,sizeof(pkReq));

	strcpy(pkReq.park.InstrumentID, m_instId);
	
	//这里需要在发送时从spi里自增
	sprintf(pkReq.park.OrderRef, "%012d", ++pDlg->m_iRef_pk);
	
	pkReq.park.Direction = m_cDir; 
	pkReq.park.CombOffsetFlag[0] = m_cKp; 

	pkReq.park.LimitPrice = (m_iLmt== LAST_DSJ)?LAST_DJ_ALIAS:m_dLmtPx;	//价格
	pkReq.park.VolumeTotalOriginal = m_iVol;	///数量	
	
	pkReq.park.OrderPriceType = m_cPxType;	
	pkReq.park.TimeCondition = m_cTmcType; 
	pkReq.park.VolumeCondition = THOST_FTDC_VC_AV;
	pkReq.park.ContingentCondition = m_conType; 
	pkReq.park.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	
	td->GetExhByInst(m_instId,pkReq.park.ExchangeID);

	pkReq.park.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pkReq.ordType = ENTERTD_LOCAL;
	pkReq.park.Status = THOST_FTDC_PAOS_NotSend;

	getCurDate(pkReq.InsertDate);
	getCurTime(pkReq.InsertTime);

	td->m_pkExVec.push_back(pkReq);
	pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(gv_Pk.size()));
	pDlg->m_xLst[PKCON_IDX].Invalidate();
}

//远程预埋单用parkid来定位
void ParkOrdDlg::GenPkServer()
{
	gc_Td->ReqParkedOrd(m_instId,m_cDir,m_cKp,m_dLmtPx,m_iVol);
}

//本地条件单和预埋单都通过OrderRef来定位
void ParkOrdDlg::GenCondLocal()
{
	CXTraderDlg* pDlg = g_Dlg;
	CtpTdSpi* td = gc_Td;

	m_conType = CondTyTable[4*m_iSelPx+m_iSelCond];

	PARKEDEX pkReq;
	ZeroMemory(&pkReq,sizeof(pkReq));

	strcpy(pkReq.park.InstrumentID, m_instId);
	
	//这里需要在发送时从spi里自增
	sprintf(pkReq.park.OrderRef, "%012d", ++pDlg->m_iRef_tj);
	
	pkReq.park.Direction = m_cDir; 
	pkReq.park.CombOffsetFlag[0] = m_cKp; 
	
	pkReq.park.LimitPrice = (m_iLmt== LAST_DSJ)?LAST_DJ_ALIAS:m_dLmtPx;	//价格
	pkReq.park.VolumeTotalOriginal = m_iVol;	///数量	
	
	pkReq.park.OrderPriceType = m_cPxType;	
	pkReq.park.TimeCondition = m_cTmcType; 
	pkReq.park.VolumeCondition = THOST_FTDC_VC_AV;
	pkReq.park.ContingentCondition = m_conType; 
	pkReq.park.StopPrice = m_dStopPx;
	pkReq.park.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	
	td->GetExhByInst(m_instId,pkReq.park.ExchangeID);

	pkReq.park.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pkReq.ordType = COND_LOCAL;
	pkReq.park.Status = THOST_FTDC_OST_NotTouched;
	
	getCurDate(pkReq.InsertDate);
	getCurTime(pkReq.InsertTime);
	
	td->m_pkExVec.push_back(pkReq);
	pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(gv_Pk.size()));
	pDlg->m_xLst[PKCON_IDX].Invalidate();
}

//条件单和止损止盈可用sysid或localid来唯一定位?
void ParkOrdDlg::GenCondServer()
{
	m_conType = CondTyTable[4*m_iSelPx+m_iSelCond];

	gc_Td->ReqOrdCond(m_instId,m_cDir,m_cKp,m_dLmtPx,m_iVol,m_dStopPx,m_conType);
}

void ParkOrdDlg::GenTouch()
{
	gc_Td->ReqOrdTouch(m_instId,m_cDir,m_cKp,m_dLmtPx,m_iVol,m_dStopPx,m_conType);
}

void ParkOrdDlg::GenTouchProf()
{
	gc_Td->ReqOrdTouch(m_instId,m_cDir,m_cKp,m_dLmtPx,m_iVol,m_dStopPx,m_conType);
}

void ParkOrdDlg::GenTmOrder()
{
	CXTraderDlg* pDlg = g_Dlg;
	CtpTdSpi* td = gc_Td;
	COleDateTime tm;
	m_tmEdit.GetDateTime(tm);
	
	if (COleDateTime::GetCurrentTime()>tm)
	{
		ShowErroTips(LoadStr(IDS_TMLESSNOW),LoadStr(IDS_MY_TIPS));
		return;
	}

	PTMORDPARAM m_ptmPara = new TMORDPARAM;
	ZeroMemory(m_ptmPara,sizeof(TMORDPARAM));
	m_ptmPara->dt.nYear = tm.GetYear();
	m_ptmPara->dt.nMonth = tm.GetMonth();
	m_ptmPara->dt.nDay = tm.GetDay();
	m_ptmPara->dt.nHour = tm.GetHour();
	m_ptmPara->dt.nMin = tm.GetMinute();
	m_ptmPara->dt.nSec = tm.GetSecond();

	m_ptmPara->pDlg = pDlg;
	
	PARKEDEX pkReq;
	ZeroMemory(&pkReq,sizeof(pkReq));
	//存tmordpara结构体的指针
	pkReq.pTmOrd = m_ptmPara;
	
	strcpy(pkReq.park.InstrumentID, m_instId);
	
	//这里需要在发送时从spi里自增
	sprintf(pkReq.park.OrderRef, "%012d", ++pDlg->m_iRef_tm);
	
	pkReq.park.Direction = m_cDir; 
	pkReq.park.CombOffsetFlag[0] = m_cKp; 
	
	pkReq.park.LimitPrice = (m_iLmt== LAST_DSJ)?LAST_DJ_ALIAS:m_dLmtPx;	//价格
	pkReq.park.VolumeTotalOriginal = m_iVol;	///数量	
	
	pkReq.park.OrderPriceType = m_cPxType;	
	pkReq.park.TimeCondition = m_cTmcType; 
	pkReq.park.VolumeCondition = THOST_FTDC_VC_AV;
	pkReq.park.ContingentCondition = THOST_FTDC_CC_Immediately; 
	pkReq.park.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	
	td->GetExhByInst(m_instId,pkReq.park.ExchangeID);
	
	pkReq.park.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pkReq.ordType = TMORD_LOCAL;
	pkReq.park.Status = THOST_FTDC_OST_NotTouched;

	sprintf(pkReq.park.ErrorMsg,"%04d-%02d-%02d %d:%02d:%02d",tm.GetYear(),tm.GetMonth(),
		tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
	getCurDate(pkReq.InsertDate);
	getCurTime(pkReq.InsertTime);
	
	memcpy(&m_ptmPara->pkReq,&pkReq,sizeof(PARKEDEX));
	
	HANDLE hTmOrd = ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)CXTraderDlg::SendTmOrd, m_ptmPara,CREATE_SUSPENDED,NULL);
	if (hTmOrd != NULL)
	{
		pkReq.hThd = hTmOrd;

		td->m_pkExVec.push_back(pkReq);
		pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(gv_Pk.size()));
		pDlg->m_xLst[PKCON_IDX].Invalidate();

		::ResumeThread(hTmOrd);
	}
}

void ParkOrdDlg::GenOrd()
{
	UpdateData(TRUE);
	switch (m_iConds)
	{
		case ENTERTD_LOCAL:
			GenPkLocal(); 
			break;
		case TMORD_LOCAL:
			GenTmOrder();
			break;
		case PARKED_SERVER:
			GenPkServer();
			break;
		case COND_LOCAL:
			GenCondLocal();
			break;
		case COND_SERVER:
			GenCondServer();
			break;
		case TOUCH_ZS:
			GenTouch();
			break;
		case TOUCH_ZY:
			GenTouchProf();
			break;
	}
	if (!g_xLst[PKCON_IDX].IsWindowVisible()){ g_Dlg->SwitchTab(PKCON_IDX); }
}
