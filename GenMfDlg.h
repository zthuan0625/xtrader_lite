#pragma once

class GenMfDlg : public CDialog
{
	DECLARE_DYNAMIC(GenMfDlg)

public:
	GenMfDlg(CWnd* pParent = NULL);
	virtual ~GenMfDlg();

	static UINT QryMrThread(LPVOID pParam);
	static UINT QryFeeThread(LPVOID pParam);
	static UINT QryOrdfrThread(LPVOID pParam);

	enum { IDD = IDD_DLG_GENMRFEE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClkBtGenmr();
	afx_msg void OnClkBtGenfee();
	afx_msg void OnGenOrdFee();
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	CString m_szGenstat;
	CWinThread *m_pQryMr,*m_pQryFee,*m_pQryOrdfr;
};
