#pragma once

#define _DEFVOLCNT 9
#define _AUSERID "Uid"
#define _ABKRGRP "BkrGroup"
#define _ASVGRP "SvrGroup"
#define _ASVPWD "SavePwd"
#define _AUSERPW "Pass"
#define _NDNTPSVR "NtpSvr"
#define _NDGENERAL "General"
#define _AGLOKEY    "GloKey"
#define _AIDLELOCK "IdleLock"
#define _ATMOSEC  "TmoSec"
#define _AFOLASTPX "foLast"
#define _APORT "Port"
#define _NDWNDINF "WndInfo"
#define _AMINGUI "MinGui"
#define _AONTOP "OnTop"
#define _ATRANS "IsTrans"
#define _AALPHA "Alpha"
#define _AXPOS "xPos"
#define _AYPOS "yPos"
#define _AWIDTH "width"
#define _AHIGHT "height"
#define _AENABLE "Enable"
#define _AINSTID "instId"
#define _AFNAME "fname"
#define _NDMISC "Misc"
#define _NDINSTLST "InstList"
#define _AISUDP "IsUdp"
#define _AISMUL "IsMulcast"
#define _NDRT "root"
#define _NDBKR "broker"
#define _ABKRNAME "BrokerName"
#define _ABKRID "BrokerID"
#define _AAUTHCD "AuthCode"
#define _NDITEM "item"
#define _AVOL "Vol"
#define _NDRECNT "Recent"
#define _ABSAVE "bSave"
#define _NDMDCFG "MdConf"
#define _NDPXYCFG "Proxy"
#define _CTRUE "true"
#define _CFALSE "false"
#define _AURL "Url"
#define _NDPATHSVR "//broker/Servers/Server"
#define	_NDNAME		"Name"
#define	_NDTRADE	"Trading"
#define	_NDMKTD		"MarketData"
#define TCP_PFS _T("tcp://%s")
#define TCP_PRE _T("tcp://")
#define SSL_PRE _T("ssl://")
#define UDP_PRE _T("udp://")
#define SOCK4_PRE _T("socks4://")
#define SOCK4A_PRE _T("socks4a://")
#define SOCK5_PRE _T("socks5://")
#define WAIT_MS	60000
#define PROXY_ADDR "%s/%s:%s@%s:%d"
#define PROXY_TMPL "socks5://u:p@s:80"
#define HKEY_BEGINID 997

class CAppCfgs
{
	public:
		bool	fInited;
		bool	fReset;

		TThostFtdcIPAddressType		m_sLocIp;
		TThostFtdcMacAddressType	m_sLocMac;

		TThostFtdcBrokerIDType	m_sBkrId;
		TThostFtdcUserIDType m_sUid;
		TThostFtdcPasswordType  m_sPwd,m_sDymPwd;
		TThostFtdcAuthCodeType m_sAtCode;
		TThostFtdcInstrumentIDType m_szInst;
		int	m_ixPos,m_iyPos,m_iWidth,m_iHeight,m_iNtpPort;
		double	 m_dScale;
		UINT m_uIdleSec,m_uAlpha,m_uOrdToSec;
		vector<BKRPARA> m_BkrParaVec;
		vector<LOGINPARA> m_pInfVec;
		bool m_bChkIdle;
		VEC_IINF m_InsinfVec;
		
		bool	m_bIsUdp,m_bIsMulcast,m_bMinGui,m_bTop,m_bProxy,m_bChkTrans;
		bool	m_bSavePwd,m_bSaveHis,m_bLastPx,m_bRiskNt;
		CString m_szProxy,m_szTitle,m_szNtpSvr;
	public:
		CAppCfgs();
		virtual ~CAppCfgs();
		void	ProcConfigs(RW_MODE md=LOAD);

		void	LoadBkrsCfg(LPCTSTR lpDir);
		void	ProxyCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	WndInfCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	GenerCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	NtpCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	MdCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	RecentCfg(xml_node& ndrt,RW_MODE md=LOAD);
		void	InsLstCfg(xml_node& ndrt,RW_MODE md=LOAD);
		BOOL	RwSvrsbyAr(CStringArray& szArTs,CStringArray& szArMd,LPCTSTR lpXml,LPCTSTR lpGrpName,RW_MODE md=LOAD);
		CString GetBkrById(TThostFtdcBrokerIDType szBkrId);
};
