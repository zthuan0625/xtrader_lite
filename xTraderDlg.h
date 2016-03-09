#pragma once

#include "DIYCtrl/XPGroupBox.h"
#include "DIYCtrl/MMStatusBar.h"
#include "DIYCtrl/NumSpinCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CXTraderDlg dialog
enum MODTYPE{MOD_PX=1,MOD_DIR};
enum TINVPCLOSE{TCLOSE=1,TBKHAND};
enum LOGIN_MODE{NORMAL_LOG=1,RECONN_LOG,CHGACC_LOG};

typedef struct ModOrdParam
{
	LPVOID pDlg;
	MODTYPE mType;
}MODPARAM,*PMODPARAM;

class CXTraderDlg : public CDialog
{
// Construction
public:
	CXTraderDlg(CWnd* pParent = NULL);	// standard constructor
	~CXTraderDlg();
	enum{ IDD = IDD_XTRADER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	static UINT SubscribeMD(LPVOID pParam);
	static UINT UnSubscribeMD(LPVOID pParam);
	static UINT QryTdAcc(LPVOID pParam);
	static UINT QryUserInf(LPVOID pParam);
	static UINT OrderThread(LPVOID pParam);
	static UINT ModOrdThread(LPVOID pParam);
	static UINT UpdateMdThread(LPVOID pParam);
	static UINT ReConnTdQry(LPVOID pParam);
	static UINT EnterTdQryPk(LPVOID pParam);
	static UINT QryBkrNtThread(LPVOID pParam);
	static UINT QryTdNtThread(LPVOID pParam);
	static UINT QryTdTokThread(LPVOID pParam);
	static UINT SendTmOrd(LPVOID pParam);
	static BOOL CALLBACK EnumProc(HWND hw, LPARAM lParam);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTraderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	//static void StartLog(const char* logpath = NULL);
	//static void StopLog();
	void ClearInvPos();
	void SubMdData(bool bSub=true);
	void Go2MiniGui();
	void RestoreGui();

	void SaveConfig();
	void InitData();
	void InitCtrs();
	void CreateStatusBar();
	void InitAllHdrs();
	void InitAllVecs();
	void InitTabs();
	void Go2InstMd();
	void SilentReLogin();
	void CancelOrd(PCORDFEx pOrd);
	void GenOrdFromPk(PARKEDEX pkReq);
	void SendLocalPkOrd();
	void SendLocalCondOrd(const CThostFtdcDepthMarketDataField* pMd);
	double GetDsjByInst(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType cDir);
	
	CString GetStatMsg(PCORDFEx pOrder);
	void ModOrd(MODTYPE mType);
	void RunMod(MODTYPE mType);
	void InvPosClose();
	void UpdateMdList();
	void ClearMemory();
	void SwitchTab(int nIndex);
	void UpdateMdPane();
	double GetFreeMoney();
	CString GetDiySysId(PPARKEDEX vpk);
	void SetStatusTxt(UINT uID, int nPane);
	void SetStatusTxt(LPCTSTR pMsg, int nPane);
	void SetTipTxt(LPCTSTR pMsg,int nTool);
	void SetPaneTxtColor(int nPane,COLORREF cr);

	COLORREF GetCondColor(TThostFtdcParkedOrderStatusType stat);
	void SwithMd2Inst(TThostFtdcInstrumentIDType instId);

	void PopupAccDlg(CThostFtdcTradingAccountField* pAcc);
	void PopupPrivInf(CThostFtdcInvestorField* pInf);
	void PopupBkAccDlg(CThostFtdcNotifyQueryAccountField* pNt);
	void PopupNoticeDlg(CThostFtdcTradingNoticeInfoField* pMsg);
	void PopupDifOrdDlg(CThostFtdcOrderField* pOrd);
	void UpdatePosProf(CThostFtdcDepthMarketDataField* pMd);

	void InitProfit(const CThostFtdcTradingAccountField *pAcc);
	void ShowNotifyIcon(/*CWnd* pWnd,*/CString sInfoTitle,CString sInfo,DWORD dwMsg);

	int GetVolByPos(const CThostFtdcInvestorPositionField* pInvPos,int iVol,int& iTdPos,int& iYdPos);
	int GetValidVol(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType cDir,char cOffFlag,int iVol,int& iTdPos,int& iYdPos);
	void GenFromOrd(CThostFtdcOrderFieldEx *pOrd);
	void GenOrdByInvPos(const CThostFtdcInvestorPositionField *pInv,TINVPCLOSE mType=TCLOSE);
	void ChgStatByBsOc(TThostFtdcDirectionType cDir,TThostFtdcOffsetFlagType cKp,int iVol);
	void ChgStatByInvPos(const CThostFtdcInvestorPositionField* pInvPos);

	void GenOrdFromPara(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType cDir,TThostFtdcOffsetFlagType cOff,
		TThostFtdcVolumeType iVol,TThostFtdcPriceType dPx,bool bLmt=true);
	CString GetIpAndMac();
	void Scroll2Mid();
	void ClearTmOrdThd();
	BOOL PopTrayMenu();
	void SetIdleTm();
	void KillIdleTm();
	void SetMainAlpha();
	void OnGlobKey(BOOL bCtl);
	void SetDlgIcon(UINT uId);
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_Notify;		//托盘图标
	UINT m_Timer,m_uSync,m_uShowBar,m_uIdle;

private:
	//CRITICAL_SECTION   m_cs ;
	//void Lock() { EnterCriticalSection(&m_cs); }
    //void UnLock() { LeaveCriticalSection(&m_cs); }
	// Generated message map functions
	//{{AFX_MSG(CXTraderDlg)
protected:
	afx_msg LRESULT TdReConnMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT MdReConnMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT TdDisConnMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT MdDisConnMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT ExhStatusMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT TdNoticeMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OpenAccMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT DelAccMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT ModAccMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT BkrNtMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT TdNtMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT QryTokMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDisplayChg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Bk2FMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT F2BkMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT ReLogOkMsg(WPARAM wParam,LPARAM lParam);
	////////////////////////////////////////////////////////////
	afx_msg LRESULT QryAccMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT QryUserMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT QryBkYe(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT UpdateMdMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT RcvRegHkMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT RcvUnRegHkMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT SaveCfgsMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	////////////////////////////////////////////////////////////////
	afx_msg void OnRestore();
	virtual BOOL OnInitDialog();
	afx_msg void OnMinimize();
	afx_msg void OnEditchgInst();
	afx_msg void OnChgEditVol();
	afx_msg void OnChgEditPx();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg void OnExit();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTips();
	afx_msg void OnViewTop();
	afx_msg void OnUpdateViewTop(CCmdUI *pCmdUI);
	///////////////下单///////////////
	afx_msg void OnBtOrder();
	afx_msg void OnBnClkChkLastPx();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTabSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnNMDblclkOnroad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClkLstOnroad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkLstOnroad(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDblclkOrdInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClkLstOrdInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkLstOrdInf(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDblclkTdInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClkLstTdInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkLstTdInf(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDblclkInvPInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClkLstInvPInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkLstInvPInf(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClkLstInsts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkLstInsts(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDblclkParkOrd(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClkParkOrd(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClkParkOrd(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDispOnRoad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindOnRoad(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnDispOrdInf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindOrdInf(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDispInvPos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindInvPos(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDispTrade(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindTrade(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDispInsts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindInsts(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDispParked(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnfindParked(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnCancelOrd();
	afx_msg void OnCancelAll();
	afx_msg void OnCsvExport();
	afx_msg void OnModDsj();
	afx_msg void OnRevOrd();
	afx_msg void OnBackHand();

	afx_msg void OnDestroy();
	afx_msg void OnClkQryAcc();
	afx_msg void OnModifyPass();
	afx_msg void OnTransBf();
	afx_msg void OnUserInf();
	afx_msg void OnCfmmc();
	afx_msg void OnGenMdFee();
	afx_msg void OnHiSettInf();
	afx_msg void OnHkeySet();
	afx_msg void OnToolParked();
	afx_msg void OnReLogin();
	afx_msg void OnTdNotice();
	afx_msg void OnBkrNotice();
	afx_msg void OnLocalSend();
	afx_msg void OnLocalRemv();
	afx_msg void OnPkRemv();
	afx_msg void OnPkAct();
	afx_msg void OnCondAct();
	afx_msg void OnDelPkAct();
	afx_msg void OnMiniMode();
	afx_msg void OnMiniTask();
	afx_msg void OnCommCfg();
	afx_msg void OnOrderExt();
	
	afx_msg void OnViewLog();
	afx_msg void OnLockAcc();
	afx_msg void OnSelKpp();
	afx_msg void OnSelDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	MMStatusBar m_StatusBar;
	CNumSpinCtrl m_SpinVol;
	CNumSpinCtrl m_SpinPrice;
	CComboBox	m_CombBS;
	CXPGroupBox m_GroupMd,m_GroupAcc;
	CColorStatic m_csCpProf;	//平仓盈亏
	CColorStatic m_csHpProf;	//持仓盈亏
	CColorStatic m_csTdFee;		//手续费
	CComboBox	m_CombOC;
	CColorStatic m_csS1P;
	CColorStatic m_csS1V;
	CColorStatic m_csLastP;
	CColorStatic m_csLastV;
	CColorStatic m_csB1P;
	CColorStatic m_csB1V;
	CColorStatic m_csSUpDown;
	CColorStatic m_csDUpDown;
	CColorStatic m_csSHighest;
	CColorStatic m_csDHighest;
	CColorStatic m_csSOpt;
	CColorStatic m_csDOpt;
	CColorStatic m_csSLowest;
	CColorStatic m_csDLowest;
	CColorStatic m_csSTotal;
	CColorStatic m_csVTotal;
	CColorStatic m_csSHold;
	CColorStatic m_csVHold;
	CColorStatic m_csSSmp;
	CColorStatic m_csDSmp;
	CColorStatic m_csSUptime;
	CColorStatic m_csSS1;
	CColorStatic m_csSLast;
	CColorStatic m_csSB1;
	CWinThread *m_pSubMd,*m_pQryAcc,*m_pOrder,*m_pMdThread,*m_pQryUinf,*m_pQryBkrNt,*m_pQryTdNt,*m_pClrPosThd;
	CWinThread *m_pModOrd,*m_pReConQry,*m_pUnSubMd,*m_pQryTdTok;
	CXListCtrl	m_xLst[PKCON_IDX+1];
	CRect		m_rcBak;

public:
	BOOL		m_bTrans,m_bUpdateOp,m_bLastPx;
	int			m_iOrdVol;
	double		m_dOrdPx,m_dOldPrice,m_dScale;

	INSINFEX *m_InstInf;
	CThostFtdcDepthMarketDataField *m_pDepthMd; //在行情面板显示的一帧数据
	CThostFtdcTradingAccountField *m_pTdAcc;
	CLINFO	*m_pCliInfo;

	CTabCtrl m_TabOption;
	CString m_szInst,m_szExpDef,m_szInstOld;
	CSize	m_OldSize;
	int	m_iRef_tm,m_iRef_pk,m_iRef_tj;
	int m_iActIdx;
	bool m_bTdClosed,m_bMinGui,m_bQryPk,m_bLocked,m_bTop;
	LOGIN_MODE m_eLogMod;
	TThostFtdcTimeType m_tDisStart;
	TThostFtdcTimeType m_tDisEnd;
	MINMAXINFO* m_plpMMI;
	////////////////////////////////////////////
	InsAr m_SubList;
	//vector<CThostFtdcInstrumentFieldEx> m_InsinfVec;
	//VECINT m_vecVol;
	////////////////////////////////////////////
	CThostFtdcBrokerTradingParamsField 	m_BkrTdPara;
	TThostFtdcDirectionType m_cDir;
	TThostFtdcOffsetFlagType m_cKpp;
};
