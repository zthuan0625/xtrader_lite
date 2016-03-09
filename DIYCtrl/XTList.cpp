#include "stdafx.h"
#include "XTList.h"

// CInPlaceEdit
CInPlaceEdit::CInPlaceEdit(int iItem, int iSubItem, CString sInitText)
	: m_sInitText( sInitText )
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;
}

CInPlaceEdit::~CInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN|| pMsg->wParam == VK_DELETE|| 
		pMsg->wParam == VK_ESCAPE|| GetKeyState(VK_CONTROL)) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	DestroyWindow();
}

void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}

void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
		if (nChar == VK_ESCAPE) {
			m_bESC = TRUE;
		}
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText(m_sInitText);
	SetFocus();
	SetSel(0, -1);
	return 0;
}

IMPLEMENT_DYNAMIC(CXTList, CListCtrl)
CXTList::CXTList(int tStartEditingDelay)
	: m_tStartEditingDelay(tStartEditingDelay)
	, m_nItemClicked(-1)
	, m_nTimerID(0)
	, m_nSubItemClicked(-1)
	, m_fInPlaceDirty(false)
{
}

CXTList::~CXTList()
{
}

void CXTList::PreSubclassWindow()
{
	EnableToolTips(TRUE);
	CListCtrl::PreSubclassWindow();
	
	DWORD dwStyle, dwExStyle;
	dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	dwStyle |= (LVS_NOSORTHEADER|LVS_REPORT|LVS_SINGLESEL);
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle);
	dwExStyle = GetExtendedStyle();
	dwExStyle |= (LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES);
	SetExtendedStyle(dwExStyle);
	
	CImageList m_ImageList; 
	m_ImageList.Create(1,22,ILC_COLOR,1,1); 	 
	SetImageList(&m_ImageList,LVSIL_SMALL);
}

int CXTList::HitTestEx(CPoint& point, int* col) const
{
	if (col) { *col = 0; }
	int row = HitTest(CPoint(0, point.y), NULL);

	if ((GetWindowLongPtr(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT) { return row; }

	int nColumnCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
	for (int top = GetTopIndex(), bottom = GetBottomIndex(); top <= bottom; top++) {
		CRect r;
		GetItemRect(top, &r, LVIR_BOUNDS);

		if (r.top <= point.y && point.y < r.bottom) {
			for (int colnum = 0; colnum < nColumnCount; colnum++) {
				int colwidth = GetColumnWidth(colnum);

				if (point.x >= r.left && point.x <= (r.left + colwidth)) {
					if (col) { *col = colnum; }
					return top;
				}

				r.left += colwidth;
			}
		}
	}

	return -1;
}

int CXTList::GetBottomIndex() const
{
	CRect r;
	GetClientRect(r);

	int nBottomIndex = GetTopIndex() + GetCountPerPage() - 1;

	if (nBottomIndex >= GetItemCount()) {
		nBottomIndex = GetItemCount() - 1;
	} else if (nBottomIndex < GetItemCount()) {
		CRect br;
		GetItemRect(nBottomIndex, br, LVIR_BOUNDS);

		if (br.bottom < r.bottom) { nBottomIndex++; }
	}

	return(nBottomIndex);
}

bool CXTList::PrepareInPlaceControl(int nRow, int nCol, CRect& rect)
{
	if (!EnsureVisible(nRow, TRUE)) { return false; }

	int nColumnCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 5) {	return false; }

	int offset = 0;
	for (int i = 0; i < nCol; i++) { offset += GetColumnWidth(i); }

	GetItemRect(nRow, &rect, LVIR_BOUNDS);

	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size(offset + rect.left, 0);
		Scroll(size);
		rect.left -= size.cx;
	}

	rect.left += offset;
	rect.right = rect.left + GetColumnWidth(nCol);
	if (rect.right > rcClient.right) { rect.right = rcClient.right; }

	rect.DeflateRect(1, 0, 0, 1);

	if (nCol == 0) {
		CRect r;
		GetItemRect(nRow, r, LVIR_LABEL);
		rect.left = r.left-1;
	}

	return true;
}

CEdit* CXTList::ShowInPlaceEdit(int nItem, int nCol)
{
	CRect rect;
	if (!PrepareInPlaceControl(nItem, nCol, rect)) { return(NULL); }

	DWORD dwStyle = /*WS_BORDER|*/WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);
	dwStyle |= (lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT ? ES_LEFT
			   : (lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT ? ES_RIGHT
			   : ES_CENTER;

	CEdit* pEdit = new CInPlaceEdit(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create(dwStyle, rect, this, IDC_EDIT_INP1);

	m_fInPlaceDirty = false;

	return pEdit;
}

BEGIN_MESSAGE_MAP(CXTList, CListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnLvnInsertitem)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnLvnDeleteitem)
	ON_EN_CHANGE(IDC_EDIT_INP1, OnEnChangeEdit1)
	ON_NOTIFY_EX(HDN_ITEMCHANGINGA, 0, OnHdnItemchanging)
	ON_NOTIFY_EX(HDN_ITEMCHANGINGW, 0, OnHdnItemchanging)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

// CXTList message handlers
void CXTList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetFocus() != this) { SetFocus(); }
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetFocus() != this) { SetFocus(); }
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CXTList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetFocus() != this) { SetFocus(); }
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CXTList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);

	if (GetFocus() != this) { SetFocus(); }

	if (m_nTimerID) 
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	int nItemClickedNow, nSubItemClickedNow;

	if ((nItemClickedNow = HitTestEx(point, &nSubItemClickedNow)) < 0) { m_nItemClicked = -1; } 
	else if (m_nItemClicked == nItemClickedNow /*&& m_nSubItemClicked == m_nSubItemClickedNow*/) 
	{
		m_nSubItemClicked = nSubItemClickedNow;

		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_BEGINLABELEDIT;
		dispinfo.item.mask = 0;
		dispinfo.item.iItem = m_nItemClicked;
		dispinfo.item.iSubItem = m_nSubItemClicked;
		if (GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo)) 
		{
			if(m_tStartEditingDelay>0) { m_nTimerID = SetTimer(1, m_tStartEditingDelay, NULL); } 
			else 
			{
				dispinfo.hdr.code = LVN_DOLABELEDIT;
				GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);
			}
		}
	} 
	else 
	{
		m_nItemClicked = nItemClickedNow;
		m_nSubItemClicked = nSubItemClickedNow;

		SetItemState(m_nItemClicked, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CXTList::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID) 
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;

		UINT flag = LVIS_FOCUSED;
		if ((GetItemState(m_nItemClicked, flag) & flag) == flag && m_nSubItemClicked >= 0) 
		{
			LV_DISPINFO dispinfo;
			dispinfo.hdr.hwndFrom = m_hWnd;
			dispinfo.hdr.idFrom = GetDlgCtrlID();
			dispinfo.hdr.code = LVN_DOLABELEDIT;
			dispinfo.item.mask = 0;
			dispinfo.item.iItem = m_nItemClicked;
			dispinfo.item.iSubItem = m_nSubItemClicked;
			GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);
		}
	} 
	else if (nIDEvent == 43) { VERIFY(KillTimer(nIDEvent)); } 
	else 
	{ CListCtrl::OnTimer(nIDEvent); }
}

void CXTList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	KillTimer(1);

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CXTList::OnLvnMarqueeBegin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMLV);
	*pResult = 1;
}

void CXTList::OnLvnInsertitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMLV);
	m_nItemClicked = -1;
	*pResult = 0;
}

void CXTList::OnLvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMLV);
	m_nItemClicked = -1;
	*pResult = 0;
}

void CXTList::OnEnChangeEdit1()
{
	m_fInPlaceDirty = true;
}

BOOL CXTList::OnHdnItemchanging(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	UNREFERENCED_PARAMETER(phdr);
	//	SetFocus();
	*pResult = 0;
	return FALSE;
}

#if _MSC_VER < 1600
int CXTList::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
#else
INT_PTR CXTList::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
#endif
{
	int col;
	int row = HitTestEx(point, &col);
	if (row == -1) { return -1; }

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	CRect rect;
	GetItemRect(row, &rect, LVIR_BOUNDS);

	for (int colnum = 0; colnum < nColumnCount; colnum++) 
	{
		int colwidth = GetColumnWidth(colnum);

		if (colnum == col) 
		{
			rect.right = rect.left + colwidth;
			break;
		}

		rect.left += colwidth;
	}

	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->rect = rect;

	return pTI->uId;
}

BOOL CXTList::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;

	if (pTTT->uFlags & TTF_IDISHWND) { nID = ::GetDlgCtrlID((HWND)nID); }
	if (nID == 0) { return FALSE; }
	pTTT->lParam = (LPARAM)m_hWnd;

	*pResult = 0;

	return static_cast<BOOL>(GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)pNMHDR));
}

int CXTList::InsertColumn(int nCol,  LPCTSTR lpszColumnHeading,
		 int nFormat, int nWidth, int nSubItem, int nMinWidth)
{
	nCol = CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);

	#if _MSC_VER >= 1600 && _WIN32_WINNT>=0x0600
	if (nCol != -1&& nMinWidth > 0
		&& IsWinVistaOrLater()) {
		LVCOLUMN col;
		col.mask	= LVCF_MINWIDTH;
		col.cxMin	= nMinWidth;
		SetColumn(nCol, &col);
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_COLUMNSNAPPOINTS);
	}	
	#endif
	return nCol;
}
