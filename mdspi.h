#pragma once
#include "StdAfx.h"

class CtpMdSpi : public CThostFtdcMdSpi
{
public:
 	CtpMdSpi(CThostFtdcMdApi* api,TThostFtdcBrokerIDType bkrid):m_pApi(api),m_iReqId(0)
	{ strcpy(m_sBkrID,bkrid); };
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	virtual void OnHeartBeatWarning(int nTimeLapse);

	virtual void OnFrontDisconnected(int nReason);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///订阅询价应答
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///取消订阅询价应答
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

public:
	void ReqUserLogin(/*TThostFtdcBrokerIDType	appId*/);
	void ReqUserLogout();

	void SubscribeForQt(char *ppInstrumentID[], int nCount);
	void UnSubscribeForQt(char *ppInstrumentID[], int nCount);
	void SubscribeMarketData(char *ppInstrumentID[], int nCount);
	void UnSubscribeMarketData(char *ppInstrumentID[], int nCount);
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
public:
	int m_iReqId;
	TThostFtdcBrokerIDType	m_sBkrID;
private:
  CThostFtdcMdApi* m_pApi;
};
