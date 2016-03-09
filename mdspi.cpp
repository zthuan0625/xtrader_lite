#include "stdafx.h"
#include "mdspi.h"
#include "xTrader.h"

extern HANDLE g_hEvent;

void CtpMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
  IsErrorRspInfo(pRspInfo);
}

void CtpMdSpi::OnFrontDisconnected(int nReason)
{
    CXTraderDlg* p = g_Dlg;
	if (p){ ::PostMessage(p->m_hWnd,WM_MDISCON_MSG,0,0); }
}
		
void CtpMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void CtpMdSpi::OnFrontConnected()
{
	ReqUserLogin(/*pApp->m_sBkrId*/);
}

void CtpMdSpi::ReqUserLogin(/*TThostFtdcBrokerIDType	appId*/)
{
	CThostFtdcReqUserLoginField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrID);
	//strcpy(m_sBkrID,appId);

	m_pApi->ReqUserLogin(&req, ++m_iReqId);
}

void CtpMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin)
	{
         CXTraderDlg* p = g_Dlg;
		if (p)
		{ ::PostMessage(p->m_hWnd,WM_MDRECON_MSG,0,0); }
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpMdSpi::ReqUserLogout()
{
	CThostFtdcUserLogoutField req;
	ZeroMemory(&req, sizeof(req));
	strcpy(req.BrokerID, m_sBkrID);
	
	m_pApi->ReqUserLogout(&req, ++m_iReqId);
}

///登出请求响应
void CtpMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pUserLogout)
	{
		
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpMdSpi::SubscribeMarketData(char *pInstId[], int nCount)
{
  m_pApi->SubscribeMarketData(pInstId, nCount); 
}

void CtpMdSpi::OnRspSubMarketData(
         CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pSpecificInstrument)
	{ /*bMdSignal = TRUE; */ }
	else
	{ /*bMdSignal = FALSE; */}
  //if(bIsLast)  SetEvent(g_hEvent);
}

void CtpMdSpi::UnSubscribeMarketData(char *pInstId[], int nCount)
{
	m_pApi->UnSubscribeMarketData(pInstId, nCount); 
}

void CtpMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
             CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pSpecificInstrument)
	{ /*bMdSignal = TRUE; */ }
	else
	{ /*bMdSignal = FALSE; */}
  //if(bIsLast)  SetEvent(g_hEvent);
}

void CtpMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
     CXTraderDlg* p = g_Dlg;
	if (p)
	{
		CThostFtdcDepthMarketDataField *pDepthMd = new CThostFtdcDepthMarketDataField;
		memcpy(pDepthMd,pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField));
		::PostMessage(p->m_hWnd,WM_UPDATEMD_MSG,0,(LPARAM)pDepthMd);
	}
}

void CtpMdSpi::SubscribeForQt(char *ppInstrumentID[], int nCount)
{
	m_pApi->SubscribeForQuoteRsp(ppInstrumentID,nCount);
}

///订阅询价应答
void CtpMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pSpecificInstrument)
	{ /*bMdSignal = TRUE; */ }
	else
	{ /*bMdSignal = FALSE; */}	
}

void CtpMdSpi::UnSubscribeForQt(char *ppInstrumentID[], int nCount)
{
	m_pApi->UnSubscribeForQuoteRsp(ppInstrumentID,nCount);
}

///取消订阅询价应答
void CtpMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pSpecificInstrument)
	{ /*bMdSignal = TRUE; */ }
	else
	{ /*bMdSignal = FALSE; */}
}

///询价通知
void CtpMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{

}

bool CtpMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{	
  bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  return ret;
}