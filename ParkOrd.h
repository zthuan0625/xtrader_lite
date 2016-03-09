#pragma once
#include "DIYCtrl/TimeEdit.h"

class ParkOrdDlg : public CDialog
{
    DECLARE_DYNAMIC(ParkOrdDlg)
// Construction
public:
	ParkOrdDlg(CWnd* pParent = NULL);   // standard constructor
	~ParkOrdDlg();
// Dialog Data
	//{{AFX_DATA(ParkOrd)
	enum { IDD = IDD_PARKORD_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void GenOrd();
	void GenPkLocal();
	void GenPkServer();
	void GenCondLocal();
	void GenCondServer();
	void GenTouch();
	void GenTouchProf();
	void GenTmOrder();
	void InitCtrls();
	void SubMd(CString sInst);
	CString FmtOrdPx(int iType);
	CString GetStatTxt();
	void InitInstCtl(VEC_IINF& infVec);
	void InitVolCtl();
	void ChkDceAny();
	void CreateToolTips();
	void SetLastPx();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ParkOrd)
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnChgStopPx();
	afx_msg void OnChgLmtPx();
	afx_msg void OnChgVol();
	afx_msg void OnChgTime();
	afx_msg void OnSelPx();
	afx_msg void OnSelCond();
	afx_msg void OnSelBs();
	afx_msg void OnSelOc();
	afx_msg void OnEditchgInst();
	afx_msg void OnClkParkLocal();
	afx_msg void OnClkParkServer();
	afx_msg void OnClkCondLocal();
	afx_msg void OnClkCondServer();
	afx_msg void OnClkTouch();
	afx_msg void OnClkProfit();
	afx_msg void OnClkTmOrd();
	afx_msg void OnClkDsj();
	afx_msg void OnClkLmtPx();
	afx_msg void OnClkAnyPx();
	// Generated message map functions
	//{{AFX_MSG(ParkOrd)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CombBasePx,m_CombConds,m_CbBs,m_CbOc;
	CToolTipCtrl   m_ToolTip;
	CString m_szStat,m_szInst;
	TThostFtdcInstrumentIDType m_instId;
	double m_dLmtPx,m_dStopPx;
	CNumSpinCtrl m_SpStopPx,m_SpLmtPx,m_SpVolPk;
	int m_iConds,m_iSelPx,m_iSelCond,m_iVol,m_iLmt;
	BOOL m_bChkMain;
	TThostFtdcTimeConditionType m_cTmcType;
	TThostFtdcContingentConditionType m_conType;
	TThostFtdcOrderPriceTypeType m_cPxType;
	TThostFtdcDirectionType m_cDir;
	TThostFtdcOffsetFlagType m_cKp;
	CDateTimeEdit m_tmEdit;
	PINSINFEX m_InstInf;
};

