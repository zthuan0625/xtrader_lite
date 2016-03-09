
#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
	//InitializeCriticalSection(&m_cs);
	m_crBkColor = BLACK; // Initializing the Background Color to the system face color.
	//m_crTextColor = YELLOW; // Initializing the text to Black
	//m_uAlign = DT_CENTER;
	m_dScale = LMT(getMasterScale(),1.0,5.0);
	SetFont(_T("Arial Black"),static_cast<int>(15*m_dScale),FW_SEMIBOLD);
}

CColorStatic::~CColorStatic()
{
}

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	//ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CColorStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CColorStatic::OnPaint()
{
	CPaintDC dc(this);

	CDC memDC;
	CRect rc;
	CBitmap memBitmap;
	CBitmap* oldBitmap;
	CFont *oldFont;
	
	GetClientRect(&rc);
	
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);
	
	m_brBkgnd.CreateSolidBrush(m_crBkColor);
	memDC.FillRect(&rc, &m_brBkgnd);
	m_brBkgnd.DeleteObject();

	memDC.SetBkMode(TRANSPARENT);
	memDC.SetBkColor(m_crBkColor);
	memDC.SetTextColor(m_crTextColor);

	oldFont = memDC.SelectObject(&m_cFont);

	memDC.DrawText(m_szText,-1,&rc,m_uAlign);
	
	memDC.SelectObject(oldFont);
	dc.BitBlt(rc.left, rc.top,rc.Width(), rc.Height(),&memDC,0, 0,SRCCOPY);
	
	memDC.SelectObject(oldBitmap);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CColorStatic::SetBkColor(COLORREF crColor)
{
	m_crBkColor = crColor; 
}

void CColorStatic::SetText(LPCTSTR lpszString,COLORREF crColor,UINT uAlign)
{
	//Lock();
	m_szText = lpszString;
	//UnLock();
	m_crTextColor = crColor;
	m_uAlign = uAlign;

	Invalidate(TRUE);
}

void CColorStatic::SetInt(int iVal,COLORREF crColor,UINT uAlign)
{
	TCHAR szTemp[MAX_PATH];
	_stprintf(szTemp,_T("%d"),iVal);

	SetText(szTemp, crColor,uAlign);
}

void CColorStatic::SetDouble(double dVal,COLORREF crColor,UINT uAlign)
{
	CString szTemp = FiltPx(dVal);
	
	SetText(szTemp, crColor,uAlign);
}

BOOL CColorStatic::SetFont(LPCTSTR szFont,int iHeight,int iWeight)
{
	LOGFONT oldFt;
	if (m_cFont.m_hObject != NULL)
	{ 
		GetFont(&oldFt);
		if (!_tcsicmp(oldFt.lfFaceName,szFont) && (oldFt.lfHeight == static_cast<int>(m_dScale*iHeight)) && (oldFt.lfWeight==static_cast<int>(m_dScale*iWeight)))
		{
			return TRUE;
		}
		else
		{
			m_cFont.DeleteObject(); 
		}	
	}
	
	return	m_cFont.CreateFont(static_cast<int>(m_dScale*iHeight), 0, 0, 0, static_cast<int>(m_dScale*iWeight),
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_NATURAL_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, szFont);
}

int CColorStatic::GetFont(LOGFONT* pLogFont)
{
	return m_cFont.GetLogFont(pLogFont);
}