#pragma once
#include "DIYCtrl/MyCombo.h"

enum BFTRANSTYPE{TRANS_B2F=1,TRANS_F2B,QRY_BKACC};

class BfTransfer : public CDialog
{
    DECLARE_DYNAMIC(BfTransfer)
// Construction
public:
	BfTransfer(CWnd* pParent = NULL);   // standard constructor
	~BfTransfer();

// Dialog Data
	//{{AFX_DATA(BfTransfer)
	enum { IDD = IDD_BFTRANS };
	CComboBox	m_cbExhType;
	MyCombo	m_cbBkLst;
	CString	m_szAccpwd;
	CString	m_szBkpwd;
	double	m_dTrsAmt;
	
	CWinThread *m_pQryBf;

	void InitCombo();
	static UINT QryBfS(LPVOID pParam);
	void Transfunc(BFTRANSTYPE tp);
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BfTransfer)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnQryBk();
	afx_msg void OnBtnQryFt();
	afx_msg void OnBtnFt2Bk();
	afx_msg void OnBtnBk2Ft();
	afx_msg void OnQryDetail();
	afx_msg void OnChgAccPwd();
	afx_msg void OnChgBkPwd();
	afx_msg void OnTransAmt();
	afx_msg void OnDestroy();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg LRESULT QryDetailMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};