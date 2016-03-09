#pragma once

class DlgSiLog : public CDialog
{
	DECLARE_DYNAMIC(DlgSiLog)

public:
	DlgSiLog(CWnd* pParent = NULL); 
	virtual ~DlgSiLog();

	void ProgressUpdate(LPCTSTR szMsg, const int nPercent);
	static UINT LoginThread(LPVOID pParam);
	enum { IDD = IDD_SILOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_prg;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
};
