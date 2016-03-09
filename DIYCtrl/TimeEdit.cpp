
#include "stdafx.h"
#include "TimeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDateTimeEdit

CDateTimeEdit::CDateTimeEdit()
{
	m_nPos = 0;
	m_bEnable = TRUE;
}

CDateTimeEdit::~CDateTimeEdit()
{
}

BEGIN_MESSAGE_MAP(CDateTimeEdit, CEdit)
	//{{AFX_MSG_MAP(CDateTimeEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDateTimeEdit message handlers
BOOL CDateTimeEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD | WS_TABSTOP | ES_MULTILINE | ES_READONLY;
	dwStyle &= ~ES_WANTRETURN;
	return CWnd::Create(_T("EDIT"), NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CDateTimeEdit::PreSubclassWindow() 
{
	ModifyStyle(ES_WANTRETURN, WS_CHILD | WS_TABSTOP | ES_MULTILINE | ES_READONLY);//归整样式
	SetDateTime(COleDateTime::GetCurrentTime());
	//垂直居中,必须有多行样式
	CRect rc;
	GetClientRect(&rc);
	CDC* pDC = GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	::InflateRect(&rc, 0,  - (rc.Height() - tm.tmHeight) / 2);
	SetRect(&rc);
	CEdit::PreSubclassWindow();
}

void CDateTimeEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_hWnd))
	{//垂直居中
		CDC* pDC = GetDC();
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		RECT rc = {0, 0, cx, cy};
		::InflateRect(&rc, 0,  - (cy - tm.tmHeight) / 2);
		SetRect(&rc);
	}	
}

HBRUSH CDateTimeEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	int nIndex = m_bEnable ? COLOR_WINDOW : COLOR_BTNFACE;
	HBRUSH hbr = ::GetSysColorBrush(nIndex);
	pDC->SetBkMode(TRANSPARENT);
	return hbr;
}

void CDateTimeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bEnable)
		return;
	//yyyy-mm-dd hh:mm:ss
	//0123456789012345678
	int nStartChar, nEndChar;
	switch (nChar)
	{
	case VK_LEFT:
		GetSel(nStartChar, nEndChar);
		if (nStartChar > DTS_YEAR)
		{
			m_nPos = SmartSel(nStartChar - 1);
		}
		return;
	case VK_RIGHT:
		GetSel(nStartChar, nEndChar);
		if (nEndChar < DTS_SECOND)
		{
			m_nPos = SmartSel(nEndChar + 1);
		}
		return;
	case VK_UP:
		SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA), 0);//数字变大
		return;
	case VK_DOWN:
		SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(0, - WHEEL_DELTA), 0);//数字变小
		return;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDateTimeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bEnable)
		return;
	if (nChar >= '0' && nChar <='9')
	{
		//yyyy-mm-dd hh:mm:ss
		//0123456789012345678
		int nStartChar, nEndChar;
		GetSel(nStartChar, nEndChar);
		TCHAR str[20];
		GetWindowText(str, sizeof(str));
		memcpy(str + m_nPos, &nChar, 1);
		SetWindowText(str);
		SetSel(nStartChar, nEndChar);
		switch (m_nPos)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			m_nPos = m_nPos < 3 ? ++ m_nPos : DTS_YEAR;
			break;
		case 5:
		case 6:
			m_nPos = m_nPos < 6 ? ++ m_nPos : DTS_MONTH;
			break;
		case 8:
		case 9:
			m_nPos = m_nPos < 9 ? ++ m_nPos : DTS_DAY;
			break;
		case 11:
		case 12:
			m_nPos = m_nPos < 12 ? ++ m_nPos : DTS_HOUR;
			break;
		case 14:
		case 15:
			m_nPos = m_nPos < 15 ? ++ m_nPos : DTS_MINUTE;
			break;
		case 17:
		case 18:
			m_nPos = m_nPos < 18 ? ++ m_nPos : DTS_SECOND;
			break;
		}
		return;
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CDateTimeEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (!m_bEnable)
		return FALSE;
	COleDateTime dateSrc;
	if (!GetDateTime(dateSrc))
		return FALSE;
	int nStartChar, nEndChar;
	CString str, s;
	int n;
	GetSel(nStartChar, nEndChar);
	GetWindowText(str);
	s = str.Mid(nStartChar, nEndChar - nStartChar);
	_stscanf(s, _T("%d"), &n);
	zDelta > 0 ? n ++ : n --;
	if (n < 0)
		return FALSE;
	int nYear = dateSrc.GetYear();
	int nMonth = dateSrc.GetMonth();
	int nDay = dateSrc.GetDay();
	int nHour = dateSrc.GetHour();
	int nMin = dateSrc.GetMinute();
	int	nSec = dateSrc.GetSecond();
	switch (nStartChar)
	{
	case DTS_YEAR:
		nYear = n;
		break;
	case DTS_MONTH:
		nMonth = n;
		break;
	case DTS_DAY:
		nDay = n;
		break;
	case DTS_HOUR:
		nHour = n;
		break;
	case DTS_MINUTE:
		nMin = n;
		break;
	case DTS_SECOND:
		nSec = n;
		break;
	}
	dateSrc.SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	SetDateTime(dateSrc);
	SetSel(nStartChar, nEndChar);
	return TRUE;
}

BOOL CDateTimeEdit::LessThanDate(COleDateTime& dt)
{
	COleDateTimeSpan ts = COleDateTime::GetCurrentTime() - dt;
	if (ts.GetTotalSeconds()>NEARZERO){ return TRUE;}

	return FALSE;
}

void CDateTimeEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bEnable)
	{
		COleDateTime dateSrc;
		if (GetDateTime(dateSrc) && LessThanDate(dateSrc))
		{
			SetDateTime(COleDateTime::GetCurrentTime());	
		}
		m_nPos = SmartSel(CharFromPos(point));
	}
	CEdit::OnLButtonUp(nFlags, point);
}

int CDateTimeEdit::SmartSel(int nPos)
{//返回选区的初始位置
	//yyyy-mm-dd hh:mm:ss
	//0123456789012345678
	int nStartChar, nEndChar;
	switch (nPos)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		nStartChar = DTS_YEAR;
		nEndChar = DTS_YEAR + 4;
		break;
	case 5:
	case 6:
	case 7:
		nStartChar = DTS_MONTH;
		nEndChar = DTS_MONTH + 2;
		break;
	case 8:
	case 9:
	case 10:
		nStartChar = DTS_DAY;
		nEndChar = DTS_DAY + 2;
		break;
	case 11:
	case 12:
	case 13:
		nStartChar = DTS_HOUR;
		nEndChar = DTS_HOUR + 2;
		break;
	case 14:
	case 15:
	case 16:
		nStartChar = DTS_MINUTE;
		nEndChar = DTS_MINUTE + 2;
		break;
	case 17:
	case 18:
	case 19:
		nStartChar = DTS_SECOND;
		nEndChar = DTS_SECOND + 2;
		break;
	default:
		nStartChar = DTS_SECOND;
	}
	SetSel(nStartChar, nEndChar);
	return nStartChar;
}

BOOL CDateTimeEdit::SetDateTime(const COleDateTime& dateSrc)
{
	BOOL bRes = FALSE;
	if (dateSrc.GetStatus() == COleDateTime::valid)
	{
		CString str = dateSrc.Format(_T("%Y-%m-%d %H:%M:%S"));
		SetWindowText(str);
		bRes = TRUE;
	}
	return bRes;
}

BOOL CDateTimeEdit::GetDateTime(COleDateTime& dateSrc)
{
	CString str;
	GetWindowText(str);
	return dateSrc.ParseDateTime(str);
}

void CDateTimeEdit::ClearDateTime()
{
	SetWindowText(_T("    -  -     :  :  "));
}

BOOL CDateTimeEdit::EnableWindow(BOOL bEnable)
{
	BOOL bRes = FALSE;
	if (m_bEnable != bEnable)
	{
		m_bEnable = bEnable;
		Invalidate();
		bRes = TRUE;
	}
	return bRes;
}