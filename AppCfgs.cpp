#include "stdafx.h"
#include "AppCfgs.h"

CAppCfgs::CAppCfgs()
{
	fInited=fReset=m_bIsUdp=m_bIsMulcast=m_bProxy=m_bMinGui=
		m_bLastPx=m_bChkTrans=false;
	m_bTop=m_bSavePwd=m_bSaveHis=m_bChkIdle=true;

	m_dScale = LMT(getMasterScale(),1.0,5.0);
	m_szProxy = _T(PROXY_TMPL);
	m_szTitle = LoadStr(IDS_TITLE);
	m_iNtpPort = 123;
	m_uIdleSec = m_uOrdToSec=5;
	m_uAlpha = 128;
	ZeroMemory(m_sBkrId,sizeof(TThostFtdcBrokerIDType));
	ZeroMemory(m_sUid,sizeof(TThostFtdcUserIDType));
	ZeroMemory(m_sPwd,sizeof(TThostFtdcPasswordType));
	ZeroMemory(m_sDymPwd,sizeof(TThostFtdcPasswordType));
	ZeroMemory(m_sAtCode,sizeof(TThostFtdcAuthCodeType));

	if (GetIpAddr(m_sLocIp)){ GetMacAddr(m_sLocIp,m_sLocMac); }
}

CAppCfgs::~CAppCfgs()
{
	ClearVec(m_BkrParaVec);
	ClearVec(m_pInfVec);
	ClearVec(m_InsinfVec);
}

void CAppCfgs::LoadBkrsCfg(LPCTSTR lpDir)
{
	//////////////////搜索并载入期商列表/////////////////////////////
	CFileFind finder;
	BOOL bWorking = finder.FindFile(lpDir);
	
	xml_document doc;
	xml_node ndrt;
	xml_parse_result result;
	char szBrkNm[MAX_PATH];
	BKRPARA pBkrp;
	while (bWorking)	
	{	
		ZeroMemory(&pBkrp,sizeof(BKRPARA));
		bWorking = finder.FindNextFile();
		_tcscpy(pBkrp.XmlPath,(LPCTSTR)finder.GetFilePath());
		/////////////////////////////////////////////////////////////	
		result = doc.load_file(pBkrp.XmlPath);
		if (result.status == status_ok) 
		{
			ndrt = doc.child(_NDRT).child(_NDBKR);
			if (!ndrt) continue;
			
			strcpy(szBrkNm,ndrt.attribute(_ABKRNAME).value());
			ansi2uni(CP_ACP,szBrkNm,pBkrp.BkrName);
			strcpy(pBkrp.BkrId,ndrt.attribute(_ABKRID).value());
			strcpy(pBkrp.AuthCode,ndrt.child_value(_AAUTHCD));
			
			m_BkrParaVec.push_back(pBkrp);	
		}
	}
	
	finder.Close();
}

void CAppCfgs::ProxyCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndProxy;
	if (md==SAVE)
	{
		ndProxy= ndrt.append_child(_NDPXYCFG);
		ndProxy.append_attribute(_AENABLE) = m_bProxy;
		
		char szPxyUrl[2*MAX_PATH];
		uni2ansi(CP_UTF8,(LPTSTR)(LPCTSTR)m_szProxy,szPxyUrl);
		ndProxy.append_attribute(_AURL) = szPxyUrl;
	}
	else
	{
		ndProxy = ndrt.child(_NDPXYCFG);
		m_bProxy = ndProxy.attribute(_AENABLE).as_bool();
		
		ansi2uni(CP_UTF8,(char*)ndProxy.attribute(_AURL).value(),m_szProxy.GetBuffer(2*MAX_PATH));
		m_szProxy.ReleaseBuffer();
		
		if (_tcsnicmp((LPCTSTR)m_szProxy,_T("socks"),5)){ m_szProxy = _T(PROXY_TMPL); }
	}
}

void CAppCfgs::WndInfCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndWInf;
	if (md==SAVE)
	{
		ndWInf = ndrt.append_child(_NDWNDINF);
		ndWInf.append_attribute(_AMINGUI) = m_bMinGui;
		ndWInf.append_attribute(_AONTOP) = m_bTop;
		ndWInf.append_attribute(_ATRANS) = m_bChkTrans;
		ndWInf.append_attribute(_AALPHA) = m_uAlpha;
		ndWInf.append_attribute(_AXPOS) = m_ixPos;
		ndWInf.append_attribute(_AYPOS) = m_iyPos;
		ndWInf.append_attribute(_AWIDTH) = m_iWidth;
		ndWInf.append_attribute(_AHIGHT) = m_iHeight;
	}
	else
	{
		ndWInf = ndrt.child(_NDWNDINF);
		m_bMinGui = ndWInf.attribute(_AMINGUI).as_bool();
		m_bTop = ndWInf.attribute(_AONTOP).as_bool();
		m_bChkTrans = ndWInf.attribute(_ATRANS).as_bool();
		m_uAlpha = ndWInf.attribute(_AALPHA).as_uint();
		m_ixPos = ndWInf.attribute(_AXPOS).as_int();
		m_iyPos = ndWInf.attribute(_AYPOS).as_int();
		m_iWidth = ndWInf.attribute(_AWIDTH).as_int();
		m_iHeight = ndWInf.attribute(_AHIGHT).as_int();
	}		
}

void CAppCfgs::InsLstCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndInst;
	if (md==SAVE)
	{
		ndInst = ndrt.append_child(_NDINSTLST);
		ndInst.append_child(node_pcdata).set_value(m_szInst);
	}
	else
	{
		ndInst = ndrt.child(_NDINSTLST);
		strcpy(m_szInst, ndInst.child_value());
	}	
}

void CAppCfgs::GenerCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndGener;
	if (md==SAVE)
	{
		ndGener = ndrt.append_child(_NDGENERAL);
		ndGener.append_attribute(_ASVPWD) = m_bSavePwd;
		ndGener.append_attribute(_AIDLELOCK) = m_bChkIdle;
		ndGener.append_attribute(_ATMOSEC) = m_uIdleSec;
		ndGener.append_attribute(_AFOLASTPX) = m_bLastPx;
	}
	else
	{
		ndGener = ndrt.child(_NDGENERAL);
		m_bSavePwd = ndGener.attribute(_ASVPWD).as_bool();
		m_bChkIdle = ndGener.attribute(_AIDLELOCK).as_bool();
		m_uIdleSec = ndGener.attribute(_ATMOSEC).as_uint();
		m_bLastPx = ndGener.attribute(_AFOLASTPX).as_bool();
	}
}

void CAppCfgs::NtpCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndNtp;
	if (md==SAVE)
	{
		char szTemp[MAX_PATH]={0};
		ndNtp = ndrt.append_child(_NDNTPSVR);
		uni2ansi(CP_UTF8, (LPTSTR)(LPCTSTR)m_szNtpSvr, szTemp);
		ndNtp.append_attribute(_AURL) = szTemp;
		ndNtp.append_attribute(_APORT) = m_iNtpPort;
	}
	else
	{
		ndNtp = ndrt.child(_NDNTPSVR);
		ansi2uni(CP_UTF8,(char*)ndNtp.attribute(_AURL).value(),m_szNtpSvr.GetBuffer(MAX_PATH));
		m_szNtpSvr.ReleaseBuffer();
		m_iNtpPort = ndNtp.attribute(_APORT).as_int();
	}
}

void CAppCfgs::MdCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndMdCfg;
	if (md==SAVE)
	{	
		ndMdCfg = ndrt.append_child(_NDMDCFG);
		ndMdCfg.append_attribute(_AISUDP) = m_bIsUdp;
		ndMdCfg.append_attribute(_AISMUL) = m_bIsMulcast;
	}
	else 
	{
		ndMdCfg = ndrt.child(_NDMDCFG);
		m_bIsUdp = ndMdCfg.attribute(_AISUDP).as_bool();
		m_bIsMulcast = ndMdCfg.attribute(_AISMUL).as_bool();
	}	
}

void CAppCfgs::RecentCfg(xml_node& ndrt,RW_MODE md)
{
	xml_node ndRecent,ndIts;
	int i =0;
	if (md==SAVE)
	{
		/////////////////////最近登录列表///////////////////////////
		ndRecent = ndrt.append_child(_NDRECNT);
		ndRecent.append_attribute(_ABSAVE) = m_bSaveHis;
		if (m_bSaveHis)
		{
			///删掉超过MAX_HIS条记录的前几条
			int iSize = static_cast<int>(m_pInfVec.size());
			if (iSize > MAX_HIS)
			{
				for (i = 0; i < (iSize - MAX_HIS); i++) { m_pInfVec.erase(m_pInfVec.begin()); }
			}
			//////////新纪录保存
			iSize = static_cast<int>(m_pInfVec.size());
			for (i = 0; i < iSize; i++)
			{
				ndIts = ndRecent.append_child(_NDITEM);
				ndIts.append_attribute(_AUSERID) = m_pInfVec[i].szUid;
				ndIts.append_attribute(_AUSERPW) = m_pInfVec[i].szPass;
				ndIts.append_attribute(_ABKRGRP) = m_pInfVec[i].iBkrGroup;
				ndIts.append_attribute(_ASVGRP) = m_pInfVec[i].iSvrGroup;
			}
		}
	}
	else
	{
		ndRecent = ndrt.child(_NDRECNT);
		m_bSaveHis = ndRecent.attribute(_ABSAVE).as_bool();
		if (m_bSaveHis)
		{
			LOGINPARA pInf;
			for (ndIts = ndRecent.first_child(); ndIts; ndIts = ndIts.next_sibling())
			{
				ZeroMemory(&pInf,sizeof(pInf));
				strcpy(pInf.szUid, ndIts.attribute(_AUSERID).value());
				strcpy(pInf.szPass, ndIts.attribute(_AUSERPW).value());
				pInf.iBkrGroup = ndIts.attribute(_ABKRGRP).as_int();
				pInf.iSvrGroup = ndIts.attribute(_ASVGRP).as_int();
				
				m_pInfVec.push_back(pInf);
			}
		}	
	}	
}

void CAppCfgs::ProcConfigs(RW_MODE md)
{
	////////////////搜索并载入期商列表/////////////
	xml_document doc;
	xml_node ndrt;
	xml_parse_result result;

	result = doc.load_file(GetSpecFilePath(LoadStr(IDS_CFG_FILE)),parse_full);
	if (md==LOAD)
	{
		if (fInited) { return; }

		LoadBkrsCfg(GetSpecFilePath(LoadStr(IDS_BKRS_DIR)));

		if (result.status == status_ok)
		{
			ndrt = doc.child(_NDRT);
			if (!ndrt) return;
			
			InsLstCfg(ndrt);
			NtpCfg(ndrt);
			MdCfg(ndrt);
			ProxyCfg(ndrt);
			GenerCfg(ndrt);
			RecentCfg(ndrt);
			WndInfCfg(ndrt);
		}
		else
		{ ShowErroTips(IDS_CFGERR, IDS_MY_TIPS); }		
		
		fInited = true;
	}
	else
	{
		if (!fInited) { return; }
		if (result.status==status_ok)
		{
			ndrt = doc.child(_NDRT);
			if (!ndrt) return;
			ndrt.remove_child(_NDGENERAL);
			ndrt.remove_child(_NDNTPSVR);
			ndrt.remove_child(_NDMDCFG);
			ndrt.remove_child(_NDPXYCFG);
			ndrt.remove_child(_NDINSTLST);
			ndrt.remove_child(_NDWNDINF);
			ndrt.remove_child(_NDRECNT);
			
			InsLstCfg(ndrt,SAVE);
			NtpCfg(ndrt,SAVE);
			MdCfg(ndrt,SAVE);
			ProxyCfg(ndrt,SAVE);
			GenerCfg(ndrt,SAVE);
			RecentCfg(ndrt,SAVE);
			WndInfCfg(ndrt,SAVE);
			//////////////////////////////////////
			
			doc.save_file(GetSpecFilePath(LoadStr(IDS_CFG_FILE)), PUGIXML_TEXT("\t"), format_default, encoding_utf8);
		}		
	}
}

BOOL CAppCfgs::RwSvrsbyAr(CStringArray& szArTs,CStringArray& szArMd,LPCTSTR lpXml,LPCTSTR lpGrpName,RW_MODE md)
{
	if (!fInited) { return FALSE; }

	xml_document doc;
	xml_parse_result result = doc.load_file(lpXml,parse_full);
	if (result.status == status_ok)
	{
		char sTemp[MAX_PATH];
		TCHAR tTd[MAX_PATH],tMd[MAX_PATH];
		CString tName,tTrading,tMData;
		xpath_node_set xpsVr = doc.select_nodes(_NDPATHSVR);
		xpath_node_set::const_iterator xpit = xpsVr.begin();
		if (md==LOAD)
		{
			///////////读出服务器//////////////////
			szArTs.RemoveAll();
			szArMd.RemoveAll();
			for (xpit; xpit !=  xpsVr.end(); ++xpit)
			{
				xml_node ndRt = xpit->node();
				strcpy(sTemp,ndRt.child(_NDNAME).child_value());//
				ansi2uni(CP_ACP,sTemp,tName.GetBuffer(MAX_PATH));
				tName.ReleaseBuffer();
				
				if (!tName.Compare(lpGrpName))
				{
					xml_node ndIts,ndTd = ndRt.child(_NDTRADE),ndMd = ndRt.child(_NDMKTD);
					
					for (ndIts = ndTd.first_child(); ndIts; ndIts = ndIts.next_sibling())
					{
						ansi2uni(CP_ACP,(char*)ndIts.child_value(),tTd);
						
						if (!_tcsnicmp(tTd,SSL_PRE,6))
						{
							tTrading.Format(_T("%s"),tTd);
						}
						else
						{
							tTrading.Format(TCP_PFS,tTd);
						}
						
						szArTs.Add(tTrading);
					}
					
					for (ndIts = ndMd.first_child(); ndIts; ndIts = ndIts.next_sibling())
					{
						ansi2uni(CP_ACP,(char*)ndIts.child_value(),tMd);
						
						if (!_tcsnicmp(tMd,SSL_PRE,6))
						{
							tMData.Format(_T("%s"),tMd);
						}
						else
						{
							//UDP前缀时一样用TCP但是createapi里多了参数
							if (!_tcsnicmp(tMd,UDP_PRE,6))
							{
								tMData.Format(_T("%s"),tMd);
								tMData.Replace(UDP_PRE,TCP_PRE);
							}
							else
							{ tMData.Format(TCP_PFS,tMd); }
						}
						
						szArMd.Add(tMData);
					}
					
					break;
				}	
			}
		}
		else
		{
			int iPos = 0;
			int iTdSvrs = static_cast<int>(szArTs.GetSize());
			int iMdSvrs = static_cast<int>(szArMd.GetSize());
			if (iTdSvrs<1 || iMdSvrs<1){ return FALSE; }
			for (xpit; xpit !=  xpsVr.end(); ++xpit)
			{
				xml_node ndRt = xpit->node();
				ansi2uni(CP_ACP,(char*)ndRt.child(_NDNAME).child_value(),tName.GetBuffer(MAX_PATH));
				tName.ReleaseBuffer();
				
				if (!tName.Compare(lpGrpName))
				{
					ndRt.remove_child(_NDTRADE);
					ndRt.remove_child(_NDMKTD);

					xml_node ndIts;
					xml_node ndTd = ndRt.append_child(_NDTRADE);
					xml_node ndMd = ndRt.append_child(_NDMKTD);

					for (iPos=0;iPos<iTdSvrs;iPos++)
					{
						ndIts = ndTd.append_child(_NDITEM);
						uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)(szArTs[iPos].Mid(6)),sTemp);
						ndIts.append_child(node_pcdata).set_value(sTemp);
					}
					
					for (iPos=0;iPos<iMdSvrs;iPos++)
					{
						ndIts = ndMd.append_child(_NDITEM);
						uni2ansi(CP_ACP,(LPTSTR)(LPCTSTR)szArMd[iPos].Mid(6),sTemp);
						ndIts.append_child(node_pcdata).set_value(sTemp);
					}
					
					doc.save_file(lpXml, PUGIXML_TEXT("\t"), format_default, encoding_auto);
					break;
				}	
			}
		}
	}
	else
		return FALSE;
	
	return TRUE;
}

CString CAppCfgs::GetBkrById(TThostFtdcBrokerIDType szBkrId)
{
	CString szRet=_T("");
	for (UINT i=0;i<m_BkrParaVec.size();i++)
	{
		if (!strcmp(m_BkrParaVec[i].BkrId,szBkrId))
		{ szRet = m_BkrParaVec[i].BkrName; break;}
	}
	return szRet;
}
