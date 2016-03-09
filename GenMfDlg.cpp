#include "stdafx.h"
#include "xTrader.h"
#include "GenMfDlg.h"

extern HANDLE g_hEvent;

IMPLEMENT_DYNAMIC(GenMfDlg, CDialog)

GenMfDlg::GenMfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GenMfDlg::IDD, pParent)
{
	m_szGenstat =_T("");
	m_pQryFee = NULL;
	m_pQryOrdfr = NULL;
	m_pQryMr = NULL;
}

GenMfDlg::~GenMfDlg()
{
}

void GenMfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GENSTAT, m_szGenstat);
}


BEGIN_MESSAGE_MAP(GenMfDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_GENMR, OnClkBtGenmr)
	ON_BN_CLICKED(IDC_BT_GENFEE, OnClkBtGenfee)
	ON_BN_CLICKED(IDC_BT_GENORDFR, OnGenOrdFee)
END_MESSAGE_MAP()

BOOL GenMfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	#ifndef _REAL_CTP_
    GetDlgItem(IDC_BT_GENORDFR)->EnableWindow(FALSE);
	#endif
	return TRUE;  
}

void GenMfDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_pQryFee)
	{
		TermThread(m_pQryFee->m_hThread);
		m_pQryFee= NULL;
	}
	
	if (m_pQryOrdfr)
	{
		TermThread(m_pQryOrdfr->m_hThread);
		m_pQryOrdfr = NULL;
	}
	
	if (m_pQryMr)
	{
		TermThread(m_pQryMr->m_hThread);
		m_pQryMr = NULL;
	}

	delete this;
}

UINT GenMfDlg::QryMrThread(LPVOID pParam)
{
	GenMfDlg* pDlg = static_cast<GenMfDlg*>(pParam);
	CXTraderApp* app = g_App;
	CtpTdSpi* td = app->m_cT;
	VEC_IINF& vinf = app->m_cfg.m_InsinfVec;

	EnableClose(pDlg,FALSE);
	DWORD dwRet;
	PFTMGR pMgr = new FTMGR;
	POPMGR pOpMgr = new OPMGR;

	xml_document doc;
	xml_node proot,pFtr,pOpt,nodeMgrs;
	xml_parse_result result;
	bool bTag = false;
	
	char outMgrXml[MAX_PATH],szPath[MAX_PATH];
	sprintf(outMgrXml,MGR_XML,g_s.m_sUid);

	GetCurDir(szPath,outMgrXml);

	TCHAR szFile[MAX_PATH],szInst[MAX_PATH];
	ansi2uni(CP_ACP,szPath,szFile);

	BOOL bRet=GenXmlHdr(szFile);
	/////////////////////////////////////////////
	result = doc.load_file(szFile,parse_full);
	if (result.status!=status_ok) {return FALSE;}
	if (bRet) { doc.remove_child("Fees"); }

	proot = doc.child("Mgrs");
	pFtr = proot.child("Futures");
	pOpt = proot.child("Options");
	VIT_if it;
	int i=0;
	for (it = vinf.begin(); it != vinf.end();it++,i++)
	{	
		if (it->iinf.ProductClass == THOST_FTDC_PC_Futures)
		{
			td->ReqQryInstMgr(it->iinf.InstrumentID);
			dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
			if (dwRet==WAIT_OBJECT_0)
			{
				ResetEvent(g_hEvent);
				memcpy(pMgr,&td->m_MargRateRev,sizeof(FTMGR));
				if (strcmp(td->m_MargRateRev.InstrumentID,""))
				{
					//////////////////////////////////////////////////////////
					if (!bTag)
					{
						char szInvRg[6];
						sprintf(szInvRg,"%c",pMgr->InvestorRange);
						proot.append_attribute("InvRange") = szInvRg;
						proot.append_attribute("BkrID") = pMgr->BrokerID;
						proot.append_attribute("InvID") = pMgr->InvestorID;
						sprintf(szInvRg,"%c",pMgr->HedgeFlag);
						proot.append_attribute("Flag") = szInvRg;
						bTag = true;
					}
					nodeMgrs = pFtr.append_child("Mgr");
					
					nodeMgrs.append_attribute("InstID") = pMgr->InstrumentID;
					nodeMgrs.append_attribute("LongByM") = pMgr->LongMarginRatioByMoney;
					nodeMgrs.append_attribute("LongByV") = pMgr->LongMarginRatioByVolume;
					nodeMgrs.append_attribute("ShortByM") = pMgr->ShortMarginRatioByMoney;
					nodeMgrs.append_attribute("ShortByV") = pMgr->ShortMarginRatioByVolume;
					/////////////////////////////////////////////////////////
					
					ansi2uni(CP_ACP,pMgr->InstrumentID,szInst);
					pDlg->m_szGenstat.Format(_T("查询第%d条保证金率成功,合约代码:%s"),i,szInst);
					pDlg->UpdateData(FALSE);
							
				}
				else
				{
					pDlg->m_szGenstat.Format(_T("查询第%d条手保证金率为空,不记录."),i);
					pDlg->UpdateData(FALSE);
				}
			}
			else
			{
				pDlg->m_szGenstat = _T("查询保证金率失败!");
				pDlg->UpdateData(FALSE);
				break;
			}
		}
		else
		{
			td->ReqQryOpTdCost(it->iinf.InstrumentID);
			dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
			if (dwRet==WAIT_OBJECT_0)
			{
				ResetEvent(g_hEvent);
				memcpy(pOpMgr,&td->m_OpTdCost,sizeof(OPMGR));
				if (strcmp(td->m_MargRateRev.InstrumentID,""))
				{
					//////////////////////////////////////////////////////////
					if (!bTag)
					{
						char szInvRg[6];
						proot.append_attribute("BkrID") = pOpMgr->BrokerID;
						proot.append_attribute("InvID") = pOpMgr->InvestorID;
						sprintf(szInvRg,"%c",pOpMgr->HedgeFlag);
						proot.append_attribute("Flag") = szInvRg;
						bTag = true;
					}
					nodeMgrs = pOpt.append_child("Mgr");
					
					nodeMgrs.append_attribute("InstID") = pOpMgr->InstrumentID;
					nodeMgrs.append_attribute("Fixed") = FiltDb(pOpMgr->FixedMargin);
					nodeMgrs.append_attribute("Mini") = FiltDb(pOpMgr->MiniMargin);
					nodeMgrs.append_attribute("Royalty") = FiltDb(pOpMgr->Royalty);
					nodeMgrs.append_attribute("ExhFixed") = FiltDb(pOpMgr->ExchFixedMargin);
					nodeMgrs.append_attribute("ExhMini") = FiltDb(pOpMgr->ExchMiniMargin);
					
					ansi2uni(CP_ACP,pOpMgr->InstrumentID,szInst);
					pDlg->m_szGenstat.Format(_T("查询第%d条保证金率成功,合约代码:%s"),i,szInst);
					pDlg->UpdateData(FALSE);
				}
				else
				{
					pDlg->m_szGenstat.Format(_T("查询第%d条手保证金率为空,不记录."),i);
					pDlg->UpdateData(FALSE);
				}
			}
			else
			{
				pDlg->m_szGenstat = _T("查询保证金率失败!");
				pDlg->UpdateData(FALSE);
				break;
			}
		}
	}

    doc.save_file(outMgrXml,PUGIXML_TEXT("\t"),format_default,encoding_utf8);

	EnableClose(pDlg,TRUE);
	pDlg->m_szGenstat = _T("已经完成所有操作!");
	pDlg->UpdateData(FALSE);
	/////////////////////////////////////////////
	pDlg->m_pQryMr = NULL;
	DEL(pMgr); DEL(pOpMgr);
	return 0;
}

UINT GenMfDlg::QryFeeThread(LPVOID pParam)
{
	GenMfDlg* pDlg = static_cast<GenMfDlg*>(pParam);
	CXTraderApp* app = g_App;
	CtpTdSpi* td = app->m_cT;
	VEC_IINF& vinf = app->m_cfg.m_InsinfVec;

	EnableClose(pDlg,FALSE);
	xml_document doc;
	xml_node proot,nodeFees;
	xml_parse_result result;
	bool bTag = false;

	PFEEREX  pFee= new FEEREX;
	DWORD dwRet;

	char outFeeXml[MAX_PATH],szPath[MAX_PATH];
	sprintf(outFeeXml,FEE_XML,g_s.m_sUid);
	
	GetCurDir(szPath,outFeeXml);
	/////////////////////////////////////////////////////////////////
	TCHAR szFile[MAX_PATH],szInst[MAX_PATH];
	ansi2uni(CP_ACP,szPath,szFile);

	BOOL bRet = GenXmlHdr(szFile);
	//////////////////////////////////////////////////////
	result = doc.load_file(szFile,parse_full);
	if (result.status!=status_ok) {return FALSE;}
	if (bRet) { doc.remove_child("Mgrs"); }

	proot = doc.child("Fees");
	
	TThostFtdcInstrumentIDType pIDOld;
	strcpy(pIDOld,"UNKN");

	VIT_if it;
	int i=0;
	for (it = vinf.begin(); it != vinf.end();it++,i++)
	{
		if (it->iinf.ProductClass == THOST_FTDC_PC_Futures)
		{
			if (!strcmp(pIDOld,it->iinf.ProductID)) { continue; }
			
			td->ReqQryInstFee(it->iinf.InstrumentID);
			dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
			if (dwRet==WAIT_OBJECT_0)
			{
				ResetEvent(g_hEvent);
				if (strcmp(td->m_FeeRateRev.fee.InstrumentID,""))
				{
					memcpy(pFee,&td->m_FeeRateRev,sizeof(FEEREX));
					strcpy(pIDOld,pFee->fee.InstrumentID);
					//////////////////////////////////////////////////////////
					if (!bTag)
					{
						char szInvRg[6];
						sprintf(szInvRg,"%c",pFee->fee.InvestorRange);
						proot.append_attribute("InvRange") = szInvRg;
						proot.append_attribute("BkrID") = pFee->fee.BrokerID;
						proot.append_attribute("InvID") = pFee->fee.InvestorID;
						bTag = true;
					}
					nodeFees = proot.append_child("Fee");
					
					nodeFees.append_attribute("PID") = pFee->fee.InstrumentID;
					nodeFees.append_attribute("OpenByM") = pFee->fee.OpenRatioByMoney;
					nodeFees.append_attribute("OpenByV") = pFee->fee.OpenRatioByVolume;
					nodeFees.append_attribute("CloseByM") = pFee->fee.CloseRatioByMoney;
					nodeFees.append_attribute("CloseByV") = pFee->fee.CloseRatioByVolume;
					nodeFees.append_attribute("ClosetByM") = pFee->fee.CloseTodayRatioByMoney;
					nodeFees.append_attribute("ClosetByV") = pFee->fee.CloseTodayRatioByVolume;
					
					ansi2uni(CP_ACP,pFee->fee.InstrumentID,szInst);
					pDlg->m_szGenstat.Format(_T("查询第%d条手续费率成功,品种代码:%s"),i,szInst);
					pDlg->UpdateData(FALSE);
					
				}
				else
				{
					pDlg->m_szGenstat.Format(_T("查询第%d条手续费率为空,不记录."),i);
					pDlg->UpdateData(FALSE);
				}
			}
			else
			{
				pDlg->m_szGenstat = _T("查询手续费率失败!");
				pDlg->UpdateData(FALSE);
				break;
			}
		}
		else
		{
			if (!strcmp(pIDOld,it->iinf.ProductID)) { continue; }
			
			td->ReqQryOpFeeRate(it->iinf.InstrumentID);
			dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
			if (dwRet==WAIT_OBJECT_0)
			{
				ResetEvent(g_hEvent);
				if (strcmp(td->m_FeeRateRev.fee.InstrumentID,""))
				{
					memcpy(pFee,&td->m_FeeRateRev,sizeof(FEEREX));
					strcpy(pIDOld,pFee->fee.InstrumentID);
					//////////////////////////////////////////////////////////
					if (!bTag)
					{
						char szInvRg[6];
						sprintf(szInvRg,"%c",pFee->fee.InvestorRange);
						proot.append_attribute("InvRange") = szInvRg;
						proot.append_attribute("BkrID") = pFee->fee.BrokerID;
						proot.append_attribute("InvID") = pFee->fee.InvestorID;
						bTag = true;
					}
					nodeFees = proot.append_child("Fee");
					
					nodeFees.append_attribute("PID") = pFee->fee.InstrumentID;
					nodeFees.append_attribute("OpenByM") = pFee->fee.OpenRatioByMoney;
					nodeFees.append_attribute("OpenByV") = pFee->fee.OpenRatioByVolume;
					nodeFees.append_attribute("CloseByM") = pFee->fee.CloseRatioByMoney;
					nodeFees.append_attribute("CloseByV") = pFee->fee.CloseRatioByVolume;
					nodeFees.append_attribute("ClosetByM") = pFee->fee.CloseTodayRatioByMoney;
					nodeFees.append_attribute("ClosetByV") = pFee->fee.CloseTodayRatioByVolume;
					nodeFees.append_attribute("StrikeByM") = pFee->StrikeRatioByMoney;
					nodeFees.append_attribute("StrikeByV") = pFee->StrikeRatioByVolume;

					ansi2uni(CP_ACP,pFee->fee.InstrumentID,szInst);
					pDlg->m_szGenstat.Format(_T("查询第%d条手续费率成功,品种代码:%s"),i,szInst);
					pDlg->UpdateData(FALSE);
				}
				else
				{
					pDlg->m_szGenstat.Format(_T("查询第%d条手续费率为空,不记录."),i);
					pDlg->UpdateData(FALSE);
				}
			}
			else
			{
				pDlg->m_szGenstat = _T("查询手续费率失败!");
				pDlg->UpdateData(FALSE);
				break;
			}
		}

	}

    doc.save_file(outFeeXml,PUGIXML_TEXT("\t"),format_default,encoding_utf8);
    
	EnableClose(pDlg,TRUE);
	pDlg->m_szGenstat = _T("已经完成所有操作!");
	pDlg->UpdateData(FALSE);
	///////////////////////////////////////////////////////////////
	pDlg->m_pQryFee = NULL;
	DEL(pFee);
	return 0;
}

UINT GenMfDlg::QryOrdfrThread(LPVOID pParam)
{
	#ifdef _REAL_CTP_
	GenMfDlg* pDlg = static_cast<GenMfDlg*>(pParam);
	CXTraderApp* app = g_App;
	CtpTdSpi* td = app->m_cT;
	VEC_IINF& vinf = app->m_cfg.m_InsinfVec;

	EnableClose(pDlg,FALSE);
	xml_document doc;
	xml_node proot,nodeFees;
	xml_parse_result result;
	bool bTag = false;

	CThostFtdcInstrumentOrderCommRateField*  pFee= new CThostFtdcInstrumentOrderCommRateField;
	DWORD dwRet;

	char outFeeXml[MAX_PATH],szPath[MAX_PATH];
	sprintf(outFeeXml,ORDFEE_XML,g_s.m_sUid);

	GetCurDir(szPath,outFeeXml);
	/////////////////////////////////////////////////////////////////
	TCHAR szFile[MAX_PATH],szInst[MAX_PATH];
	ansi2uni(CP_ACP,szPath,szFile);

	BOOL bRet = GenXmlHdr(szFile);
	//////////////////////////////////////////////////////
	result = doc.load_file(szFile,parse_full);
	if (result.status!=status_ok) {return FALSE;}
	if (bRet) { doc.remove_child("Mgrs"); }
	proot = doc.child("Fees");

	TThostFtdcInstrumentIDType pIDOld;
	strcpy(pIDOld,"UNKN");

	VIT_if it;
	int i=0;
	for (it = vinf.begin(); it != vinf.end();it++,i++)
	{
		if (!strcmp(it->iinf.ExchangeID,_CFX))	//中金所指数期货才限制
		{
			if (!strcmp(pIDOld,it->iinf.ProductID)) { continue; }
			
			td->ReqQryInstOrdCommRate(it->iinf.InstrumentID);
			dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
			if (dwRet==WAIT_OBJECT_0)
			{
				ResetEvent(g_hEvent);
				if (strcmp(td->m_OrdfRate.InstrumentID,""))
				{
					memcpy(pFee,&td->m_OrdfRate,sizeof(CThostFtdcInstrumentOrderCommRateField));
					strcpy(pIDOld,pFee->InstrumentID);
					//////////////////////////////////////////////////////////
					if (!bTag)
					{
						char szInvRg[6];
						sprintf(szInvRg,"%c",pFee->InvestorRange);
						proot.append_attribute("InvRange") = szInvRg;
						proot.append_attribute("BkrID") = pFee->BrokerID;
						proot.append_attribute("InvID") = pFee->InvestorID;
						sprintf(szInvRg,"%c",pFee->HedgeFlag);
						proot.append_attribute("HedgeFlag") = szInvRg;
						bTag = true;
					}
					nodeFees = proot.append_child("Fee");
					
					nodeFees.append_attribute("PID") = pFee->InstrumentID;
					nodeFees.append_attribute("OrdByVol") = pFee->OrderCommByVolume;
					nodeFees.append_attribute("DelByVol") = pFee->OrderActionCommByVolume;

					ansi2uni(CP_ACP,pFee->InstrumentID,szInst);
					pDlg->m_szGenstat.Format(_T("查询第%d条报单费率成功,品种代码:%s"),i,szInst);
					pDlg->UpdateData(FALSE);
					
				}
				else
				{
					pDlg->m_szGenstat.Format(_T("查询第%d条报单费率为空,不记录."),i);
					pDlg->UpdateData(FALSE);
				}
			}
			else
			{
				pDlg->m_szGenstat = _T("查询报单费率失败!");
				pDlg->UpdateData(FALSE);
				break;
			}
		}
	}

	doc.save_file(outFeeXml,PUGIXML_TEXT("\t"),format_default,encoding_utf8);
		
	EnableClose(pDlg,TRUE);
	pDlg->m_szGenstat = _T("已经完成所有操作!");
	pDlg->UpdateData(FALSE);
	///////////////////////////////////////////////////////////////
	pDlg->m_pQryOrdfr = NULL;
	DEL(pFee);

	#endif
	return 0;
}

void GenMfDlg::OnClkBtGenmr()
{
	if(!m_pQryMr){ AfxBeginThread((AFX_THREADPROC)GenMfDlg::QryMrThread, this); }
}

void GenMfDlg::OnClkBtGenfee()
{
	if(!m_pQryFee){ AfxBeginThread((AFX_THREADPROC)GenMfDlg::QryFeeThread, this); }
}

void GenMfDlg::OnGenOrdFee()
{
	if(!m_pQryOrdfr){ AfxBeginThread((AFX_THREADPROC)GenMfDlg::QryOrdfrThread, this); }
}

void GenMfDlg::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void GenMfDlg::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}