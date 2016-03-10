#include "stdafx.h"
#include "xTrader.h"
#include "DlgOrdEx.h"

IMPLEMENT_DYNAMIC(DlgOrdEx, CDialog)
DlgOrdEx::DlgOrdEx(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLG_ORDEX, pParent)
{
	m_dPrice = 0.001;
	m_iVol = 1;
	m_bChkMain = TRUE;
	m_bChkMinVol = FALSE;
	m_bAutoClose = FALSE;
	m_iMinVol = 0;
	m_iPxType = 0;
	m_iHedgeType = 0;
	m_bGuiExpand = TRUE;
	m_iPxTick = 1;
    m_dScale = LMT(getMasterScale(),1.0,5.0);
	m_cPxType = THOST_FTDC_OPT_LimitPrice;
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_cVolcType = THOST_FTDC_VC_AV;
	m_cHgType = THOST_FTDC_HF_Speculation;
	m_InstInf = new INSINFEX;
	ZeroMemory(m_InstInf,sizeof(INSINFEX));
}

DlgOrdEx::~DlgOrdEx()
{
	DEL(m_InstInf);
}

void DlgOrdEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CBINST_EX, m_szInst);
	DDX_Text(pDX, IDC_EDPX_EX, m_dPrice);
	DDX_Text(pDX, IDC_EDVOL_EX, m_iVol);
	DDX_Check(pDX, IDC_CHKMAIN_EX, m_bChkMain);
	DDX_Check(pDX, IDC_CHKMINVOL, m_bChkMinVol);
	DDX_Text(pDX, IDC_EDITMINVOL, m_iMinVol);
	DDX_Radio(pDX, IDC_RDNORMAL, m_iPxType);
	DDX_Radio(pDX, IDC_RDSPECU, m_iHedgeType);
	DDV_MaxChars(pDX, m_szInst, 30);
	DDX_Control(pDX, IDC_SPPXEX, m_SpLmtPx);
	DDX_Control(pDX, IDC_SPVOLEX, m_SpVol);
	DDX_Control(pDX, IDC_SPMINVOLEX, m_SpMinVol);
	DDX_Check(pDX, IDC_AUTOCLOSE, m_bAutoClose);
	DDX_Control(pDX, IDC_SPPXTICK, m_SpPxTick);
	DDX_Text(pDX,IDC_NPXTICK,m_iPxTick);
}

BEGIN_MESSAGE_MAP(DlgOrdEx, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTBUYOPEN, OnClkBuyOpen)
	ON_BN_CLICKED(IDC_BTSELLOPEN, OnClkSellOpen)
	ON_BN_CLICKED(IDC_BTBUYCLOSE, OnClkBuyClose)
	ON_BN_CLICKED(IDC_BTSELLCLOSE, OnClkSellClose)
	ON_BN_CLICKED(IDC_BTEXEC, OnClkOpExec)
	ON_BN_CLICKED(IDC_BTABANDON, OnClkOpAbandon)
	ON_BN_CLICKED(IDC_BTCOMB, OnClkCombo)
	ON_BN_CLICKED(IDC_BTUNCOMB, OnClkUnCombo)
	ON_BN_CLICKED(IDC_CHKMINVOL, OnClkChkMinVol)
	ON_EN_CHANGE(IDC_EDITMINVOL, OnEnChangeMinVol)
	ON_EN_CHANGE(IDC_NPXTICK,OnEnChgPxTick)
	ON_BN_CLICKED(IDC_BTQUITEX, OnClkClose)
	ON_EN_UPDATE(IDC_CBINST_EX, OnEditchgInst)
	ON_EN_CHANGE(IDC_EDPX_EX, OnChgLmtPx)
	ON_EN_CHANGE(IDC_EDVOL_EX, OnChgVolEx)
	ON_BN_CLICKED(IDC_RDSPECU, OnClkRdSpecu)
	ON_BN_CLICKED(IDC_RDARBIT, OnClkRdArbit)
	ON_BN_CLICKED(IDC_RDHEDGE, OnClkRdHedge)
	ON_BN_CLICKED(IDC_RDMARKETM, OnClkRdMM)
	ON_BN_CLICKED(IDC_RDNORMAL, OnClkRdLmtNormal)
	ON_BN_CLICKED(IDC_RDFOK, OnClkRdFOK)
	ON_BN_CLICKED(IDC_RDFAK, OnClkRdFAK)
	ON_BN_CLICKED(IDC_RDANYNORMAL, OnClkRdAnyNormal)
	ON_BN_CLICKED(IDC_RDBESTANY, OnClkRdAnyBest)
	ON_BN_CLICKED(IDC_RDL2BEST, OnClkRdL2Best)
	ON_BN_CLICKED(IDC_RDANYLMT, OnClkRdAnyLmt)
	ON_BN_CLICKED(IDC_RDBESTLMT, OnClkRdBestLmt)
	ON_BN_CLICKED(IDC_RDL2LMT, OnClkRdL2Lmt)
	ON_BN_CLICKED(IDC_BTEXPAND, OnClkExpand)
	ON_BN_CLICKED(IDC_AUTOCLOSE, OnClkAutoClose)
END_MESSAGE_MAP()

BOOL DlgOrdEx::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	CreateToolTips();
	return TRUE;
}

void DlgOrdEx::InitCtrls()
{
	GetDlgItem(IDC_CBINST_EX)->SetWindowText(m_szInst);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,m_instId);

	EnableWndFAK(FALSE);

	m_SpLmtPx.SetBuddy(GetDlgItem(IDC_EDPX_EX));
	m_SpLmtPx.SetTrimTrailingZeros(FALSE);
	m_SpVol.SetBuddy(GetDlgItem(IDC_EDVOL_EX));
	m_SpVol.SetDecimalPlaces(0);
	m_SpVol.SetTrimTrailingZeros(TRUE);
	m_SpVol.SetPos(1.0*m_iVol);
	
    m_SpMinVol.SetBuddy(GetDlgItem(IDC_EDITMINVOL));
	m_SpMinVol.SetDecimalPlaces(0);
	m_SpMinVol.SetTrimTrailingZeros(TRUE);
	m_SpMinVol.SetPos(0);

	m_SpPxTick.SetBuddy(GetDlgItem(IDC_NPXTICK));
	m_SpPxTick.SetDecimalPlaces(0);
	m_SpPxTick.SetRangeAndDelta(-1000,1000,1);
	m_SpPxTick.SetTrimTrailingZeros(TRUE);
	m_SpPxTick.SetPos(1);

	double dPxTick=1;
	int iDig = 1;
	BOOL bRet = gc_Td->IsValidInst(m_szInst,m_InstInf);
	if (bRet)
	{
		dPxTick = m_InstInf->iinf.PriceTick;
		iDig = JudgeDigit(dPxTick);
		
		m_SpLmtPx.SetDecimalPlaces (iDig);
		m_SpLmtPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, dPxTick);
		m_SpVol.SetRangeAndDelta(1,m_InstInf->iinf.MaxLimitOrderVolume, 1);
	}

	GetDlgItem(IDC_NPXTICK)->EnableWindow(m_bAutoClose);
	ChkTdCode();
	ChkCfxAny();

	UpdateData(FALSE);
}

BOOL DlgOrdEx::PreTranslateMessage(MSG * pMsg)
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

void DlgOrdEx::CreateToolTips()
{
	if (!m_ToolTip.Create(this,TTS_NOPREFIX|TTS_BALLOON /*|TTS_ALWAYSTIP*/))
		return;
	
	m_ToolTip.SetFont(GetFont());
	
	if (::SendMessage(m_ToolTip.m_hWnd,TTM_SETTITLE,TTI_INFO, (LPARAM)(LPCTSTR)LoadStr(IDS_MY_TIPS)))
	{ AddToolTips(this,&m_ToolTip); }
}

void DlgOrdEx::ChkCfxAny()
{
	BOOL bRet = gc_Td->JgExhByInst(m_instId,_CFX);

	GetDlgItem(IDC_RDL2BEST)->EnableWindow(bRet);
	GetDlgItem(IDC_RDANYLMT)->EnableWindow(bRet);
	GetDlgItem(IDC_RDBESTLMT)->EnableWindow(bRet);
	GetDlgItem(IDC_RDL2LMT)->EnableWindow(bRet);
}

void DlgOrdEx::ChkTdCode()
{
	CtpTdSpi* td = gc_Td;
	vector<CThostFtdcTradingCodeField>::iterator it=td->m_TdCodeVec.begin();

	for (it;it!=td->m_TdCodeVec.end();it++)
	{
		if (it->ClientIDType==THOST_FTDC_CIDT_Speculation)
		{ GetDlgItem(IDC_RDSPECU)->EnableWindow(TRUE); }
		else if (it->ClientIDType==THOST_FTDC_CIDT_Arbitrage)
		{ GetDlgItem(IDC_RDARBIT)->EnableWindow(TRUE); }
		else if (it->ClientIDType==THOST_FTDC_CIDT_Hedge)
		{ GetDlgItem(IDC_RDHEDGE)->EnableWindow(TRUE); }
		else if (it->ClientIDType==THOST_FTDC_CIDT_MarketMaker)
		{ GetDlgItem(IDC_RDMARKETM)->EnableWindow(TRUE); }	
	}
}

void DlgOrdEx::OnDestroy()
{
	CDialog::OnDestroy();
	
	gc_Td->m_bAutoClose = FALSE;
	delete this;
}

void DlgOrdEx::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgOrdEx::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

void DlgOrdEx::SetLastPx()
{
	CThostFtdcDepthMarketDataField LastMd;
	ZeroMemory(&LastMd,sizeof(CThostFtdcDepthMarketDataField));

	gc_Td->GetMdByInst(m_instId,&LastMd);
	m_SpLmtPx.SetPos(LMT(LastMd.LastPrice,m_InstInf->LowLmtPx,m_InstInf->UpLmtPx));
}

void DlgOrdEx::SubMd(CString sInst)
{
	CXTraderApp* pApp = g_App;
	CtpTdSpi* td = pApp->m_cT;
	if (td->IsValidInst(sInst,m_InstInf))
	{
		TThostFtdcInstrumentIDType instId;
		CThostFtdcDepthMarketDataField LastMd;
		ZeroMemory(&LastMd,sizeof(CThostFtdcDepthMarketDataField));
		
		uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)sInst,instId);
		
		double dPriceTick = m_InstInf->iinf.PriceTick;
		int iMaxVol = m_InstInf->iinf.MaxLimitOrderVolume;
		
		td->GetMdByInst(instId,&LastMd);
		
		m_SpVol.SetRangeAndDelta(1, iMaxVol, 1);
		m_SpLmtPx.SetDecimalPlaces(JudgeDigit(dPriceTick));
		m_SpLmtPx.SetRangeAndDelta(m_InstInf->LowLmtPx, m_InstInf->UpLmtPx, m_InstInf->iinf.PriceTick);
		if(!JgAnyPxOrd(m_cPxType,m_cTmcType))
		{ m_SpLmtPx.SetPos(LMT(LastMd.LastPrice,m_InstInf->LowLmtPx,m_InstInf->UpLmtPx)); }
		
		LPSTR* pInst = new LPSTR;
		pInst[0] = instId;
		pApp->m_cQ->SubscribeMarketData(pInst,1);
		
		DEL(pInst);
	}
}

void DlgOrdEx::GenOrder()
{
	ORDTHREADP *pOrdEx = new ORDTHREADP;
	ZeroMemory(pOrdEx,sizeof(ORDTHREADP));

	pOrdEx->pDlg = g_Dlg;
	strcpy(pOrdEx->ordReq.InstrumentID,m_instId);
	
	pOrdEx->ordReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	pOrdEx->ordReq.CombOffsetFlag[0] = m_cOffType;
	pOrdEx->ordReq.CombHedgeFlag[0] = m_cHgType;
	pOrdEx->ordReq.Direction = m_cDir;

	pOrdEx->ordReq.LimitPrice = m_dPrice;
	pOrdEx->ordReq.VolumeTotalOriginal = m_iVol;
	pOrdEx->ordReq.MinVolume = m_iMinVol;
	pOrdEx->ordReq.OrderPriceType = m_cPxType;
	pOrdEx->ordReq.TimeCondition = m_cTmcType;
	pOrdEx->ordReq.VolumeCondition = m_cVolcType;
	
	int iValidVol = g_Dlg->GetValidVol(pOrdEx->ordReq.InstrumentID,pOrdEx->ordReq.Direction,
			pOrdEx->ordReq.CombOffsetFlag[0],m_iVol,pOrdEx->iTdPos,pOrdEx->iYdPos);

	if (iValidVol<1 ){ return; }

	pOrdEx->ordReq.VolumeTotalOriginal = iValidVol;
	AfxBeginThread((AFX_THREADPROC)CXTraderDlg::OrderThread,pOrdEx);
}

void DlgOrdEx::OnClkBuyOpen()
{
	m_cDir = THOST_FTDC_D_Buy;
	m_cOffType = THOST_FTDC_OF_Open;

	GenOrder();
}

void DlgOrdEx::OnClkSellOpen()
{
	m_cDir = THOST_FTDC_D_Sell;
	m_cOffType = THOST_FTDC_OF_Open;

	GenOrder();
}

void DlgOrdEx::OnClkBuyClose()
{
	m_cDir = THOST_FTDC_D_Buy;
	m_cOffType = THOST_FTDC_OF_Close;

	GenOrder();
}

void DlgOrdEx::OnClkSellClose()
{
	m_cDir = THOST_FTDC_D_Sell;
	m_cOffType = THOST_FTDC_OF_Close;
	
	GenOrder();
}

void DlgOrdEx::OnClkOpExec()
{
	gc_Td->ReqExecOrdInsert(m_instId,m_iVol,THOST_FTDC_OF_Close,m_cHgType,
		THOST_FTDC_ACTP_Exec,THOST_FTDC_PD_Net);
}

void DlgOrdEx::OnClkOpAbandon()
{
	gc_Td->ReqExecOrdInsert(m_instId,m_iVol,THOST_FTDC_OF_Close,m_cHgType,
		THOST_FTDC_ACTP_Abandon,THOST_FTDC_PD_Net);
}

void DlgOrdEx::OnClkCombo()
{
	gc_Td->ReqCombActInsert(m_instId,THOST_FTDC_D_Buy,m_iVol,
		THOST_FTDC_CMDR_Comb,m_cHgType);
}

void DlgOrdEx::OnClkUnCombo()
{
	gc_Td->ReqCombActInsert(m_instId,THOST_FTDC_D_Buy,m_iVol,
		THOST_FTDC_CMDR_UnComb,m_cHgType);
}

void DlgOrdEx::OnClkChkMinVol()
{
	m_iPxType = 2;
	m_bChkMinVol = !m_bChkMinVol;
	GetDlgItem(IDC_EDITMINVOL)->EnableWindow(m_bChkMinVol);

	if (!m_bChkMinVol){ m_SpMinVol.SetPos(0); }
	else
	{
		if (m_iMinVol<=1) { m_SpMinVol.SetPos(1); }
	}
	m_cVolcType = m_iMinVol?THOST_FTDC_VC_MV:THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnEnChangeMinVol()
{
	UpdateData();

	if (m_bChkMinVol)
	{
		if (m_iMinVol<=1) { m_SpMinVol.SetPos(1); }
	}
}

void DlgOrdEx::OnEnChgPxTick()
{
	UpdateData();

	gc_Td->m_iPxTick = m_iPxTick;
}

void DlgOrdEx::OnClkClose()
{
	OnOK();
}

void DlgOrdEx::OnEditchgInst()
{
	GetDlgItem(IDC_CBINST_EX)->GetWindowText(m_szInst);
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)m_szInst,m_instId);
	SubMd(m_szInst);

	ChkCfxAny();
}

void DlgOrdEx::OnChgLmtPx()
{
	UpdateData();
}

void DlgOrdEx::OnChgVolEx()
{
	UpdateData();
}

void DlgOrdEx::OnClkRdSpecu()
{
	m_iHedgeType = 0;

	m_cHgType = THOST_FTDC_HF_Speculation;
}

void DlgOrdEx::OnClkRdArbit()
{
	m_iHedgeType = 1;

	m_cHgType = THOST_FTDC_HF_Arbitrage;
}

void DlgOrdEx::OnClkRdHedge()
{
	m_iHedgeType = 2;

	m_cHgType = THOST_FTDC_HF_Hedge;
}

void DlgOrdEx::OnClkRdMM()
{
	m_iHedgeType = 3;

	m_cHgType = THOST_FTDC_HF_MarketMaker;
}

void DlgOrdEx::EnableWndFAK(BOOL bEnable)
{
	GetDlgItem(IDC_CHKMINVOL)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDITMINVOL)->EnableWindow(bEnable && m_bChkMinVol);

	if (!bEnable || !m_bChkMinVol)	{ m_iMinVol = 0; m_SpMinVol.SetPos(0); }
}

void DlgOrdEx::EnableLmtPx(BOOL bEnable)
{
	GetDlgItem(IDC_EDPX_EX)->EnableWindow(bEnable);
	if (!bEnable){ m_dPrice = 0; m_SpLmtPx.SetPos(0); }
}

void DlgOrdEx::OnClkRdLmtNormal()
{
	m_iPxType = 0;
	m_bChkMinVol = FALSE;
	SetLastPx();
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(TRUE);
	EnableWndFAK(FALSE);

	m_cPxType = THOST_FTDC_OPT_LimitPrice;	
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdFOK()
{
	m_iPxType = 1;
	m_bChkMinVol = FALSE;
	SetLastPx();
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(TRUE);
	EnableWndFAK(FALSE);

	m_cPxType = THOST_FTDC_OPT_LimitPrice;	
	m_cTmcType = THOST_FTDC_TC_IOC;
	m_cVolcType = THOST_FTDC_VC_CV;
}

void DlgOrdEx::OnClkRdFAK()
{
	m_iPxType = 2;
	EnableLmtPx(TRUE);
	EnableWndFAK(TRUE);
	SetLastPx();
	
	m_cPxType = THOST_FTDC_OPT_LimitPrice;	
	m_cTmcType = THOST_FTDC_TC_IOC;
	m_cVolcType = m_bChkMinVol?THOST_FTDC_VC_MV:THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdAnyNormal()
{
	m_iPxType = 3;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);

	m_cPxType = THOST_FTDC_OPT_AnyPrice;	
	m_cTmcType = THOST_FTDC_TC_IOC;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdAnyBest()
{
	m_iPxType = 4;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);
	
	m_cPxType = THOST_FTDC_OPT_BestPrice;	
	m_cTmcType = THOST_FTDC_TC_IOC;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdL2Best()
{
	m_iPxType = 5;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);
	
	m_cPxType = THOST_FTDC_OPT_FiveLevelPrice;	
	m_cTmcType = THOST_FTDC_TC_IOC;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdAnyLmt()
{
	m_iPxType = 6;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);
	
	m_cPxType = THOST_FTDC_OPT_AnyPrice;	
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdBestLmt()
{
	m_iPxType = 7;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);
	
	m_cPxType = THOST_FTDC_OPT_BestPrice;	
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkRdL2Lmt()
{
	m_iPxType = 8;
	m_bChkMinVol = FALSE;
	GetDlgItem(IDC_CHKMINVOL)->SendMessage(BM_SETCHECK);
	EnableLmtPx(FALSE);
	EnableWndFAK(FALSE);
	
	m_cPxType = THOST_FTDC_OPT_FiveLevelPrice;	
	m_cTmcType = THOST_FTDC_TC_GFD;
	m_cVolcType = THOST_FTDC_VC_AV;
}

void DlgOrdEx::OnClkExpand()
{
	m_bGuiExpand = !m_bGuiExpand;
	if (m_bGuiExpand)
	{
		GetDlgItem(IDC_BTEXPAND)->SetWindowText(_T("<<"));
		RestoreGui();
	}
	else
	{
		GetDlgItem(IDC_BTEXPAND)->SetWindowText(_T(">>"));
		MiniGui();
	}
}

void DlgOrdEx::ShowHideWnds(int nCmdShow)
{
	CWnd* pChild = NULL;
	CRect rc0,rcBO,rcChild,rcQt;
	GetDlgItem(IDC_BTQUITEX)->GetWindowRect(&rcQt);
	GetDlgItem(IDC_BTBUYOPEN)->GetWindowRect(rcBO);

	LONG xDelta = ((nCmdShow==SW_SHOW)?1:-1)*static_cast<LONG>(10*m_dScale);
	LONG yDelta = ((nCmdShow==SW_SHOW)?1:-1)*static_cast<LONG>(210*m_dScale);
	for (pChild = GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		pChild->GetWindowRect(rcChild);
		if (pChild==GetDlgItem(IDC_BTQUITEX))
		{
			rcQt.left += xDelta;
			rcQt.right += xDelta;
			rcQt.top += yDelta;
			rcQt.bottom += yDelta;
			ScreenToClient(&rcQt);
			pChild->MoveWindow(rcQt);
		}
		else if (pChild==GetDlgItem(IDC_BTCOVOPEN) || rcChild.top>rcBO.bottom)
		{ pChild->ShowWindow(nCmdShow); }
	}

	GetWindowRect(&rc0);
	rc0.bottom += yDelta;
	MoveWindow(rc0);
}

void DlgOrdEx::MiniGui()
{
	ShowHideWnds(SW_HIDE);
}

void DlgOrdEx::RestoreGui()
{
	ShowHideWnds(SW_SHOW);
}

void DlgOrdEx::OnClkAutoClose()
{
	m_bAutoClose = !m_bAutoClose;

	gc_Td->m_bAutoClose = m_bAutoClose;
	if (!m_bAutoClose){ m_SpPxTick.SetPos(0); }

	GetDlgItem(IDC_NPXTICK)->EnableWindow(m_bAutoClose);
}
