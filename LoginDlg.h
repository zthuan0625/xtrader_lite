#pragma once
#include "DIYCtrl/MyCombo.h"

class LoginDlg : public CDialog
{
    DECLARE_DYNAMIC(LoginDlg)
// Construction
public:
	LoginDlg(CWnd* pParent = NULL);   // standard constructor

	~LoginDlg();

	void LoadConfig();
	void SaveConfig();
	void SplitPxyUrl(CString szUrl,int& iType,CString& szSkAddr,int& m_iSkPort,CString& szSkUser,CString& szSkPwd);
	void ProgressUpdate(LPCTSTR szMsg,const int nPercent);
	static UINT LoginThread(LPVOID pParam);
// Dialog Data
	//{{AFX_DATA(LoginDlg)
	enum { IDD = IDD_LOGIN };
	MyCombo m_ComboIsp,m_ComboBkr;
	CString	m_szUid,m_szPass,m_sDymPwd,m_szBrkName,m_szProxy;
	BOOL	m_bSavePwd;
	int		m_iSelBkr,m_iSelSvr;
	CProgressCtrl	m_prgs;
	CStringArray m_szArTs,m_szArMd;
	//}}AFX_DATA

public:
	CWinThread *m_pLogin;
	BOOL m_bModeLess;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LoginDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;
// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(LoginDlg)
	afx_msg void OnLogin();
	afx_msg void OnQuit();
	afx_msg void OnNetset();
	afx_msg void OnSave();
	afx_msg void OnSelSvr();
	afx_msg void OnSelBkr();
	afx_msg void OnEditChgBkr();
	afx_msg void OnDestroy();
	afx_msg void OnChangeUser();
	afx_msg void OnChangePass();
	afx_msg void OnChangeDym();
	afx_msg void OnBnClkExtra();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
