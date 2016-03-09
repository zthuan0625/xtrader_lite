
#pragma once

class CNumSpinCtrl : public CSpinButtonCtrl
{
// Construction
public:
	CNumSpinCtrl();
	virtual ~CNumSpinCtrl();

// Operations
public:
	void GetRangeAndDelta(double &lower, double& upper, double& delta);
	void SetRangeAndDelta(double lower, double upper, double delta);
	
	double GetPos();
	void   SetPos(double val);

	int  GetDecimalPlaces ();
	void SetDecimalPlaces (int num_places);
	void SetFormatString (LPCTSTR lpszFormatString = NULL);
	
	void SetTrimTrailingZeros (BOOL bTrim)		{ m_bTrimTrailingZeros = bTrim; }
	BOOL GetTrimTrailingZeros ()					{ return m_bTrimTrailingZeros; }
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumSpinCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitSpinCtrl();
	void SetIntPos (double pos);
	void SetValueForBuddy (double val);
	CString FormatValue (double val);

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumSpinCtrl)
	afx_msg BOOL OnDeltapos (NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

/// Attributes
protected:
	double m_MinVal;
	double m_MaxVal;
	double m_Delta;
	UINT m_IntRange;
	int m_NumDecPlaces;
	CString m_strFormat;
	BOOL m_bTrimTrailingZeros;
	char m_DecSymbol;
};
