#pragma once
#include "DIYCtrl/SliderEx.h"

class DlgOrdEx : public CDialog
{
	DECLARE_DYNAMIC(DlgOrdEx)

public:
	DlgOrdEx(CWnd* pParent = NULL);   // 标准构造函数
	~DlgOrdEx();

	enum { IDD = IDD_DLG_ORDEX };

	void InitCtrls();
	void EnableWndFAK(BOOL bEnable);
	void EnableLmtPx(BOOL bEnable);
	void SubMd(CString sInst);
	void GenOrder();
	void MiniGui();
	void RestoreGui();
	void ShowHideWnds(int nCmdShow);
	void ChkCfxAny();
	void ChkTdCode();
	void CreateToolTips();
	void SetLastPx();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnClkBuyOpen();
	afx_msg void OnClkSellOpen();
	afx_msg void OnClkBuyClose();
	afx_msg void OnClkSellClose();
	afx_msg void OnClkOpExec();
	afx_msg void OnClkOpAbandon();
	afx_msg void OnClkCombo();
	afx_msg void OnClkUnCombo();
	afx_msg void OnClkClose();
	afx_msg void OnClkChkMinVol();
	afx_msg void OnEnChangeMinVol();
	afx_msg void OnEnChgPxTick();
	//afx_msg void OnSelInst();
	afx_msg void OnEditchgInst();
	afx_msg void OnChgLmtPx();
	afx_msg void OnChgVolEx();
	afx_msg void OnClkRdSpecu();
	afx_msg void OnClkRdArbit();
	afx_msg void OnClkRdHedge();
	afx_msg void OnClkRdMM();
	afx_msg void OnClkRdLmtNormal();
	afx_msg void OnClkRdFOK();
	afx_msg void OnClkRdFAK();
	afx_msg void OnClkRdAnyNormal();
	afx_msg void OnClkRdAnyBest();
	afx_msg void OnClkRdL2Best();
	afx_msg void OnClkRdAnyLmt();
	afx_msg void OnClkRdBestLmt();
	afx_msg void OnClkRdL2Lmt();
	afx_msg void OnClkExpand();
	afx_msg void OnClkAutoClose();

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl   m_ToolTip;
	CString m_szInst;
	double m_dPrice,m_dScale;
	BOOL m_bGuiExpand,m_bChkMain,m_bChkMinVol,m_bAutoClose;
	int m_iMinVol,m_iPxTick,m_iPxType,m_iHedgeType,m_iVol;
	CNumSpinCtrl m_SpLmtPx, m_SpVol,m_SpMinVol,m_SpPxTick;
	//vector<INSINFEX> m_InsMainVec;
	//VECINT		m_vecVol;
	PINSINFEX m_InstInf;
	TThostFtdcInstrumentIDType      m_instId;
	TThostFtdcDirectionType         m_cDir;
	TThostFtdcOrderPriceTypeType    m_cPxType;
	TThostFtdcTimeConditionType     m_cTmcType;
	TThostFtdcVolumeConditionType   m_cVolcType;
	TThostFtdcHedgeFlagType         m_cHgType;
	TThostFtdcOffsetFlagType        m_cOffType;
};
