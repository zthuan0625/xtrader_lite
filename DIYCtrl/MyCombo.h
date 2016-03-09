#pragma once
/////////////////////////////////////////////////////////////////////////////
// MyCombo window

class MyCombo : public CComboBox
{
// Construction
public:
	MyCombo();
	virtual ~MyCombo();

    //BOOL m_bAutoComplete;

	// Generated message map functions
protected:
	//{{AFX_MSG(MyCombo)
	//afx_msg void OnEditUpdate();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
