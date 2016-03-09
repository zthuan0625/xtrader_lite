#pragma once
#include "DIYCtrl/XTList.h"

enum PXY_TYPE{T_SOCKS4,T_SOCKS4A,T_SOCKS5};
class DlgNetSel : public CDialog
{
	DECLARE_DYNAMIC(DlgNetSel)

public:
	DlgNetSel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgNetSel();

// 对话框数据
	enum { IDD = IDD_DLG_NETSET };

	static UINT GetSvrDelay(LPVOID pParam);
	static UINT TestSvrsGrp(LPVOID pParam);
public:
	void InitLists();
	void InitCombo();
	void GetDelay();
	void GetPxyCfg();
	void ChkProxy(BOOL bUse);
	BOOL CheckInpSvr(LPCTSTR szInp);
	void TestTdAndMd();
	void TestFunc(CStringArray& sVrGrp,CXTList& cLst,DWORD dwTmout);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bUseProxy;
	LPVOID m_pLogin;
	CWinThread *m_pTest;
	CComboBox m_CombSockLst;
	int m_iPxyType;
	CString m_szSkAddr;
	int m_iSkPort;
	CString m_szSkUser;
	CString m_szSkPwd;
	CXTList m_LstTds,m_LstMds;
	CString m_szGrpName,m_szXml;
	bool	m_IsModified;
	CStringArray m_szArTs,m_szArMd;
public:
	afx_msg void OnBnClkChkSock();
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnTestSpeed();
	afx_msg void OnEnChgPxyServ();
	afx_msg void OnEnChgPxyPort();
	afx_msg void OnEnChgPxyUser();
	afx_msg void OnEnChgPxyPass();
	afx_msg void OnBeginEditTdLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoEditTdLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEditTdLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginEditMdLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoEditMdLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEditMdLst(NMHDR* pNMHDR, LRESULT* pResult);
};

struct TESTSVRPARAM
{
	PVOID pDlg;
	CString szSvr;
	int iPort;
	__int64 iDelayMs;
};