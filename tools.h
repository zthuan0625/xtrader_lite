#pragma once
#include "global.h"
#include "MyMsgBox.h"

void TermThread(HANDLE hThd);
int GetHoldPos(const CThostFtdcInvestorPositionField* pInv);
char MapDirection(char src, bool toOrig=true);
char MapOffset(char src, bool toOrig=true);
CString FormatLine(LPCTSTR lpItem,CString szValue,CString szFill,int iTotalLen);
void Fee2String(CString& szIn,double dOpenByM,double dOpenByV,double dCloseByM,
	double dCloseByV,double dClosetByM,double dClosetByV);

void ShowErroTips(UINT nID1, UINT nID2);
void ShowErroTips(LPCTSTR lpMsg, LPCTSTR lpTitle);

CString JgTdStatus(TThostFtdcInstrumentStatusType bType);
CString JgEnterReason(TThostFtdcInstStatusEnterReasonType bReason);
CString JgTdPxSrc(TThostFtdcPriceSourceType bSrc);
CString JgOrdType(TThostFtdcOrderTypeType bType);

int outStrAs4(CString& szIn);
BOOL SetHighPriority();
void Fee2String(CString& szIn,double dOpenByM,double dOpenByV,double dCloseByM,
double dCloseByV,double dClosetByM,double dClosetByV);
CString JgProType(TThostFtdcProductClassType bType);

CString JgExecRet(TThostFtdcExecResultType bType);

CString JgExhPro(TThostFtdcExchangePropertyType bType);
CString JgCardType(TThostFtdcIdCardTypeType bType);
CString JgCombType(TThostFtdcCombinationTypeType bType);
CString JgTdType(TThostFtdcTradeTypeType bType);

CString JgOcType(TThostFtdcOffsetFlagType bType);
CString JgAnyOrdType(TThostFtdcOrderPriceTypeType cPxType,TThostFtdcTimeConditionType cTcType);

CString JgOrdSubmitStat(TThostFtdcOrderSubmitStatusType bType);
CString JgOrdStatType(TThostFtdcContingentConditionType bType);
CString JgCcOrdType(TThostFtdcContingentConditionType bType);
CString JgOrdCondType(TThostFtdcContingentConditionType bType,TThostFtdcPriceType dStopPx);
CString JgParkOrdStat(TThostFtdcParkedOrderStatusType bType);
CString JgPosBsType(TThostFtdcPosiDirectionType bType);
CString JgBsType(TThostFtdcDirectionType bType);
CString JgTbType(TThostFtdcHedgeFlagType bType);
CString JgTdCodeType(TThostFtdcClientIDTypeType bType);
CString JgExchage(TThostFtdcExchangeIDType ExID);
CString JgBfTdType(TThostFtdcTradeCodeType TdCode);
CString JgBkName(TThostFtdcProtocalIDType bType);
CString GetSpecFilePath(LPCTSTR lpsz);

//ansi°æ±¾
void GetCurDir(LPSTR lpRet,LPCSTR lpDir);

int res2file(LPCTSTR lpName,LPCTSTR lpType,LPCTSTR filename);

void List2Csv(CXListCtrl* pList,LPCTSTR lpName);

CString GenDef(LPCTSTR lpPre,LPCTSTR lpExt);

CString TransTime(TThostFtdcTradeTimeType TdTm);
BOOL GenXmlHdr(LPCTSTR lpFile);

CString GetCTPErr(TThostFtdcErrorMsgType ErrorMsg);
void VerifyEdit(CWnd* pDlg,int nID,bool bAllowDigit);

BOOL DelFolder(LPCTSTR pszFolder);
void EnableClose(CWnd* pWnd,BOOL bEnable);
bool IsTdPeriod();
bool IsParkedOrd(CThostFtdcParkedOrderField* pOrd);
void OrdConvter(CThostFtdcInputOrderField& ord,PARKEDEX& pkEx,ORDCONV_TYPE ct=INP2PARKED);
void OrdConvter(CThostFtdcOrderField& ord,PARKEDEX& pkEx,ORDCONV_TYPE ct=ORD2PARKED);
void CondErr2Pk(const CThostFtdcErrorConditionalOrderField *pErr,PARKEDEX& pkEx);
void GenErrOrdByInpt(const CThostFtdcInputOrderField *pInt,CORDFEx *pOrd);
CString JgParkedType(TThostFtdcCondParkType ordType);

bool GetIpAddr(TThostFtdcIPAddressType pszAddr/*, int nFamily = AF_INET*/);
bool GetMacAddr(LPCSTR sIP,TThostFtdcMacAddressType szMac);
/////////////////////////////////////////
CString GetFileOnly(LPCTSTR Path);
CString GetFolderOnly(LPCTSTR Path);
CString AddSlash(LPCTSTR Path);
CString RemoveSlash(LPCTSTR Path);
CString GetFileExt(LPCTSTR Path);
CString RenameFileExt(LPCTSTR Path, LPCTSTR Ext);
CString CompactPath(LPCTSTR Path, UINT cchMax);
CString GetModulePath(HMODULE hModule);
CString GetProgramPath();
CString GetProgramDir();
bool ExploreToFile(CString path);
CString GetResUrl(LPCTSTR szName);
CString GetResUrl(UINT uId);
void fmtNtMsg(CString& strIn,LPCTSTR lpLink,LPCTSTR lpPre);
void SplitFrtUrl(CString szUrl,CString& szSvr,int& iPort);
void GetDelayMs(LPCTSTR lpSvr,int iPort,__int64& iMs);
void AddToolTips(CWnd* pDlg,CToolTipCtrl* ptt);
BOOL SetPrivilege(LPCTSTR privName, BOOL bEnable);
BOOL PcBeep(UINT uFreq=0xfffffff,UINT uMs=666);
void FirstLetter(int nCode,char* bLetter);
void GetFirstLetter(CString strName, CString& strFirstLetter);
//////////////////////////////////////////
BOOL SetWndAlpha(HWND hwnd, int iAlpha);
BOOL IsWinVerGreaterThan(DWORD dwMajor, DWORD dwMinor);
BOOL IsWinVerEqualTo(DWORD dwMajor, DWORD dwMinor);
BOOL GetNtVerNo(DWORD& dwMajor, DWORD& dwMinor, DWORD& dwBuildNo);
BOOL IsWinXP();
BOOL IsWinXPOrLater();
BOOL IsWinVista();
BOOL IsWinVistaOrLater();
BOOL IsWinSeven();
BOOL IsWinSevenOrLater();
BOOL IsWinEight();
BOOL IsWinEightOrLater();
BOOL IsWinTen();
BOOL IsWinTenOrLater();

BOOL IsWow64();
BOOL IsW64();
