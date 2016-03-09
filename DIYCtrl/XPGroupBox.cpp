#include "stdafx.h"
#include "XPGroupBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXPGroupBox,CButton);

CXPGroupBox::CXPGroupBox()
{
    m_strTitle = _T("");
	
	m_clrBorder = ::GetSysColor(COLOR_3DSHADOW);
	m_clrClientBackground = ::GetSysColor(COLOR_BTNFACE);

	m_clrTitleText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTitleBackground = ::GetSysColor(COLOR_BTNFACE);
	
	m_nType = XPGB_FRAME;
	m_dwAlignment = SS_LEFT;
}

CXPGroupBox::~CXPGroupBox()
{
}

BEGIN_MESSAGE_MAP(CXPGroupBox, CButton)
	//{{AFX_MSG_MAP(CXPGroupBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXPGroupBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CXPGroupBox::MyPaint(CDC* dc)
{
  	CRect	rectClient;
	GetClientRect(rectClient);
	
	// Defalte Rect
	rectClient.DeflateRect(1,1);
	
	// Get Text Rect 
	CSize sizeText;
	CRect rectText, rectFrame;
	CRect rectTitle, rectContent;
	
	CFont *pOldFont = dc->SelectObject(&m_font);
  		
	// get Text if need
	if ( m_strTitle.IsEmpty() )
	{ 
        GetWindowText(m_strTitle);
		if ( ! m_strTitle.IsEmpty() )
			m_strTitle = _T(" ") + m_strTitle + _T(" ");
	}
	
	if ( ! m_strTitle.IsEmpty() )
	{
		sizeText = dc->GetTextExtent(m_strTitle);
	}
	else
	{
		sizeText.cx = 0;
		sizeText.cy = 0;
	}

	if ( m_nType == XPGB_FRAME ) // Frame style
	{
		// Calculate Text Rect 
		switch(m_dwAlignment)
		{
		case SS_LEFT:	
			rectText.top = rectClient.top;
			rectText.left = rectClient.left + 10;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx;
			break;
		case	SS_CENTER:	
			rectText.top = rectClient.top;
			rectText.left = rectClient.left + (rectClient.Width() - sizeText.cx) / 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx;
			break;
		case	SS_RIGHT	:
			rectText.top = rectClient.top;
			rectText.right = rectClient.right -10 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.left = rectText.right - sizeText.cx;
			break;
		}
		
		//  Calculate Frame rect
		rectFrame.left = rectClient.left;
		rectFrame.top = rectClient.top + sizeText.cy/2;
		
		rectFrame.right = rectClient.right;
		rectFrame.bottom = rectFrame.top + rectClient.Height() - sizeText.cy/2; 
		
		// Draw Frame border
		CPen penFrame;
		CBrush brushBKFrame(m_clrTitleBackground);
		
		penFrame.CreatePen(PS_SOLID, 1, m_clrBorder);
		
		CPen* pOldPen = dc->SelectObject(&penFrame);
		CBrush* pOldBrush = (CBrush*)dc->SelectStockObject(NULL_BRUSH);
		
		dc->RoundRect(rectFrame, CPoint(10,10)); 
		
		dc->SelectObject(pOldPen);
		dc->SelectObject(pOldBrush); 
		
		dc->IntersectClipRect(rectText);
		dc->FillSolidRect(rectText, m_clrTitleBackground);
	}
	else  // Windows Style
	{
		// Calculate Title size
		rectTitle.top = rectClient.top;
		rectTitle.left = rectClient.left ;
		
		rectTitle.right = rectClient.right;
		rectTitle.bottom = rectClient.top + sizeText.cy + 4;
		
		// Draw Title round rect
		CPen penFrame;
		CBrush brushBKTitle(m_clrTitleBackground);
		CBrush brushBKContent(m_clrClientBackground);
		
		penFrame.CreatePen(PS_SOLID, 1, m_clrBorder);
		
		CPen* pOldPen = dc->SelectObject(&penFrame);
		CBrush* pOldBrush = dc->SelectObject(&brushBKTitle);
		
		dc->RoundRect(rectClient, CPoint(10, 10)); 
		
		dc->SelectObject(pOldBrush); 
		
		// Draw content area
		rectContent.left = rectClient.left;
		rectContent.top = rectClient.top + sizeText.cy + 4;
		
		rectContent.right = rectClient.right;
		rectContent.bottom = rectContent.top + rectClient.Height() - sizeText.cy - 4; 
		
		pOldBrush = dc->SelectObject(&brushBKContent); 
		
		dc->Rectangle(rectContent);  
		
		dc->SelectObject(pOldPen);
		dc->SelectObject(pOldBrush); 
		
		// Calculate Text Rect 
		switch(m_dwAlignment)
		{
		case SS_LEFT:	
			rectText.top = rectTitle.top + 2;
			rectText.left = rectTitle.left + 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx ;
			break;
		case	SS_CENTER:	
			rectText.top = rectTitle.top + 2;
			rectText.left = rectTitle.left + (rectTitle.Width() - sizeText.cx) / 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx ;
			break;
		case	SS_RIGHT	:
			rectText.top = rectTitle.top + 2;
			rectText.right = rectClient.right - 2  ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.left = rectText.right - sizeText.cx;
			break;
		}	
	}
    
	COLORREF clrOldText = dc->SetTextColor(m_clrTitleText);
	UINT nMode = dc->SetBkMode(TRANSPARENT);
	
	dc->DrawText(m_strTitle, &rectText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP );
	
	// restore DC
	dc->SetBkMode(nMode);
    dc->SetTextColor(clrOldText);
    dc->SelectObject(pOldFont);
}

void CXPGroupBox::OnPaint() 
{
	CPaintDC dc(this);
	//CMemDC memDC(&dc);

	MyPaint(&dc);
}

BOOL CXPGroupBox::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= BS_ICON;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CXPGroupBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= BS_ICON;
	return CButton::PreCreateWindow(cs);
}

void CXPGroupBox::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_ICON);
	CFont* cf = GetFont();
	if(cf !=NULL)
	{
		cf->GetObject(sizeof(m_lf),&m_lf);
	}
	else
	{
		GetObject(GetStockObject(SYSTEM_FONT),sizeof(m_lf),&m_lf);
	}

	ReconstructFont();
}

void CXPGroupBox::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}

void CXPGroupBox::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

CXPGroupBox& CXPGroupBox::SetXPGroupStyle(XPGroupBoxStyle eStyle) 
{
   m_nType = eStyle;
   UpdateSurface();
   return *this;
}

CXPGroupBox& CXPGroupBox::SetFont(LOGFONT lf)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));
	ReconstructFont();
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetFontBold(BOOL bBold)
{
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetFontName(const CString& strFont, BYTE byCharSet)
{
	m_lf.lfCharSet = byCharSet;

	_tcscpy(m_lf.lfFaceName,strFont);
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CXPGroupBox& CXPGroupBox::SetFontUnderline(BOOL bSet)
{
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

CXPGroupBox& CXPGroupBox::SetFontItalic(BOOL bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;	
}

CXPGroupBox& CXPGroupBox::SetFontSize(int nSize)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);

	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;

	ReconstructFont();
	UpdateSurface();

	return *this;
}

CXPGroupBox& CXPGroupBox::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder = clrBorder;
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetCatptionTextColor(COLORREF clrText ) 
{
	m_clrTitleText = clrText;
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetBackgroundColor(COLORREF clrBKClient)
{
	m_clrTitleBackground = clrBKClient;
	m_clrClientBackground = clrBKClient;
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetBackgroundColor(COLORREF clrBKTilte,  COLORREF clrBKClient)
{
	m_clrTitleBackground = clrBKTilte;
	m_clrClientBackground = clrBKClient;
	UpdateSurface();
	return *this;
}

CXPGroupBox& CXPGroupBox::SetText(LPCTSTR lpszText)
{
	if(IsWindow(this->GetSafeHwnd())) 
	{
		m_strTitle = lpszText;
		m_strTitle = _T(" ") + m_strTitle + _T(" ");
		UpdateSurface();
	}
	
	return *this;
}

CXPGroupBox& CXPGroupBox::SetAlignment(DWORD dwType)
{
	switch(dwType)
	{
	default	:	ASSERT(false);
	case SS_LEFT:	
		m_dwAlignment =  SS_LEFT;
		break;
	case	SS_CENTER:	
		m_dwAlignment = SS_CENTER;
		break;
	case	SS_RIGHT	:
		m_dwAlignment = SS_RIGHT;
		break;
	}
	UpdateSurface();
	
	return *this;
}


