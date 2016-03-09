#include "StdAfx.h"
#include "traderspi.h"
#include "xTrader.h"

extern HANDLE g_hEvent;
void CtpTdSpi::OnFrontConnected()
{
    if (g_Dlg && g_Dlg->m_bTdClosed){ ClrAllVecs(); }
	if (strlen(m_sAtCode)) { ReqAuth(PROD_INFO,m_sAtCode); }
	else { ReqUserLogin(); }
}

CtpTdSpi::~CtpTdSpi()
{
	ClrAllVecs();
}

void CtpTdSpi::ReqAuth(TThostFtdcProductInfoType prodInf,TThostFtdcAuthCodeType	AuthCode)
{
	CThostFtdcReqAuthenticateField req;
	
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strcpy(req.UserID, m_sUid);
	strcpy(req.UserProductInfo, prodInf);    	
	strcpy(req.AuthCode,AuthCode);  
	
	m_pApi->ReqAuthenticate(&req,++m_iReqId);
}

//如果需要认证 就在成功后继续登录
void CtpTdSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo) { memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); }
	
	if(bIsLast && !IsErrorRspInfo(pRspInfo)) { ReqUserLogin(); }
}

void CtpTdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strcpy(req.UserID, m_sUid);
	strcpy(req.Password, m_sPwd);
	if (strlen(m_sDymPwd)) { strcpy(req.OneTimePassword,m_sDymPwd); }
	
	strcpy(req.UserProductInfo,PROD_INFO);

	int iRet = m_pApi->ReqUserLogin(&req, ++m_iReqId);
}

#define TIME_NULL "--:--:--"

void CtpTdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( pRspInfo){memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField));}

	if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin )
	{  
		// 保存会话参数	
		m_iFrtId = pRspUserLogin->FrontID;
		m_iSesId = pRspUserLogin->SessionID;
		strcpy(m_sTdday,pRspUserLogin->TradingDay);

		m_iOrdRef = atoi(pRspUserLogin->MaxOrderRef);

		SYSTEMTIME curTime;
		::GetLocalTime(&curTime);
		CTime tc(curTime);
		int i=0,iHour[5],iMin[5],iSec[5];
		TThostFtdcTimeType ExhTime[5];
		strcpy(ExhTime[0],pRspUserLogin->SHFETime);
		strcpy(ExhTime[1],pRspUserLogin->DCETime);
		strcpy(ExhTime[2],pRspUserLogin->CZCETime);
		strcpy(ExhTime[3],pRspUserLogin->FFEXTime);
		strcpy(ExhTime[4],pRspUserLogin->INETime);

		for (i=0;i<5;i++)
		{
			if (!strcmp(ExhTime[i],TIME_NULL))
			{iHour[i]=curTime.wHour; iMin[i]=curTime.wMinute; iSec[i]=curTime.wSecond;}
			else
			{ sscanf(ExhTime[i], "%d:%d:%d", &iHour[i], &iMin[i], &iSec[i]); }

			m_tsEXnLocal[i] = CTime(curTime.wYear,curTime.wMonth,curTime.wDay,iHour[i],iMin[i],iSec[i])-tc;
		}
		if (g_Dlg) { ::PostMessage(g_Dlg->m_hWnd,WM_TDRECON_MSG,0,0); }
	}

  if(bIsLast) SetEvent(g_hEvent);
}

const char* CtpTdSpi::GetTradingDay()
{
	//登录才能准确获取
	return m_sTdday;
}

void CtpTdSpi::ReqUserLogout()
{
	CThostFtdcUserLogoutField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strcpy(req.UserID, m_sUid);
	m_pApi->ReqUserLogout(&req, ++m_iReqId);
}

///登出请求响应
void CtpTdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pUserLogout)
	{ }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqSetInfConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	m_pApi->ReqSettlementInfoConfirm(&req, ++m_iReqId);
}

void CtpTdSpi::OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( !IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm)
	{
 
	}
	else
	{ 
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       }
	}

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryNotice()
{
	CThostFtdcQryNoticeField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	
	while (true)
	{
		int iRet = m_pApi->ReqQryNotice(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) &&  pNotice){ m_BkrNtVec.push_back(*pNotice); }
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	
	if (bIsLast)
	{
		if (g_Dlg){ SendNotifyMessage(g_Dlg->m_hWnd,WM_QRYBKRNT_MSG,0,0); } 
		SetEvent(g_hEvent); 
	}
}

void CtpTdSpi::ReqQryTdNotice()
{
	CThostFtdcQryTradingNoticeField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	while (true)
	{
		int iRet = m_pApi->ReqQryTradingNotice(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) &&  pTradingNotice){ m_TdNoticeVec.push_back(*pTradingNotice); }
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	
	if(bIsLast) 
	{
		if (g_Dlg){ SendNotifyMessage(g_Dlg->m_hWnd,WM_QRYTDNT_MSG,0,0); } 
		SetEvent(g_hEvent); 
	}
}

void CtpTdSpi::ReqQrySetInfConfirm()
{
	CThostFtdcQrySettlementInfoConfirmField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	while (true)
	{
		int iRet = m_pApi->ReqQrySettlementInfoConfirm(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) &&  pSettlementInfoConfirm)
	{
		//
	}
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQrySetInf(TThostFtdcDateType TdDay)
{
	CThostFtdcQrySettlementInfoField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.TradingDay,TdDay);

	while (true)
	{
		int iRet =	m_pApi->ReqQrySettlementInfo(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) &&  pSettlementInfo){ m_StmiVec.push_back(*pSettlementInfo); }
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }

	if(bIsLast) 
	{ 		
		if (g_Dlg){ SendNotifyMessage(HWND_BROADCAST,WM_QRYSMI_MSG,0,0); } 
		else
		{ SetEvent(g_hEvent); }
	}
}

void CtpTdSpi::ReqQryInst(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentField req;
	ZeroMemory(&req, sizeof(req));
	if (instId != NULL){ strcpy(req.InstrumentID, instId); }
	while (true)
	{
		int iRet = m_pApi->ReqQryInstrument(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument)
	{
		if(((pInstrument->ProductClass ==THOST_FTDC_PC_Futures) ||(pInstrument->ProductClass ==THOST_FTDC_PC_Options)||
			(pInstrument->ProductClass ==THOST_FTDC_PC_SpotOption) /*||(pInstrument->ProductClass ==THOST_FTDC_PC_Combination)*/)&& 
			(pInstrument->IsTrading))
		{ 	
			CThostFtdcInstrumentFieldEx infEx;
			ZeroMemory(&infEx,sizeof(CThostFtdcInstrumentFieldEx));
			memcpy(&infEx,pInstrument,sizeof(CThostFtdcInstrumentField));
			FiltInfEx(infEx);

			string strkey = pInstrument->InstrumentID;
			MIT_if iter = m_InsinfMap.find(strkey);
			if(iter == m_InsinfMap.end()){ m_InsinfMap.insert(make_pair(strkey,infEx)); }
			else
			{ iter->second = infEx; }
		}
	}
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }

	if(bIsLast) 
	{
		CAppCfgs& s = g_s;
		sort(s.m_InsinfVec.begin(),s.m_InsinfVec.end(),cmpInst);
		ClearMap(m_FeeRateMap);  ClearMap(m_MgrFtMap);

		SetEvent(g_hEvent);
	}
}

void CtpTdSpi::ReqQryTdAcc()
{
	CThostFtdcQryTradingAccountField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet = m_pApi->ReqQryTradingAccount(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

//如果有担保 信用账户 可能需要vec来接收多账户信息
void CtpTdSpi::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount, 
   CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount)
	{ memcpy(&m_TdAcc,pTradingAccount,sizeof(CThostFtdcTradingAccountField)); }
	else
	{  
        if (pRspInfo) 
        { 
             memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
             ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }

	if(bIsLast) 
	{ 		
		if (g_Dlg)
		{ SendNotifyMessage(g_Dlg->m_hWnd,WM_QRYACC_MSG,0,0); } 
		//else
		{ SetEvent(g_hEvent); }
	}
}

//INSTRUMENT_ID设成部分字段,例如IF10,就能查出所有IF10打头的头寸
void CtpTdSpi::ReqQryInvPos(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL)
	{strcpy(req.InstrumentID, instId);}		

	while (true)
	{
		int iRet = m_pApi->ReqQryInvestorPosition(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}
	
void CtpTdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPosition )
	{ JoinTdYdPos(pInvestorPosition); }
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
	if(bIsLast) SetEvent(g_hEvent);	
}

void CtpTdSpi::JoinTdYdPos(const CThostFtdcInvestorPositionField *pInv)
{
	if (pInv->Position >0)
	{
		VIT_pos vpos = m_InvPosVec.begin();
		for (vpos;vpos != m_InvPosVec.end();vpos++)
		{
			if (!strcmp(vpos->InstrumentID,pInv->InstrumentID) && (vpos->PosiDirection==pInv->PosiDirection))
			{	
				vpos->YdPosition += pInv->YdPosition;
				vpos->Position += pInv->Position;
				vpos->LongFrozen += pInv->LongFrozen;
				vpos->ShortFrozen += pInv->ShortFrozen;
				vpos->LongFrozenAmount += pInv->LongFrozenAmount;
				vpos->ShortFrozenAmount += pInv->ShortFrozenAmount;
				vpos->OpenVolume += pInv->OpenVolume;
				vpos->CloseVolume += pInv->CloseVolume;
				vpos->OpenAmount += pInv->OpenAmount;
				vpos->CloseAmount += pInv->CloseAmount;
				vpos->PositionCost += pInv->PositionCost;
				vpos->PreMargin += pInv->PreMargin;
				vpos->UseMargin += pInv->UseMargin;
				vpos->FrozenMargin += pInv->FrozenMargin;
				vpos->FrozenCash += pInv->FrozenCash;
				vpos->FrozenCommission += pInv->FrozenCommission;
				vpos->CashIn += pInv->CashIn;
				vpos->Commission += pInv->Commission;
				vpos->CloseProfit += pInv->CloseProfit;
				vpos->PositionProfit += pInv->PositionProfit;
				vpos->OpenCost += pInv->OpenCost;
				vpos->CombPosition += pInv->CombPosition;
				vpos->CombLongFrozen += pInv->CombLongFrozen;
				vpos->CombShortFrozen += pInv->CombShortFrozen;
				vpos->CloseProfitByDate += pInv->CloseProfitByDate;
				vpos->CloseProfitByTrade += pInv->CloseProfitByTrade;
				vpos->TodayPosition += pInv->TodayPosition;
					
				vpos->StrikeFrozen += pInv->StrikeFrozen;
				vpos->StrikeFrozenAmount += pInv->StrikeFrozenAmount;
				vpos->AbandonFrozen += pInv->AbandonFrozen;
				break;
			}
		}

		if (vpos == m_InvPosVec.end()){ m_InvPosVec.push_back(*pInv); }
	}
	//else if (pInv->Position == 0)
	//{
	//	//持仓量为0的表示清过仓 但里面记录了YdPos的原始值
	//}	
}

//INSTRUMENT_ID设成部分字段,例如IF10,就能查出所有IF10打头的头寸
void CtpTdSpi::ReqQryInvPosEx(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionDetailField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) {strcpy(req.InstrumentID, instId);}		
	
	while (true)
	{
		int iRet = m_pApi->ReqQryInvestorPositionDetail(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, 
									CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPositionDetail)
	{

	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryInvPosCombEx(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionCombineDetailField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) {strcpy(req.CombInstrumentID, instId);}	

	while (true)
	{
		int iRet = m_pApi->ReqQryInvestorPositionCombineDetail(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionDetail, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPositionDetail)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

///请求查询交易所
void CtpTdSpi::ReqQryExh(TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryExchangeField req;
	if (ExhId!=NULL) { strcpy(req.ExchangeID,ExhId); }	
	
	while (true)
	{
		int iRet = m_pApi->ReqQryExchange(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询交易所响应
void CtpTdSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pExchange){ m_ExhVec.push_back(*pExchange); }
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	if(bIsLast)
	{ 
		//SendNotifyMessage(HWND_BROADCAST,WM_QRYEXH_MSG,0,0);
		SetEvent(g_hEvent); 
	}
}

///请求查询行情
void CtpTdSpi::ReqQryDepthMD(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryDepthMarketDataField req;
	ZeroMemory(&req,sizeof(req));

	if (instId != NULL){ strcpy(req.InstrumentID,instId); }

	while (true)
	{
		int iRet = 	m_pApi->ReqQryDepthMarketData(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

///请求查询行情响应
void CtpTdSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( !IsErrorRspInfo(pRspInfo) &&  pDepthMarketData)
	{
		string strkey = pDepthMarketData->InstrumentID;
		MIT_md iter = m_DepthMdMap.find(strkey);
		if(iter == m_DepthMdMap.end())
		{ m_DepthMdMap.insert(make_pair(strkey,*pDepthMarketData)); }
		else
		{ iter->second = *pDepthMarketData; }	
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) { SetEvent(g_hEvent); }
}

///请求查询报单
void CtpTdSpi::ReqQryOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,TThostFtdcOrderSysIDType ordSysId,
							 TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd)
{
	CThostFtdcQryOrderField req;
	ZeroMemory(&req,sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if(ExhId !=NULL) strcpy(req.ExchangeID,ExhId);
	if(instId !=NULL) strcpy(req.InstrumentID,instId);
	if(ordSysId !=NULL) strcpy(req.OrderSysID,ordSysId);
	if(tmStart !=NULL) strcpy(req.InsertTimeStart,tmStart);
	if(tmEnd !=NULL) strcpy(req.InsertTimeEnd,tmEnd);
	
	while (true)
	{
		int iRet = 	m_pApi->ReqQryOrder(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询报单响应
void CtpTdSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pOrder )
	{
		if (!IsCondOrd(pOrder))
		{
			CORDFEx order(pOrder,GetTdAvgPx(pOrder->BrokerOrderSeq));
			VIT_ord vod = find(m_orderVec.begin(), m_orderVec.end(), order);
			if (vod!=m_orderVec.end()){ *vod = order; }
			else
			{ m_orderVec.push_back(order); }
			
			//掉线重查时此处需要修正
			if(JgCancelStat(&order)){ m_onRoadVec.push_back(order); }
		}
		else
		{
			PARKEDEX pkEx;
			OrdConvter(*pOrder,pkEx,ORD2PARKED);
			if (pOrder->ContingentCondition > THOST_FTDC_CC_ParkedOrder) //条件单
			{ pkEx.ordType = COND_SERVER; }
			else if (pOrder->ContingentCondition == THOST_FTDC_CC_Touch)
			{ pkEx.ordType = TOUCH_ZS; }
			else if (pOrder->ContingentCondition == THOST_FTDC_CC_TouchProfit)
			{ pkEx.ordType = TOUCH_ZY; }
			
			VIT_pk vpk = m_pkExVec.begin();
			for (vpk;vpk!=m_pkExVec.end();vpk++)
			{
				if (!strcmp(vpk->OrderSysID,pOrder->OrderSysID))
				{ 
					*vpk = pkEx;
					break; 
				}
			}
			
			if(vpk == m_pkExVec.end()) {  m_pkExVec.push_back(pkEx); }
		}
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
  if(bIsLast) 
  {
    sort(m_orderVec.begin(),m_orderVec.end(),cmpOrder);
    SetEvent(g_hEvent);
  }
}

///请求查询成交
void CtpTdSpi::ReqQryTd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,TThostFtdcTradeIDType tdId,
							TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd)
{
	CThostFtdcQryTradeField req;
	ZeroMemory(&req,sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	if (ExhId !=NULL)strcpy(req.ExchangeID,ExhId);
	if (instId !=NULL)strcpy(req.InstrumentID,instId);
	if (tdId !=NULL)strcpy(req.TradeID,tdId);
	if (tmStart !=NULL)strcpy(req.TradeTimeStart,tmStart);
	if (tmEnd !=NULL)strcpy(req.TradeTimeEnd,tmEnd);

	while (true)
	{
		int iRet =	m_pApi->ReqQryTrade(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

///请求查询成交响应
void CtpTdSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pTrade )
	{ 
		CTDFEx td(pTrade,GetTdFee(pTrade));
		VIT_td vtd = find(m_tradeVec.begin(), m_tradeVec.end(), td);
		if (vtd!=m_tradeVec.end()){ *vtd = td; }
		else
		{ m_tradeVec.push_back(td); }
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

  if(bIsLast) 
  {
    sort(m_tradeVec.begin(),m_tradeVec.end(),cmpTrade);
    SetEvent(g_hEvent);
  }
}

UINT CtpTdSpi::TmOutAutoDellThd(LPVOID pParam)
{
	PTOAUTODELP pAutoDel = static_cast<PTOAUTODELP>(pParam);
	CtpTdSpi* td = static_cast<CtpTdSpi*>(pAutoDel->td);
	
	if (pAutoDel)
	{
		Sleep(1000*pAutoDel->uTmOut+1);
		
		//唯一定位执行撤单立即返回
		if (strlen(pAutoDel->ordSysId))
		{
			//条件单
			VIT_pk vpk=td->m_pkExVec.begin();
			for(vpk;vpk!=td->m_pkExVec.end();vpk++)
			{
				if (!strcmp(vpk->OrderSysID,pAutoDel->ordSysId) &&
					!strcmp(vpk->park.ExchangeID,pAutoDel->ExhId))
				{ 
					if (vpk->park.Status == THOST_FTDC_OST_NotTouched)
					{ td->ReqOrdCancel(pAutoDel->ExhId,pAutoDel->ordSysId); }
					break;
				}
			}
		}
		else
		{
			//普通单
			VIT_ord vord = td->m_orderVec.begin();
			for (vord;vord!=td->m_orderVec.end();vord++)
			{
				if (vord->Ord.FrontID == pAutoDel->iFrtId && 
					vord->Ord.SessionID == pAutoDel->iSesId &&
					!strcmp(vord->Ord.OrderRef,pAutoDel->sRef) &&
					!strcmp(vord->Ord.InstrumentID,pAutoDel->instId))
					
				{
					if (vord->Ord.OrderStatus == THOST_FTDC_OST_PartTradedQueueing ||
						vord->Ord.OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
					{ td->ReqOrdCancel(pAutoDel->iFrtId,pAutoDel->iSesId,pAutoDel->sRef,pAutoDel->instId); }
					break;
				}
			}
		}
	
		DEL(pAutoDel);
	}

	return 0;
}

//////////////////////////////////////////////////
void CtpTdSpi::AutoDelLocOrd(const CThostFtdcInputOrderField *pInp)	
{
	CAppCfgs& s = g_s;
	//if (s.m_bOrdTmOut)
	{
		PTOAUTODELP pAutoDel = new TOAUTODELP(this,s.m_uOrdToSec,m_iFrtId,m_iSesId,
			pInp->OrderRef,pInp->InstrumentID);
		
		AfxBeginThread((AFX_THREADPROC)TmOutAutoDellThd,pAutoDel);
	}
}

void CtpTdSpi::AutoDelNoLocOrd(const CThostFtdcOrderField *pOrder)
{
	CAppCfgs& s = g_s;
	/////////////////////其它终端报单也监测////////////////////////
	if (/*s.m_bOrdTmOut &&*/
		(strcmp(pOrder->IPAddress,s.m_sLocIp) || 
		strcmp(pOrder->MacAddress,s.m_sLocMac) ||
		strcmp(pOrder->UserProductInfo,PROD_INFO)) &&
		(pOrder->OrderStatus == THOST_FTDC_OST_Unknown ||
		pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing ||
		pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing))
	{
		PTOAUTODELP pAutoDel = new TOAUTODELP(this,s.m_uOrdToSec,pOrder->FrontID,
			pOrder->SessionID,pOrder->OrderRef,pOrder->InstrumentID);
		
		AfxBeginThread((AFX_THREADPROC)TmOutAutoDellThd,pAutoDel);
	}
}

//动态止损只能是本地单,普通止损可为服务器或本地条件单
void CtpTdSpi::StopAfterTd(const TThostFtdcPriceType dStopPx,bool bDym)
{

}

void CtpTdSpi::ReqOrds(CThostFtdcInputOrderField* pInp)
{
    CAppCfgs& s = g_s;
	strcpy(pInp->BrokerID, m_sBkrId);
	strncpy(pInp->InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	sprintf(pInp->OrderRef, "%012d", ++m_iOrdRef); 
	
	pInp->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pInp->IsAutoSuspend = 0;
	pInp->UserForceClose = 0;

	strcpy(pInp->IPAddress,s.m_sLocIp);
    strcpy(pInp->MacAddress,s.m_sLocMac);
	
	m_pApi->ReqOrderInsert(pInp, ++m_iReqId);
	//////////////////////////////////////////////////
	AutoDelLocOrd(pInp);
}

void CtpTdSpi::ReqOrdLimit(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,
	 TThostFtdcOffsetFlagType kpp,TThostFtdcPriceType price,TThostFtdcVolumeType vol)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	 
	strcpy(req.InstrumentID, instId); 	

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;	
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.LimitPrice = price;
	req.VolumeTotalOriginal = vol;
	req.TimeCondition = THOST_FTDC_TC_GFD;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	req.ContingentCondition = THOST_FTDC_CC_Immediately;

	ReqOrds(&req);
}

///市价单
void CtpTdSpi::ReqOrdAny(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcOffsetFlagType kpp,TThostFtdcVolumeType vol)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	
	strcpy(req.InstrumentID, instId); 	

	req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;

	req.VolumeTotalOriginal = vol;
	req.TimeCondition = THOST_FTDC_TC_IOC;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	
	ReqOrds(&req);
}

//中金所高级市价单
void CtpTdSpi::ReqOrdAnyCfx(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcOffsetFlagType kpp,
		TThostFtdcVolumeType vol,TThostFtdcOrderPriceTypeType OrdPxType,TThostFtdcTimeConditionType tmCond)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	
	strcpy(req.InstrumentID, instId); 	
	
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.OrderPriceType = OrdPxType;
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;
	
	req.VolumeTotalOriginal = vol;
	req.TimeCondition = tmCond;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	
	ReqOrds(&req);
}

void CtpTdSpi::ReqOrdCond(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcOffsetFlagType kpp,TThostFtdcPriceType price,
	TThostFtdcVolumeType vol,TThostFtdcPriceType stopPx,TThostFtdcContingentConditionType conType)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	
	strcpy(req.InstrumentID, instId);	

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;
	req.LimitPrice = price;
	req.VolumeTotalOriginal = vol;
	req.TimeCondition = THOST_FTDC_TC_GFD;
	req.VolumeCondition = THOST_FTDC_VC_AV;

	req.ContingentCondition = conType; 
	req.StopPrice = stopPx;
	
	ReqOrds(&req);
}

//大商所止损止盈 最新>/<触发价时触发新报单,市价或限价
//ContingentCondition:Touch/TouchProfit
void CtpTdSpi::ReqOrdTouch(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcOffsetFlagType kpp,
	TThostFtdcPriceType price,TThostFtdcVolumeType vol,TThostFtdcPriceType stopPx,TThostFtdcContingentConditionType conType)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	
	strcpy(req.InstrumentID, instId);	
	TThostFtdcOrderPriceTypeType tPx = (price<NEARZERO)?THOST_FTDC_OPT_AnyPrice:THOST_FTDC_OPT_LimitPrice;
	
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.OrderPriceType = tPx;
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;
	req.LimitPrice = price;
	req.VolumeTotalOriginal = vol;
	req.TimeCondition = (tPx==THOST_FTDC_OPT_AnyPrice)?THOST_FTDC_TC_IOC:THOST_FTDC_TC_GFD;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	
	req.ContingentCondition = conType; 
	req.StopPrice = stopPx;
	
	ReqOrds(&req);
}

//FAK(Fill And Kill)指令就是将报单的有效期设为THOST_FTDC_TC_IOC,同时,成交量类型设为THOST_FTDC_VC_AV,即任意数量;
//FOK(Fill Or Kill)指令是将报单的有效期类型设置为THOST_FTDC_TC_IOC,同时将成交量类型设置为THOST_FTDC_VC_CV,即全部数量.
//此外,在FAK指令下,还可指定最小成交量,即在指定价位、满足最小成交数量以上成交,剩余订单被系统撤销,否则被系统全部撤销.此种状况下,
//有效期类型设置为THOST_FTDC_TC_IOC,数量条件设为THOST_FTDC_VC_MV,同时设定MinVolume字段.

void CtpTdSpi::ReqOrdFAOK(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcOffsetFlagType kpp,
			TThostFtdcPriceType price,TThostFtdcVolumeType vol,TThostFtdcVolumeConditionType volconType,TThostFtdcVolumeType minVol)
{
	CThostFtdcInputOrderField req;
	ZeroMemory(&req, sizeof(req));	
 
	strcpy(req.InstrumentID, instId); 	

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp;
	req.LimitPrice = price;
	req.VolumeTotalOriginal = vol;
	req.TimeCondition = THOST_FTDC_TC_IOC;
	req.VolumeCondition = volconType;
	req.MinVolume = minVol;
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	
	ReqOrds(&req);
}

void CtpTdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CXTraderDlg* pDlg = g_Dlg;
	if( IsErrorRspInfo(pRspInfo) || (!pInputOrder))
	{
		if (pInputOrder->StopPrice>NEARZERO) 
		{
			//这里是条件单错误
			PARKEDEX pkEx;
			OrdConvter(*pInputOrder,pkEx,INP2PARKED);
			getCurDate(pkEx.InsertDate);
			getCurTime(pkEx.InsertTime);
			pkEx.ordType = COND_SERVER;
			pkEx.park.Status = THOST_FTDC_OST_ErrOrd;
			strcpy(pkEx.park.ErrorMsg,pRspInfo->ErrorMsg);
			if (pInputOrder->ContingentCondition==THOST_FTDC_CC_Touch)
			{ pkEx.ordType = TOUCH_ZS; }
			else if (pInputOrder->ContingentCondition==THOST_FTDC_CC_TouchProfit)
			{ pkEx.ordType = TOUCH_ZY; }

			VIT_pk vpk = m_pkExVec.begin();
			for (vpk;vpk!=m_pkExVec.end();vpk++)
			{
				if (vpk->ordType==pkEx.ordType && !strcmp(vpk->park.OrderRef,pInputOrder->OrderRef))
				{ 
					*vpk = pkEx;
					break; 
				}
			}
			
			if(vpk == m_pkExVec.end()) { m_pkExVec.push_back(pkEx); }
			if(pDlg)
			{
                pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
                if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
			}
		}
		else
		{
			CORDFEx ErrOrd;
			GenErrOrdByInpt(pInputOrder,&ErrOrd);
			strcpy(ErrOrd.Ord.StatusMsg,pRspInfo->ErrorMsg);

			m_orderVec.push_back(ErrOrd);
			if(pDlg)
			{
                pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(m_orderVec.size()));
                if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
			}
		}
		//if(g_s.m_bWarnSnd) { PlaySnd(g_s.m_szActErr); } 
	}
}

///报单录入错误回报
void CtpTdSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	CXTraderDlg* pDlg = g_Dlg;
	CORDFEx ErrOrd;
	GenErrOrdByInpt(pInputOrder,&ErrOrd);
	strcpy(ErrOrd.Ord.StatusMsg,pRspInfo->ErrorMsg);

	m_orderVec.push_back(ErrOrd);
	if (pDlg)
	{
		pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(m_orderVec.size()));
		if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
	}	
	//if(g_s.m_bWarnSnd) { PlaySnd(g_s.m_szActErr); } 
}

// 三组都可以用来撤单
//InstrumentID + FrontID + SessionID + OrderRef
//ExchangeID + TraderID + OrderLocalID
//ExchangeID + OrderSysID
void CtpTdSpi::CancelOrd2(CThostFtdcOrderFieldEx* pOrd)
{
	ReqOrdCancel(pOrd->Ord.ExchangeID,pOrd->Ord.OrderSysID);
}

void CtpTdSpi::ReqOrdCancel(TThostFtdcExchangeIDType ExhId,TThostFtdcOrderSysIDType ordSysId)
{
	CThostFtdcInputOrderActionField req;
	ZeroMemory(&req, sizeof(req));
	CAppCfgs& s = g_s;

	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	strcpy(req.ExchangeID, ExhId);
	strcpy(req.OrderSysID, ordSysId);

	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);

	req.ActionFlag = THOST_FTDC_AF_Delete;
	
	m_pApi->ReqOrderAction(&req, ++m_iReqId);
}

void CtpTdSpi::CancelOrd(CThostFtdcOrderFieldEx* pOrd)
{
	ReqOrdCancel(pOrd->Ord.FrontID,pOrd->Ord.SessionID,pOrd->Ord.OrderRef,pOrd->Ord.InstrumentID);
}

void CtpTdSpi::ReqOrdCancel(TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId,
		TThostFtdcOrderRefType sRef,TThostFtdcInstrumentIDType instId)
{
	CThostFtdcInputOrderActionField req;
	ZeroMemory(&req, sizeof(req));
	CAppCfgs& s = g_s;

	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.InstrumentID, instId);
	strcpy(req.OrderRef, sRef);	
	req.FrontID = iFrtId;	
	req.SessionID = iSesId;
	
	req.ActionFlag = THOST_FTDC_AF_Delete;
	
	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);

	m_pApi->ReqOrderAction(&req, ++m_iReqId);
}

void CtpTdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
  if (IsErrorRspInfo(pRspInfo) || (!pInputOrderAction))
  {
	 if (g_Dlg)
	 { 
        g_Dlg->SetStatusTxt(GetCTPErr(pRspInfo->ErrorMsg),2);
       // if(g_s.m_bWarnSnd) { PlaySnd(g_s.m_szActErr); }  
	 }
  }
}

void CtpTdSpi::CancelAll()
{
	VIT_ord vod = m_onRoadVec.begin();
	for (vod;vod!=m_onRoadVec.end();vod++){ CancelOrd(&(*vod)); }
}

///撤单错误回报
void CtpTdSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) 
{
	if( IsErrorRspInfo(pRspInfo) || (!pOrderAction))
	{ 
        ShowCbErrs(pRspInfo->ErrorMsg); 
        //if(g_s.m_bWarnSnd) { PlaySnd(g_s.m_szActErr); } 
	}	
}

///提示条件单校验错误
void CtpTdSpi::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder)
{
	if (pErrorConditionalOrder->ErrorID)
	{
		CXTraderDlg* pDlg = g_Dlg;
		VIT_pk vpk = m_pkExVec.begin();
		for (vpk;vpk!=m_pkExVec.end();vpk++)
		{
			if (!strcmp(vpk->OrderSysID,pErrorConditionalOrder->OrderLocalID))
			{ 
				strcpy(vpk->park.ErrorMsg,pErrorConditionalOrder->ErrorMsg);
				break; 
			}
		}
		if (pDlg)
		{
		pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
		if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
}

///////////////////////从开仓生成平仓单/////////////////////////////
void CtpTdSpi::GenCloseFromOrd(CThostFtdcOrderField *pOrd,int nPxTick)
{
	if (pOrd->CombOffsetFlag[0] == THOST_FTDC_OF_Open)
	{
		TThostFtdcOffsetFlagType kpp;
		TThostFtdcDirectionType cDir = THOST_FTDC_D_Sell+THOST_FTDC_D_Buy-pOrd->Direction;
		TThostFtdcPriceType dPx;
		if (cDir==THOST_FTDC_D_Sell)
		{ dPx = pOrd->LimitPrice+nPxTick*GetPxTick(pOrd->InstrumentID); }
		else
		{ dPx = pOrd->LimitPrice-nPxTick*GetPxTick(pOrd->InstrumentID); }
		
		if(!strcmp(pOrd->ExchangeID,_SFE))
		{ kpp=THOST_FTDC_OF_CloseToday; }
		else
		{ kpp=THOST_FTDC_OF_Close; }

		ReqOrdLimit(pOrd->InstrumentID,cDir,kpp,dPx,pOrd->VolumeTraded); //此处有bug
	}
}

void CtpTdSpi::GenCloseFromTd(CThostFtdcTradeField *pTd,int nPxTick)
{
	if (pTd->OffsetFlag == THOST_FTDC_OF_Open)
	{
		TThostFtdcOffsetFlagType kpp;
		TThostFtdcDirectionType cDir = THOST_FTDC_D_Sell+THOST_FTDC_D_Buy-pTd->Direction;
		
		TThostFtdcPriceType dPx;
		if (cDir==THOST_FTDC_D_Sell)
		{ dPx = pTd->Price+nPxTick*GetPxTick(pTd->InstrumentID); }
		else
		{ dPx = pTd->Price-nPxTick*GetPxTick(pTd->InstrumentID); }
	
		if(!strcmp(pTd->ExchangeID,_SFE))
		{ kpp=THOST_FTDC_OF_CloseToday; }
		else
		{ kpp=THOST_FTDC_OF_Close; }

		ReqOrdLimit(pTd->InstrumentID,cDir,kpp,dPx,pTd->Volume);
	}
}

//////////////报单回报里的持仓冻结处理///////////////////////////////
void CtpTdSpi::UpdatePosByOrd(LPVOID pParam,const CThostFtdcOrderField *pOrder)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);

	TThostFtdcDirectionType cDir;
	VIT_pos vpos = m_InvPosVec.begin();
	for(vpos;vpos!=m_InvPosVec.end();vpos++)
	{
		cDir = vpos->PosiDirection -2;
		
		if(!strcmp(pOrder->InstrumentID,vpos->InstrumentID)&& (pOrder->Direction ==(THOST_FTDC_D_Buy+THOST_FTDC_D_Sell-cDir)) &&
			(pOrder->CombOffsetFlag[0]!=THOST_FTDC_OF_Open))
		{
			if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			{	
				if (pOrder->Direction == THOST_FTDC_D_Buy)
				{ vpos->LongFrozen -= pOrder->VolumeTotal; }
				else
				{ vpos->ShortFrozen -= pOrder->VolumeTotal; }
			}
			else
			{
				//交易所回报
				if (strlen(pOrder->OrderSysID))
				{
					if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded ||
						pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
					{
						if (pOrder->Direction == THOST_FTDC_D_Buy)
						{ vpos->LongFrozen -= pOrder->VolumeTraded; }
						else
						{ vpos->ShortFrozen -= pOrder->VolumeTraded; }
					}
				}
				else
				{
					//CTP平台回报
					if(pOrder->OrderStatus==THOST_FTDC_OST_Unknown)
					{
						if (pOrder->Direction == THOST_FTDC_D_Buy)
						{ vpos->LongFrozen += pOrder->VolumeTotal; }
						else
						{ vpos->ShortFrozen += pOrder->VolumeTotal; }
					}
				}
			}
			
			if(pDlg && pDlg->m_eLogMod==NORMAL_LOG) {
				pDlg->m_xLst[INVPOS_IDX].SetItemCountEx(static_cast<int>(m_InvPosVec.size()));
				if(pDlg->m_xLst[INVPOS_IDX].IsWindowVisible()) { pDlg->m_xLst[INVPOS_IDX].Invalidate(); }
			}
			break;
		}
	}
}

//////////////报单回报里的条件单处理///////////////////////////////
void CtpTdSpi::ProcCondOrd(LPVOID pParam,CThostFtdcOrderField *pOrder)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	CAppCfgs& s = g_s;

	PARKEDEX pkEx;
	OrdConvter(*pOrder,pkEx,ORD2PARKED);
	if (pOrder->ContingentCondition > THOST_FTDC_CC_ParkedOrder) //条件单
	{ pkEx.ordType = COND_SERVER; }
	else if (pOrder->ContingentCondition == THOST_FTDC_CC_Touch)
	{ pkEx.ordType = TOUCH_ZS; }
	else if (pOrder->ContingentCondition == THOST_FTDC_CC_TouchProfit)
	{ pkEx.ordType = TOUCH_ZY; }
	else
	{ return; }
	
	VIT_pk vpk = m_pkExVec.begin();
	for (vpk;vpk!=m_pkExVec.end();vpk++)
	{
		if (!strcmp(vpk->OrderSysID,pOrder->OrderSysID))
		{ 
			*vpk = pkEx;
			break; 
		}
	}
	
	if(vpk == m_pkExVec.end()) { m_pkExVec.push_back(pkEx); }
	if (pDlg){
		pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
		if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
	}
	/////////////////////监测条件单////////////////////////
	if (/*s.m_bOrdTmOut &&*/ (pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_Open)
		&& pOrder->OrderStatus==THOST_FTDC_OST_NotTouched)
	{
		PTOAUTODELP pAutoDel = new TOAUTODELP(this,s.m_uOrdToSec,pOrder->ExchangeID,pOrder->OrderSysID);
		AfxBeginThread((AFX_THREADPROC)TmOutAutoDellThd,pAutoDel);
	}
}

void CtpTdSpi::PostOrdWarnMsg(LPVOID pParam,const CThostFtdcOrderField *pOrder)
{
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	CAppCfgs& s = g_s;
	//////////////////非本地报单提示////////////////////////
	if (/*s.m_bChkNotLoc && */
		strcmp(pOrder->IPAddress,s.m_sLocIp) || 
		strcmp(pOrder->MacAddress,s.m_sLocMac) ||
		strcmp(pOrder->UserProductInfo,PROD_INFO))
	{
		if (pDlg){
			CThostFtdcOrderField *pSend = new CThostFtdcOrderField;
			memcpy(pSend,pOrder,sizeof(CThostFtdcOrderField));
			pDlg->PostMessage(WM_NOTLOCORD_MSG,0,(LPARAM)pSend); 
		}
	}
}

/////////////////////////////////////////////////////////////////////
///报单回报
void CtpTdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    CXTraderDlg* pDlg = g_Dlg;
	CAppCfgs& s = g_s;
    ////////////////////////////////////////////////////////
	if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
	{ 
        SetEvent(g_hEvent); 
        //if(s.m_bWarnSnd) { PlaySnd(s.m_szDelOk); } 
	}
	else
	{  
		PostOrdWarnMsg(pDlg,pOrder);
		////////////////////报单成功提示///////////////////////////
        //if(s.m_bWarnSnd) { PlaySnd(s.m_szOrdOk); }
    }

#ifdef _REAL_CTP_
	double dOdByV,dDelByV,dFee=0;
	GetOrdfRByInst(pOrder->InstrumentID,dOdByV,dDelByV);
	if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled){ dFee = dDelByV; }
	else
	{ 
        if (strlen(pOrder->OrderSysID)){ dFee = dOdByV; } 
	}
	if (pDlg){ pDlg->m_pTdAcc->Commission += dFee; }
#endif

	if(!IsCondOrd(pOrder)) //条件单另外处理
	{
		CORDFEx order(pOrder);
		VIT_ord vod=find(m_orderVec.begin(), m_orderVec.end(), order);
		//////修改已发出的报单状态
		if(vod!=m_orderVec.end()) 
		{
			memcpy(&vod->Ord,pOrder,sizeof(CThostFtdcOrderField));
			if (pDlg){
				pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(m_orderVec.size()));
				if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
			}
				
			///////////////////////////刷新挂单列表/////////////////////
			VIT_ord vod2 = find(m_onRoadVec.begin(), m_onRoadVec.end(),order);
			if(vod2 != m_onRoadVec.end()) 
			{  
				//已经在列表的 如果完成 则删除
				if (JgOverStat(&order)){
                     m_onRoadVec.erase(vod2); 
                     if(m_onRoadVec.size()==0){ ClearVec(m_onRoadVec); }
				 }
				if(JgCancelStat(&order)){*vod2 = order; }
			}
			else
			{
				//未加入挂单的加入列表
				if (JgCancelStat(&order)){ m_onRoadVec.push_back(order); }
			}

			if (pDlg){
				pDlg->m_xLst[ONROAD_IDX].SetItemCountEx(static_cast<int>(m_onRoadVec.size()));
				if(pDlg->m_xLst[ONROAD_IDX].IsWindowVisible()) { pDlg->m_xLst[ONROAD_IDX].Invalidate(); }
			}
		} 
		else 
		{   ///////新增加委托单
			m_orderVec.push_back(order);

			AutoDelNoLocOrd(pOrder);
			/////////////////////////////////////////////////////////////////
			if (pDlg){
				pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(m_orderVec.size()));
				if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
			}
			if (JgCancelStat(&order))
			{
				m_onRoadVec.push_back(order);
				if (pDlg){
					pDlg->m_xLst[ONROAD_IDX].SetItemCountEx(static_cast<int>(m_onRoadVec.size()));
					if(pDlg->m_xLst[ONROAD_IDX].IsWindowVisible()) { pDlg->m_xLst[ONROAD_IDX].Invalidate(); }
				}
			}
		}
		////////////////////////刷新持仓////////////////////////////////
		UpdatePosByOrd(pDlg,pOrder);
	}
	else  
	{ ProcCondOrd(pDlg,pOrder); }

	if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled){ ResetEvent(g_hEvent); }
}

void CtpTdSpi::UpdatePosByTd(LPVOID pParam,const CThostFtdcTradeField *pTrade)
{
	CTDFEx td(pTrade);
	m_tradeVec.push_back(td);
	////////////////////////刷新持仓////////////////////////////////
	CXTraderDlg* pDlg = static_cast<CXTraderDlg*>(pParam);
	bool bExist = false;
	int iFlag=0,iTdDelta=0;
	int iMul = FindInstMul(pTrade->InstrumentID);
	TThostFtdcDirectionType cDir;
	TThostFtdcOffsetFlagType cFlag;
	double dLastCost = 0.0,dClosePro=0.0,dfRByM,dfRByV,dFee=0;
	
	VIT_pos vpos = m_InvPosVec.begin();
	for(vpos;vpos!=m_InvPosVec.end();vpos++)
	{
		cDir = vpos->PosiDirection -2;
		if (!strcmp(pTrade->InstrumentID,vpos->InstrumentID)&& (pTrade->Direction ==cDir) && (pTrade->OffsetFlag==THOST_FTDC_OF_Open))
		{
			bExist = true;
			GetfRateByInst(pTrade->InstrumentID,THOST_FTDC_OF_Open,dfRByM,dfRByV);
			GetInstMgr(pTrade->InstrumentID,vpos->PosiDirection,vpos->MarginRateByMoney,vpos->MarginRateByVolume);
			dFee = dfRByM * iMul * pTrade->Price * pTrade->Volume + dfRByV * pTrade->Volume;
			vpos->Commission += dFee;

			if (pDlg) { pDlg->m_pTdAcc->Commission += dFee; }
			vpos->Position += pTrade->Volume;
			vpos->OpenVolume += pTrade->Volume;
			vpos->TodayPosition += pTrade->Volume;
			vpos->PositionCost += pTrade->Volume * pTrade->Price * iMul;
			vpos->UseMargin += (pTrade->Volume * pTrade->Price * iMul*vpos->MarginRateByMoney+pTrade->Volume*vpos->MarginRateByVolume);

			break;
		}
		if(!strcmp(pTrade->InstrumentID,vpos->InstrumentID)&& (pTrade->Direction ==(THOST_FTDC_D_Buy+THOST_FTDC_D_Sell-cDir)) &&
			(pTrade->OffsetFlag!=THOST_FTDC_OF_Open))
		{
			bExist = true;
			iFlag = (vpos->PosiDirection == THOST_FTDC_PD_Long)?1:(-1);
			//上次持仓成本
			dLastCost = vpos->PositionCost;

			vpos->Position -= pTrade->Volume;
			vpos->OpenVolume -= pTrade->Volume;
			vpos->CloseVolume += pTrade->Volume;
			vpos->CloseAmount += pTrade->Price*pTrade->Volume*iMul;
					
			//无昨仓则平的全是今仓
			if(vpos->YdPosition==0)
			{ 
				iTdDelta = pTrade->Volume; 
				cFlag = THOST_FTDC_OF_CloseToday;
			}
			else
			{ 
				iTdDelta = 0;
				cFlag = THOST_FTDC_OF_Close;
			}	
			vpos->TodayPosition -= iTdDelta;

			GetfRateByInst(pTrade->InstrumentID,cFlag,dfRByM,dfRByV);
			dFee = dfRByM * iMul * pTrade->Price * pTrade->Volume + dfRByV * pTrade->Volume;
			vpos->Commission += dFee;
	
			vpos->PositionCost = GetCurPosOpenCost(&(*vpos),iMul);
			GetInstMgr(pTrade->InstrumentID,vpos->PosiDirection,vpos->MarginRateByMoney,vpos->MarginRateByVolume);
			vpos->UseMargin = (vpos->PositionCost*vpos->MarginRateByMoney+vpos->Position*vpos->MarginRateByVolume);
			//逐笔平仓盈亏
			dClosePro = iFlag*(iMul * pTrade->Price * pTrade->Volume-(dLastCost-vpos->PositionCost));

			//这里是否准确?!
			if (pDlg) { 
				pDlg->m_pTdAcc->Commission += dFee;
				pDlg->m_pTdAcc->CloseProfit += dClosePro; 
			}
			vpos->CloseProfit += dClosePro;
			///逐日盯市平仓盈亏
			vpos->CloseProfitByDate += dClosePro;
			///逐笔对冲平仓盈亏
			vpos->CloseProfitByTrade += dClosePro;

			if(vpos->Position==0) { 
                m_InvPosVec.erase(vpos); 
                if(m_InvPosVec.size()==0){ ClearVec(m_InvPosVec); }
			}
			break;
		}
	}

	//检索不到就建立新仓
	if(!bExist)
	{
		GetfRateByInst(pTrade->InstrumentID,THOST_FTDC_OF_Open,dfRByM,dfRByV);
		dFee = dfRByM * iMul * pTrade->Price * pTrade->Volume + dfRByV * pTrade->Volume;
		if (pDlg){ pDlg->m_pTdAcc->Commission += dFee; }

		CThostFtdcInvestorPositionField newPos;
		ZeroMemory(&newPos,sizeof(CThostFtdcInvestorPositionField));
		strcpy(newPos.TradingDay,pTrade->TradingDay);
		strcpy(newPos.InstrumentID,pTrade->InstrumentID);
		strcpy(newPos.BrokerID,pTrade->BrokerID);
		strcpy(newPos.InvestorID,pTrade->InvestorID);
		newPos.Commission = dFee;
		newPos.PosiDirection = pTrade->Direction+2;
		newPos.HedgeFlag = pTrade->HedgeFlag;
		newPos.PositionDate = THOST_FTDC_PSD_Today;
		newPos.Position = pTrade->Volume;
		newPos.TodayPosition = pTrade->Volume;
		newPos.OpenVolume = pTrade->Volume;
		newPos.OpenAmount = pTrade->Volume * pTrade->Price * iMul;
		newPos.PositionCost = pTrade->Volume * pTrade->Price * iMul;
		GetSettPx(pTrade->InstrumentID,newPos.PreSettlementPrice,newPos.SettlementPrice);
		GetInstMgr(newPos.InstrumentID,newPos.PosiDirection,newPos.MarginRateByMoney,newPos.MarginRateByVolume);

		newPos.UseMargin = pTrade->Volume*newPos.MarginRateByVolume +
			pTrade->Volume*pTrade->Price*iMul*newPos.MarginRateByMoney;

		newPos.ExchangeMargin = pTrade->Volume*newPos.MarginRateByVolume +
			pTrade->Volume*pTrade->Price*iMul*newPos.MarginRateByMoney;

		m_InvPosVec.push_back(newPos);
	}

	////////////////////////刷新各List////////////////////////////////
	VIT_td vtd = m_tradeVec.end()-1; //此处注意会不会有溢出
	vtd->dFee = dFee;
	///////////成交均价///////////////////
	VIT_ord vod=find_if(m_orderVec.begin(), m_orderVec.end(), 
		bind2nd(ptr_fun(findSeqNo),pTrade->BrokerOrderSeq));
	if (vod!=m_orderVec.end()){ vod->dAvgPx = GetTdAvgPx(pTrade->BrokerOrderSeq); }
	
	if (pDlg){
		pDlg->m_xLst[ORDER_IDX].SetItemCountEx(static_cast<int>(m_orderVec.size()));
		if(pDlg->m_xLst[ORDER_IDX].IsWindowVisible()) { pDlg->m_xLst[ORDER_IDX].Invalidate(); }
			
		pDlg->m_xLst[TRADE_IDX].SetItemCountEx(static_cast<int>(m_tradeVec.size()));
		if(pDlg->m_xLst[TRADE_IDX].IsWindowVisible()) { pDlg->m_xLst[TRADE_IDX].Invalidate(); }
	
		pDlg->m_xLst[INVPOS_IDX].SetItemCountEx(static_cast<int>(m_InvPosVec.size()));
		if(pDlg->m_xLst[INVPOS_IDX].IsWindowVisible()) { pDlg->m_xLst[INVPOS_IDX].Invalidate(); }
	}
}

///////////////////////成交通知///////////////////////////
void CtpTdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if(m_bAutoClose) { GenCloseFromTd(pTrade,m_iPxTick); }
	
	UpdatePosByTd(g_Dlg,pTrade);
	
	//if(g_s.m_bWarnSnd) { PlaySnd(g_s.m_szTdOk); } 
}

////////////////////////////////////////////////////////////////////
int CtpTdSpi::FindInstMul(const TThostFtdcInstrumentIDType instId)
{
	int iMul = INSERT_OP;
	
	string strkey(instId);
	MIT_if mif= m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) {  iMul = mif->second.iinf.VolumeMultiple;}
	
	return iMul;
}

//////////////////////////////////////////////////////////////////////
double CtpTdSpi::GetTdAvgPx(TThostFtdcSequenceNoType BkrOrdSeq)
{
	int iVol=0;
	double dPrice=0,dAmt=0;
	
	for (VIT_td vtd=m_tradeVec.begin();vtd != m_tradeVec.end();vtd++)
	{
		if (vtd->trade.BrokerOrderSeq==BkrOrdSeq)
		{
			dAmt += vtd->trade.Volume * vtd->trade.Price;
			iVol += vtd->trade.Volume;
		}
	}
	
	if (iVol>=1){ dPrice = dAmt/iVol; }
	
	return dPrice;
}

double CtpTdSpi::GetFrozenMgr(const PCORDFEx pOrder,BKRTDPARA& TdPara)
{
	double dMgrFrz=0,dMgrPx = GetMgrPx(TdPara,pOrder);
	string strkey = pOrder->Ord.InstrumentID;
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		if (JgCancelStat(pOrder))
		{
			if (pOrder->Ord.CombOffsetFlag[0]==THOST_FTDC_OF_Open)
			{
				if (pOrder->Ord.Direction==THOST_FTDC_D_Buy)
				{
					dMgrFrz = mif->second.LongMgrByMoney * mif->second.iinf.VolumeMultiple * dMgrPx * pOrder->Ord.VolumeTotal + 
						mif->second.LongMgrByVolume * pOrder->Ord.VolumeTotal;
				}
				else
				{
					dMgrFrz = mif->second.ShortMgrByMoney * mif->second.iinf.VolumeMultiple * dMgrPx * pOrder->Ord.VolumeTotal + 
						mif->second.ShortMgrByVolume * pOrder->Ord.VolumeTotal;
				}
			}
		}
	}
	
	return dMgrFrz;
}

double CtpTdSpi::GetTdFee(const CThostFtdcTradeField* pTrade)
{
	double dFee=0;
	string strkey = pTrade->InstrumentID;
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		//和期权手续费计算合并
		//if( mif->second.iinf.ProductClass ==THOST_FTDC_PC_Futures || 
		//	mif->second.iinf.ProductClass ==THOST_FTDC_PC_Options ||
		//	mif->second.iinf.ProductClass ==THOST_FTDC_PC_SpotOption)
		//{
		switch (pTrade->OffsetFlag)
		{
            case THOST_FTDC_OF_Open:
                dFee = mif->second.fee.OpenRatioByMoney * mif->second.iinf.VolumeMultiple * pTrade->Price * pTrade->Volume + 
                    mif->second.fee.OpenRatioByVolume * pTrade->Volume;
                break;
            case THOST_FTDC_OF_Close:
            case THOST_FTDC_OF_CloseYesterday:
                dFee = mif->second.fee.CloseRatioByMoney * mif->second.iinf.VolumeMultiple * pTrade->Price * pTrade->Volume + 
                    mif->second.fee.CloseRatioByVolume * pTrade->Volume;
                break;
            case THOST_FTDC_OF_CloseToday:
                dFee = mif->second.fee.CloseTodayRatioByMoney * mif->second.iinf.VolumeMultiple * pTrade->Price * pTrade->Volume + 
                    mif->second.fee.CloseTodayRatioByVolume * pTrade->Volume;
                break;
            default:
                break;
		}
		//}
	}
	return dFee;
}

double CtpTdSpi::GetMgrPx(BKRTDPARA& TdPara,const PCORDFEx pOrder)
{
	double dMgrPx = 0;
	string strkey = pOrder->Ord.InstrumentID;
	MIT_md vmd = m_DepthMdMap.find(strkey);
	if (vmd != m_DepthMdMap.end())
	{
		switch (TdPara.MarginPriceType)
		{
            case THOST_FTDC_MPT_PreSettlementPrice:
                dMgrPx = vmd->second.PreSettlementPrice;
                break;
            case THOST_FTDC_MPT_SettlementPrice:
                dMgrPx = vmd->second.LastPrice;
                break;
            case THOST_FTDC_MPT_AveragePrice: //此处应该是成交均价?
                dMgrPx = vmd->second.AveragePrice;
                break;
            default:
                dMgrPx = pOrder->Ord.LimitPrice;
                break;
		}
	}
	return dMgrPx;
}

void CtpTdSpi::GetFrozMgrFee(const PCORDFEx pOrder,BKRTDPARA& TdPara,double& dMgrFrz,double& dFeeFrz)
{
	dMgrFrz=0,dFeeFrz=0;
	double dMgrPx = GetMgrPx(TdPara,pOrder);
	
	string strkey(pOrder->Ord.InstrumentID);
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		if (JgCancelStat(pOrder))
		{
			switch(pOrder->Ord.CombOffsetFlag[0])
			{
                case THOST_FTDC_OF_Open:
                    if (pOrder->Ord.Direction==THOST_FTDC_D_Buy)
                    {
                        dMgrFrz = mif->second.LongMgrByMoney * mif->second.iinf.VolumeMultiple * dMgrPx * pOrder->Ord.VolumeTotal + 
                            mif->second.LongMgrByVolume * pOrder->Ord.VolumeTotal;
                    }
                    else
                    {
                        dMgrFrz = mif->second.ShortMgrByMoney * mif->second.iinf.VolumeMultiple * dMgrPx * pOrder->Ord.VolumeTotal + 
                            mif->second.ShortMgrByVolume * pOrder->Ord.VolumeTotal;
                    }
                    
                    dFeeFrz = mif->second.fee.OpenRatioByMoney * mif->second.iinf.VolumeMultiple * pOrder->Ord.LimitPrice * pOrder->Ord.VolumeTotal + 
                        mif->second.fee.OpenRatioByMoney * pOrder->Ord.VolumeTotal;
                    break;
                case THOST_FTDC_OF_Close:
                case THOST_FTDC_OF_CloseYesterday:
                    dFeeFrz = mif->second.fee.CloseRatioByMoney * mif->second.iinf.VolumeMultiple * pOrder->Ord.LimitPrice * pOrder->Ord.VolumeTotal + 
                        mif->second.fee.CloseRatioByVolume * pOrder->Ord.VolumeTotal;
                    
                    break;
                case THOST_FTDC_OF_CloseToday:
                    dFeeFrz = mif->second.fee.CloseTodayRatioByMoney * mif->second.iinf.VolumeMultiple * pOrder->Ord.LimitPrice * pOrder->Ord.VolumeTotal + 
                        mif->second.fee.CloseTodayRatioByVolume * pOrder->Ord.VolumeTotal;
                    break;
                default:
                    break;
			}
		}
	}
}

//因为infmap是静态的无需更新 这样获取数据更可靠!
void CtpTdSpi::GetUpLoLmt(TThostFtdcInstrumentIDType instId,TThostFtdcPriceType& dUpLmt,TThostFtdcPriceType& dLoLmt)
{
	dLoLmt = 0.1;
	dUpLmt = 1;
	
	string strkey(instId);
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		dLoLmt = mif->second.LowLmtPx;
		dUpLmt = mif->second.UpLmtPx;
	}	
}

void CtpTdSpi::GetMdByInst(TThostFtdcInstrumentIDType instId,CThostFtdcDepthMarketDataField* pMd)
{
	string strkey(instId);
	MIT_md vmd = m_DepthMdMap.find(strkey);
	if(vmd != m_DepthMdMap.end()) { memcpy(pMd,&(vmd->second),sizeof(CThostFtdcDepthMarketDataField)); }
}

void CtpTdSpi::GetLmtMktVol(TThostFtdcInstrumentIDType instId,TThostFtdcVolumeType& iLmt,TThostFtdcVolumeType& iMkt)
{
	iLmt=1;
	iMkt=1;
	string strkey(instId);
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		iLmt = mif->second.iinf.MaxLimitOrderVolume;
		iMkt = mif->second.iinf.MaxMarketOrderVolume;
	}
}

BOOL CtpTdSpi::JgExhByInst(const TThostFtdcInstrumentIDType szInst,TThostFtdcExchangeIDType ExhId)
{
	string strkey = szInst;
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		if(!strcmp(mif->second.iinf.ExchangeID,ExhId))
		{ return TRUE; }
	}
	
	return FALSE;
}

//此函数有问题,因为预埋撤单的sysid可能不是order的报单编号
void CtpTdSpi::GetValBySysId(TThostFtdcOrderSysIDType ordSysId,int& iVol,TThostFtdcOffsetFlagType& cOff,
		TThostFtdcDirectionType& cDir,TThostFtdcOrderPriceTypeType& cPx,TThostFtdcHedgeFlagType& cHedge)
{
	cOff = THOST_FTDC_OF_Open; 
	cDir = THOST_FTDC_D_Buy;
	iVol = 0;
	cPx = THOST_FTDC_OPT_LimitPrice;
	VIT_ord vod = find_if(m_orderVec.begin(), m_orderVec.end(), bind2nd(ptr_fun(findbySysID),ordSysId));
	if (vod != m_orderVec.end()) 
	{ 
		cOff = vod->Ord.CombOffsetFlag[0]; 
		cDir = vod->Ord.Direction;
		iVol = vod->Ord.VolumeTotalOriginal;
		cPx = vod->Ord.OrderPriceType;
		cHedge = vod->Ord.CombHedgeFlag[0];
	}
}

void CtpTdSpi::GetExhByInst(TThostFtdcInstrumentIDType szInst,TThostFtdcExchangeIDType sExh)
{
	string strkey = szInst;
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{ strcpy(sExh,mif->second.iinf.ExchangeID); }
	else
	{ ZeroMemory(sExh,sizeof(TThostFtdcExchangeIDType)); }
}

BOOL CtpTdSpi::IsValidInst(CString szInst,PINSINFEX pInf)
{
	TThostFtdcInstrumentIDType instId;
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)szInst,instId);
	
	string strkey(instId);
	MIT_if mif= m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{
		memcpy(pInf,&mif->second,sizeof(INSINFEX));
		return TRUE;
	}
	
	return FALSE;
}

void CtpTdSpi::GetSettPx(const TThostFtdcInstrumentIDType instId,double& dprePx,double& dlastPx)
{
	dprePx = 0.0; dlastPx = 0.0;
	string strkey(instId);
	MIT_md mmd = m_DepthMdMap.find(strkey);
	if(mmd!=m_DepthMdMap.end())
	{ 
		dprePx = mmd->second.PreSettlementPrice; 
		dlastPx = mmd->second.SettlementPrice; 
	}
}

//此处注意 是手动选择cFlag 而不是成交里的标记!
void CtpTdSpi::GetfRateByInst(const TThostFtdcInstrumentIDType instId,TThostFtdcOffsetFlagType cFlag,
			TThostFtdcRatioType& dfRByM,TThostFtdcRatioType& dfRByV)
{
	dfRByM=0;
	dfRByV=0;
	string strkey(instId);
	MIT_if mif= m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{  
		switch (cFlag)
		{
		case THOST_FTDC_OF_Open:
			dfRByM = mif->second.fee.OpenRatioByMoney;
			dfRByV = mif->second.fee.OpenRatioByVolume;
			break;
		case THOST_FTDC_OF_Close:
		case THOST_FTDC_OF_CloseYesterday:
			dfRByM = mif->second.fee.CloseRatioByMoney;
			dfRByV = mif->second.fee.CloseRatioByVolume;
			break;
		case THOST_FTDC_OF_CloseToday:	
			dfRByM = mif->second.fee.CloseTodayRatioByMoney;
			dfRByV = mif->second.fee.CloseTodayRatioByVolume;
			break;
		}
	}
}

void CtpTdSpi::GetInstMgr(const TThostFtdcInstrumentIDType instId,TThostFtdcPosiDirectionType posDir,
				TThostFtdcRatioType& dMgrByM,TThostFtdcRatioType& dMgrByV)
{
	dMgrByM = 0.0;
	dMgrByV = 0.0;
	string strkey(instId);
	MIT_if mif= m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{  
		dMgrByM = (posDir==THOST_FTDC_PD_Long)?mif->second.LongMgrByMoney:mif->second.ShortMgrByMoney;
		dMgrByV = (posDir==THOST_FTDC_PD_Long)?mif->second.LongMgrByVolume:mif->second.ShortMgrByVolume;
	}
}

//需要考虑持有隔夜单!只能在OnRtnTrade里调用保证tdvec是最新,未被修改
//今仓按FIFO计算 昨仓按结算价计成本
double CtpTdSpi::GetCurPosOpenCost(const CThostFtdcInvestorPositionField* pInv,int iMul)
{
	double dCost=0.0;
	int iOffset=0,iVol=0,iPos = pInv->Position,iTdPos = pInv->TodayPosition;
	int iYdPos = iPos-iTdPos;
	if (iPos<1) { return dCost; }

	if (iTdPos>0)
	{
		TThostFtdcDirectionType  tDir = pInv->PosiDirection-2;
		VRIT_td iter=m_tradeVec.rbegin();
		for(iter;iter!=m_tradeVec.rend();iter++)
		{	//合约匹配
			if((!strcmp(iter->trade.InstrumentID,pInv->InstrumentID)) && 
				(iter->trade.OffsetFlag==THOST_FTDC_OF_Open) && (iter->trade.Direction==tDir))
			{
				dCost += iter->trade.Volume*iMul*iter->trade.Price;
				iVol += iter->trade.Volume;
				if(iVol>=iTdPos)
				{
					iOffset = iVol-iTdPos;
					dCost -= iOffset*iMul*iter->trade.Price;
					break;
				}
			}
		}
	}

	if (iYdPos>0){ dCost += iYdPos*iMul*pInv->PreSettlementPrice; }

	return dCost;
}

TThostFtdcPriceType CtpTdSpi::GetPxTick(TThostFtdcInstrumentIDType instId)
{
	TThostFtdcPriceType dPxTick=0;
	string strkey(instId);
	MIT_if mif = m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end())  { dPxTick = mif->second.iinf.PriceTick; }
	
	return dPxTick;
}

#ifdef _REAL_CTP_
void CtpTdSpi::GetOrdfRByInst(TThostFtdcInstrumentIDType instId,TThostFtdcRatioType& dOrdByV,
			TThostFtdcRatioType& dDelByV)
{
	dOrdByV=0;
	dDelByV=0;
	string strkey(instId);
	MIT_if mif= m_InsinfMap.find(strkey);
	if(mif != m_InsinfMap.end()) 
	{  
		dOrdByV = mif->second.fee.OrderCommByVolume;
		dDelByV = mif->second.fee.OrderActionCommByVolume;
	}
}
#endif

BOOL CtpTdSpi::InitMgrFee()
{
	char szPath[MAX_PATH],szMgr[MAX_PATH],szFee[MAX_PATH],szOdf[MAX_PATH];

	//mgr和fee文件
	sprintf(szMgr,MGR_XML,m_sUid);
	sprintf(szFee,FEE_XML,m_sUid);
	sprintf(szOdf,ORDFEE_XML,m_sUid);
	
	xml_document doc;
	xml_node proot,pFtr,pOpt,nodeFees,nodeMgrs;
	xml_parse_result result;

	GetCurDir(szPath,szFee);
	result = doc.load_file(szPath,parse_full);
	if (result.status!=status_ok) 
	{
		ShowErroTips(IDS_LOADFEE_ERR,IDS_MY_TIPS);
		return FALSE;
	}

	proot = doc.child("Fees");

	char InvRange[4];
	//注意所有同品种合约手续费一样
	FEEREX fRate;
	string strkey;
	for (nodeFees=proot.first_child();nodeFees;nodeFees=nodeFees.next_sibling())
	{
		strcpy(fRate.fee.BrokerID, proot.attribute("BkrID").value());
		strcpy(fRate.fee.InvestorID, proot.attribute("InvID").value());
		strcpy(InvRange, proot.attribute("InvRange").value());
		fRate.fee.InvestorRange = InvRange[0];

		strcpy(fRate.fee.InstrumentID, nodeFees.attribute("PID").value());
		fRate.fee.OpenRatioByMoney = nodeFees.attribute("OpenByM").as_double();
		fRate.fee.OpenRatioByVolume = nodeFees.attribute("OpenByV").as_double();
		fRate.fee.CloseRatioByMoney = nodeFees.attribute("CloseByM").as_double();
		fRate.fee.CloseRatioByVolume = nodeFees.attribute("CloseByV").as_double();
		fRate.fee.CloseTodayRatioByMoney = nodeFees.attribute("ClosetByM").as_double();
		fRate.fee.CloseTodayRatioByVolume = nodeFees.attribute("ClosetByV").as_double();

		fRate.StrikeRatioByMoney = nodeFees.attribute("StrikeByM").as_double();
		fRate.StrikeRatioByVolume = nodeFees.attribute("StrikeByV").as_double();
		strkey = fRate.fee.InstrumentID;
		m_FeeRateMap.insert(make_pair(strkey,fRate));
	}

	////////////////////////////////////////////////////////////////
	GetCurDir(szPath,szMgr);
	result = doc.load_file(szPath,parse_full);
	if (result.status!=status_ok) 
	{
		ShowErroTips(IDS_LOADMGR_ERR,IDS_MY_TIPS);
		return FALSE;
	}
	
	proot = doc.child("Mgrs");
	pFtr = proot.child("Futures");
	pOpt = proot.child("Options");
	//注意同品种合约保证金可能不一样!
	FTMGR MgRate;
	OPMGR OpMgr;
	for (nodeMgrs=pFtr.first_child();nodeMgrs;nodeMgrs=nodeMgrs.next_sibling())
	{
		strcpy(MgRate.BrokerID, proot.attribute("BkrID").value());
		strcpy(MgRate.InvestorID, proot.attribute("InvID").value());
		strcpy(InvRange, proot.attribute("InvRange").value());
		MgRate.InvestorRange = InvRange[0];
		strcpy(InvRange, proot.attribute("Flag").value());
		MgRate.HedgeFlag = InvRange[0];
		
		strcpy(MgRate.InstrumentID, nodeMgrs.attribute("InstID").value());
		MgRate.LongMarginRatioByMoney = nodeMgrs.attribute("LongByM").as_double();
		MgRate.LongMarginRatioByVolume = nodeMgrs.attribute("LongByV").as_double();
		MgRate.ShortMarginRatioByMoney = nodeMgrs.attribute("ShortByM").as_double();
		MgRate.ShortMarginRatioByVolume = nodeMgrs.attribute("ShortByV").as_double();
		MgRate.IsRelative = 0;
		
		strkey = MgRate.InstrumentID;
		m_MgrFtMap.insert(make_pair(strkey,MgRate));
	}

	for (nodeMgrs=pOpt.first_child();nodeMgrs;nodeMgrs=nodeMgrs.next_sibling())
	{
		strcpy(OpMgr.BrokerID, proot.attribute("BkrID").value());
		strcpy(OpMgr.InvestorID, proot.attribute("InvID").value());
		strcpy(InvRange, proot.attribute("Flag").value());
		OpMgr.HedgeFlag = InvRange[0];

		strcpy(OpMgr.InstrumentID,nodeMgrs.attribute("InstID").value());
		OpMgr.FixedMargin = nodeMgrs.attribute("Fixed").as_double();
		OpMgr.MiniMargin = nodeMgrs.attribute("Mini").as_double();
		OpMgr.Royalty = nodeMgrs.attribute("Royalty").as_double();
		OpMgr.ExchFixedMargin = nodeMgrs.attribute("ExhFixed").as_double();
		OpMgr.ExchMiniMargin = nodeMgrs.attribute("ExhMini").as_double();

		strkey = OpMgr.InstrumentID;
		m_MgrOptMap.insert(make_pair(strkey,OpMgr));
	}

	#ifdef _REAL_CTP_
	GetCurDir(szPath,szOdf);
	result = doc.load_file(szPath,parse_full);
	if (result.status!=status_ok) 
	{
		ShowErroTips(IDS_LOADFEE_ERR,IDS_MY_TIPS);
		return FALSE;
	}
	
	proot = doc.child("Fees");
	//注意所有同品种合约手续费一样
	for (nodeFees=proot.first_child();nodeFees;nodeFees=nodeFees.next_sibling())
	{
		fRate.OrderCommByVolume = nodeFees.attribute("OrdByVol").as_double();
		fRate.OrderActionCommByVolume = nodeFees.attribute("DelByVol").as_double();
		strkey = nodeFees.attribute("PID").value();
		MIT_fee mfe = m_FeeRateMap.find(strkey);
		if (mfe != m_FeeRateMap.end())
		{
			mfe->second.OrderCommByVolume = fRate.OrderCommByVolume;
			mfe->second.OrderActionCommByVolume = fRate.OrderActionCommByVolume;
		}
	}

	#endif
/////////////////////////////////////////////////////////////////

	return TRUE;
}

//把额外的InstInfEx数据从行情和手续费保证金率里读取
void CtpTdSpi::FiltInfEx(INSINFEX& InfEx)
{
	int iMul =1;
	
	string strkey;
	double dAmt = 0;
	strkey = InfEx.iinf.InstrumentID;
	MIT_mgr mmgr = m_MgrFtMap.find(strkey);
	if(mmgr != m_MgrFtMap.end())
	{
		InfEx.LongMgrByMoney = mmgr->second.LongMarginRatioByMoney;
		InfEx.LongMgrByVolume = mmgr->second.LongMarginRatioByVolume;
		InfEx.ShortMgrByMoney = mmgr->second.ShortMarginRatioByMoney;
		InfEx.ShortMgrByVolume = mmgr->second.ShortMarginRatioByVolume;
	}
	////////////////////////////////////////
	strkey = InfEx.iinf.ProductID;
	MIT_fee mfe = m_FeeRateMap.find(strkey);
	if(mfe != m_FeeRateMap.end())
	{
		InfEx.fee.OpenRatioByMoney = mfe->second.fee.OpenRatioByMoney;
		InfEx.fee.OpenRatioByVolume = mfe->second.fee.OpenRatioByVolume;
		InfEx.fee.CloseRatioByMoney = mfe->second.fee.CloseRatioByMoney;
		InfEx.fee.CloseRatioByVolume = mfe->second.fee.CloseRatioByVolume;
		InfEx.fee.CloseTodayRatioByMoney = mfe->second.fee.CloseTodayRatioByMoney;
		InfEx.fee.CloseTodayRatioByVolume = mfe->second.fee.CloseTodayRatioByVolume;
		InfEx.fee.StrikeRatioByMoney = mfe->second.StrikeRatioByMoney;
		InfEx.fee.StrikeRatioByVolume = mfe->second.StrikeRatioByVolume;
	#ifdef _REAL_CTP_
		InfEx.fee.OrderCommByVolume = mfe->second.OrderCommByVolume;
		InfEx.fee.OrderActionCommByVolume = mfe->second.OrderActionCommByVolume;
	#endif
	}
	////////////////////////////////////////
	strkey = InfEx.iinf.InstrumentID;
	iMul = InfEx.iinf.VolumeMultiple;
	MIT_md mmd = m_DepthMdMap.find(strkey);
	if(mmd!=m_DepthMdMap.end())
	{
		if (!strcmp(mmd->second.ExchangeID,_CZCE)) 
		{ mmd->second.Turnover *= iMul; }
		else
		{ mmd->second.AveragePrice /= iMul; }
		
		if(mmd->second.Turnover<NEARZERO||mmd->second.Turnover==DBL_MAX)
		{ dAmt = mmd->second.OpenInterest*iMul*mmd->second.PreSettlementPrice; }
		else
		{ dAmt = mmd->second.Turnover; }
			
		InfEx.OpenInterest = mmd->second.OpenInterest;
		InfEx.Turnover = dAmt;
		InfEx.UpLmtPx = mmd->second.UpperLimitPrice;
		InfEx.LowLmtPx = mmd->second.LowerLimitPrice;
			
		gv_inf.push_back(InfEx);		
	}
	////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////
void CtpTdSpi::OnFrontDisconnected(int nReason)
{
	if (g_Dlg) { ::PostMessage(g_Dlg->m_hWnd,WM_TDISCON_MSG,0,0); }
}

void CtpTdSpi::OnHeartBeatWarning(int nTimeLapse)
{

}

///请求查询交易编码
void CtpTdSpi::ReqQryTdCode()
{
	CThostFtdcQryTradingCodeField req;
	ZeroMemory(&req, sizeof(req));

	//req.ClientIDType = THOST_FTDC_CIDT_Speculation;
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQryTradingCode(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pTradingCode )
	{ m_TdCodeVec.push_back(*pTradingCode); }
	else
	{  if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

  if(bIsLast) SetEvent(g_hEvent);
}

///请求查询合约保证金率
void CtpTdSpi::ReqQryInstMgr(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentMarginRateField req;

	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) {strcpy(req.InstrumentID, instId);}	
	req.HedgeFlag = THOST_FTDC_HF_Speculation;

	while (true)
	{
		int iRet =	m_pApi->ReqQryInstrumentMarginRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInstrumentMarginRate )
	{ memcpy(&m_MargRateRev,  pInstrumentMarginRate, sizeof(CThostFtdcInstrumentMarginRateField));	}
	else
	{ ZeroMemory(&m_MargRateRev,sizeof(CThostFtdcInstrumentMarginRateField)); }
	
    if(bIsLast) SetEvent(g_hEvent);
}

///请求查询合约手续费率
void CtpTdSpi::ReqQryInstFee(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentCommissionRateField req;
	
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL){strcpy(req.InstrumentID, instId);}	
	while (true)
	{
		int iRet = m_pApi->ReqQryInstrumentCommissionRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInstrumentCommissionRate )
	{
		memcpy(&m_FeeRateRev,  pInstrumentCommissionRate, sizeof(CThostFtdcInstrumentCommissionRateField)); 	
	}
	else
	{ ZeroMemory(&m_FeeRateRev,sizeof(FEEREX)); }
  if(bIsLast) SetEvent(g_hEvent);

}

///请求查询报单手续费
#ifdef _REAL_CTP_
void CtpTdSpi::ReqQryInstOrdCommRate(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentOrderCommRateField req;
	ZeroMemory(&req,sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if(instId != NULL) { strcpy(req.InstrumentID,instId); }

	while (true)
	{
		int iRet = m_pApi->ReqQryInstrumentOrderCommRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

void CtpTdSpi::OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInstrumentOrderCommRate )
	{ memcpy(&m_OrdfRate,pInstrumentOrderCommRate,sizeof(CThostFtdcInstrumentOrderCommRateField)); }
	else
	{ ZeroMemory(&m_OrdfRate,sizeof(CThostFtdcInstrumentOrderCommRateField)); }

	if(bIsLast)
	{  
		//SendNotifyMessage(HWND_BROADCAST,WM_QRYORDFRATE_MSG,0,0);
		SetEvent(g_hEvent); 
	}	
}
#endif

/////////////行权报单//////////////////////////////////////
void CtpTdSpi::ReqExecOrdInsert(TThostFtdcInstrumentIDType instId,TThostFtdcVolumeType iVol,
	TThostFtdcOffsetFlagType tOc,TThostFtdcHedgeFlagType cTb,TThostFtdcActionTypeType cAct,
	TThostFtdcPosiDirectionType	cPosDir,TThostFtdcExecOrderPositionFlagType	cRev,
	TThostFtdcExecOrderCloseFlagType tClose)
{
	CThostFtdcInputExecOrderField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	strcpy(req.InstrumentID, instId);
	req.Volume = iVol;
	req.OffsetFlag = tOc;
	req.HedgeFlag = cTb;
	req.ActionType = cAct;
	req.PosiDirection = cPosDir;
	req.ReservePositionFlag = cRev; //CFFEX UnRev,DCE/CZCE Rev
	req.CloseFlag = tClose; //CFFEX auto,DCE/CZCE not

	sprintf(req.ExecOrderRef, "%012d", ++m_iExcRef);
	
	m_pApi->ReqExecOrderInsert(&req,++m_iReqId);
}

///执行宣告录入请求响应
void CtpTdSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || !pInputExecOrder )
	{
		//pRspInfo->ErrorMsg
	}
	if(bIsLast) SetEvent(g_hEvent);	
}

///执行宣告通知
void CtpTdSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	
}

///执行宣告录入错误回报
void CtpTdSpi::OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pInputExecOrder))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }	
}

///做市商报价
void CtpTdSpi::ReqQtInsert(TThostFtdcInstrumentIDType instId,TThostFtdcPriceType dAskPx,TThostFtdcPriceType dBidPx,
	TThostFtdcVolumeType iAskVol,TThostFtdcVolumeType iBidVol,TThostFtdcOffsetFlagType tAskOc,
	TThostFtdcOffsetFlagType tBidOc,TThostFtdcHedgeFlagType tAsktb,TThostFtdcHedgeFlagType tBidtb)
{
	CThostFtdcInputQuoteField req;
	
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	strcpy(req.InstrumentID, instId);	
	//strcpy(req.AskOrderRef,ASK_REF);
	//strcpy(req.BidOrderRef,BID_REF);

	req.AskPrice = dAskPx;
	req.BidPrice = dBidPx;
	req.AskVolume = iAskVol;
	req.BidVolume = iBidVol;

	req.AskOffsetFlag = tAskOc;
	req.BidOffsetFlag = tBidOc;
	req.AskHedgeFlag = tAsktb;
	req.BidHedgeFlag = tBidtb;
	sprintf(req.QuoteRef, "%012d", ++m_iQtRef);

	m_pApi->ReqQuoteInsert(&req,++m_iReqId);
}
///做市商报价请求响应
void CtpTdSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || !pInputQuote )
	{
		//pRspInfo->ErrorMsg
	}
	if(bIsLast) SetEvent(g_hEvent);	
}

///做市报价通知
void CtpTdSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{

}

///报价操作请求exchid,qtsysid
void CtpTdSpi::ReqQtCancel(TThostFtdcInstrumentIDType instId,TThostFtdcOrderRefType sQtRef,
						  TThostFtdcFrontIDType iFtId,TThostFtdcSessionIDType iSesId)
{
	CThostFtdcInputQuoteActionField req;
	ZeroMemory(&req,sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.InstrumentID, instId);
	strcpy(req.QuoteRef, sQtRef); //报单引用	
	req.FrontID = iFtId;           //前置编号	
	req.SessionID = iSesId;       //会话编号
	
	req.ActionFlag = THOST_FTDC_AF_Delete; 

	m_pApi->ReqQuoteAction(&req,++m_iReqId);
}

///报价操作请求响应
void CtpTdSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || !pInputQuoteAction )
	{
		//pRspInfo->ErrorMsg	
	}
	if(bIsLast) SetEvent(g_hEvent);	
}

///客户询价请求
void CtpTdSpi::ReqForQt(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcInputForQuoteField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) { strcpy(req.InstrumentID, instId); }	

	sprintf(req.ForQuoteRef, "%012d", ++m_i4QtRef);
	
	m_pApi->ReqForQuoteInsert(&req,++m_iReqId);
}

///询价录入请求响应
void CtpTdSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || !pInputForQuote )
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
	if(bIsLast) SetEvent(g_hEvent);	
}

///询价通知
void CtpTdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) 
{

}

void CtpTdSpi::ReqQryEWOffset(TThostFtdcInstrumentIDType instId, TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryEWarrantOffsetField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) { strcpy(req.InstrumentID, instId); }	
	if (ExhId!=NULL) { strcpy(req.ExchangeID, ExhId); }	
	
	while (true)
	{
		int iRet = m_pApi->ReqQryEWarrantOffset(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询仓单折抵信息响应
void CtpTdSpi::OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pEWarrantOffset )
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			 memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			 ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryOpTdCost(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryOptionInstrTradeCostField req;
	ZeroMemory(&req, sizeof(req));
		
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL)
	{strcpy(req.InstrumentID, instId);}	
	req.HedgeFlag = THOST_FTDC_HF_Speculation;

	while (true)
	{
		int iRet = m_pApi->ReqQryOptionInstrTradeCost(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询期权交易成本响应
void CtpTdSpi::OnRspQryOptionInstrTradeCost(CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pOptionInstrTradeCost )
	{
		memcpy(&m_OpTdCost,  pOptionInstrTradeCost, sizeof(CThostFtdcOptionInstrTradeCostField)); 	
	}
	else
	{
		ZeroMemory(&m_OpTdCost,sizeof(CThostFtdcOptionInstrTradeCostField));
	}
  if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryOpFeeRate(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryOptionInstrCommRateField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL)
	{strcpy(req.InstrumentID, instId);}	

	while (true)
	{
		int iRet = m_pApi->ReqQryOptionInstrCommRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

///请求查询期权合约手续费响应
void CtpTdSpi::OnRspQryOptionInstrCommRate(CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pOptionInstrCommRate )
	{
		memcpy(&m_FeeRateRev,  pOptionInstrCommRate, sizeof(CThostFtdcOptionInstrCommRateField)); 	
	}
	else
	{
		ZeroMemory(&m_FeeRateRev,sizeof(CThostFtdcOptionInstrCommRateField));
	}
  if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryExhMgr(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryExchangeMarginRateField req;
	ZeroMemory(&req,sizeof(req));
	
	strcpy(req.BrokerID, m_sBkrId);	
	if (instId!=NULL) {strcpy(req.InstrumentID, instId);}	
	req.HedgeFlag = THOST_FTDC_HF_Speculation;
	
	while (true)
	{
		int iRet = m_pApi->ReqQryExchangeMarginRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

void CtpTdSpi::OnRspQryExchangeMarginRate(CThostFtdcExchangeMarginRateField *pExchangeMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pExchangeMarginRate )
	{
		
	}
	if(bIsLast) SetEvent(g_hEvent);
}


void CtpTdSpi::ReqQryExhMgrAdj(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryExchangeMarginRateAdjustField req;
	ZeroMemory(&req,sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);	
	if (instId!=NULL) {strcpy(req.InstrumentID, instId);}	
	req.HedgeFlag = THOST_FTDC_HF_Speculation;

	while (true)
	{
		int iRet = m_pApi->ReqQryExchangeMarginRateAdjust(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

void CtpTdSpi::OnRspQryExchangeMarginRateAdjust(CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pExchangeMarginRateAdjust )
	{

	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryExecOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
	TThostFtdcExecOrderSysIDType ExecOrdId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd)
{
	CThostFtdcQryExecOrderField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) { strcpy(req.InstrumentID, instId); }	
	if (ExhId!=NULL) { strcpy(req.ExchangeID, ExhId); }	
	if (ExecOrdId!=NULL) { strcpy(req.ExecOrderSysID, ExecOrdId); }	
	if (tmStart!=NULL) { strcpy(req.InsertTimeStart, tmStart); }	
	if (tmEnd!=NULL) { strcpy(req.InsertTimeEnd, tmEnd); }	
	
	while (true)
	{
		int iRet = m_pApi->ReqQryExecOrder(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}	
}

///请求查询执行宣告响应
void CtpTdSpi::OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pExecOrder )
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqExecCancel(TThostFtdcInstrumentIDType	instId,TThostFtdcOrderRefType OrdRef,
		TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId)
{
	CThostFtdcInputExecOrderActionField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
		
	strcpy(req.ExecOrderRef,OrdRef);
	strcpy(req.InstrumentID,instId);

	req.FrontID = iFrtId;
	req.SessionID = iSesId;
	m_pApi->ReqExecOrderAction(&req,++m_iReqId);
}

void CtpTdSpi::ReqExecCancel(TThostFtdcExchangeIDType ExhId,TThostFtdcExecOrderSysIDType sysId)
{
	CThostFtdcInputExecOrderActionField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	strcpy(req.ExecOrderSysID,sysId);
	strcpy(req.ExchangeID,ExhId);

	m_pApi->ReqExecOrderAction(&req,++m_iReqId);
}

///执行宣告操作请求响应
void CtpTdSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || !pInputExecOrderAction )
	{
		//pRspInfo->ErrorMsg
		
	}
	if(bIsLast) SetEvent(g_hEvent);
}
	///执行宣告操作错误回报
void CtpTdSpi::OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pExecOrderAction))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }	
}

//////////////////请求查询用户资料/////////////
void CtpTdSpi::ReqQryInvest()
{
	CThostFtdcQryInvestorField req;

	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQryInvestor(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

//可能需要vec来记录多条信息
void CtpTdSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInvestor )
	{
		CThostFtdcInvestorField *pInv = new CThostFtdcInvestorField;
		memcpy(pInv,pInvestor,sizeof(CThostFtdcInvestorField));
		
		PostMessage(g_Dlg->m_hWnd,WM_QRYUSER_MSG,0,(LPARAM)pInv);
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

  if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqUserPwdUpdate(TThostFtdcPasswordType szNewPass,TThostFtdcPasswordType szOldPass)
{
	CThostFtdcUserPasswordUpdateField req;

	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strcpy(req.UserID, m_sUid);
	strcpy(req.NewPassword, szNewPass);    	
	strcpy(req.OldPassword,szOldPass);  

	m_pApi->ReqUserPasswordUpdate(&req,++m_iReqId);
}

///用户口令更新请求响应
void CtpTdSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pUserPasswordUpdate )
	{ ShowErroTips(IDS_MODPASSOK,IDS_STRTIPS); }
	else
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
	if(bIsLast) SetEvent(g_hEvent);
}

//资金账户密码
void CtpTdSpi::ReqTdAccPwdUpdate(TThostFtdcPasswordType szNewPass,TThostFtdcPasswordType szOldPass)
{
	CThostFtdcTradingAccountPasswordUpdateField req;

	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));
	strcpy(req.NewPassword, szNewPass);    	
	strcpy(req.OldPassword,szOldPass);  

	m_pApi->ReqTradingAccountPasswordUpdate(&req,++m_iReqId);
}

///资金账户口令更新请求响应
void CtpTdSpi::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pTradingAccountPasswordUpdate )
	{
		ShowErroTips(IDS_MODPASSOK,IDS_STRTIPS);
	}
	else
	{
		ShowCbErrs(pRspInfo->ErrorMsg);
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryAccreg()
{
	CThostFtdcQryAccountregisterField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);	
	strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQryAccountregister(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pAccountregister)
	{
		strcpy(m_sCurIdt, pAccountregister->CurrencyID);  //币种类型 唯一吗?
		m_AccRegVec.push_back(*pAccountregister);
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryTransBk(TThostFtdcBankIDType bkId,TThostFtdcBankBrchIDType bkBrchId)
{
	CThostFtdcQryTransferBankField req;
	ZeroMemory(&req, sizeof(req));
	if(bkId != NULL) { strcpy(req.BankID,bkId); }
	if(bkBrchId != NULL) { strcpy(req.BankBrchID,bkBrchId); }

	while (true)
	{
		int iRet =	m_pApi->ReqQryTransferBank(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

void CtpTdSpi::OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pTransferBank)
	{
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryContractBk(TThostFtdcBankIDType bkId,TThostFtdcBankBrchIDType bkBrchId)
{
	CThostFtdcQryContractBankField req;

	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	

	if(bkId != NULL) { strcpy(req.BankID,bkId); }
	if(bkBrchId != NULL) { strcpy(req.BankBrchID,bkBrchId); }

	while (true)
	{
		int iRet =	m_pApi->ReqQryContractBank(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

  void CtpTdSpi::OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pContractBank)
	{

	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

  //////////////////////////////////////////期货发起银行资金转期货请求///////////////////////////////////////
void CtpTdSpi::ReqBk2FByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,
	 TThostFtdcPasswordType Pwd,TThostFtdcTradeAmountType TdAmt)
{
	CThostFtdcReqTransferField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);	 
	strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));
	strcpy(req.TradeCode,VTC_FTBK2FT);
	strcpy(req.BankBranchID,VTC_BKBRCH_ID);
	strcpy(req.CurrencyID,m_sCurIdt);
	strcpy(req.BankID,bkId);
	strcpy(req.BankPassWord,bkPwd);
	strcpy(req.Password,Pwd);
	req.TradeAmount = TdAmt;
	req.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;

	m_pApi->ReqFromBankToFutureByFuture(&req,++m_iReqId);
}
 
 ///期货发起银行资金转期货应答
void CtpTdSpi::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( IsErrorRspInfo(pRspInfo) || (!pReqTransfer))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
	//if(bIsLast) SetEvent(g_hEvent);	 
}
///期货发起银行资金转期货通知
void CtpTdSpi::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
	  if(g_Dlg/* && (!m_bReconnect)*/)
	  {
		  if(pRspTransfer->ErrorID!=0)
		  { ShowCbErrs(pRspTransfer->ErrorMsg); }
		  else
		  { 
			  CThostFtdcRspTransferField* pTrans = new CThostFtdcRspTransferField();
			  memcpy(pTrans,  pRspTransfer, sizeof(CThostFtdcRspTransferField));
			  ::PostMessage(g_Dlg->m_hWnd,WM_BK2F_MSG,0,(LPARAM)pTrans);
			  //ShowErroTips(IDS_BFTRANS_OK,IDS_STRTIPS); 
		  }
	  }
  //SetEvent(g_hEvent);
}

///期货发起银行资金转期货错误回报
void CtpTdSpi::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pReqTransfer))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
}

///////////////////////////////////////////期货发起期货资金转银行请求///////////////////////////////////////////
void CtpTdSpi::ReqF2BkByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,
			TThostFtdcPasswordType Pwd,TThostFtdcTradeAmountType TdAmt)
 {
	 CThostFtdcReqTransferField req;
	 ZeroMemory(&req, sizeof(req));
	 
	 strcpy(req.BrokerID, m_sBkrId);	 
	 strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));
	 strcpy(req.TradeCode,VTC_FTFT2BK);
	 strcpy(req.BankBranchID,VTC_BKBRCH_ID);
	 strcpy(req.CurrencyID,m_sCurIdt);
	 strcpy(req.BankID,bkId);
	 strcpy(req.BankPassWord,bkPwd);
	 strcpy(req.Password,Pwd);
	 req.TradeAmount = TdAmt;
	 req.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;
	 
	 m_pApi->ReqFromFutureToBankByFuture(&req,++m_iReqId);
 }

 ///期货发起期货资金转银行应答
 void CtpTdSpi::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
 {
	 if( IsErrorRspInfo(pRspInfo) || (!pReqTransfer))
	 { ShowCbErrs(pRspInfo->ErrorMsg); }
	// if(bIsLast) SetEvent(g_hEvent);	
 }
 
///期货发起期货资金转银行通知
void CtpTdSpi::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
	  if(g_Dlg/*&&(!m_bReconnect)*/)
	  {
		  if(pRspTransfer->ErrorID!=0)
		  { ShowCbErrs(pRspTransfer->ErrorMsg); }
		  else
		  { 
			CThostFtdcRspTransferField* pTrans = new CThostFtdcRspTransferField();
			memcpy(pTrans,  pRspTransfer, sizeof(CThostFtdcRspTransferField));
			::PostMessage(g_Dlg->m_hWnd,WM_F2BK_MSG,0,(LPARAM)pTrans);
			  //ShowErroTips(IDS_BFTRANS_OK,IDS_STRTIPS); 
		  }
	  }  
  //SetEvent(g_hEvent);
}

///期货发起期货资金转银行错误回报
void CtpTdSpi::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pReqTransfer))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
}

 ///////////////////////////////////////////////////期货发起查询银行余额请求///////////////////////////////////////////////
void CtpTdSpi::ReqQryBkAccMoneyByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,TThostFtdcPasswordType Pwd)
{
	CThostFtdcReqQueryAccountField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID, m_sBkrId);	 
	strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));
	strcpy(req.TradeCode,VTC_FTQRYBKAMT);
	strcpy(req.BankBranchID,VTC_BKBRCH_ID);
	strcpy(req.CurrencyID,m_sCurIdt);
	strcpy(req.BankID,bkId);
	strcpy(req.BankPassWord,bkPwd);
	strcpy(req.Password,Pwd);

	req.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;

	while (true)
	{
		int iRet =	m_pApi->ReqQueryBankAccountMoneyByFuture(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

///期货发起查询银行余额应答
void CtpTdSpi::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( IsErrorRspInfo(pRspInfo) || (!pReqQueryAccount))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
	//if(bIsLast) SetEvent(g_hEvent);	
	
}

///期货发起查询银行余额通知
void CtpTdSpi::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount)
{
	if(pNotifyQueryAccount->ErrorID ==0)
	{
		if (g_Dlg /*&& (!m_bReconnect)*/)
		{
			CThostFtdcNotifyQueryAccountField *pNotify = new CThostFtdcNotifyQueryAccountField();
			memcpy(pNotify,pNotifyQueryAccount,sizeof(CThostFtdcNotifyQueryAccountField));

			::PostMessage(g_Dlg->m_hWnd,WM_QRYBKYE_MSG,0,(LPARAM)pNotify);
		}
	}
	//SetEvent(g_hEvent);	
}

///期货发起查询银行余额错误回报
void CtpTdSpi::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pReqQueryAccount))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
}

///////////////////////////////////////查询转账流水////////////////////////////////////////////
/// "204005"
void CtpTdSpi::ReqQryTfSerial(TThostFtdcBankIDType bkId)
{
	CThostFtdcQryTransferSerialField req;
	ZeroMemory(&req, sizeof(req));

	strcpy(req.BrokerID, m_sBkrId);	 
	strncpy(req.AccountID, m_sUid,sizeof(TThostFtdcAccountIDType));

	strcpy(req.BankID,bkId);

	while (true)
	{
		int iRet =	m_pApi->ReqQryTransferSerial(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}
///请求查询转帐流水响应
void CtpTdSpi::OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pTransferSerial)
	{
		m_BfTransVec.push_back(*pTransferSerial);
	}
	if(bIsLast) 
	{ 
		if (g_Dlg){ SendNotifyMessage(HWND_BROADCAST,WM_QRYBFLOG_MSG,0,0); } 
		else
		{ SetEvent(g_hEvent); }			
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///系统运行时期货端手工发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CtpTdSpi::OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal)
{

}

///系统运行时期货端手工发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CtpTdSpi::OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal)
{

}

///系统运行时期货端手工发起冲正银行转期货错误回报
void CtpTdSpi::OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{

}

///系统运行时期货端手工发起冲正期货转银行错误回报
void CtpTdSpi::OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
}

///期货发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CtpTdSpi::OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal)
{
}

///期货发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CtpTdSpi::OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal)
{
}

///银行发起银期开户通知
void CtpTdSpi::OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount)
{
	if(pOpenAccount && g_Dlg)
	{
		CThostFtdcOpenAccountField* pMsg = new CThostFtdcOpenAccountField();
		memcpy(pMsg,pOpenAccount,sizeof(CThostFtdcOpenAccountField));
		
		::PostMessage(g_Dlg->m_hWnd,WM_OPENACC_MSG,0,(LPARAM)pMsg); 
	}
}

///银行发起银期销户通知
void CtpTdSpi::OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount)
{
	if(pCancelAccount && g_Dlg)
	{
		CThostFtdcCancelAccountField* pMsg = new CThostFtdcCancelAccountField();
		memcpy(pMsg,pCancelAccount,sizeof(CThostFtdcCancelAccountField));
		
		::PostMessage(g_Dlg->m_hWnd,WM_DELACC_MSG,0,(LPARAM)pMsg); 
	}
}

///银行发起变更银行账号通知
void CtpTdSpi::OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount)
{
	if(pChangeAccount && g_Dlg)
	{
		CThostFtdcChangeAccountField* pMsg = new CThostFtdcChangeAccountField();
		memcpy(pMsg,pChangeAccount,sizeof(CThostFtdcChangeAccountField));
		
		::PostMessage(g_Dlg->m_hWnd,WM_MODACC_MSG,0,(LPARAM)pMsg); 
	}
}

void CtpTdSpi::ReqQryCFMMCTdAccKey()
{
	CThostFtdcQryCFMMCTradingAccountKeyField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	 
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQryCFMMCTradingAccountKey(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pCFMMCTradingAccountKey)
	{
	/*
		char strMsg[2*MAX_PATH];
		sprintf(strMsg,CFMMC_TMPL,pCFMMCTradingAccountKey->ParticipantID,pCFMMCTradingAccountKey->AccountID,
			pCFMMCTradingAccountKey->KeyID,pCFMMCTradingAccountKey->CurrentKey);

		ShellExecuteA(NULL,"open",strMsg,NULL, NULL, SW_SHOW);
		*/
	}
	else
	{ ShowCbErrs(pRspInfo->ErrorMsg); }
	//if(bIsLast) SetEvent(g_hEvent);	
}

//请求保证金中心令牌
void CtpTdSpi::ReqQryCFMMCTdAccTok()
{
	CThostFtdcQueryCFMMCTradingAccountTokenField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);	 
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQueryCFMMCTradingAccountToken(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询CFMMC令牌响应
void CtpTdSpi::OnRspQueryCFMMCTradingAccountToken(CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(IsErrorRspInfo(pRspInfo) || !pQueryCFMMCTradingAccountToken){ ShowCbErrs(pRspInfo->ErrorMsg); }
}

///CFMMC用户令牌
void CtpTdSpi::OnRtnCFMMCTradingAccountToken(CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken)
{
	if(pCFMMCTradingAccountToken)
	{
		char strMsg[MAX_PATH];
		sprintf(strMsg,CFMMC_TMPL,pCFMMCTradingAccountToken->ParticipantID,pCFMMCTradingAccountToken->AccountID,
			pCFMMCTradingAccountToken->KeyID,pCFMMCTradingAccountToken->Token);
		//ShellExecuteA(NULL,"open",strMsg,NULL, NULL, SW_SHOW);

		TCHAR* szMsg = new TCHAR[2*MAX_PATH+2];
		ansi2uni(CP_ACP,strMsg,szMsg);
		::PostMessage(g_Dlg->m_hWnd,WM_QRYACCTOK_MSG,0,(LPARAM)szMsg); 
	}
}

void CtpTdSpi::ReqQryBkrTdParams()
{
	CThostFtdcQryBrokerTradingParamsField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));

	while (true)
	{
		int iRet =	m_pApi->ReqQryBrokerTradingParams(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pBrokerTradingParams)
	{ memcpy(&m_BkrTdPara,pBrokerTradingParams,sizeof(CThostFtdcBrokerTradingParamsField)); }
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast){ SetEvent(g_hEvent); }
}

void CtpTdSpi::ReqQryBkrTdAlgos(TThostFtdcExchangeIDType ExhId,TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryBrokerTradingAlgosField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	if(ExhId != NULL) { strcpy(req.ExchangeID, ExhId); }
	if(instId != NULL) { strcpy(req.InstrumentID, instId); }
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryBrokerTradingAlgos(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pBrokerTradingAlgos)
	{ memcpy(&m_BkrTdAlgo,pBrokerTradingAlgos,sizeof(CThostFtdcBrokerTradingAlgosField)); }
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) { SetEvent(g_hEvent); }
}

///预埋单录入请求
void CtpTdSpi::ReqParkedOrd(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, 
        TThostFtdcOffsetFlagType kpp,TThostFtdcPriceType price, TThostFtdcVolumeType vol)
{
	CThostFtdcParkedOrderField req;
	CAppCfgs& s = g_s;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.InstrumentID, instId);
	
	sprintf(req.OrderRef, "%012d", ++m_iOrdRef);
	
	req.Direction = dir;	
	req.CombOffsetFlag[0] = kpp;
	req.LimitPrice = price;	//价格
	req.VolumeTotalOriginal = vol;	///数量	

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;	
	req.TimeCondition = THOST_FTDC_TC_GFD; 
	req.VolumeCondition = THOST_FTDC_VC_AV;
	req.ContingentCondition = THOST_FTDC_CC_ParkedOrder; 
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	//req.IsAutoSuspend = 0; 
	//req.UserForceClose = 0;
	//THOST_FTDC_UT_Investor
	//THOST_FTDC_UT_Operator
	//THOST_FTDC_UT_SuperUser
	req.UserType = THOST_FTDC_UT_Investor;
	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);
	
	m_pApi->ReqParkedOrderInsert(&req, ++m_iReqId);
}

///预埋单录入请求响应
void CtpTdSpi::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pParkedOrder)
	{
        CXTraderDlg* pDlg = g_Dlg;
		PARKEDEX pkReq;
		ZeroMemory(&pkReq,sizeof(pkReq));
		memcpy(&pkReq.park,pParkedOrder,sizeof(CThostFtdcParkedOrderField));
		pkReq.ordType = PARKED_SERVER;
		
		getCurDate(pkReq.InsertDate);
		getCurTime(pkReq.InsertTime);
		m_pkExVec.push_back(pkReq);
		if(pDlg)
		{
            pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
            if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
	else
	{ 
		if (pRspInfo) 
		{
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField));
			ShowCbErrs(pRspInfo->ErrorMsg);
		}	
	}
	//if(bIsLast){ SetEvent(g_hEvent); }
}

///请求查询预埋单
void CtpTdSpi::ReqQryParkedOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryParkedOrderField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if(instId != NULL) { strcpy(req.InstrumentID,instId); }
	if(ExhId != NULL) { strcpy(req.ExchangeID,ExhId); }

	while (true)
	{
		int iRet =	m_pApi->ReqQryParkedOrder(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询预埋单响应
void CtpTdSpi::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pParkedOrder)
	{
        CXTraderDlg* pDlg = g_Dlg;
		PARKEDEX pkReq;
		ZeroMemory(&pkReq,sizeof(pkReq));
		memcpy(&pkReq.park,pParkedOrder,sizeof(CThostFtdcParkedOrderField));
		pkReq.ordType = PARKED_SERVER;
			
		getCurDate(pkReq.InsertDate);
		getCurTime(pkReq.InsertTime);

		VIT_pk vpk = m_pkExVec.begin();
		for (vpk;vpk!=m_pkExVec.end();vpk++)
		{
			if (vpk->ordType==pkReq.ordType && !strcmp(vpk->park.ParkedOrderID,pParkedOrder->ParkedOrderID))
			{ 
                *vpk = pkReq;
               break; 
			}
		}
			
		if(vpk == m_pkExVec.end()) { m_pkExVec.push_back(pkReq); }
		if (pDlg)
		{
            pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
            if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

///请求删除预埋单
void CtpTdSpi::ReqRmParkedOrd(TThostFtdcParkedOrderIDType ParkedOrdId)
{
	CThostFtdcRemoveParkedOrderField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.ParkedOrderID,ParkedOrdId);
	m_pApi->ReqRemoveParkedOrder(&req, ++m_iReqId);
}

///删除预埋单响应
void CtpTdSpi::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pRemoveParkedOrder)
	{
        CXTraderDlg* pDlg = g_Dlg;

		VIT_pk vpk = m_pkExVec.begin();
		for (vpk;vpk!=m_pkExVec.end();vpk++)
		{
			if (!strcmp(vpk->park.ParkedOrderID,pRemoveParkedOrder->ParkedOrderID))
			{ 
				vpk->park.Status = THOST_FTDC_PAOS_Deleted;
				break; 
			}
		}
		if (pDlg)
		{
			pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
			if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	//if(bIsLast) SetEvent(g_hEvent);
}

///预埋撤单录入请求 这个会报找不到合约
void CtpTdSpi::ReqParkedOrdAct(CThostFtdcOrderFieldEx* pOrd)
{
	// FrontID + SessionID + OrdRef + InstrumentID or ExchangeID + OrderSysID 
	CThostFtdcParkedOrderActionField req;
	ZeroMemory(&req,sizeof(req));
	CAppCfgs& s = g_s;

	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	if (pOrd!=NULL)
	{
		strcpy(req.InstrumentID, pOrd->Ord.InstrumentID);
		strcpy(req.ExchangeID, pOrd->Ord.ExchangeID);
		strcpy(req.OrderSysID, pOrd->Ord.OrderSysID);
	}
	
	req.ActionFlag = THOST_FTDC_AF_Delete;  //操作标志
	//THOST_FTDC_UT_Investor
	//THOST_FTDC_UT_Operator
	//THOST_FTDC_UT_SuperUser
	req.UserType = THOST_FTDC_UT_Investor;
	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);

	m_pApi->ReqParkedOrderAction(&req,++m_iReqId);
}

///预埋撤单录入请求
void CtpTdSpi::ReqParkedOrdAct2(CThostFtdcOrderFieldEx* pOrd)
{
	// FrontID + SessionID + OrdRef + InstrumentID or ExchangeID + OrderSysID 
	CThostFtdcParkedOrderActionField req;
	ZeroMemory(&req,sizeof(req));
	CAppCfgs& s = g_s;

	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	if (pOrd!=NULL)
	{
		strcpy(req.InstrumentID, pOrd->Ord.InstrumentID);
		strcpy(req.OrderRef, pOrd->Ord.OrderRef);	
		req.FrontID = pOrd->Ord.FrontID;	
        req.SessionID = pOrd->Ord.SessionID;
	}

	req.ActionFlag = THOST_FTDC_AF_Delete;  //操作标志
	//THOST_FTDC_UT_Investor
	//THOST_FTDC_UT_Operator
	//THOST_FTDC_UT_SuperUser
	req.UserType = THOST_FTDC_UT_Investor;
	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);

	m_pApi->ReqParkedOrderAction(&req,++m_iReqId);
}

///预埋撤单录入请求响应
void CtpTdSpi::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pParkedOrderAction)
	{
        CXTraderDlg* pDlg = g_Dlg;

		PARKEDEX pkReq;
		ZeroMemory(&pkReq,sizeof(pkReq));
		memcpy(&pkReq.pkAct,pParkedOrderAction,sizeof(CThostFtdcParkedOrderField));
		pkReq.ordType = PARKED_ACT;
			
		getCurDate(pkReq.InsertDate);
		getCurTime(pkReq.InsertTime);
		//if (pRspInfo) strcpy(pkReq.pkAct.ErrorMsg,pRspInfo->ErrorMsg);
			
		m_pkExVec.push_back(pkReq); 
		if (pDlg)
		{	
			pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
			if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}			
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	//if(bIsLast){ SetEvent(g_hEvent); }
}

///查询预埋撤单
void CtpTdSpi::ReqQryParkedOrdAct(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryParkedOrderActionField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	if(instId != NULL) { strcpy(req.InstrumentID,instId); }
	if(ExhId != NULL) { strcpy(req.ExchangeID,ExhId); }

	while (true)
	{
		int iRet =	m_pApi->ReqQryParkedOrderAction(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询预埋撤单响应
void CtpTdSpi::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if( !IsErrorRspInfo(pRspInfo) && pParkedOrderAction)
	{
        CXTraderDlg* pDlg = g_Dlg;
		PARKEDEX pkReq;
		ZeroMemory(&pkReq,sizeof(pkReq));
		memcpy(&pkReq.pkAct,pParkedOrderAction,sizeof(CThostFtdcParkedOrderField));
		pkReq.ordType = PARKED_ACT;
			
		getCurDate(pkReq.InsertDate);
		getCurTime(pkReq.InsertTime);

		VIT_pk vpk = m_pkExVec.begin();
		for (vpk;vpk!=m_pkExVec.end();vpk++)
		{
			if (vpk->ordType = PARKED_ACT && !strcmp(vpk->pkAct.ParkedOrderActionID,pParkedOrderAction->ParkedOrderActionID))
			{ 
				*vpk = pkReq;
				break; 
			}
		}
			
		if(vpk == m_pkExVec.end()) { m_pkExVec.push_back(pkReq); }
		if (pDlg)
		{
            pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
            if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

///请求删除 预埋撤单
void CtpTdSpi::ReqRmParkedOrdAct(TThostFtdcParkedOrderActionIDType ParkedOrdActId)
{
	CThostFtdcRemoveParkedOrderActionField  req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID, m_sUid,sizeof(TThostFtdcInvestorIDType));
	strcpy(req.ParkedOrderActionID,ParkedOrdActId);

	m_pApi->ReqRemoveParkedOrderAction(&req, ++m_iReqId);
}

///删除预埋撤单响应
void CtpTdSpi::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if( !IsErrorRspInfo(pRspInfo) && pRemoveParkedOrderAction)
	{
        CXTraderDlg* pDlg = g_Dlg;
	
		VIT_pk vpk = m_pkExVec.begin();
		for (vpk;vpk!=m_pkExVec.end();vpk++)
		{
			if (vpk->ordType == PARKED_ACT && !strcmp(vpk->pkAct.ParkedOrderActionID,pRemoveParkedOrderAction->ParkedOrderActionID))
			{ 
				vpk->pkAct.Status = THOST_FTDC_PAOS_Deleted;
				break;
			}
		}
        if (pDlg)
		{
			pDlg->m_xLst[PKCON_IDX].SetItemCountEx(static_cast<int>(m_pkExVec.size()));
			if(pDlg->m_xLst[PKCON_IDX].IsWindowVisible()) { pDlg->m_xLst[PKCON_IDX].Invalidate(); }
		}	
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	//if(bIsLast) SetEvent(g_hEvent);	
}

void CtpTdSpi::ReqQryProd(TThostFtdcInstrumentIDType prodId,TThostFtdcProductClassType prodClass)
{
	CThostFtdcQryProductField  req;
	ZeroMemory(&req, sizeof(req));

	if (prodId!=NULL){ strcpy(req.ProductID,prodId); }
	req.ProductClass = prodClass; 

	while (true)
	{
		int iRet =	m_pApi->ReqQryProduct(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询产品响应
void CtpTdSpi::OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pProduct)
	{
		
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryExhRate(TThostFtdcCurrencyIDType srcCurId,TThostFtdcCurrencyIDType	dstCurId)
{
	CThostFtdcQryExchangeRateField  req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strcpy(req.FromCurrencyID,srcCurId);
	strcpy(req.ToCurrencyID,dstCurId);

	while (true)
	{
		int iRet =	m_pApi->ReqQryExchangeRate(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

void CtpTdSpi::OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pExchangeRate)
	{
		
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);	
}

void CtpTdSpi::ReqQrySecAgentACIDMap(TThostFtdcCurrencyIDType curId)
{
	CThostFtdcQrySecAgentACIDMapField  req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strcpy(req.UserID,m_sUid);
	strncpy(req.AccountID,m_sUid,sizeof(TThostFtdcAccountIDType));
	strcpy(req.CurrencyID,curId);
	
	while (true)
	{
		int iRet =	m_pApi->ReqQrySecAgentACIDMap(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询二级代理操作员银期权限响应
void CtpTdSpi::OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pSecAgentACIDMap)
	{
		
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);	
}

///请求查询产品组
void CtpTdSpi::ReqQryProdGroup(TThostFtdcInstrumentIDType prodId,TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryProductGroupField req;
	ZeroMemory(&req, sizeof(req));

	if (prodId!=NULL){ strcpy(req.ProductID,prodId); }
	if (ExhId!=NULL){ strcpy(req.ExchangeID,ExhId); }
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryProductGroup(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

///请求查询产品组
void CtpTdSpi::OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pProductGroup)
	{
		
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);		
}

///请求查询组合合约安全系数
void CtpTdSpi::ReqQryCombInstGuard(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryCombInstrumentGuardField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	if (instId!=NULL){ strcpy(req.InstrumentID,instId); }
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryCombInstrumentGuard(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

///请求查询组合合约安全系数响应
void CtpTdSpi::OnRspQryCombInstrumentGuard(CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pCombInstrumentGuard)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);	
}

///请求查询申请组合
void CtpTdSpi::ReqQryCombAct(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcQryCombActionField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL){ strcpy(req.InstrumentID,instId); }
	if (ExhId!=NULL){ strcpy(req.ExchangeID,ExhId); }
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryCombAction(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 	
}

///请求查询申请组合响应
void CtpTdSpi::OnRspQryCombAction(CThostFtdcCombActionField *pCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pCombAction)
	{
		//pRspInfo->ErrorMsg
	}
	else
	{
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
	}

	if(bIsLast) SetEvent(g_hEvent);	
}

///中金所申报组合/拆分 CombDirection 指令方向-组合/拆分
//申报组合后享受保证金优惠 平仓前必须先申报拆分 不能直接平组合 
void CtpTdSpi::ReqCombActInsert(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType tDir,
	TThostFtdcVolumeType iVol,TThostFtdcCombDirectionType tCombDir,TThostFtdcHedgeFlagType tFlag)
{
	CThostFtdcInputCombActionField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL){ strcpy(req.InstrumentID,instId); }
	sprintf(req.CombActionRef, "%012d", ++m_iOrdRef);
	strcpy(req.ExchangeID,_CFX); 

	req.Direction = tDir;
	req.Volume = iVol;
	req.CombDirection = tCombDir;
	req.HedgeFlag = tFlag;

	while (true)
	{
		int iRet =	m_pApi->ReqCombActionInsert(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///申请组合删除请求响应
void CtpTdSpi::OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if( IsErrorRspInfo(pRspInfo) || !pInputCombAction)
	{
		
	}

	if(bIsLast) SetEvent(g_hEvent);		
}

///申请组合通知
void CtpTdSpi::OnRtnCombAction(CThostFtdcCombActionField *pCombAction)
{
	
}

///申请组合录入错误回报
void CtpTdSpi::OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo) 
{
	if( IsErrorRspInfo(pRspInfo) || (!pInputCombAction))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }	
}

///请求查询最大报单数量
void CtpTdSpi::ReqQryMaxOrdVol(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType tDir,
					TThostFtdcOffsetFlagType tOc,TThostFtdcHedgeFlagType tFlag)
{
	CThostFtdcQueryMaxOrderVolumeField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (instId!=NULL) { strcpy(req.InstrumentID,instId); }
	req.Direction = tDir;
	req.OffsetFlag = tOc;
	req.HedgeFlag = tFlag;
	
	while (true)
	{
		int iRet =	m_pApi->ReqQueryMaxOrderVolume(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///查询最大报单数量响应
void CtpTdSpi::OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pQueryMaxOrderVolume)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }

	if(bIsLast) SetEvent(g_hEvent);	
}

///请求查询投资者品种/跨品种保证金
void CtpTdSpi::ReqQryInvProdGrpMrg(TThostFtdcInstrumentIDType prodId,TThostFtdcHedgeFlagType tFlag)
{
	CThostFtdcQryInvestorProductGroupMarginField req;
	ZeroMemory(&req, sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	if (prodId!=NULL) { strcpy(req.ProductGroupID,prodId); }
	req.HedgeFlag = tFlag;
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryInvestorProductGroupMargin(&req, ++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	} 
}

///请求查询投资者品种/跨品种保证金响应
void CtpTdSpi::OnRspQryInvestorProductGroupMargin(CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInvestorProductGroupMargin)
	{
		
	}
	else
	{  
       if (pRspInfo) 
       { 
         memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
         ShowCbErrs(pRspInfo->ErrorMsg);
       } 
    }

	if(bIsLast) SetEvent(g_hEvent);	
}

///请求查询产品报价汇率
void CtpTdSpi::ReqQryProdExhRate(TThostFtdcInstrumentIDType prodId)
{
	CThostFtdcQryProductExchRateField req;
	ZeroMemory(&req,sizeof(req));

	if (prodId!=NULL) { strcpy(req.ProductID,prodId); }
	while (true)
	{
		int iRet =	m_pApi->ReqQryProductExchRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
	
}
///请求查询产品报价汇率
void CtpTdSpi::OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pProductExchRate)
	{
		
	}
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	
	if(bIsLast) SetEvent(g_hEvent);	
}

void CtpTdSpi::ReqBatDel()
{
	ReqBatchOrdAct(m_iFrtId,m_iSesId);
}

//大商所专用批量撤单
void CtpTdSpi::ReqBatchOrdAct(TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId,
		TThostFtdcExchangeIDType ExhId)
{
	CThostFtdcInputBatchOrderActionField req;
	ZeroMemory(&req,sizeof(req));
	CAppCfgs& s= g_s;

	strcpy(req.ExchangeID,ExhId);
	req.FrontID = iFrtId;
	req.SessionID = iSesId;

	strcpy(req.IPAddress,s.m_sLocIp);
    strcpy(req.MacAddress,s.m_sLocMac);

	m_pApi->ReqBatchOrderAction(&req,++m_iReqId);
}

///批量报单操作请求响应
void CtpTdSpi::OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputBatchOrderAction)
	{
		
	}
	else
	{  
        if (pRspInfo) 
        { 
            memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
            ShowCbErrs(pRspInfo->ErrorMsg);
        } 
    }
	
	//if(bIsLast) SetEvent(g_hEvent);	
}

///批量报单操作错误回报
void CtpTdSpi::OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if( IsErrorRspInfo(pRspInfo) || (!pBatchOrderAction))
	{ ShowCbErrs(pRspInfo->ErrorMsg); }	
}
	
void CtpTdSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	if(pInstrumentStatus && g_Dlg)
	{
		VIT_stat vit = m_statVec.begin();
		for (vit;vit!=m_statVec.end();vit++)
		{
			if (!strcmp(pInstrumentStatus->ExchangeID,vit->ExchangeID) && 
				vit->InstrumentStatus==pInstrumentStatus->InstrumentStatus)
			{ break; }
		}

		if(vit==m_statVec.end())
		{ 
			m_statVec.push_back(*pInstrumentStatus);
					
			CThostFtdcInstrumentStatusField* pStat = new CThostFtdcInstrumentStatusField();
			memcpy(pStat,pInstrumentStatus,sizeof(CThostFtdcInstrumentStatusField));
			::PostMessage(g_Dlg->m_hWnd,WM_EXHSTAT_MSG,0,(LPARAM)pStat); 
		}
	}
}

///交易通知
void CtpTdSpi::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo)
{
	if(pTradingNoticeInfo && g_Dlg)
	{
		if (strlen(pTradingNoticeInfo->FieldContent))
		{
			CThostFtdcTradingNoticeInfoField* pNotice = new CThostFtdcTradingNoticeInfoField();
			memcpy(pNotice,pTradingNoticeInfo,sizeof(CThostFtdcTradingNoticeInfoField));
			
			::PostMessage(g_Dlg->m_hWnd,WM_NOTICE_MSG,0,(LPARAM)pNotice); 
		}
	}	
}

///中金所公告通知
void CtpTdSpi::OnRtnBulletin(CThostFtdcBulletinField *pBulletin)
{
	if(pBulletin && g_Dlg)
	{
		if (strlen(pBulletin->Content))
		{
			CThostFtdcBulletinField* pBt = new CThostFtdcBulletinField();
			memcpy(pBt,pBulletin,sizeof(CThostFtdcBulletinField));
			
			::PostMessage(g_Dlg->m_hWnd,WM_CFXBT_MSG,0,(LPARAM)pBt); 
		}
	}	
}

void CtpTdSpi::ReqQryQt(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
	TThostFtdcOrderSysIDType qtSysId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd)
{
	CThostFtdcQryQuoteField req;
	ZeroMemory(&req,sizeof(req));
	
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));

	if (instId!=NULL) { strcpy(req.InstrumentID,instId); }
	if (ExhId!=NULL) { strcpy(req.ExchangeID,ExhId); }
	if (qtSysId!=NULL) { strcpy(req.QuoteSysID,qtSysId); }
	if (tmStart!=NULL) { strcpy(req.InsertTimeStart,tmStart); }
	if (tmEnd!=NULL) { strcpy(req.InsertTimeEnd,tmEnd); }
	
	while (true)
	{
		int iRet =	m_pApi->ReqQryQuote(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

void CtpTdSpi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pQuote)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryForQt(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
		TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd)
{
	CThostFtdcQryForQuoteField req;
	ZeroMemory(&req,sizeof(req));
	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));

	if (instId!=NULL) { strcpy(req.InstrumentID,instId); }
	if (ExhId!=NULL) { strcpy(req.ExchangeID,ExhId); }
	if (tmStart!=NULL) { strcpy(req.InsertTimeStart,tmStart); }
	if (tmEnd!=NULL) { strcpy(req.InsertTimeEnd,tmEnd); }
	while (true)
	{
		int iRet =	m_pApi->ReqQryForQuote(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

///请求查询询价响应
void CtpTdSpi::OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if( !IsErrorRspInfo(pRspInfo) && pForQuote)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::ReqQryMMInstFeeRate(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryMMInstrumentCommissionRateField req;
	ZeroMemory(&req,sizeof(req));

	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	if (instId!=NULL) { strcpy(req.InstrumentID,instId); }
	while (true)
	{
		int iRet =	m_pApi->ReqQryMMInstrumentCommissionRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

///请求查询做市商合约手续费率响应
void CtpTdSpi::OnRspQryMMInstrumentCommissionRate(CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pMMInstrumentCommissionRate)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
	if(bIsLast) SetEvent(g_hEvent);	
}

void CtpTdSpi::ReqQryMMOpCommRate(TThostFtdcInstrumentIDType instId)
{	
	CThostFtdcQryMMOptionInstrCommRateField req;
	ZeroMemory(&req,sizeof(req));

	strcpy(req.BrokerID,m_sBkrId);
	strncpy(req.InvestorID,m_sUid,sizeof(TThostFtdcInvestorIDType));
	
	if (instId!=NULL) { strcpy(req.InstrumentID,instId); }
	while (true)
	{
		int iRet =	m_pApi->ReqQryMMOptionInstrCommRate(&req,++m_iReqId);
		if (!IsFlowControl(iRet)){ break; }
		else
		{ Sleep(FLOWCTL_TO); }
	}
}

///请求查询做市商期权合约手续费响应
void CtpTdSpi::OnRspQryMMOptionInstrCommRate(CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pMMOptionInstrCommRate)
	{
		
	}
	else
	{  
		if (pRspInfo) 
		{ 
			memcpy(&m_RspMsg,pRspInfo,sizeof(CThostFtdcRspInfoField)); 
			ShowCbErrs(pRspInfo->ErrorMsg);
		} 
    }
	
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpTdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return ret;
}

void CtpTdSpi::ShowCbErrs(TThostFtdcErrorMsgType ErrMsg)
{
	ShowErroTips(GetCTPErr(ErrMsg),LoadStr(IDS_MY_TIPS));
}

void CtpTdSpi::ClrAllVecs()
{
	ClearVec(g_s.m_InsinfVec); 
	ClearVec(m_orderVec);
	ClearVec(m_tradeVec);
	ClearVec(m_pkExVec);
	ClearMap(m_InsinfMap);
	ClearVec(m_StmiVec);
	ClearVec(m_AccRegVec);
	ClearVec(m_onRoadVec);
	ClearVec(m_TdCodeVec);
	ClearVec(m_InvPosVec);
	ClearVec(m_BfTransVec);
	ClearMap(m_DepthMdMap);
	ClearVec(m_statVec);
	ClearVec(m_ExhVec);
	ClearVec(m_BkrNtVec);
	ClearVec(m_TdNoticeVec);
	ClearMap(m_MgrFtMap);
	ClearMap(m_MgrOptMap);
	ClearMap(m_FeeRateMap);

	for(int i=0;i<5;i++)
	{ m_tsEXnLocal[i] = NULL;}

	ZeroMemory(&m_RspMsg,sizeof(CThostFtdcRspInfoField));
	ZeroMemory(&m_BkrTdPara,sizeof(CThostFtdcBrokerTradingParamsField));
	ZeroMemory(&m_BkrTdAlgo,sizeof(CThostFtdcBrokerTradingAlgosField));
	ZeroMemory(&m_OpTdCost,sizeof(CThostFtdcOptionInstrTradeCostField));
	ZeroMemory(&m_TdAcc,sizeof(CThostFtdcTradingAccountField));
	ZeroMemory(&m_FeeRateRev,sizeof(CThostFtdcOptionInstrCommRateField));
	//ZeroMemory(&m_sCurIdt,sizeof(TThostFtdcCurrencyIDType));
	#ifdef _REAL_CTP_
	ZeroMemory(&m_OrdfRate,sizeof(CThostFtdcInstrumentOrderCommRateField));
	#endif

}
