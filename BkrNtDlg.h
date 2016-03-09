#pragma once

#include "DIYCtrl/HtmlCtrl.h"
////////////////////////////

class BkrNtDlg : public CDialog
{
	DECLARE_DYNAMIC(BkrNtDlg)

public:
	BkrNtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~BkrNtDlg();

	CMyHtmlCtrl	m_Page;
	CString		m_szUrl,m_szTitle;
	BOOL		m_bMemory;
	CString		m_sContent;

// 对话框数据
	enum { IDD = IDD_DLG_BKRNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()
};
