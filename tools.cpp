#include "stdafx.h"
#include "tools.h"
#include "CostMs.h"

int GetHoldPos(const CThostFtdcInvestorPositionField* pInv)
{
	int iHoldPos=0;
	if (pInv->PosiDirection == THOST_FTDC_PD_Long)
	{ iHoldPos = pInv->Position-pInv->ShortFrozen; }
	else if (pInv->PosiDirection == THOST_FTDC_PD_Short)
	{ iHoldPos = pInv->Position-pInv->LongFrozen; }

	return iHoldPos;
}

void TermThread(HANDLE hThd)
{
    TerminateThread(hThd, 0);
    CloseHandle(hThd);
    hThd = NULL;
}

//人民币符号￥
char MapDirection(char src, bool toOrig)
{
	if(toOrig)
	{
		if('b'==src||'B'==src) { src = THOST_FTDC_D_Buy; }
		else if('s'==src||'S'==src)
		{ src = THOST_FTDC_D_Sell; }
	}
	else
	{
		if(src==THOST_FTDC_D_Buy){ src='B'; }
		else if(src==THOST_FTDC_D_Sell)
		{ src='S'; }
	}
	return src;
}

char MapOffset(char src, bool toOrig)
{
	if(toOrig)
	{
		if('o'==src||'O'==src){ src=THOST_FTDC_OF_Open; }
		else if('c'==src||'C'==src){ src=THOST_FTDC_OF_Close; }
		else if('j'==src||'J'==src){ src=THOST_FTDC_OF_CloseToday; }
	}
	else
	{
		if(src==THOST_FTDC_OF_Open){src='O';}
		else if(src==THOST_FTDC_OF_Close){src='C';}
		else if(src==THOST_FTDC_OF_CloseToday){src='J';}
	}
	return src;
}

void OrdConvter(CThostFtdcInputOrderField& ord,PARKEDEX& pkEx,ORDCONV_TYPE ct)
{
	//逐个字段复制更靠谱
	if (ct==INP2PARKED)
	{
		ZeroMemory(&pkEx,sizeof(PARKEDEX));
		strcpy(pkEx.park.BrokerID,ord.BrokerID);
		strcpy(pkEx.park.InvestorID,ord.InvestorID);
		strcpy(pkEx.park.InstrumentID,ord.InstrumentID);
		strcpy(pkEx.park.OrderRef,ord.OrderRef);
		strcpy(pkEx.park.UserID,ord.UserID);
		pkEx.park.OrderPriceType = ord.OrderPriceType;
		pkEx.park.Direction = ord.Direction;
		pkEx.park.CombOffsetFlag[0] = ord.CombOffsetFlag[0];
		pkEx.park.CombHedgeFlag[0] = ord.CombHedgeFlag[0];
		pkEx.park.LimitPrice = ord.LimitPrice;
		pkEx.park.VolumeTotalOriginal= ord.VolumeTotalOriginal;
		pkEx.park.TimeCondition = ord.TimeCondition;
		strcpy(pkEx.park.GTDDate,ord.GTDDate);
		pkEx.park.VolumeCondition= ord.VolumeCondition;
		pkEx.park.MinVolume= ord.MinVolume;
		pkEx.park.ContingentCondition= ord.ContingentCondition;
		pkEx.park.StopPrice= ord.StopPrice;
		pkEx.park.ForceCloseReason= ord.ForceCloseReason;
		pkEx.park.IsAutoSuspend= ord.IsAutoSuspend;
		strcpy(pkEx.park.BusinessUnit,ord.BusinessUnit);
		pkEx.park.RequestID= ord.RequestID;
		pkEx.park.UserForceClose= ord.UserForceClose;
		pkEx.park.IsSwapOrder = ord.IsSwapOrder;
		strcpy(pkEx.park.ExchangeID,ord.ExchangeID);
		strcpy(pkEx.park.InvestUnitID,ord.InvestUnitID);
		strcpy(pkEx.park.AccountID,ord.AccountID);
		strcpy(pkEx.park.CurrencyID,ord.CurrencyID);
		strcpy(pkEx.park.ClientID,ord.ClientID);
		strcpy(pkEx.park.IPAddress,ord.IPAddress);
		strcpy(pkEx.park.MacAddress,ord.MacAddress);
	}

	if (ct==PK_LOCCOND2INP || ct==PARKED2INP)
	{
		ZeroMemory(&ord,sizeof(CThostFtdcInputOrderField));
		strcpy(ord.BrokerID,pkEx.park.BrokerID);
		strcpy(ord.InvestorID,pkEx.park.InvestorID);
		strcpy(ord.InstrumentID,pkEx.park.InstrumentID);
		strcpy(ord.OrderRef,pkEx.park.OrderRef);
		strcpy(ord.UserID,pkEx.park.UserID);
		ord.OrderPriceType = pkEx.park.OrderPriceType;
		ord.Direction = pkEx.park.Direction;
		ord.CombOffsetFlag[0] = pkEx.park.CombOffsetFlag[0];
		ord.CombHedgeFlag[0] = pkEx.park.CombHedgeFlag[0];
		ord.LimitPrice = pkEx.park.LimitPrice;
		ord.VolumeTotalOriginal = pkEx.park.VolumeTotalOriginal;
		ord.TimeCondition = pkEx.park.TimeCondition;
		strcpy(ord.GTDDate,pkEx.park.GTDDate);
		ord.VolumeCondition = pkEx.park.VolumeCondition;
		ord.MinVolume = pkEx.park.MinVolume;
		ord.ForceCloseReason = pkEx.park.ForceCloseReason;
		ord.IsAutoSuspend = pkEx.park.IsAutoSuspend;
		strcpy(ord.BusinessUnit,pkEx.park.BusinessUnit);
		ord.RequestID = pkEx.park.RequestID;
		ord.UserForceClose = pkEx.park.UserForceClose;
		strcpy(ord.ExchangeID,pkEx.park.ExchangeID);
		strcpy(ord.InvestUnitID,pkEx.park.InvestUnitID);
		strcpy(ord.AccountID,pkEx.park.AccountID);
		strcpy(ord.CurrencyID,pkEx.park.CurrencyID);
		strcpy(ord.ClientID,pkEx.park.ClientID);
		strcpy(ord.IPAddress,pkEx.park.IPAddress);
		strcpy(ord.MacAddress,pkEx.park.MacAddress);
		ord.IsSwapOrder = pkEx.park.IsSwapOrder;
		if (ct==PK_LOCCOND2INP)
		{
			ord.StopPrice = 0;
			ord.ContingentCondition = THOST_FTDC_CC_Immediately;
		}
		else
		{
			ord.StopPrice = pkEx.park.StopPrice;
			ord.ContingentCondition = pkEx.park.ContingentCondition;
		}
	}
}

void OrdConvter(CThostFtdcOrderField& ord,PARKEDEX& pkEx,ORDCONV_TYPE ct)
{
	if (ct==ORD2PARKED)
	{
		ZeroMemory(&pkEx,sizeof(PARKEDEX));
		
		strcpy(pkEx.park.BrokerID,ord.BrokerID);
		strcpy(pkEx.park.InvestorID,ord.InvestorID);
		strcpy(pkEx.park.InstrumentID,ord.InstrumentID);
		strcpy(pkEx.park.OrderRef,ord.OrderRef);
		strcpy(pkEx.park.UserID,ord.UserID);
		pkEx.park.OrderPriceType = ord.OrderPriceType;
		pkEx.park.Direction = ord.Direction;
		strcpy(pkEx.park.CombOffsetFlag,ord.CombOffsetFlag);
		strcpy(pkEx.park.CombHedgeFlag,ord.CombHedgeFlag);
		pkEx.park.LimitPrice = ord.LimitPrice;
		pkEx.park.VolumeTotalOriginal = ord.VolumeTotalOriginal;
		pkEx.park.TimeCondition = ord.TimeCondition;
		strcpy(pkEx.park.GTDDate,ord.GTDDate);
		pkEx.park.VolumeCondition = ord.VolumeCondition;
		pkEx.park.MinVolume = ord.MinVolume;
		pkEx.park.ContingentCondition = ord.ContingentCondition;
		pkEx.park.StopPrice = ord.StopPrice;
		pkEx.park.ForceCloseReason = ord.ForceCloseReason;
		pkEx.park.IsAutoSuspend = ord.IsAutoSuspend;
		strcpy(pkEx.park.BusinessUnit,ord.BusinessUnit);
		pkEx.park.RequestID = ord.RequestID;
		pkEx.park.UserForceClose = ord.UserForceClose;
		strcpy(pkEx.park.ExchangeID,ord.ExchangeID);
		strcpy(pkEx.park.ClientID,ord.ClientID);
		
		pkEx.park.Status = ord.OrderStatus;
		strcpy(pkEx.InsertDate,ord.InsertDate);
		strcpy(pkEx.InsertTime,ord.InsertTime);
		strcpy(pkEx.park.ErrorMsg,ord.StatusMsg);
		pkEx.park.IsSwapOrder = ord.IsSwapOrder;
		
		strcpy(pkEx.BranchID,ord.BranchID);
		strcpy(pkEx.park.InvestUnitID,ord.InvestUnitID);
		strcpy(pkEx.park.AccountID,ord.AccountID);
		strcpy(pkEx.park.CurrencyID,ord.CurrencyID);
		strcpy(pkEx.park.IPAddress,ord.IPAddress);
		strcpy(pkEx.park.MacAddress,ord.MacAddress);

		strcpy(pkEx.OrderSysID,ord.OrderSysID);
	}
	
	if (ct==PARKED2ORD)
	{
		ZeroMemory(&ord,sizeof(CThostFtdcOrderField));
		strcpy(ord.BrokerID,pkEx.park.BrokerID);
		strcpy(ord.InvestorID,pkEx.park.InvestorID);
		strcpy(ord.InstrumentID,pkEx.park.InstrumentID);
		strcpy(ord.OrderRef,pkEx.park.OrderRef);
		strcpy(ord.UserID,pkEx.park.UserID);
		ord.OrderPriceType = pkEx.park.OrderPriceType;
		ord.Direction = pkEx.park.Direction;
		strcpy(ord.CombOffsetFlag,pkEx.park.CombOffsetFlag);
		strcpy(ord.CombHedgeFlag,pkEx.park.CombHedgeFlag);
		ord.LimitPrice = pkEx.park.LimitPrice;
		ord.VolumeTotalOriginal = pkEx.park.VolumeTotalOriginal;
		ord.TimeCondition = pkEx.park.TimeCondition;
		strcpy(ord.GTDDate,pkEx.park.GTDDate);
		ord.VolumeCondition = pkEx.park.VolumeCondition;
		ord.MinVolume = pkEx.park.MinVolume;
		ord.ContingentCondition = pkEx.park.ContingentCondition;
		ord.StopPrice = pkEx.park.StopPrice;
		ord.ForceCloseReason = pkEx.park.ForceCloseReason;
		ord.IsAutoSuspend = pkEx.park.IsAutoSuspend;
		strcpy(ord.BusinessUnit,pkEx.park.BusinessUnit);
		ord.RequestID = pkEx.park.RequestID;
		ord.UserForceClose = pkEx.park.UserForceClose;
		strcpy(ord.ExchangeID,pkEx.park.ExchangeID);
		strcpy(pkEx.park.ClientID,ord.ClientID);
		
		ord.OrderStatus = pkEx.park.Status;
		strcpy(ord.InsertDate,pkEx.InsertDate);
		strcpy(ord.InsertTime,pkEx.InsertTime);
		strcpy(ord.StatusMsg,pkEx.park.ErrorMsg);
		ord.IsSwapOrder = pkEx.park.IsSwapOrder;
	
		strcpy(ord.BranchID,pkEx.BranchID);
		strcpy(ord.InvestUnitID,pkEx.park.InvestUnitID);
		strcpy(ord.AccountID,pkEx.park.AccountID);
		strcpy(ord.CurrencyID,pkEx.park.CurrencyID);
		strcpy(ord.IPAddress,pkEx.park.IPAddress);
		strcpy(ord.MacAddress,pkEx.park.MacAddress);

		strcpy(ord.OrderSysID,pkEx.OrderSysID);
	}
}

void CondErr2Pk(const CThostFtdcErrorConditionalOrderField *pErr,PARKEDEX& pkEx)
{
	ZeroMemory(&pkEx,sizeof(PARKEDEX));
	
	strcpy(pkEx.park.BrokerID,pErr->BrokerID);
	strcpy(pkEx.park.InvestorID,pErr->InvestorID);
	strcpy(pkEx.park.InstrumentID,pErr->InstrumentID);
	strcpy(pkEx.park.OrderRef,pErr->OrderRef);
	strcpy(pkEx.park.UserID,pErr->UserID);
	pkEx.park.OrderPriceType = pErr->OrderPriceType;
	pkEx.park.Direction = pErr->Direction;
	strcpy(pkEx.park.CombOffsetFlag,pErr->CombOffsetFlag);
	strcpy(pkEx.park.CombHedgeFlag,pErr->CombHedgeFlag);
	pkEx.park.LimitPrice = pErr->LimitPrice;
	pkEx.park.VolumeTotalOriginal = pErr->VolumeTotalOriginal;
	pkEx.park.TimeCondition = pErr->TimeCondition;
	strcpy(pkEx.park.GTDDate,pErr->GTDDate);
	pkEx.park.VolumeCondition = pErr->VolumeCondition;
	pkEx.park.MinVolume = pErr->MinVolume;
	pkEx.park.ContingentCondition = pErr->ContingentCondition;
	pkEx.park.StopPrice = pErr->StopPrice;
	pkEx.park.ForceCloseReason = pErr->ForceCloseReason;
	pkEx.park.IsAutoSuspend = pErr->IsAutoSuspend;
	strcpy(pkEx.park.BusinessUnit,pErr->BusinessUnit);
	pkEx.park.RequestID = pErr->RequestID;
	pkEx.park.UserForceClose = pErr->UserForceClose;
	strcpy(pkEx.park.ExchangeID,pErr->ExchangeID);
	
	//这里一定是校验错误
	pkEx.park.Status = THOST_FTDC_OST_ErrOrd;
	strcpy(pkEx.InsertDate,pErr->InsertDate);
	strcpy(pkEx.InsertTime,pErr->InsertTime);
	
	pkEx.park.ErrorID = pErr->ErrorID;
	strcpy(pkEx.park.ErrorMsg,pErr->ErrorMsg);
	pkEx.park.IsSwapOrder = pErr->IsSwapOrder;

	strcpy(pkEx.BranchID,pErr->BranchID);
	strcpy(pkEx.park.InvestUnitID,pErr->InvestUnitID);
	strcpy(pkEx.park.AccountID,pErr->AccountID);
	strcpy(pkEx.park.CurrencyID,pErr->CurrencyID);
	strcpy(pkEx.park.IPAddress,pErr->IPAddress);
	strcpy(pkEx.park.MacAddress,pErr->MacAddress);
	strcpy(pkEx.OrderSysID,pErr->OrderSysID);
}

int outStrAs4(CString& szIn)
{
	CString szTemp = szIn;

	int iLen = szTemp.GetLength();
	int iDotPos = szTemp.ReverseFind('.');
	
	CString szDigEnd=_T("");
	if (iDotPos>0)
	{
		szDigEnd = szTemp.Mid(iDotPos+1,iLen-iDotPos-1);
		szTemp = szTemp.Mid(0,iDotPos);
	}
	szTemp.MakeReverse();

	int iLen2 = szTemp.GetLength();
	int iTimes = iLen2 /4;
	int iLeft = iLen2 %4;

	CString szOut =_T("");
	for (int i=0;i<iTimes;i++)
	{
		szOut += szTemp.Mid(i*4,4);
		if ((i!=(iTimes-1))||(iLeft!=1)||(*(szTemp.GetBuffer(0) + iLen2-1)!='-'))
		{ szOut += ','; }	
	}

	if (iLeft>0){ szOut += szTemp.Mid(iLen2-iLeft,iLeft); }
	else
	{ szOut.TrimRight(','); }

	szOut.MakeReverse();
	szIn = szOut;

	if (!szDigEnd.IsEmpty()){ szIn.Format(_T("%s.%s"),(LPCTSTR)szOut,(LPCTSTR)szDigEnd); }
	
	return 0;
}

void Fee2String(CString& szIn,double dOpenByM,double dOpenByV,double dCloseByM,
			double dCloseByV,double dClosetByM,double dClosetByV)
{
	szIn =_T("0.0");

	if (dOpenByV>NEARZERO)
	{
		if(dCloseByV>NEARZERO)
		{
			if (dClosetByV>NEARZERO){ szIn.Format(_T("开%g/手,平%g/手,平今%g/手"),dOpenByV,dCloseByV,dClosetByV); }
			else
			{ szIn.Format(_T("开%g/手,平%g/手,平今免"),dOpenByV,dCloseByV); }
		}
		else
		{ 
			if (dClosetByV>NEARZERO){ szIn.Format(_T("开%g/手,平今%g/手,平仓免"),dOpenByV,dClosetByV); }
			else
			{ szIn.Format(_T("开%g/手,平免"),dOpenByV); }	
		}
			
	}
	else if (dOpenByM>NEARZERO)
	{
		if(dCloseByM>NEARZERO)
		{
			if (dClosetByM>NEARZERO)
			{ szIn.Format(_T("开%g%%%%,平%g%%%%,平今%g%%%%"),dOpenByM*10000,dCloseByM*10000,dClosetByM*10000);}
			else 
			{ szIn.Format(_T("开%g%%%%,平%g%%%%,平今免"),dOpenByM*10000,dCloseByM*10000);}
		}
		else
		{ 
			if (dClosetByM>NEARZERO)
			{ szIn.Format(_T("开%g%%%%,平今%g%%%%,平仓免"),dOpenByM*10000,dClosetByM*10000); }
			else
			{ szIn.Format(_T("开%g%%%%,平免"),dOpenByM*10000);	}
		}
	}
}

void GenErrOrdByInpt(const CThostFtdcInputOrderField *pInt,CORDFEx *pOrd)
{
	ZeroMemory(pOrd, sizeof(CORDFEx));
	
	getCurTime(pOrd->Ord.InsertTime);
	getCurDate(pOrd->Ord.InsertDate);
	
	strcpy(pOrd->Ord.InstrumentID,pInt->InstrumentID);
	pOrd->Ord.Direction = pInt->Direction;
	strcpy(pOrd->Ord.CombOffsetFlag,pInt->CombOffsetFlag);
	pOrd->Ord.OrderStatus = THOST_FTDC_OST_ErrOrd;
	pOrd->Ord.OrderPriceType = pInt->OrderPriceType;
	pOrd->Ord.LimitPrice = pInt->LimitPrice;
	pOrd->Ord.VolumeTotalOriginal = pInt->VolumeTotalOriginal;
	
	pOrd->Ord.TimeCondition = pInt->TimeCondition;
	pOrd->Ord.VolumeCondition = pInt->VolumeCondition;
	pOrd->Ord.VolumeTotal = pInt->VolumeTotalOriginal;
	pOrd->Ord.VolumeTraded = 0;

	strcpy(pOrd->Ord.ExchangeID, pInt->ExchangeID);
	strcpy(pOrd->Ord.InvestUnitID, pInt->InvestUnitID);
	strcpy(pOrd->Ord.AccountID, pInt->AccountID);
	strcpy(pOrd->Ord.CurrencyID, pInt->CurrencyID);
	strcpy(pOrd->Ord.IPAddress, pInt->IPAddress);
	strcpy(pOrd->Ord.MacAddress, pInt->MacAddress);
}

CString FormatLine(LPCTSTR lpItem,CString szValue,CString szFill,int iTotalLen)
{
	CString szItem(lpItem);
	CString szOut = _T("");
	
	if (szFill.IsEmpty()) { return _T("");}
	
	int iLen = szItem.GetLength()+szValue.GetLength()+szFill.GetLength();
	if (iLen>=iTotalLen) { return _T(""); }
	
	int iFill=(iTotalLen-3*szItem.GetLength()/2-szValue.GetLength())/(szFill.GetLength());
	
	szOut += szItem;
	for (int i=0;i<iFill;i++) {szOut += szFill;}
	szOut += szValue;
	szOut += _T("\r\n");
	
	return szOut;
}

void ShowErroTips(UINT nID1, UINT nID2)
{
	CMyMsgBox mbox(AfxGetApp()->m_pMainWnd);
	mbox.MessageBox(LoadStr(nID1),LoadStr(nID2),1,true,MB_ICONINFORMATION);
}

void ShowErroTips(LPCTSTR lpMsg, LPCTSTR lpTitle)
{
	CMyMsgBox mbox(AfxGetApp()->m_pMainWnd);
	mbox.MessageBox(lpMsg,lpTitle,1,true,MB_ICONINFORMATION);
}

CString JgParkedType(TThostFtdcCondParkType ordType)
{
	CString szTemp =_T("");
	switch (ordType)
	{
		case ENTERTD_LOCAL:
			szTemp = _T("本地预埋");
			break;
		case PARKED_SERVER:
			szTemp = _T("远程预埋");
			break;
		case COND_LOCAL:
			szTemp = _T("本地条件");
			break;
		case COND_SERVER:
			szTemp = _T("远程条件");
			break;
		case TOUCH_ZS:
			szTemp = _T("止损单");
			break;
		case TOUCH_ZY:
			szTemp = _T("止盈单");
			break;
		case TMORD_LOCAL:
			szTemp = _T("本地定时");
			break;
		case PARKED_ACT:
			szTemp = _T("撤单预埋");
			break;	
	}
	return szTemp;
}

CString JgTdStatus(TThostFtdcInstrumentStatusType bType)
{
	CString szIn =_T("");

	switch(bType)
	{
	case THOST_FTDC_IS_BeforeTrading:
		szIn = _T("开盘前");
		break;
	case THOST_FTDC_IS_NoTrading:
		szIn = _T("非交易");
		break;
	case THOST_FTDC_IS_Continous:
		szIn = _T("连续交易");
		break;
	case THOST_FTDC_IS_AuctionOrdering:
		szIn = _T("集合竞价报单");
		break;
	case THOST_FTDC_IS_AuctionBalance:
		szIn = _T("集合竞价价格平衡");
		break;
	case THOST_FTDC_IS_AuctionMatch:
		szIn = _T("集合竞价撮合");
		break;
	case THOST_FTDC_IS_Closed:
		szIn = _T("收盘");
		break;
	default:
		szIn = _T("未知");
		break;
	}

	return szIn;
}

CString JgEnterReason(TThostFtdcInstStatusEnterReasonType bReason)
{
	CString szTmp = _T("");
	switch (bReason)
	{
		case THOST_FTDC_IER_Automatic:
			szTmp= _T("自动切换");
			break;
		case THOST_FTDC_IER_Manual:
			szTmp = _T("手动切换");
			break;
		case THOST_FTDC_IER_Fuse:
			szTmp = _T("熔断");
			break;
	}
	return szTmp;
}

CString JgTdPxSrc(TThostFtdcPriceSourceType bSrc)
{
	CString szTmp = _T("");
	switch (bSrc)
	{
	case THOST_FTDC_PSRC_LastPrice:
		szTmp= _T("前成交价");
		break;
	case THOST_FTDC_PSRC_Buy:
		szTmp = _T("买委托价");
		break;
	case THOST_FTDC_PSRC_Sell:
		szTmp = _T("卖委托价");
		break;
	}
	return szTmp;
}

CString JgOrdType(TThostFtdcOrderTypeType bType)
{
	CString szTmp = _T("");
	switch (bType)
	{
		case THOST_FTDC_ORDT_Normal:
			szTmp= _T("普通");
			break;
		case THOST_FTDC_ORDT_DeriveFromQuote:
			szTmp = _T("报价衍生");
			break;
		case THOST_FTDC_ORDT_DeriveFromCombination:
			szTmp = _T("组合衍生");
			break;
		case THOST_FTDC_ORDT_Combination:
			szTmp= _T("组合报单");
			break;
		case THOST_FTDC_ORDT_ConditionalOrder:
			szTmp = _T("条件单");
			break;
		case THOST_FTDC_ORDT_Swap:
			szTmp = _T("互换单");
			break;
	}
	return szTmp;
}

BOOL SetHighPriority()
{
	return !SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

CString JgProType(TThostFtdcProductClassType bType)
{
	CString szTmp ;
	
	switch(bType)
	{
	case THOST_FTDC_PC_Futures:
		szTmp = _T("期货");
		break;
	case THOST_FTDC_PC_Options:
		szTmp = _T("期权");
		break;
	case THOST_FTDC_PC_Combination:
		szTmp = _T("组合");
		break;
	case THOST_FTDC_PC_Spot:
		szTmp = _T("即期");
		break;
	case THOST_FTDC_PC_EFP:
		szTmp = _T("期转现");
		break;
	case THOST_FTDC_PC_SpotOption:
		szTmp = _T("现权");
		break;
	default:
		szTmp=_T("未知");
		break;
	}
	return szTmp;
}

CString JgExecRet(TThostFtdcExecResultType bType)
{
	CString szTmp;
	switch(bType)
	{
		case THOST_FTDC_OER_NoExec:
			szTmp = _T("未执行");
			break;
		case THOST_FTDC_OER_Canceled:
			szTmp = _T("已取消");
			break;
		case THOST_FTDC_OER_OK:
			szTmp = _T("已执行");
			break;
		case THOST_FTDC_OER_NoPosition:
			szTmp = _T("持仓不够");
			break;
		case THOST_FTDC_OER_NoDeposit:
			szTmp = _T("资金不足");
			break;
		case THOST_FTDC_OER_NoParticipant:
			szTmp = _T("无此经纪");
			break;
		case THOST_FTDC_OER_NoClient:
			szTmp = _T("无此客户");
			break;
		case THOST_FTDC_OER_NoInstrument:
			szTmp = _T("无此合约");
			break;
		case THOST_FTDC_OER_NoRight:
			szTmp = _T("无执行权");
			break;
		case THOST_FTDC_OER_InvalidVolume:
			szTmp = _T("无效数量");
			break;
		case THOST_FTDC_OER_NoEnoughHistoryTrade:
			szTmp = _T("无足够成交");
			break;
		case THOST_FTDC_OER_Unknown:
			szTmp = _T("未知");
			break;
	}

	return szTmp;
}

CString JgExhPro(TThostFtdcExchangePropertyType bType)
{
	CString szTmp;
	
	switch(bType)
	{
	case THOST_FTDC_EXP_Normal:
		szTmp = _T("普通");
		break;
	case THOST_FTDC_EXP_GenOrderByTrade:
		szTmp = _T("从成交生成报单");
		break;
	}
	return szTmp;
}

CString JgCardType(TThostFtdcIdCardTypeType bType)
{
	CString szTmp;
	
	switch(bType)
	{
	case THOST_FTDC_ICT_EID:
		szTmp = _T("组织机构代码");
		break;
	case THOST_FTDC_ICT_IDCard:
		szTmp = _T("身份证");
		break;
	case THOST_FTDC_ICT_OfficerIDCard:
		szTmp = _T("军官证");
		break;
	case THOST_FTDC_ICT_PoliceIDCard:
		szTmp = _T("警官证");
		break;
	case THOST_FTDC_ICT_SoldierIDCard:
		szTmp = _T("士兵证");
		break;
	case THOST_FTDC_ICT_HouseholdRegister:
		szTmp = _T("户口簿");
		break;
	case THOST_FTDC_ICT_Passport:
		szTmp = _T("护照");
		break;
	case THOST_FTDC_ICT_TaiwanCompatriotIDCard:
		szTmp = _T("台胞证");
		break;
	case THOST_FTDC_ICT_HomeComingCard:
		szTmp = _T("回乡证");
		break;
	case THOST_FTDC_ICT_LicenseNo:
		szTmp = _T("营业执照号");
		break;
	case THOST_FTDC_ICT_TaxNo:
		szTmp = _T("税务登记号");
		break;
	case THOST_FTDC_ICT_HMMainlandTravelPermit:
		szTmp = _T("港澳往内地证");
		break;
	case THOST_FTDC_ICT_TwMainlandTravelPermit:
		szTmp = _T("台湾往内地证");
		break;
	case THOST_FTDC_ICT_DrivingLicense:
		szTmp = _T("驾照");
		break;
	case THOST_FTDC_ICT_SocialID:
		szTmp = _T("当地社保ID");
		break;
	case THOST_FTDC_ICT_LocalID:
		szTmp = _T("当地身份证");
		break;
	case THOST_FTDC_ICT_BusinessRegistration:
		szTmp = _T("商业登记证");
		break;
	case THOST_FTDC_ICT_HKMCIDCard:
		szTmp = _T("港澳身份证");
		break;
	case THOST_FTDC_ICT_AccountsPermits:
		szTmp = _T("人行许可证");
		break;
	case THOST_FTDC_ICT_OtherCard:
		szTmp = _T("其他证件");
		break;
	}
	return szTmp;
}

CString JgCombType(TThostFtdcCombinationTypeType bType)
{
	CString szTmp;
	
	switch(bType)
	{
	case THOST_FTDC_COMBT_Future:
		szTmp = _T("期货组合");
		break;
	case THOST_FTDC_COMBT_BUL:
		szTmp = _T("垂直价差BUL");
		break;
	case THOST_FTDC_COMBT_BER:
		szTmp = _T("垂直价差BER");
		break;
	case THOST_FTDC_COMBT_STD:
		szTmp = _T("跨式组合");
		break;
	case THOST_FTDC_COMBT_STG:
		szTmp = _T("宽跨式组合");
		break;
	case THOST_FTDC_COMBT_PRT:
		szTmp = _T("备兑组合");
		break;
	case THOST_FTDC_COMBT_CLD:
		szTmp = _T("时间价差组合");
		break;
	default:
		szTmp=_T("未知");
		break;
	}
	return szTmp;
}

CString JgOptType(TThostFtdcOptionsTypeType bType)
{
	CString szTmp;
	
	switch(bType)
	{
	case THOST_FTDC_CP_CallOptions:
		szTmp = _T("认购");
		break;
	case THOST_FTDC_CP_PutOptions:
		szTmp = _T("认沽");
		break;
	default:
		szTmp=_T("未知");
		break;
	}
	return szTmp;
}

CString JgTdType(TThostFtdcTradeTypeType bType)
{
	CString szTmp;

	switch(bType)
	{
	case 0:
		szTmp = _T("普通成交");
		break;
	case 1:
		szTmp = _T("期权执行");
		break;
	case 2:
		szTmp = _T("OTC成交");
		break;
	case 3:
		szTmp = _T("期转现");
		break;
	case 4:
		szTmp = _T("组合成交");
		break;
	case '#':
		szTmp = _T("组合拆分");
		break;
	default:
		szTmp=_T("未知");
		break;
	}
	return szTmp;
}

CString JgOcType(TThostFtdcOffsetFlagType bType)
{
	CString szTmp = _T("未知");
	
	switch(bType)
	{
	case THOST_FTDC_OF_Open:
		szTmp = _T("开仓");
		break;
	case THOST_FTDC_OF_Close:
	case THOST_FTDC_OF_CloseToday:
	case THOST_FTDC_OF_CloseYesterday:
		szTmp = _T("平仓");
		break;
	case THOST_FTDC_OF_ForceClose:
		szTmp = _T("强平");
		break;
	/*
	case THOST_FTDC_OF_CloseToday:
		szTmp = _T("平今");
		break;
	case THOST_FTDC_OF_CloseYesterday:
		szTmp = _T("平昨");
		break;
	*/
	case THOST_FTDC_OF_ForceOff:
		szTmp=_T("强减");
		break;
	case THOST_FTDC_OF_LocalForceClose:
		szTmp=_T("本地强平");
		break;
	default:
		szTmp=_T("未知");
		break;
	}

	return szTmp;
}

CString JgOrdSubmitStat(TThostFtdcOrderSubmitStatusType bType)
{
	CString szTmp = _T("未知");
	
	switch(bType)
	{
        case THOST_FTDC_OSS_InsertSubmitted:
            szTmp = _T("交易所:委托已提交");
            break;
        case THOST_FTDC_OSS_CancelSubmitted:
            szTmp = _T("交易所:撤单已提交");
            break;
        case THOST_FTDC_OSS_ModifySubmitted:
            szTmp = _T("交易所:改单已提交");
            break;
        case THOST_FTDC_OSS_Accepted:
            szTmp = _T("交易所:已经接受");
            break;
        case THOST_FTDC_OSS_InsertRejected:
            szTmp = _T("CTP:报单已被拒");
            break;
        case THOST_FTDC_OSS_CancelRejected:
            szTmp = _T("CTP:撤单已被拒");
            break;
        case THOST_FTDC_OSS_ModifyRejected:
            szTmp=_T("CTP:改单已被拒");
            break;
        default:
            szTmp=_T("CTP:未知");
            break;
	}
	
	return szTmp;
}

CString JgOrdStatType(TThostFtdcContingentConditionType bType)
{
	CString szTmp = _T("错误");
	
	switch(bType)
	{
        case THOST_FTDC_OST_AllTraded:
            szTmp = _T("全部成交");
            break;
        case THOST_FTDC_OST_PartTradedQueueing:
            szTmp = _T("部分成交");
            break;
        case THOST_FTDC_OST_PartTradedNotQueueing:
            szTmp = _T("部分成交不在队列中");
            break;
        case THOST_FTDC_OST_NoTradeQueueing:
            szTmp = _T("未成交");
            break;
        case THOST_FTDC_OST_NoTradeNotQueueing:
            szTmp = _T("未成交不在队列中");
            break;
        case THOST_FTDC_OST_Canceled:
            szTmp=_T("已撤单");
            break;
        case THOST_FTDC_OST_Unknown:
            szTmp=_T("未知");
            break;
        case THOST_FTDC_OST_NotTouched:
            szTmp=_T("尚未触发");
            break;
        case THOST_FTDC_OST_Touched:
            szTmp=_T("已触发");
            break;
	}
	
	return szTmp;
}

CString JgCcOrdType(TThostFtdcContingentConditionType bType)
{
	CString szTmp = _T("立即");
	
	switch(bType)
	{
        case THOST_FTDC_CC_Immediately:
            szTmp = _T("立即单");
            break;
        case THOST_FTDC_CC_Touch:
            szTmp = _T("止损单");
            break;
        case THOST_FTDC_CC_TouchProfit:
            szTmp = _T("止盈单");
            break;
        case THOST_FTDC_CC_ParkedOrder:
            szTmp = _T("预埋单");
            break;
        default:
            szTmp = _T("条件单");
            break;
	}
	
	return szTmp;
}

CString JgOrdCondType(TThostFtdcContingentConditionType bType,TThostFtdcPriceType dStopPx)
{
	CString szTmp = _T("未知");
	
	switch(bType)
	{
		case THOST_FTDC_CC_Touch:
			szTmp.Format(_T("最新价<%s止损"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_TouchProfit:
			szTmp.Format(_T("最新价>%s止盈"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_LastPriceGreaterThanStopPrice:
			szTmp.Format(_T("最新价＞%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_LastPriceGreaterEqualStopPrice:
			szTmp.Format(_T("最新价≥%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_LastPriceLesserThanStopPrice:
			szTmp.Format(_T("最新价＜%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_LastPriceLesserEqualStopPrice:
			szTmp.Format(_T("最新价≤%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_AskPriceGreaterThanStopPrice:
			szTmp.Format(_T("卖一价＞%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_AskPriceGreaterEqualStopPrice:
			szTmp.Format(_T("卖一价≥%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_AskPriceLesserThanStopPrice:
			szTmp.Format(_T("卖一价＜%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_AskPriceLesserEqualStopPrice:
			szTmp.Format(_T("卖一价≤%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_BidPriceGreaterThanStopPrice:
			szTmp.Format(_T("买一价＞%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_BidPriceGreaterEqualStopPrice:
			szTmp.Format(_T("买一价≥%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_BidPriceLesserThanStopPrice:
			szTmp.Format(_T("买一价＜%s"),FiltPx(dStopPx));
			break;
		case THOST_FTDC_CC_BidPriceLesserEqualStopPrice:
			szTmp.Format(_T("买一价≤%s"),FiltPx(dStopPx));
			break;
	}
	
	return szTmp;
}

CString JgParkOrdStat(TThostFtdcParkedOrderStatusType bType)
{
	CString szTmp = _T("未知");
	
	switch(bType)
	{
		case THOST_FTDC_PAOS_NotSend:
			szTmp = _T("未发送");
			break;
		case THOST_FTDC_PAOS_Send:
			szTmp = _T("已发送");
			break;
		case THOST_FTDC_PAOS_Deleted:
			szTmp = _T("已删除");
			break;
		case THOST_FTDC_OST_Touched:
			szTmp = _T("已触发");
			break;
		case THOST_FTDC_OST_NotTouched:
			szTmp = _T("未触发");
			break;
		case THOST_FTDC_OST_Canceled:
			szTmp = _T("已撤单");
			break;
		case THOST_FTDC_OST_ErrOrd:
			szTmp = _T("错误");
			break;
	}
	
	return szTmp;
}

CString JgPosBsType(TThostFtdcPosiDirectionType bType)
{
	CString szTmp ;
	
	switch(bType)
	{
        case THOST_FTDC_PD_Net:
            szTmp = _T("净");
            break;
        case THOST_FTDC_PD_Long:
            szTmp = _T("多头");
            break;
        case THOST_FTDC_PD_Short:
            szTmp = _T("空头");
            break;
        default:
            szTmp=_T("未知");
            break;
	}
	return szTmp;
}

CString JgBsType(TThostFtdcDirectionType bType)
{
	CString szTmp ;
	
	switch(bType)
	{
        case THOST_FTDC_D_Buy:
            szTmp = _T("买");
            break;
        case THOST_FTDC_D_Sell:
            szTmp = _T("卖");
            break;
        default:
            szTmp=_T("未知");
            break;
	}
	return szTmp;
}

CString JgTbType(TThostFtdcHedgeFlagType bType)
{
	CString szTmp ;
	
	switch(bType)
	{
        case THOST_FTDC_HF_Speculation:
            szTmp = _T("投机");
            break;
        case THOST_FTDC_HF_Arbitrage:
            szTmp = _T("套利");
            break;
        case THOST_FTDC_HF_Hedge:
            szTmp = _T("套保");
            break;
        case THOST_FTDC_HF_MarketMaker:
            szTmp = _T("市商");
            break;
        default:
            szTmp=_T("未知");
            break;
	}
	return szTmp;
}

CString JgTdCodeType(TThostFtdcClientIDTypeType bType)
{
	CString szTmp ;
	
	switch(bType)
	{
        case THOST_FTDC_CIDT_Speculation:
            szTmp = _T("投机");
            break;
        case THOST_FTDC_CIDT_Arbitrage:
            szTmp = _T("套利");
            break;
        case THOST_FTDC_CIDT_Hedge:
            szTmp = _T("套保");
            break;
        case THOST_FTDC_CIDT_MarketMaker:
            szTmp = _T("市商");
            break;
        default:
            szTmp=_T("未知");
            break;
	}
	return szTmp;
}

CString JgExchage(TThostFtdcExchangeIDType ExID)
{
	CString szTmp = _T("神交所");
	if (!strcmp(ExID,_CZCE))
	{
		szTmp = LoadStr(IDS_CZCE);
	}
	if (!strcmp(ExID,_DCE))
	{
		szTmp = LoadStr(IDS_DCE);
	}
	if (!strcmp(ExID,_SFE))
	{
		szTmp = LoadStr(IDS_SHFE);
	}
	if (!strcmp(ExID,_CFX))
	{
		szTmp = LoadStr(IDS_CFFEX);
	}
	if (!strcmp(ExID,_CINE))
	{
		szTmp = LoadStr(IDS_CINE);
	}

	return szTmp;
}

CString JgBfTdType(TThostFtdcTradeCodeType TdCode)
{
	CString szTmp = _T("未知");
	if (!strcmp(TdCode,"202001"))
	{ szTmp = _T("银行资金转期货"); }
	else if(!strcmp(TdCode,"202002"))
	{ szTmp = _T("期货资金转银行"); }
	else if(!strcmp(TdCode,"203001"))
	{ szTmp = _T("冲正银行转期货"); }
	else if(!strcmp(TdCode,"203002"))
	{ szTmp = _T("冲正期货转银行"); }
	else if(!strcmp(TdCode,"204002"))
	{ szTmp = _T("查询银行余额"); }
	else if(!strcmp(TdCode,"204004"))
	{ szTmp = _T("查询银期直通车"); }
	else if(!strcmp(TdCode,"204005"))
	{ szTmp = _T("查询转账明细"); }
	else if(!strcmp(TdCode,"204006"))
	{ szTmp = _T("查询银行状态"); }
	else if(!strcmp(TdCode,"204009"))
	{ szTmp = _T("查询当日流水"); }
	else if(!strcmp(TdCode,"206001"))
	{ szTmp = _T("入金通知"); }
	else if(!strcmp(TdCode,"206002"))
	{ szTmp = _T("出金通知"); }
	else if(!strcmp(TdCode,"901001"))
	{ szTmp = _T("开通银期直通车"); }
	else if(!strcmp(TdCode,"901002"))
	{ szTmp = _T("解除银期直通车"); }
	else if(!strcmp(TdCode,"905001"))
	{ szTmp = _T("期商签到"); }
	else if(!strcmp(TdCode,"905002"))
	{ szTmp = _T("期商签退"); }
	else if(!strcmp(TdCode,"905003"))
	{ szTmp = _T("同步密钥"); }
	return szTmp;
}

CString JgBkName(TThostFtdcProtocalIDType bType)
{
	CString szIn =_T("");
	switch(bType)
	{
        case THOST_FTDC_PID_ABCProtocal:
            szIn = _T("农业银行");
            break;
        case THOST_FTDC_PID_CBCProtocal:
            szIn = _T("中国银行");
            break;
        case THOST_FTDC_PID_BOCOMProtocal:
            szIn = _T("交通银行");
            break;
        case THOST_FTDC_PID_CCBProtocal:
            szIn = _T("建设银行");
            break;
        case THOST_FTDC_PID_ICBCProtocal:
            szIn = _T("工商银行");
            break;
        case THOST_FTDC_PID_FBTPlateFormProtocal:
            szIn = _T("其他银行");
            break;
	}	
	return szIn;
}

CString GetSpecFilePath(LPCTSTR lpsz)
{
	TCHAR szFilePath[MAX_PATH]={0};
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	_tcscat(szFilePath,lpsz);

	return CString(szFilePath);
}

//ansi版本
void GetCurDir(LPSTR lpRet,LPCSTR lpDir)
{
	char szFilePath[MAX_PATH]={0};
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[1] = 0;
	strcat(szFilePath,lpDir);
	strcpy(lpRet,szFilePath);
}

int res2file(LPCTSTR lpName,LPCTSTR lpType,LPCTSTR filename)
{
	HRSRC myres = FindResource (NULL,lpName,lpType);
	HGLOBAL gl = LoadResource (NULL,myres);
	LPVOID lp = LockResource(gl);
	HANDLE fp = CreateFile(filename ,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if (!fp) { return false; }
	
	DWORD a;
	if (!WriteFile(fp,lp,SizeofResource (NULL,myres),&a,NULL)) { return false; }
	
	CloseHandle (fp);
	FreeResource (gl);
	return true;
}

void List2Csv(CXListCtrl* pList,LPCTSTR lpName)
{
	CMemFile mmf;
	BYTE bBom[3]={0xEF,0xBB,0xBF};
	BYTE bDot = ',';
	BYTE bEnter = '\n';
	mmf.Write(&bBom,3);
	
	int nItem = pList->GetItemCount();
	int nColumns = pList->GetHeaderCtrl()->GetItemCount();
	HDITEM hd;
	TCHAR pItem[MAX_PATH];
	char utf8[3*MAX_PATH];
	CString str;
	int nCol = 0; 
	for (nCol = 0; nCol < nColumns; nCol++)
	{
		ZeroMemory(pItem,_tcslen(pItem));
		hd.pszText=pItem;
		hd.cchTextMax=MAX_PATH;
		hd.mask = HDI_TEXT;
		pList->GetHeaderItem(nCol, &hd);
		
		//str.Format(_T("\"%s\""),pItem);
		uni2ansi(CP_UTF8,pItem,utf8);
		mmf.Write(utf8,static_cast<int>(strlen(utf8)));
		if (nCol < nColumns-1){ mmf.Write(&bDot,1); }
		if (nCol == nColumns-1){ mmf.Write(&bEnter,1); }	
	}
	
	for (int nRow = 0; nRow < nItem; nRow++)
	{
		for (nCol = 0; nCol < nColumns; nCol++)
		{
			str.Format(_T("\"%s\""),(LPCTSTR)pList->GetItemText(nRow,nCol));
			uni2ansi(CP_UTF8,(LPTSTR)(LPCTSTR)str,utf8);

			mmf.Write(utf8,static_cast<int>(strlen(utf8)));
			if (nCol < nColumns-1){ mmf.Write(&bDot,1); }
			if (nCol == nColumns-1){ mmf.Write(&bEnter,1); }
		}
	}
	
	CFile fLog(lpName, CFile::modeReadWrite | CFile::modeCreate | CFile::typeText);
	
	UINT uLen = static_cast<UINT>(mmf.GetLength());
	PBYTE pMemF = mmf.Detach();
	fLog.Write(pMemF,uLen);
	fLog.Close();
	mmf.Close();
}

void VerifyEdit(CWnd* pDlg,int nID,bool bAllowDigit)
{
	CString szTxt;
	CEdit* pEdit = (CEdit*)pDlg->GetDlgItem(nID);
	pEdit->GetWindowText(szTxt);

	LPCTSTR lp = szTxt.GetBuffer(0);
	int iLen = szTxt.GetLength();
	
	BOOL bRet = FALSE;
	for(int i=0;*(lp+i) != '\0';i++)
	{
		if (bAllowDigit)
		{ bRet = (isdigit(*(lp+i)) || *(lp+i) =='.'); }
		else
		{ bRet = isdigit(*(lp+i)); }

		if (!bRet)
		{
			pEdit->SetSel(i, i+1);
			pEdit->Clear();
		}
	}
	
	pEdit->GetWindowText(szTxt);
	if (!szTxt.IsEmpty()){ pDlg->UpdateData(); }	
}

CString GenDef(LPCTSTR lpPre,LPCTSTR lpExt)
{
	CString szDef;
	SYSTEMTIME tm;
	
	::GetLocalTime(&tm);
	szDef.Format(_T("%s_%04d%02d%02d%02d%02d%02d.%s"), lpPre,tm.wYear,tm.wMonth,tm.wDay,
		tm.wHour,tm.wMinute, tm.wSecond,lpExt);
	
	return szDef;
}

CString TransTime(TThostFtdcTradeTimeType TdTm)
{
	CString szTm=_T("");
	ansi2uni(CP_ACP,TdTm,szTm.GetBuffer(MAX_PATH));
	szTm.ReleaseBuffer();
	
	return szTm;
}

BOOL GenXmlHdr(LPCTSTR lpFile)
{
	CFile fLog(lpFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeText);
		
	LPCSTR szHdr ="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Fees />\n<Mgrs>\n<Futures />\n<Options />\n</Mgrs>";
	BYTE bBom[3]={0xEF,0xBB,0xBF};
	fLog.Write(&bBom,3);
		
	fLog.Write(szHdr, static_cast<int>(strlen(szHdr)));
	fLog.Close();
		
	return TRUE;
}

CString GetCTPErr(TThostFtdcErrorMsgType ErrorMsg)
{
	CString szOut;
	ansi2uni(CP_ACP,ErrorMsg,szOut.GetBuffer(MAX_PATH));
	szOut.ReleaseBuffer();

	return szOut;
}

CString JgAnyOrdType(TThostFtdcOrderPriceTypeType cPxType,TThostFtdcTimeConditionType cTcType)
{
	CString szTemp = _T("—");

	if (cPxType==THOST_FTDC_OPT_AnyPrice)
	{
		if (cTcType==THOST_FTDC_TC_IOC) { szTemp = _T("市价"); }
		if (cTcType==THOST_FTDC_TC_GFD) { szTemp = _T("市转限"); }
	}

	if (cPxType==THOST_FTDC_OPT_BestPrice)
	{
		if (cTcType==THOST_FTDC_TC_IOC) { szTemp = _T("最优"); }
		if (cTcType==THOST_FTDC_TC_GFD) { szTemp = _T("优转限"); }
	}

	if (cPxType==THOST_FTDC_OPT_FiveLevelPrice)
	{
		if (cTcType==THOST_FTDC_TC_IOC) { szTemp = _T("L2市"); }
		if (cTcType==THOST_FTDC_TC_GFD) { szTemp = _T("L2转限"); }
	}

	return szTemp;
}

BOOL DelFolder(LPCTSTR pszFolder)
{
	SHFILEOPSTRUCT	fos;
	
	ZeroMemory(&fos, sizeof(fos)) ;
	fos.hwnd = HWND_DESKTOP;
	fos.wFunc = FO_DELETE ;
	fos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION ;
	fos.pFrom = pszFolder ;
	
	if(!SHFileOperation(&fos))
		return FALSE;
	return TRUE;
}

///////////////////////仅处理ipv4//////////////////////////////////////
bool GetIpAddr(TThostFtdcIPAddressType szAddr)
{
    char strHost[MAX_PATH] = { 0 };
    if (SOCKET_ERROR != gethostname(strHost, sizeof(strHost)))
    {
	  addrinfo hints,*res;
	  
	  ZeroMemory(&hints, sizeof(addrinfo));
	  hints.ai_family = AF_INET; 
	  hints.ai_socktype = SOCK_STREAM;
	  if (getaddrinfo(strHost,NULL,&hints,&res) == -1) { return false; }

	  if (res != NULL && res->ai_addr != NULL)
	  {
		  sockaddr_in *addr = (sockaddr_in*)res->ai_addr;
		  strcpy(szAddr,inet_ntoa(addr->sin_addr));

		  freeaddrinfo(res);
		  return true;
	  }
	  //for (cur = res; cur != NULL; cur = cur->ai_next) 
    }

    return false;
}

bool GetMacAddr(LPCSTR sIP,TThostFtdcMacAddressType szMac)  
{  
	PIP_ADAPTER_INFO pinfo=NULL;
	DWORD len=0;
	DWORD nError;
	
	nError = GetAdaptersInfo(pinfo,&len);
	if (nError==ERROR_NO_DATA||nError==ERROR_NOT_SUPPORTED){ return false; }
	if (nError==ERROR_BUFFER_OVERFLOW)
	{
		pinfo= (PIP_ADAPTER_INFO)malloc(len);
		nError = GetAdaptersInfo(pinfo,&len);
		if ( nError != 0 ){ return false; }
	}
	
	while ( pinfo != NULL )
	{
		PIP_ADDR_STRING pAddressList = &(pinfo->IpAddressList);
		while( pAddressList != NULL )
		{
			if ( !_stricmp(pAddressList->IpAddress.String,sIP))
			{
				sprintf(szMac,"%02X:%02X:%02X:%02X:%02X:%02X",
				pinfo->Address[0],pinfo->Address[1],pinfo->Address[2],
				pinfo->Address[3],pinfo->Address[4],pinfo->Address[5]);

				return true;
			}
			else
				pAddressList = pAddressList->Next;
		}
		pinfo = pinfo->Next;
	}
	return false; 
 }
 
BOOL SetWndAlpha(HWND hwnd, int iAlpha)
{
	::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	return ::SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), iAlpha, LWA_ALPHA);
}

void EnableClose(CWnd* pWnd,BOOL bEnable)
{
	if(pWnd)
	{
		CMenu* pSysMenu = pWnd->GetSystemMenu(FALSE);
		if(pSysMenu != NULL)
		{
			if (bEnable)
			{ pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_ENABLED); }
			else
			{ pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND| MF_DISABLED|MF_GRAYED); }
		}
	}
}

BOOL IsWndActive(HWND h)
{
	return (::GetActiveWindow() == h);
}

void AddToolTips(CWnd* pDlg,CToolTipCtrl* ptt)
{
	CString strTip;
	for (CWnd* pChild = pDlg->GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT)) 
	{
		strTip = LoadStr(pChild->GetDlgCtrlID());
		if (!strTip.IsEmpty()) { ptt->AddTool(pChild, strTip); }
	}
}

BOOL SetPrivilege(LPCTSTR privName, BOOL bEnable)
{
	HANDLE				hToken;
	TOKEN_PRIVILEGES	tp = {1};
	
	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))
		return FALSE;
	
	BOOL ret = ::LookupPrivilegeValue(NULL, privName, &tp.Privileges[0].Luid);
	
	if (ret) {
		if (bEnable) tp.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
		else		 tp.Privileges[0].Attributes = 0;
		
		ret = ::AdjustTokenPrivileges(hToken, FALSE, &tp, 0, 0, 0);
	}
	::CloseHandle(hToken);
	
	return	ret;
}

BOOL PcBeep(UINT uFreq,UINT uMs)
{
	return Beep(uFreq,uMs);
}

void FirstLetter(int nCode,char* bLetter)
{
	strcpy(bLetter,"");
	
	if((nCode==333) ||(nCode >= 1601 && nCode < 1637)){ strcpy(bLetter,"A"); }
	if((nCode==334) ||(nCode >= 1637 && nCode < 1833)){ strcpy(bLetter,"B"); }
	if(nCode >= 1833 && nCode < 2078){ strcpy(bLetter,"C"); }
	if(nCode >= 2078 && nCode < 2274){ strcpy(bLetter,"D"); }
	if(nCode >= 2274 && nCode < 2302){ strcpy(bLetter,"E"); }
	if((nCode==7058) ||(nCode==6029) ||(nCode >= 2302 && nCode < 2433)){ strcpy(bLetter,"F"); }
	if((nCode==7860) ||(nCode >= 2433 && nCode < 2594)){ strcpy(bLetter,"G"); }
	if((nCode==7072) ||(nCode==4848)||(nCode >= 2594 && nCode < 2787)){ strcpy(bLetter,"H"); }
	if(nCode >= 2787 && nCode < 3106){ strcpy(bLetter,"J"); }
	if(nCode >= 3106 && nCode < 3212){ strcpy(bLetter,"K"); }
	if((nCode==6880) ||(nCode==8775) ||(nCode==7914) ||(nCode==7321) ||(nCode==8275) ||(nCode >= 3212 && nCode < 3472)){ strcpy(bLetter,"L"); }
	if((nCode==7866) ||(nCode >= 3472 && nCode < 3635)){ strcpy(bLetter,"M"); }
	if(nCode >= 3635 && nCode < 3722){ strcpy(bLetter,"N"); }
	if(nCode >= 3722 && nCode < 3730){ strcpy(bLetter,"O"); }
	if((nCode==6907) ||(nCode >= 3730 && nCode < 3858)){ strcpy(bLetter,"P"); }
	if(nCode >= 3858 && nCode < 4027){ strcpy(bLetter,"Q"); }
	if(nCode >= 4027 && nCode < 4086){ strcpy(bLetter,"R"); }
	if(nCode >= 4086 && nCode < 4390){ strcpy(bLetter,"S"); }
	if((nCode==7849) ||(nCode==8311) ||(nCode >= 4390 && nCode < 4558)){ strcpy(bLetter,"T"); }
	if(nCode >= 4558 && nCode < 4684){ strcpy(bLetter,"W"); }
	if((nCode==6817) ||(nCode==8646) ||((nCode!=4848)&&(nCode >= 4684 && nCode < 4925))){ strcpy(bLetter,"X"); }
	if((nCode==7163) ||(nCode==8014) ||(nCode >= 4925 && nCode < 5249)){ strcpy(bLetter,"Y"); }
	if((nCode==5958) ||(nCode >= 5249 && nCode < 5590)){ strcpy(bLetter,"Z"); }
}

void GetFirstLetter(CString strName, CString& strFirstLetter)
{
    BYTE ucHigh, ucLow;
    int  nCode;
    char bRet[4]={0};
    strFirstLetter.Empty();
	char szName[MAX_PATH]={0},szRet[MAX_PATH]={0};
	char ch2[2]={0};
	uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)strName,szName);
	
	int iLen = static_cast<int>(strlen(szName));
    for (int i=0; i<iLen; i++)
    {
		if ( (BYTE)szName[i] < 0x80) 
		{ 
			if ((szName[i] != 0x20)&&(szName[i] != '*'))
			{
				ch2[0] = szName[i];
				strcat(szRet,ch2);
			}
			continue;	
		}
		
        ucHigh = (BYTE)szName[i];
        ucLow  = (BYTE)szName[i+1];
		
        if ( ucHigh < 0xa1 || ucLow < 0xa1)
            continue;
        else
            nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0;
		
        FirstLetter(nCode,bRet);
		strcat(szRet,(LPCSTR)bRet);
        i++;
    }
	
	ansi2uni(CP_ACP,szRet,strFirstLetter.GetBuffer(MAX_PATH));
	strFirstLetter.ReleaseBuffer();
}

///////////////////////////////////////////
CString GetFileOnly(LPCTSTR Path)
{
	CString temp = (LPCTSTR) Path; 
	::PathStripPath(temp.GetBuffer(0));
	temp.ReleaseBuffer(-1);
	return temp;
}

CString GetFolderOnly(LPCTSTR Path)
{
	CString temp = (LPCTSTR) Path; // Force CString to make a copy
	::PathRemoveFileSpec(temp.GetBuffer(0));
	temp.ReleaseBuffer(-1);
	return temp;
}

CString AddSlash(LPCTSTR Path)
{
	CString cs = Path;
	::PathAddBackslash(cs.GetBuffer(_MAX_PATH));
	cs.ReleaseBuffer(-1);
	if(cs.IsEmpty()) { cs = _T("\\"); }
	return cs;
}

CString RemoveSlash(LPCTSTR Path)
{
	CString cs = Path;
	::PathRemoveBackslash(cs.GetBuffer(_MAX_PATH));
	cs.ReleaseBuffer(-1);
	return cs;
}

CString GetFileExt(LPCTSTR Path)
{
	CString cs;
	cs = ::PathFindExtension(Path);
	return cs;
}

CString RenameFileExt(LPCTSTR Path, LPCTSTR Ext)
{
	CString cs = Path;
	::PathRenameExtension(cs.GetBuffer(_MAX_PATH), Ext);
	return cs;
}

CString CompactPath(LPCTSTR Path, UINT cchMax)
{
	CString cs = Path;
	WCHAR pathbuf[MAX_PATH] = { 0 };
	if (::PathCompactPathEx(pathbuf, cs, cchMax, 0)) { cs = pathbuf; }

	return cs;
}

CString GetModulePath(HMODULE hModule)
{
	CString ret;
	int pos, len = MAX_PATH - 1;
	for (;;) {
		pos = GetModuleFileName(hModule, ret.GetBuffer(len), len);
		if (pos == len) {
			len *= 2;
			ret.ReleaseBuffer(0);
			continue;
		}
		ret.ReleaseBuffer(pos);
		break;
	}

	ASSERT(!ret.IsEmpty());
	return ret;
}

CString GetProgramPath()
{
	return GetModulePath(NULL);
}

CString GetProgramDir()
{
	return AddSlash(GetFolderOnly(GetModulePath(NULL)));
}

bool ExploreToFile(CString path)
{
	bool success = false;
	HRESULT res = CoInitialize(NULL);
	
	if (res == S_OK || res == S_FALSE) {
		LPITEMIDLIST pidl;
		
		if (SHParseDisplayName(path, NULL, &pidl, 0, NULL) == S_OK) {
			success = SUCCEEDED(SHOpenFolderAndSelectItems(pidl, 0, NULL, 0));
			CoTaskMemFree(pidl);
		}
		
		CoUninitialize();
	}
	
	return success;
}

CString GetResUrl(LPCTSTR szName)
{
	CString szRes;
	szRes.Format(_T("res://%s/%s"), GetProgramPath(), szName);
	return szRes;
}

CString GetResUrl(UINT uId)
{
	CString szRes;
	szRes.Format(_T("res://%s/%d"), GetProgramPath(), uId);
	return szRes;
}

void fmtNtMsg(CString& strIn,LPCTSTR lpLink,LPCTSTR lpPre)
{
	CString szOut(strIn),szLink;
	szOut.Replace(_T("\n"),_T("<BR>"));
	
	szLink.Format(_T("<BR><P ALIGN=CENTER>%s: <i><span style='font-family:Arial'><a href=\"%s\">%s</a></span></i></P>"),lpPre,lpLink,lpLink);
	szOut += szLink;
	
	strIn = szOut;
}

void SplitFrtUrl(CString szUrl,CString& szSvr,int& iPort)
{
	int iPos;
	if (!_tcsnicmp(szUrl,_T("socks"),5))
	{
		CString szSnP;
		iPos = szUrl.Find(_T("@"));
		szSnP = szUrl.Mid(iPos+1);
		
		iPos = szSnP.Find(':');
		szSvr = szSnP.Mid(0,iPos);
		iPort = _ttoi(szSnP.Mid(iPos+1));
	}
	else if(!_tcsnicmp(szUrl,_T("tcp"),3))
	{
		iPos = szUrl.ReverseFind(':');
		int iPos2 = szUrl.Find(_T("//"));
		
		iPort = _ttoi(szUrl.Mid(iPos+1));
		szSvr = szUrl.Mid(iPos2+2,iPos-iPos2-2);
	}
}

void GetDelayMs(LPCTSTR lpSvr,int iPort,__int64& iMs)
{
	CSocket *pSock = new CSocket;
	if (pSock->Socket())
	{
		CCostMs tmCost;
		BOOL bRes = pSock->Connect(lpSvr, iPort);

		if (!bRes){ iMs = -1;}
		else
		{ iMs = tmCost.CostTime(); }
		pSock->Close();
	}
	DEL(pSock);
}

BOOL IsWinVerGreaterThan(DWORD dwMajor, DWORD dwMinor)
{
	OSVERSIONINFOEXW osvi = { 0 };
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajor;
	osvi.dwMinorVersion = dwMinor;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER);
	if (::VerifyVersionInfoW(&osvi, VER_MAJORVERSION, dwlConditionMask))
		return TRUE;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER);

	return ::VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask);
}

BOOL IsWinVerEqualTo(DWORD dwMajor, DWORD dwMinor)
{
	OSVERSIONINFOEXW osvi = { 0 };
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajor;
	osvi.dwMinorVersion = dwMinor;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);

	return ::VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask);
}

BOOL GetNtVerNo(DWORD& dwMajor, DWORD& dwMinor, DWORD& dwBuildNo)
{
	BOOL bRet = FALSE;
	typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
	pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
	pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(GetModuleHandle(_T("ntdll")), "RtlGetNtVersionNumbers");
	if (pfRtlGetNtVersionNumbers)
	{
		pfRtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuildNo);
		dwBuildNo &= 0x0ffff;
		bRet = TRUE;
	}

	return bRet;
}

BOOL IsWinXP()
{ return  IsWinVerEqualTo(5,1); }

BOOL IsWinXPOrLater()
{ return IsWinVerGreaterThan(5,1); }

BOOL IsWinVista()
{ return IsWinVerEqualTo(6, 0); }

BOOL IsWinVistaOrLater()
{ return IsWinVerGreaterThan(6, 0); }

BOOL IsWinSeven()
{ return IsWinVerGreaterThan(6, 1); }

BOOL IsWinSevenOrLater()
{ return IsWinVerGreaterThan(6, 1); }

BOOL IsWinEight()
{ return IsWinVerEqualTo(6, 2); }

BOOL IsWinEightOrLater()
{ return IsWinVerGreaterThan(6, 2); }

BOOL IsWinTen()
{ return IsWinVerEqualTo(0xA, 0);}

BOOL IsWinTenOrLater()
{ return IsWinVerGreaterThan(0xA, 0); }

BOOL IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
	BOOL bIsWow64 = FALSE;
	if (NULL == fnIsWow64Process) {
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
	}
	if (NULL != fnIsWow64Process) {
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64;
}

BOOL IsW64()
{
#ifdef _WIN64
	return TRUE;
#endif

	return IsWow64();
}
