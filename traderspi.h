#pragma once
#include "StdAfx.h"

#define FLOWCTL_TO 200
#define CFMMC_TMPL "https://investorservice.cfmmc.com/loginByKey.do?companyID=%s&userid=%s&keyid=%d&passwd=%s"

typedef struct TmOutDelPara
{
	LPVOID	td;
	UINT	uTmOut;	//秒
	TThostFtdcFrontIDType iFrtId;
	TThostFtdcSessionIDType iSesId;
	TThostFtdcOrderRefType sRef;
	TThostFtdcInstrumentIDType instId;
	TThostFtdcExchangeIDType ExhId;
	TThostFtdcOrderSysIDType ordSysId;
	TmOutDelPara(LPVOID td,UINT uTmOut,const TThostFtdcExchangeIDType ExhId,
		const TThostFtdcOrderSysIDType ordSysId)
	{
		this->td = td; this->uTmOut = uTmOut; 
		this->iSesId = 0; this->iFrtId = 0;
		strcpy(this->ExhId,ExhId); strcpy(this->ordSysId,ordSysId);
		ZeroMemory(this->sRef,sizeof(this->sRef)); 
		ZeroMemory(this->instId,sizeof(this->instId));
	}
	TmOutDelPara(LPVOID td=NULL,UINT uTmOut=0,TThostFtdcFrontIDType iFrtId=0,
		TThostFtdcSessionIDType iSesId=0,const TThostFtdcOrderRefType sRef="",
		const TThostFtdcInstrumentIDType instId="")
	{
		this->td = td; this->uTmOut = uTmOut; this->iFrtId = iFrtId; this->iSesId = iSesId;
		strcpy(this->sRef,sRef); strcpy(this->instId,instId);
		ZeroMemory(this->ExhId,sizeof(this->ExhId)); 
		ZeroMemory(this->ordSysId,sizeof(this->ordSysId));
	}
}TOAUTODELP,*PTOAUTODELP;

class CtpTdSpi : public CThostFtdcTraderSpi
{
public:
	CtpTdSpi(CThostFtdcTraderApi* api,TThostFtdcBrokerIDType bkrId,TThostFtdcUserIDType usrId,TThostFtdcPasswordType pwd,
		TThostFtdcPasswordType dympwd,TThostFtdcAuthCodeType atCode) : m_pApi(api),m_iReqId(0),m_iExcRef(0),m_iQtRef(0),
		m_i4QtRef(0),m_iFrtId(-1),m_iSesId(-1),m_bAutoClose(FALSE),m_iPxTick(1)
	{ 
		strcpy(m_sBkrId,bkrId); strcpy(m_sUid,usrId);  strcpy(m_sPwd,pwd); 
		strcpy(m_sDymPwd,dympwd); strcpy(m_sAtCode,atCode); strcpy(m_sCurIdt,_CNY);
	};
   ~CtpTdSpi();

	virtual void OnFrontConnected();
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer);
	
	virtual void OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer);
	
    virtual void OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal);
	
    virtual void OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal);
	
	virtual void OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount);
	
	virtual void OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal);
	
	virtual void OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal);
	
	virtual void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnFrontDisconnected(int nReason);
		
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRtnCFMMCTradingAccountToken(CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken);
	
	virtual void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
	
	virtual void OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
	virtual void OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///////////////////////////////////////////////////////////////////////////////
	virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast); 
	
	virtual void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

	virtual void OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount);
	
	virtual void OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount);
	
	virtual void OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount);
	
	virtual void OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryOptionInstrTradeCost(CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryOptionInstrCommRate(CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	#ifdef _REAL_CTP_
	virtual void OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	#endif
	virtual void OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQueryCFMMCTradingAccountToken(CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryExchangeMarginRate(CThostFtdcExchangeMarginRateField *pExchangeMarginRateAdjust, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryExchangeMarginRateAdjust(CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryCombInstrumentGuard(CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryCombAction(CThostFtdcCombActionField *pCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryInvestorProductGroupMargin(CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnRtnBulletin(CThostFtdcBulletinField *pBulletin);

	virtual void OnRtnCombAction(CThostFtdcCombActionField *pCombAction);

	virtual void OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryMMInstrumentCommissionRate(CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspQryMMOptionInstrCommRate(CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
	static UINT TmOutAutoDellThd(LPVOID pParam);
	void ReqUserLogin();
	void ReqUserLogout();

	const char* GetTradingDay();
	void ReqSetInfConfirm();
	void ReqQrySetInfConfirm();
	void ReqQrySetInf(TThostFtdcDateType TdDay);
	void ReqQryNotice();
	void ReqQryTdNotice();
	void ReqQryTdCode();
	void ReqQryInvest();
	void ReqBatDel();
	void ReqBatchOrdAct(TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId,
		TThostFtdcExchangeIDType ExhId=_DCE);
	void ReqQryProd(TThostFtdcInstrumentIDType prodId,TThostFtdcProductClassType prodClass=THOST_FTDC_PC_Futures);
	void ReqQryExhRate(TThostFtdcCurrencyIDType srcCurId,TThostFtdcCurrencyIDType dstCurId);

	#ifdef _REAL_CTP_
	void ReqQryInstOrdCommRate(TThostFtdcInstrumentIDType instId);
	#endif

	void ReqQryProdExhRate(TThostFtdcInstrumentIDType prodId);
	void ReqQrySecAgentACIDMap(TThostFtdcCurrencyIDType curId);
	void ReqQryCFMMCTdAccKey();
	void ReqQryCFMMCTdAccTok();
	void ReqQryMMInstFeeRate(TThostFtdcInstrumentIDType instId);
	void ReqQryMMOpCommRate(TThostFtdcInstrumentIDType instId);
	void ReqQryExh(TThostFtdcExchangeIDType ExhId);
	void ReqQryDepthMD(TThostFtdcInstrumentIDType instId);
	void ReqQryOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,TThostFtdcOrderSysIDType ordSysId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd);
	
	void ReqQryTd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,TThostFtdcTradeIDType tdId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd);

	void ReqQryAccreg();
	void ReqQryContractBk(TThostFtdcBankIDType bkId,TThostFtdcBankBrchIDType bkBrchId);
	void ReqQryTransBk(TThostFtdcBankIDType bkId,TThostFtdcBankBrchIDType bkBrchId);

	void ReqQryForQt(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
		TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd);
	void ReqQryQt(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
	TThostFtdcOrderSysIDType qtSysId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd);

	void ReqQryTfSerial(TThostFtdcBankIDType bkId);
	void ReqBk2FByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,TThostFtdcPasswordType Pwd,TThostFtdcTradeAmountType TdAmt);
	
	void ReqF2BkByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,TThostFtdcPasswordType Pwd,TThostFtdcTradeAmountType TdAmt);
	
	void ReqQryBkAccMoneyByF(TThostFtdcBankIDType bkId,TThostFtdcPasswordType bkPwd,TThostFtdcPasswordType Pwd);
	
	void ReqQryInstMgr(TThostFtdcInstrumentIDType instId);
	void ReqQryInstFee(TThostFtdcInstrumentIDType instId);
	void ReqQryExhMgr(TThostFtdcInstrumentIDType instId);
	void ReqQryExhMgrAdj(TThostFtdcInstrumentIDType instId);
	void ReqQryInst(TThostFtdcInstrumentIDType instId);
	void ReqQryTdAcc();
	void ReqQryExhRate();
	void ReqQrySecAgentACIDMap();
	void ReqExecOrdInsert(TThostFtdcInstrumentIDType instId,TThostFtdcVolumeType iVol,TThostFtdcOffsetFlagType tOc=THOST_FTDC_OF_Close,
		TThostFtdcHedgeFlagType cTb=THOST_FTDC_HF_Speculation,TThostFtdcActionTypeType cAct=THOST_FTDC_ACTP_Exec,
		TThostFtdcPosiDirectionType cPosDir=THOST_FTDC_PD_Long,TThostFtdcExecOrderPositionFlagType cRev=THOST_FTDC_EOPF_UnReserve,
		TThostFtdcExecOrderCloseFlagType tClose=THOST_FTDC_EOCF_AutoClose);

	void ReqQtInsert(TThostFtdcInstrumentIDType instId,TThostFtdcPriceType dAskPx,TThostFtdcPriceType dBidPx,
	TThostFtdcVolumeType iAskVol,TThostFtdcVolumeType iBidVol,TThostFtdcOffsetFlagType tAskOc=THOST_FTDC_OF_Open,
	TThostFtdcOffsetFlagType tBidOc=THOST_FTDC_OF_Open,TThostFtdcHedgeFlagType tAsktb=THOST_FTDC_HF_Speculation,
	TThostFtdcHedgeFlagType tBidtb=THOST_FTDC_HF_Speculation);
	void ReqQtCancel(TThostFtdcInstrumentIDType instId,TThostFtdcOrderRefType sQtRef,
		TThostFtdcFrontIDType iFtId,TThostFtdcSessionIDType iSesId);

	void ReqForQt(TThostFtdcInstrumentIDType instId);
	void ReqQryEWOffset(TThostFtdcInstrumentIDType instId, TThostFtdcExchangeIDType ExhId);
	void ReqQryOpTdCost(TThostFtdcInstrumentIDType instId);
	void ReqQryOpFeeRate(TThostFtdcInstrumentIDType instId);
	void ReqQryExecOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId,
		TThostFtdcExecOrderSysIDType ExecOrdId,TThostFtdcTimeType tmStart,TThostFtdcTimeType tmEnd);
	void ReqExecCancel(TThostFtdcInstrumentIDType	instId,TThostFtdcOrderRefType OrdRef,
		TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId);
	void ReqExecCancel(TThostFtdcExchangeIDType ExhId,TThostFtdcExecOrderSysIDType sysId);

	void ReqAuth(TThostFtdcProductInfoType prodInf,TThostFtdcAuthCodeType AuthCode);
	
	void ReqQryInvPos(TThostFtdcInstrumentIDType instId);
	void JoinTdYdPos(const CThostFtdcInvestorPositionField* pInv);
	void ReqQryInvPosEx(TThostFtdcInstrumentIDType instId);
	void ReqQryInvPosCombEx(TThostFtdcInstrumentIDType instId);
	void ReqOrds(CThostFtdcInputOrderField* pInp);
	
	void ReqOrdLimit(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, 
		TThostFtdcOffsetFlagType kpp,TThostFtdcPriceType price,TThostFtdcVolumeType vol);

	void ReqOrdAny(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, 
		TThostFtdcOffsetFlagType kpp,TThostFtdcVolumeType vol);

	void ReqOrdAnyCfx(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcOffsetFlagType kpp,
		TThostFtdcVolumeType vol,TThostFtdcOrderPriceTypeType OrdPxType=THOST_FTDC_OPT_AnyPrice,
		TThostFtdcTimeConditionType tmCond=THOST_FTDC_TC_IOC);

	void ReqOrdFAOK(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcOffsetFlagType kpp,
		TThostFtdcPriceType price,TThostFtdcVolumeType vol,TThostFtdcVolumeConditionType volconType=THOST_FTDC_VC_AV,TThostFtdcVolumeType minVol=0);

	void ReqOrdCond(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcOffsetFlagType kpp,
		TThostFtdcPriceType price,TThostFtdcVolumeType vol,TThostFtdcPriceType stopPx,
		TThostFtdcContingentConditionType conType=THOST_FTDC_CC_Immediately);

	void ReqOrdTouch(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcOffsetFlagType kpp,
		TThostFtdcPriceType price,TThostFtdcVolumeType vol,TThostFtdcPriceType stopPx,
		TThostFtdcContingentConditionType conType=THOST_FTDC_CC_Touch);

	void CancelOrd(CORDFEx* pOrd);
	void CancelOrd2(CORDFEx* pOrd);
	void ReqOrdCancel(TThostFtdcExchangeIDType ExhId,TThostFtdcOrderSysIDType ordSysId);
	void ReqOrdCancel(TThostFtdcFrontIDType iFrtId,TThostFtdcSessionIDType iSesId,
		TThostFtdcOrderRefType sRef,TThostFtdcInstrumentIDType instId);

	void ReqUserPwdUpdate(TThostFtdcPasswordType szNewPass,TThostFtdcPasswordType szOldPass);

	void ReqTdAccPwdUpdate(TThostFtdcPasswordType szNewPass,TThostFtdcPasswordType szOldPass);

	void ReqQryBkrTdParams();
	void ReqQryBkrTdAlgos(TThostFtdcExchangeIDType ExhId,TThostFtdcInstrumentIDType instId);

	void ReqParkedOrd(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, 
        TThostFtdcOffsetFlagType kpp,TThostFtdcPriceType price, TThostFtdcVolumeType vol);
	void ReqParkedOrdAct(CThostFtdcOrderFieldEx* pOrd);
	void ReqParkedOrdAct2(CThostFtdcOrderFieldEx* pOrd);
	
	void ReqQryParkedOrd(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId);
	void ReqQryParkedOrdAct(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId);

	void ReqRmParkedOrd(TThostFtdcParkedOrderIDType ParkedOrdId);
	void ReqRmParkedOrdAct(TThostFtdcParkedOrderActionIDType ParkedOrdActId);

	void ReqQryProdGroup(TThostFtdcInstrumentIDType prodId,TThostFtdcExchangeIDType	ExhId);

	void ReqQryCombInstGuard(TThostFtdcInstrumentIDType instId);

	void ReqQryInvProdGrpMrg(TThostFtdcInstrumentIDType prodId,TThostFtdcHedgeFlagType tFlag);

	void ReqCombActInsert(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType tDir=THOST_FTDC_D_Sell,
	TThostFtdcVolumeType iVol=1,TThostFtdcCombDirectionType tCombDir=THOST_FTDC_CMDR_UnComb,TThostFtdcHedgeFlagType tFlag=THOST_FTDC_HF_Speculation);

	void ReqQryCombAct(TThostFtdcInstrumentIDType instId,TThostFtdcExchangeIDType ExhId=_CFX);

	void ReqQryMaxOrdVol(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType tDir=THOST_FTDC_D_Sell,
		TThostFtdcOffsetFlagType tOc=THOST_FTDC_OF_Open,TThostFtdcHedgeFlagType tFlag=THOST_FTDC_HF_Speculation);
	
	void GetfRateByInst(const TThostFtdcInstrumentIDType instId,TThostFtdcOffsetFlagType cFlag,
		TThostFtdcRatioType& dfRByM,TThostFtdcRatioType& dfRByV);
	void GetInstMgr(const TThostFtdcInstrumentIDType instId,TThostFtdcPosiDirectionType posDir,
	TThostFtdcRatioType& dMgrByM,TThostFtdcRatioType& dMgrByV);
	double GetCurPosOpenCost(const CThostFtdcInvestorPositionField* pInv,int iMul);
	void GetSettPx(const TThostFtdcInstrumentIDType instId,double& dprePx,double& dlastPx);
	BOOL JgExhByInst(const TThostFtdcInstrumentIDType szInst,TThostFtdcExchangeIDType ExhId);
	BOOL IsValidInst(CString szInst,PINSINFEX pInf);

	double GetTdFee(const CThostFtdcTradeField* pTrade);
	int FindInstMul(const TThostFtdcInstrumentIDType instId);
	double GetTdAvgPx(TThostFtdcSequenceNoType BkrOrdSeq);
	double GetMgrPx(BKRTDPARA& TdPara,const PCORDFEx pOrder);
	double GetFrozenMgr(const PCORDFEx pOrder,BKRTDPARA& TdPara);
	void GetFrozMgrFee(const PCORDFEx pOrder,BKRTDPARA& TdPara,double& dMgrFrz,double& dFeeFrz);
	void GetUpLoLmt(TThostFtdcInstrumentIDType instId,TThostFtdcPriceType& dUpLmt,TThostFtdcPriceType& dLoLmt);
	void GetLmtMktVol(TThostFtdcInstrumentIDType instId,TThostFtdcVolumeType& iLmt,TThostFtdcVolumeType& iMkt);
	void GetMdByInst(TThostFtdcInstrumentIDType instId,CThostFtdcDepthMarketDataField* pMd);
	void GetExhByInst(TThostFtdcInstrumentIDType szInst,TThostFtdcExchangeIDType sExh);
	void GetValBySysId(TThostFtdcOrderSysIDType ordSysId,int& iVol,TThostFtdcOffsetFlagType& cOff,
	TThostFtdcDirectionType& cDir,TThostFtdcOrderPriceTypeType& cPx,TThostFtdcHedgeFlagType& cHedge);
	TThostFtdcPriceType GetPxTick(TThostFtdcInstrumentIDType instId);

	#ifdef _REAL_CTP_
	void GetOrdfRByInst(TThostFtdcInstrumentIDType instId,TThostFtdcRatioType& dOrdByV,TThostFtdcRatioType& dDelByV);
	#endif

	void FiltInfEx(INSINFEX& InfEx);
	BOOL InitMgrFee();
	void UpdatePosByOrd(LPVOID pParam,const CThostFtdcOrderField *pOrder);
	void ProcCondOrd(LPVOID pParam,CThostFtdcOrderField *pOrder);
	void UpdatePosByTd(LPVOID pParam,const CThostFtdcTradeField *pTrade);

	void PostOrdWarnMsg(LPVOID pParam,const CThostFtdcOrderField *pOrder);
	void AutoDelNoLocOrd(const CThostFtdcOrderField *pOrder);
	void AutoDelLocOrd(const CThostFtdcInputOrderField *pInp);

	void CancelAll();
	void StopAfterTd(const TThostFtdcPriceType dStopPx,bool bDym = false);

	void GenCloseFromOrd(CThostFtdcOrderField *pOrd,int nPxTick);
	void GenCloseFromTd(CThostFtdcTradeField *pTd,int nPxTick);
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	void ShowCbErrs(TThostFtdcErrorMsgType ErrMsg);

	void ClrAllVecs();
public:
	TThostFtdcBrokerIDType m_sBkrId;		// 经纪公司
	TThostFtdcUserIDType m_sUid;		// 投资者代码
	TThostFtdcPasswordType m_sPwd,m_sDymPwd;
	TThostFtdcAuthCodeType m_sAtCode;
	BOOL		m_bAutoClose;
	int			m_iPxTick;
	
	int	 m_iReqId,m_iFrtId,m_iSesId,m_iOrdRef,m_iCondRef,m_iExcRef,m_iQtRef,m_i4QtRef;
	TThostFtdcDateType m_sTdday;
	////////////////////////////////////////////
	vector<CORDFEx> m_orderVec,m_onRoadVec;	
	vector<CTDFEx> m_tradeVec;
	vector<PARKEDEX> m_pkExVec;
	map_str_InsInfEx m_InsinfMap;
	VEC_STAT m_statVec;
	vector<CThostFtdcSettlementInfoField> m_StmiVec;
	vector<CThostFtdcAccountregisterField> m_AccRegVec;
	vector<CThostFtdcTradingCodeField> m_TdCodeVec;
	vector<CThostFtdcInvestorPositionField> m_InvPosVec;
	vector<CThostFtdcTransferSerialField> m_BfTransVec;
	map_str_DepthMd m_DepthMdMap;
	map_str_Mgr m_MgrFtMap;
	map_str_Optc m_MgrOptMap;
	map_str_FeeEx m_FeeRateMap;
	vector<CThostFtdcExchangeField> m_ExhVec;
	vector<CThostFtdcNoticeField> m_BkrNtVec;
	vector<CThostFtdcTradingNoticeField> m_TdNoticeVec;
	CThostFtdcInstrumentCommissionRateFieldEx m_FeeRateRev;
	CThostFtdcInstrumentMarginRateField m_MargRateRev;
	CThostFtdcOptionInstrTradeCostField m_OpTdCost;
	#ifdef _REAL_CTP_
	CThostFtdcInstrumentOrderCommRateField m_OrdfRate;
	#endif
	CThostFtdcBrokerTradingParamsField 	m_BkrTdPara;
	CThostFtdcBrokerTradingAlgosField  	m_BkrTdAlgo;
	////////////////////////////////////////////
	CThostFtdcTradingAccountField m_TdAcc;
	TThostFtdcCurrencyIDType m_sCurIdt; //币种
	CThostFtdcRspInfoField m_RspMsg;
	CTimeSpan m_tsEXnLocal[5];

private:
  CThostFtdcTraderApi* m_pApi;
};
