#pragma once

#include "resource.h"
#include "pugixml.hpp"
#include "base64.h"
#include "DIYCtrl/ColorStatic.h"
#include "DIYCtrl/XListCtrl.h"

#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <functional>
#include <XUTILITY>
//#include "log4z.h"

using namespace std;
using namespace pugi;
//using namespace zsummer::log4z;

//#define _REAL_CTP_
#define _NEED_LOGIN_

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0     
#endif

#ifdef _WIN64
#include "api_libs64/ThostFtdcMdApi.h"
#include "api_libs64/ThostFtdcTraderApi.h"
#if _MSC_VER < 1600
#pragma comment(lib,"bufferoverflowu.lib")
#endif
#pragma comment(lib,"api_libs64/thostmduserapi.lib")
#pragma comment(lib,"api_libs64/thosttraderapi.lib")
#else
#include "api_libs/ThostFtdcMdApi.h"
#include "api_libs/ThostFtdcTraderApi.h"
#pragma comment(lib,"api_libs/thostmduserapi.lib")
#pragma comment(lib,"api_libs/thosttraderapi.lib")
#endif

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "IPHlpApi.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")

enum RW_MODE {LOAD=0,SAVE};
enum CONDPX_TYPE{LAST_DSJ = 0,LMT_PX,ANY_PX};
enum ORDCONV_TYPE{INP2PARKED = 1,ORD2PARKED,PARKED2INP,PARKED2ORD,PK_LOCCOND2INP};
enum TAB_IDX{ONROAD_IDX = 0,ORDER_IDX,INVPOS_IDX,TRADE_IDX,INSLST_IDX,PKCON_IDX};
enum T_CONDPARKED{ENTERTD_LOCAL = 0,TMORD_LOCAL,PARKED_SERVER,COND_LOCAL,COND_SERVER,TOUCH_ZS,TOUCH_ZY,COND_ERROR,PARKED_ACT};

enum ONRDITEM{COL_SYSID1=0,COL_INST1,COL_DIR1,COL_KPP1,COL_VOLLEFT1,COL_ORDPX1,COL_ORDTM1,COL_FRZMGR1};

enum ORDINFITEM{COL_SYSID2=0,COL_INST2,COL_DIR2,COL_KPP2,COL_ORDSTAT2,COL_ORDPX2,
COL_VOLALL2,COL_VOLLEFT2,COL_VOLTD2,COL_AVGPX2,COL_ORDTM2,COL_FRZMGR2,COL_FRZFEE2,COL_STATMSG2};

enum INVPOSITEM{COL_INST3=0,COL_POSDIR3,COL_POS3,COL_VALIDPOS3,COL_AVGPX3,
COL_PROFIT3,COL_USEMGR3,COL_SUMPROFIT3};

enum TRADEITEM{COL_INST4=0,COL_DIR4,COL_KPP4,COL_TDPX4,COL_TDVOL4,
COL_TDTM4,COL_TDFEE4,COL_HEDGE4,COL_TDTYPE4,COL_EXHID4,COL_TDID4,COL_SYSID4};

enum INSTSITEM{COL_PRODID5=0,COL_INST5,COL_INSTNAME5,COL_EXHID5,COL_VOLMUL5,
COL_PXTICK5,COL_PRODCLS5,COL_EXPDATE5,COL_STKPX5,COL_LONGMGR5,COL_FEERATE5};

enum PKCONDITEM{COL_DIYSYSID6=0,COL_PKTYPE6,COL_PKSTAT6,COL_PKCOND6,COL_INST6,
COL_DIR6,COL_KPP6,COL_PKPX6,COL_PKVOL6,COL_HEDGE6,COL_EXHID6,COL_PKTM6,COL_STATMSG6};

typedef char TThostFtdcStockPriceAlias[10];	//"ask1" "last"
typedef char TThostFtdcAnyPxTypeAlias[10];	//"best" "anyLmt"
typedef char TThostFtdcOrdTypeAlias[10];	//"best" "anyLmt"

typedef struct myInstID
{
	TThostFtdcInstrumentIDType	instId;
	myInstID(TThostFtdcInstrumentIDType sId=NULL)
	{		
		if (sId != NULL){ strcpy(this->instId,sId); }
		else
		{ ZeroMemory(this->instId,sizeof(TThostFtdcInstrumentIDType)); } 
	}
	myInstID(LPCSTR sId)
	{		
		if (strlen(sId)<31){ strcpy(this->instId,sId); }
	}

	bool operator==(const myInstID& s) const
	{ return (strcmp(this->instId,s.instId)==0); }
}INSTMENT;

typedef vector<BYTE> btAr;
typedef vector<INSTMENT> InsAr;
typedef vector<INSTMENT>::iterator InsIt;
///合约保证金率
typedef CThostFtdcInstrumentMarginRateField FTMGR,*PFTMGR;
typedef CThostFtdcOptionInstrTradeCostField OPMGR,*POPMGR;
///合约手续费率
typedef CThostFtdcInstrumentCommissionRateField FEERATE,*PFEERATE;
//typedef CThostFtdcOptionInstrCommRateField OPFEERATE,*POPFEERATE;

typedef CThostFtdcInstrumentField INSTINFO,*PINSTINFO;
typedef CThostFtdcOrderField CORDF,*PCORDF;
typedef CThostFtdcParkedOrderField PARKORDF,*PPARKORDF;
typedef CThostFtdcParkedOrderActionField PKORDCANCEL,*PPKORDCANCEL;
typedef int TThostFtdcCondParkType;
typedef CThostFtdcBrokerTradingParamsField BKRTDPARA,*PBKRTDPARA;

typedef struct myDate
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMin;
	int nSec;
}MYDATE,*PMYDATE;

typedef struct CThostFtdcParkedOrderFieldEx
{
	PARKORDF park;
	PKORDCANCEL pkAct;
	TThostFtdcCondParkType ordType;
	TThostFtdcDateType	InsertDate;
	TThostFtdcTimeType	InsertTime;
	TThostFtdcOrderSysIDType OrderSysID;
	TThostFtdcBranchIDType	BranchID;
	//tmord线程传入的参数指针
	LPVOID					pTmOrd;
	//tmord线程句柄
	HANDLE					hThd;
}PARKEDEX,*PPARKEDEX;

typedef struct CThostFtdcTradeFieldEx
{
public:
	CThostFtdcTradeField trade;
	TThostFtdcMoneyType	dFee;
	CThostFtdcTradeFieldEx(const CThostFtdcTradeField* td=NULL,TThostFtdcMoneyType dFee=0)
	{ 
		if (td!=NULL){ memcpy(&this->trade,td,sizeof(CThostFtdcTradeField)); }
		else
		{ ZeroMemory(&this->trade,sizeof(CThostFtdcTradeField)); }
		this->dFee = dFee;
	}
	
	bool operator==(const CThostFtdcTradeFieldEx& k) const
	{ return (memcmp(&this->trade,&k.trade,sizeof(CThostFtdcTradeField))==0); }
} CTDFEx,*PCTDFEx;

typedef struct CThostFtdcOrderFieldEx
{
public:
	CThostFtdcOrderField	Ord;
	double					dAvgPx;
	CThostFtdcOrderFieldEx(const CThostFtdcOrderField* pOrd=NULL,double dVal=0)
	{ 
		if (pOrd!=NULL){ memcpy(&this->Ord,pOrd,sizeof(CThostFtdcOrderField)); }
		else
		{ ZeroMemory(&this->Ord,sizeof(CThostFtdcOrderField)); }
		
		this->dAvgPx=dVal; 
	}
	bool operator==(const CThostFtdcOrderFieldEx& k) const
	{ return (this->Ord.BrokerOrderSeq==k.Ord.BrokerOrderSeq); }

}CORDFEx,*PCORDFEx;

typedef struct CThostFtdcInstrumentCommissionRateFieldEx
{
	FEERATE fee;
	TThostFtdcRatioType StrikeRatioByMoney;
	TThostFtdcRatioType StrikeRatioByVolume;
	#ifdef _REAL_CTP_
	TThostFtdcRatioType	OrderCommByVolume;
	TThostFtdcRatioType	OrderActionCommByVolume;
	#endif
}FEEREX,*PFEEREX;

//期货期权手续费字段合并
typedef struct InstFee
{
	TThostFtdcRatioType	OpenRatioByMoney;
	TThostFtdcRatioType	OpenRatioByVolume;
	TThostFtdcRatioType	CloseRatioByMoney;
	TThostFtdcRatioType	CloseRatioByVolume;
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	TThostFtdcRatioType	CloseTodayRatioByVolume;
	TThostFtdcRatioType StrikeRatioByMoney;
	TThostFtdcRatioType StrikeRatioByVolume;
	#ifdef _REAL_CTP_
	TThostFtdcRatioType	OrderCommByVolume;
	TThostFtdcRatioType	OrderActionCommByVolume;
	#endif
}TDFEE,*PTDFEE;

typedef struct OpMgrate
{
	TThostFtdcMoneyType	FixedMargin;
	TThostFtdcMoneyType	MiniMargin;
	TThostFtdcMoneyType	Royalty;
	TThostFtdcMoneyType	ExchFixedMargin;
	TThostFtdcMoneyType	ExchMiniMargin;
}OPMGRATE,*POPMGRATE;

typedef struct CThostFtdcInstrumentFieldEx
{
	INSTINFO iinf;
	//期货保证金率
	TThostFtdcRatioType	LongMgrByMoney;
	TThostFtdcMoneyType	LongMgrByVolume;
	TThostFtdcRatioType	ShortMgrByMoney;
	TThostFtdcMoneyType	ShortMgrByVolume;
	OPMGRATE	omgr;			//期权保证金率
	TDFEE		fee;			//合并的手续费率
	TThostFtdcLargeVolumeType	OpenInterest;
	TThostFtdcMoneyType	Turnover;
	TThostFtdcPriceType	UpLmtPx;
	TThostFtdcPriceType	LowLmtPx;
}INSINFEX,*PINSINFEX;

typedef struct ClientInf
{
	TThostFtdcFrontIDType	iFrtId;
	TThostFtdcSessionIDType	iSesId;
	TCHAR ProdInf[11];
}CLINFO,*PCLINFO;

typedef char TThostFtdcEncPasswordType[64];

typedef struct tmOrdParam
{
	LPVOID pDlg;
	PARKEDEX pkReq;
	MYDATE dt;
}TMORDPARAM,*PTMORDPARAM;

struct MyLess{
	bool operator()(const string &lhs,const string &rhs) const
	{ return (_stricmp(lhs.c_str(),rhs.c_str())<0); }
};

typedef vector<CThostFtdcInstrumentFieldEx> VEC_IINF;
typedef vector<CThostFtdcInvestorPositionField> VEC_INVPOS;
typedef vector<CORDFEx> VEC_ORDER;
typedef vector<CTDFEx> VEC_TRADE;
typedef vector<CThostFtdcInstrumentStatusField> VEC_STAT;

typedef map<string,CThostFtdcDepthMarketDataField,MyLess> map_str_DepthMd;
typedef map<string,CThostFtdcInstrumentFieldEx,MyLess> map_str_InsInfEx;
typedef map<string,CThostFtdcInstrumentMarginRateField,MyLess> map_str_Mgr;
typedef map<string,CThostFtdcOptionInstrTradeCostField,MyLess> map_str_Optc;
typedef map<string,CThostFtdcInstrumentCommissionRateFieldEx,MyLess> map_str_FeeEx;

typedef map_str_DepthMd::iterator MIT_md;
typedef map_str_InsInfEx::iterator MIT_if;
typedef map_str_Mgr::iterator MIT_mgr;
typedef map_str_FeeEx::iterator MIT_fee;
typedef VEC_STAT::iterator VIT_stat;
typedef VEC_ORDER::iterator VIT_ord;
typedef VEC_ORDER::reverse_iterator VRIT_ord;
typedef VEC_IINF::iterator VIT_if;
typedef VEC_INVPOS::iterator VIT_pos;
typedef VEC_INVPOS::reverse_iterator VRIT_pos;
typedef VEC_TRADE::iterator VIT_td;
typedef VEC_TRADE::reverse_iterator VRIT_td;
typedef vector<PARKEDEX>::iterator VIT_pk;
typedef vector<PARKEDEX>::reverse_iterator VRIT_pk;

typedef struct LoginPara
{
	TThostFtdcInvestorIDType szUid;
	TThostFtdcEncPasswordType szPass;
	int iBkrGroup;
	int iSvrGroup;
}LOGINPARA,*PLOGINPARA;
typedef vector<LOGINPARA>::iterator VIT_lp;

typedef struct BkrPara
{
	TCHAR XmlPath[MAX_PATH];
	TCHAR BkrName[64];
	char BkrId[11];
	///认证码
	char AuthCode[17];
}BKRPARA,*PBKRPARA;
typedef vector<BKRPARA>::iterator VITBkr;

typedef struct OrdThreadPara
{
	LPVOID pDlg;
	CThostFtdcInputOrderField ordReq;
	int iTdPos;
	int iYdPos;
}ORDTHREADP,*PORDTHREADP;

#define MaxHkCount 0xff
#define PROD_INFO "HJXTRADER2"
//#define PROD_INFO "Q7"
#define DBL_MAX 1.7976931348623158e+308 
#define DBL_MIN 2.2250738585072014e-308
#define FEE_XML "fee_%s.xml"
#define MGR_XML "mgr_%s.xml"
#define ORDFEE_XML "ordfr_%s.xml"
#define AUTHOR _T("t0trader")
#define INSERT_OP (-1)
#define TOBE_DONE _T("to-be-finished")

///自己补充
#define THOST_FTDC_OST_ErrOrd 'x'

#define _CZCE "CZCE"
#define _DCE "DCE"
#define _SFE "SHFE"
#define _CFX "CFFEX"
#define _CINE "INE"

#define _CNY "CNY"
#define _RMB "RMB"
#define _USD "USD"
#define _HKD "HKD"

#define REFRESH_TIMER 100
#define IDLE_TIMER 101

#define DIR_BUYA "买入"
#define DIR_SELLA "卖出"

#define DIR_BUY _T(DIR_BUYA)
#define DIR_SELL _T(DIR_SELLA)

#define ORD_OA "开仓"
#define ORD_CA "平仓"

#define ORD_O _T(ORD_OA)
#define ORD_C _T(ORD_CA)
//#define ORD_CT _T("平今")

#define INVPOS_ITMES 8
#define ONROAD_ITMES 8
#define TRADE_ITMES 12
#define ORDER_ITMES 14
#define ALLINST_ITMES 11
#define PARKORD_ITMES 13
#define MAX_HIS 20
#define NEARZERO (1e-8)
#define IPADDR_LEN (sizeof(TThostFtdcIPAddressType))

#define UNCOMP _T("—")

#define LMT(a, l, h) ((a)>(h)?(h):(a)<(l)?(l):(a))
#define LMTDEF(a, l, h, def) ((a)>(h)||(a)<(l)?(def):(a))
#define GETWORD(b)		*(WORD*)(b)
#define GETDWORD(b)		*(DWORD*)(b)

#define DEL(p) { if (p) { delete (p); (p) = NULL; } }
#define DELX(p) { if (p) { delete [] (p);  (p) = NULL; } }
#define RELZ(p) { if (p) { (p)->RegisterSpi(NULL); (p)->Release(); (p) = NULL; } }
#define CLOSEH(p) { if (p) { if ((p) != INVALID_HANDLE_VALUE) VERIFY(CloseHandle(p)); (p) = NULL; } }

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define WM_UPDATEMD_MSG (WM_USER+1)
#define WM_TDRECON_MSG (WM_USER+2)
#define WM_MDRECON_MSG (WM_USER+3)
#define WM_TDISCON_MSG (WM_USER+4)
#define WM_MDISCON_MSG (WM_USER+5)
#define WM_EXHSTAT_MSG (WM_USER+6)
#define WM_NOTICE_MSG (WM_USER+7)
#define WM_OPENACC_MSG (WM_USER+8)
#define WM_DELACC_MSG (WM_USER+9)
#define WM_MODACC_MSG (WM_USER+10)
#define WM_QRYBKRNT_MSG (WM_USER+11)
#define WM_QRYTDNT_MSG (WM_USER+12)
#define WM_QRYACCTOK_MSG (WM_USER+13)
#define WM_F2BK_MSG (WM_USER+15)
#define WM_BK2F_MSG (WM_USER+16)
//中金所通告
#define WM_CFXBT_MSG (WM_USER+17)
#define WM_RELOGINOK (WM_USER+18)
#define WM_CONTMENU_MSG (WM_USER+19)
#define WM_NOTLOCORD_MSG (WM_USER+20)
#define WM_REGHK_MSG (WM_USER+21)
#define WM_UNREGHK_MSG (WM_USER+22)
#define WM_SAVECFGS (WM_USER+23)
#define WM_QRYUSER_MSG (WM_USER+24)
#define WM_QRYBKYE_MSG (WM_USER+25)
#define WM_QRYACC_MSG (WM_USER+26)

#define CPN_SELENDOK         (WM_USER + 1004)
#define CPN_SELENDCANCEL     (WM_USER + 1005)
///默认银行分支机构ID
#define VTC_BKBRCH_ID "0000"
///期货发起查询银行余额
#define VTC_FTQRYBKAMT "204002"
///期货发起银行资金转期货
#define VTC_FTBK2FT "202001"
///期货发起期货资金转银行
#define VTC_FTFT2BK "202002"

#define OLEDT_HALFSECOND (1.0 / (2.0 * (60.0 * 60.0 * 24.0)))
//定义一个负值来辨别
#define LAST_DJ_ALIAS (-2)

#define Swap(a, b) {  a ^= b; b ^= a; a ^= b; }
#define swaps_p(a)  { BYTE *pa = (BYTE *)&(a); Swap(pa[0], pa[1]); }
			
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
const UINT WM_QRYBFLOG_MSG = ::RegisterWindowMessage(_T("QryBFLog_Msg"));
const UINT WM_QRYSMI_MSG = ::RegisterWindowMessage(_T("QrySmi_Msg"));

const TThostFtdcContingentConditionType CondTyTable[12] = 
{
    THOST_FTDC_CC_AskPriceGreaterThanStopPrice,THOST_FTDC_CC_AskPriceGreaterEqualStopPrice,
	THOST_FTDC_CC_AskPriceLesserThanStopPrice,THOST_FTDC_CC_AskPriceLesserEqualStopPrice,
	THOST_FTDC_CC_LastPriceGreaterThanStopPrice,THOST_FTDC_CC_LastPriceGreaterEqualStopPrice,
	THOST_FTDC_CC_LastPriceLesserThanStopPrice,THOST_FTDC_CC_LastPriceLesserEqualStopPrice,
	THOST_FTDC_CC_BidPriceGreaterThanStopPrice,THOST_FTDC_CC_BidPriceGreaterEqualStopPrice,
	THOST_FTDC_CC_BidPriceLesserThanStopPrice,THOST_FTDC_CC_BidPriceLesserEqualStopPrice
};

template<class T> inline void ClearVec(vector<T> &vt) 
{
    vector<T> vtTemp; 
    vtTemp.swap(vt);
}

template<class T> inline void ClearMap(map<string,T,MyLess> &mt) 
{
    map<string,T,MyLess> mtTemp; 
    mtTemp.swap(mt);
}

inline __int64 D2Int(double dIn)
{
	__int64 iOut = 0;
	if (dIn>NEARZERO){ iOut = static_cast<__int64>(dIn+0.5); }
	else if(dIn>-NEARZERO){ iOut = 0; }
	else
	{ iOut = static_cast<__int64>(dIn-0.5); }
		
	return iOut;
}

inline int JudgeDigit(double dTick)
{
	int iRes;
	if (dTick<0.01) { iRes=3; }
	else if (dTick<0.1)
	{ iRes=2; }
	else if (dTick<1)
	{ iRes=1; }
	else
	{ iRes=0; }
	
	return iRes;		
}

inline int JgCurIdType(TThostFtdcCurrencyIDType sCurId)
{
	int iRet = 0;
	if (!_stricmp(sCurId,_CNY) || !_stricmp(sCurId,_RMB)) { iRet = 0; }
	if (!_stricmp(sCurId,_USD)) { iRet = 1; }
	if (!_stricmp(sCurId,_HKD)) { iRet = 2; }
    
	return iRet;
}
// 流控判断
inline BOOL IsFlowControl(int iRet)
{
	return ((iRet == -2) || (iRet == -3));
}

inline void getCurTime(CString& strIn)
{
	SYSTEMTIME tm;
	
	::GetLocalTime(&tm);
	strIn.Format(_T("%02d:%02d:%02d"), tm.wHour, tm.wMinute, tm.wSecond);
}

inline void getCurTime(TThostFtdcTimeType& InsTm)
{
	SYSTEMTIME tm;
	
	::GetLocalTime(&tm);
	sprintf(InsTm,"%02d:%02d:%02d", tm.wHour, tm.wMinute, tm.wSecond);
}

inline void getCurDate(CString& strIn)
{
	SYSTEMTIME tm;
	
	::GetLocalTime(&tm);
	strIn.Format(_T("%04d%02d%02d"), tm.wYear, tm.wMonth, tm.wDay);
}

inline void getCurDate(TThostFtdcDateType& InsDt)
{
	SYSTEMTIME tm;
	
	::GetLocalTime(&tm);
	sprintf(InsDt,"%04d%02d%02d", tm.wYear, tm.wMonth, tm.wDay);
}

inline CString LoadStr(UINT nID)
{
    CString str;
	::LoadString(AfxGetResourceHandle(), nID, str.GetBuffer(MAX_PATH*4), MAX_PATH*4);
	str.ReleaseBuffer();
	
	return str;
}

inline size_t uni2ansi(UINT uCodepage,WCHAR *unicodestr, char *ansistr )
{
    int result = 0;
    try
    {
        int needlen = WideCharToMultiByte( uCodepage, 0, unicodestr, -1, NULL, 0, NULL, NULL );
        if( needlen < 0 ){ return needlen; }
		
        result = WideCharToMultiByte( uCodepage, 0, unicodestr, -1, ansistr, needlen + 1, NULL, NULL );
        if( result < 0 ){return result;}
        
        return strlen( ansistr );
    }
    catch( ... ){}
    return result;
}

inline size_t ansi2uni(UINT uCodepage, char *ansistr,WCHAR *unicodestr)
{
	int result = 0;
	try
	{
		int needlen = MultiByteToWideChar( uCodepage, 0, ansistr, -1, NULL, 0);
		if( needlen < 0 ){ return needlen; }
		
		result = MultiByteToWideChar( uCodepage, 0, ansistr, -1, unicodestr, needlen + 1);
		if( result < 0 ){ return result; }
		return wcslen( unicodestr );
	}
	catch( ... ){}
	
	return result;
}

inline bool cmpInst(const INSINFEX& pfirst,const INSINFEX& psecond) 
{   
	return (_stricmp(pfirst.iinf.InstrumentID,psecond.iinf.InstrumentID)<0);
}

inline bool cmpOrder(const CThostFtdcOrderFieldEx& pfirst,const CThostFtdcOrderFieldEx& psecond) 
{   
	int iRet = strcmp(pfirst.Ord.InsertDate,psecond.Ord.InsertDate);
	if (iRet==0)
	{
		int iRet2 = strcmp(pfirst.Ord.InsertTime,psecond.Ord.InsertTime);
		if (iRet2==0)
		{
			return (pfirst.Ord.BrokerOrderSeq < psecond.Ord.BrokerOrderSeq);
		}
		return (iRet2<0);
	}

	return (iRet<0);
}

inline bool cmpTrade(const CTDFEx& pfirst,const CTDFEx& psecond) 
{   
	int iRet = strcmp(pfirst.trade.TradeDate,psecond.trade.TradeDate);
	if (iRet==0)
	{
		int iRet2 = strcmp(pfirst.trade.TradeTime,psecond.trade.TradeTime);
		if (iRet2==0)
		{
			int iRet3 = strcmp(pfirst.trade.TradeID,psecond.trade.TradeID);
			if (iRet3==0)
			{
				return (pfirst.trade.BrokerOrderSeq < psecond.trade.BrokerOrderSeq);
			}
			return (iRet3 <0);
		}
		return (iRet2<0);
	}

	return (iRet<0);
}

inline bool findSeqNo(CORDFEx pOrd,int iSeq)
{
	return (pOrd.Ord.BrokerOrderSeq==iSeq);
}

inline bool findInfbyId(INSINFEX pInf,TThostFtdcInstrumentIDType InstId)
{
	return (strcmp(InstId,pInf.iinf.InstrumentID)==0);
}

inline bool findbySysID(CORDFEx pOrd,TThostFtdcOrderSysIDType OrdSysID)
{
	return (strcmp(OrdSysID,pOrd.Ord.OrderSysID)==0);
}

inline BOOL CondCompare(const CThostFtdcDepthMarketDataField* pMd,TThostFtdcInstrumentIDType instId,
			double dStopPx,TThostFtdcContingentConditionType tCond)
{	
	if (!strcmp(pMd->InstrumentID,instId) && 
		((pMd->LastPrice>dStopPx && tCond==THOST_FTDC_CC_LastPriceGreaterThanStopPrice) ||
		(pMd->LastPrice>=dStopPx && tCond==THOST_FTDC_CC_LastPriceGreaterEqualStopPrice) ||
		(pMd->LastPrice<dStopPx && tCond==THOST_FTDC_CC_LastPriceLesserThanStopPrice) ||
		(pMd->LastPrice<=dStopPx && tCond==THOST_FTDC_CC_LastPriceLesserEqualStopPrice) ||
		(pMd->AskPrice1>dStopPx && tCond==THOST_FTDC_CC_AskPriceGreaterThanStopPrice) ||
		(pMd->AskPrice1>=dStopPx && tCond==THOST_FTDC_CC_AskPriceGreaterEqualStopPrice) ||
		(pMd->AskPrice1<dStopPx && tCond==THOST_FTDC_CC_AskPriceLesserThanStopPrice) ||
		(pMd->AskPrice1<=dStopPx && tCond==THOST_FTDC_CC_AskPriceLesserEqualStopPrice) ||
		(pMd->BidPrice1>dStopPx && tCond==THOST_FTDC_CC_BidPriceGreaterThanStopPrice) ||
		(pMd->BidPrice1>=dStopPx && tCond==THOST_FTDC_CC_BidPriceGreaterEqualStopPrice) ||
		(pMd->BidPrice1<dStopPx && tCond==THOST_FTDC_CC_BidPriceLesserThanStopPrice) ||
		(pMd->BidPrice1<=dStopPx && tCond==THOST_FTDC_CC_BidPriceLesserEqualStopPrice)))
	{ return TRUE; }
	
	return FALSE;
}

inline BOOL JgCancelStat(CThostFtdcOrderFieldEx *pOrder)
{
	if((pOrder->Ord.OrderStatus == THOST_FTDC_OST_PartTradedQueueing) || 
		(pOrder->Ord.OrderStatus == THOST_FTDC_OST_NoTradeQueueing))
	{ return TRUE; }
	
	return FALSE;
}

//检测预埋单可删与否
inline BOOL JgPkRmvStat(PARKEDEX *pOrder)
{
	if ((pOrder->ordType == PARKED_ACT && pOrder->pkAct.Status == THOST_FTDC_PAOS_NotSend)||
		(pOrder->ordType == PARKED_SERVER &&pOrder->park.Status == THOST_FTDC_PAOS_NotSend))
		
	{ return TRUE; }
	
	return FALSE;
}

//检测条件单可撤与否
inline BOOL JgCondActStat(PARKEDEX *pOrder)
{
	if ((pOrder->ordType == COND_SERVER ||pOrder->ordType == TOUCH_ZS || pOrder->ordType == TOUCH_ZS) && 
		pOrder->park.Status == THOST_FTDC_OST_NotTouched)
	{
		return TRUE;
	}
	
	return FALSE;
}

inline BOOL JgPkCanSend(PARKEDEX *pOrder)
{
	if ((pOrder->ordType == TMORD_LOCAL&&pOrder->park.Status == THOST_FTDC_OST_NotTouched) ||
		(pOrder->ordType == ENTERTD_LOCAL&&pOrder->park.Status == THOST_FTDC_PAOS_NotSend) ||
		(pOrder->ordType == COND_LOCAL &&(pOrder->park.Status == THOST_FTDC_OST_NotTouched ||
		pOrder->park.Status == THOST_FTDC_PAOS_NotSend)))
	{ return TRUE; }
	
	return FALSE;
}

inline BOOL JgOverStat(CThostFtdcOrderFieldEx *pOrder)
{
	if((pOrder->Ord.OrderStatus == THOST_FTDC_OST_Canceled) || 
		(pOrder->Ord.OrderStatus == THOST_FTDC_OST_AllTraded)||
		(pOrder->Ord.OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing) || 
		(pOrder->Ord.OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing)||
		(pOrder->Ord.OrderStatus == THOST_FTDC_OST_ErrOrd))
	{ return TRUE; }
	
	return FALSE;
}

inline BOOL JgAnyPxOrd(TThostFtdcOrderPriceTypeType cPxType,TThostFtdcTimeConditionType cTcType)
{
	if ((cPxType==THOST_FTDC_OPT_AnyPrice ||cPxType==THOST_FTDC_OPT_BestPrice || 
		cPxType==THOST_FTDC_OPT_FiveLevelPrice)&& (cTcType==THOST_FTDC_TC_IOC || 
		cTcType==THOST_FTDC_TC_GFD))
	{ return TRUE; }

	return FALSE;
}

inline CString FiltPx(double dPx)
{
	CString szTemp=_T("");
	if (dPx==DBL_MAX)
	{
		szTemp = _T("―");
	}
	else
	{
		if ((dPx>-NEARZERO) && (dPx<NEARZERO)){ dPx = 0; }
		szTemp.Format(_T("%f"),dPx);
		szTemp.TrimRight('0');
		int iLen = szTemp.GetLength();
		if (!_tcscmp(szTemp.Mid(iLen-1,1),_T("."))) 
		{ szTemp.TrimRight(_T(".")); }
	}
	
	return szTemp;
}

inline double FiltDb(double dPx)
{
	if ((dPx>-NEARZERO) && (dPx<NEARZERO)){ dPx = 0.0; }
	return dPx;
}

inline bool IsCondOrd(CThostFtdcOrderField* pOrd)
{
	return (pOrd->StopPrice > NEARZERO);
}

inline bool IsCondPk(CThostFtdcParkedOrderField* pOrd)
{
	return (pOrd->StopPrice > NEARZERO);
}

inline bool IsParkedOrd(CThostFtdcParkedOrderField* pOrd)
{
	return (pOrd->ContingentCondition == THOST_FTDC_CC_ParkedOrder);
}

inline COLORREF CmpPriceColor(double d1,double d2)
{
	COLORREF clorf = WHITE;
	if (d1>d2) { clorf=RED; }
	if (d1<d2) { clorf=GREEN; }

	return clorf;
}

inline bool cmpVHold(const INSINFEX pfirst,const INSINFEX psecond) 
{    
	double dRes = pfirst.OpenInterest - psecond.OpenInterest;
	return (dRes<-NEARZERO); //double类型粗略比较来判断
}

inline BOOL IsShfeProd(INSINFEX *pInf,TThostFtdcInstrumentIDType instId)
{
	if (!strcmp(pInf->iinf.InstrumentID,instId) && !strcmp(pInf->iinf.ExchangeID,_SFE))
	{ return TRUE; }

	return FALSE;
}

inline double getGlobalDpi()
{
    //SetProcessDpiAwareness(Process_System_DPI_Aware);
    HDC dc = GetDC (0);
    const double dpi = (GetDeviceCaps (dc, LOGPIXELSX)+ GetDeviceCaps (dc, LOGPIXELSY)) / 2.0;
    ReleaseDC (0, dc);
    return dpi;
}

inline double getMasterScale()
{
    return ( getGlobalDpi() / 96.0 );
}

inline void TrimTime(CString& strTm)
{
	CString szTemp = strTm;
    if(szTemp.GetLength()>1 && (*(szTemp.GetBuffer(0))=='0'))
	{
		strTm = szTemp.Mid(1);
	}
}

inline int AtoInc(volatile int * pt)
{ return (int)InterlockedIncrement((long *)pt); }

inline int AtoDec(volatile int * pt)
{ return (int)InterlockedDecrement((long*)pt); }

template <typename T> __inline void INITDDSTRUCT(T& dd)
{
	ZeroMemory(&dd, sizeof(dd));
	dd.dwSize = sizeof(dd);
}