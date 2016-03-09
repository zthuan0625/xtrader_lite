#pragma once

#define LVN_DOLABELEDIT (LVN_FIRST+1)

class CInPlaceEdit : public CEdit
{
protected:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC; // To indicate whether ESC key was pressed

public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);
	virtual ~CInPlaceEdit();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

class CXTList : public CListCtrl
{
	DECLARE_DYNAMIC(CXTList)

private:
	int m_nItemClicked, m_nSubItemClicked;
	int m_tStartEditingDelay;
	UINT_PTR m_nTimerID;

	bool PrepareInPlaceControl(int nRow, int nCol, CRect& rect);

public:
	CXTList(int tStartEditingDelay = 500);
	virtual ~CXTList();

	int HitTestEx(CPoint& point, int* col) const;
	int GetBottomIndex() const;

	CEdit* ShowInPlaceEdit(int nItem, int nCol);
	bool m_fInPlaceDirty;
protected:
	virtual void PreSubclassWindow();
#if _MSC_VER < 1600
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
#else
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
#endif
	virtual ULONG GetGestureStatus(CPoint) { return 0; };

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLvnMarqueeBegin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnInsertitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLbnSelChangeList1();
	afx_msg BOOL OnHdnItemchanging(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,int nFormat = LVCFMT_LEFT, 
                int nWidth = -1, int nSubItem = -1,int nMinWidth = 20);
};
