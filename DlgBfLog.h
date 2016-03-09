#pragma once

class DlgBfLog : public CDialog
{
	DECLARE_DYNAMIC(DlgBfLog)

public:
	DlgBfLog(CWnd* pParent = NULL);
	virtual ~DlgBfLog();

	void UpdateLog();
	enum { IDD = IDD_DLG_BFDETAILS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LstLog;
	void InitList();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDestroy();
};
