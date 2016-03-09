#pragma once

class DlgRecent : public CDialog
{
	DECLARE_DYNAMIC(DlgRecent)

public:
	DlgRecent(CWnd* pParent = NULL);
	virtual ~DlgRecent();

	enum { IDD = IDD_DLG_RECENT };
	void InitLBox();
	void GetSvrGNByIdx(CString &szOut, int iBkrG, int iSvrG);
	void SetHScroll();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_List;
	BOOL m_bSave;
	afx_msg void OnBnClkChkRecent();
	afx_msg void OnClkLbDel();
	afx_msg void OnLbnDblclkLbRecent();
	afx_msg void OnDestroy();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnLbnSelchange();
};
