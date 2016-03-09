#include "stdafx.h"
#include "XListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXListCtrl::CXListCtrl()
{
	m_iRow = -1;
	m_iCol = -1;
	m_Font = NULL;
}

CXListCtrl::~CXListCtrl()
{
	if (m_Font) { delete m_Font; } 
}

BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,OnNMCustomdraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXListCtrl::OnDestroy()
{	
	CListCtrl::OnDestroy();
}

void CXListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	DWORD dwStyle, dwExStyle;
	dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	dwStyle |= (LVS_NOSORTHEADER|LVS_REPORT|LVS_SINGLESEL);
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle);
	dwExStyle = GetExtendedStyle();
	dwExStyle |= LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES;
	SetExtendedStyle(dwExStyle);
	
	CImageList m_ImageList; 
	m_ImageList.Create(1,22,ILC_COLOR,1,1); 	 
	SetImageList(&m_ImageList,LVSIL_SMALL);
}

int CXListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		ASSERT(FALSE);
		return -1;
	}
	
	return 0;
}

BOOL CXListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CXListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)   
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
    *pResult = CDRF_DODEFAULT;
	
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
        if ( m_iCol == pLVCD->iSubItem && m_iRow == pLVCD->nmcd.dwItemSpec)
		{
			pLVCD->clrTextBk = m_Color;
			pLVCD->clrText = m_TextColor;
			//SetFont(m_Font, false);
		}
		else
		{
			pLVCD->clrTextBk = WHITE;
			pLVCD->clrText = BLACK;
		}
		//SetFont(m_Font, false);
        *pResult = CDRF_DODEFAULT;
	}
}

BOOL CXListCtrl::SetItemCountEx(int iCount, DWORD dwFlags)
{
	return CListCtrl::SetItemCountEx(iCount, dwFlags);
}

void CXListCtrl::SetColor(int iRow, int iCol, COLORREF TextColor,COLORREF bkColor)
{
	m_iRow = iRow;
	m_iCol = iCol;
	m_Color = bkColor;
	m_TextColor = TextColor;
}

void CXListCtrl::SetSubItemFont(LOGFONT font, COLORREF color, long lsize)
{
	if (m_Font) { delete m_Font; }
	m_Font = new CFont;
	m_Font->CreateFontIndirect(&font);

	m_TextColor = color;
	m_TextSize = lsize;
}