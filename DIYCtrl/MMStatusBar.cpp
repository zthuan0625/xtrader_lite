#include "stdafx.h"
#include "MMStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MMStatusBar::MMStatusBar()
{
}

MMStatusBar::~MMStatusBar()
{
  for(int j = 0 ; j < m_oDrawStructArray.GetSize(); j++)
  {
    MMStatusDrawStruct* pDrawStruct = (MMStatusDrawStruct*)m_oDrawStructArray.GetAt(j);
    delete pDrawStruct;
  }
  m_oDrawStructArray.RemoveAll();
}


BEGIN_MESSAGE_MAP(MMStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(MMStatusBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int MMStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
  
	m_oToolTip.Create(this,TTS_ALWAYSTIP);
	m_oToolTip.Activate(TRUE);
	return 0;
}

void MMStatusBar::OnDestroy() 
{
	CStatusBar::OnDestroy();
	
	if(::IsWindow(m_oToolTip.GetSafeHwnd()))
		m_oToolTip.DestroyWindow();
}

BOOL MMStatusBar::SetIndicators(const UINT* lpIDArray,int nIDCount,const UINT* lpIDToolTipArray,const HICON* lphIconArray)
{
  ASSERT(lpIDToolTipArray==NULL || 
         AfxIsValidAddress(lpIDToolTipArray,sizeof(UINT)* nIDCount,FALSE));
  ASSERT(lphIconArray==NULL || 
         AfxIsValidAddress(lphIconArray,sizeof(HICON)* nIDCount,FALSE));

  if(!CStatusBar::SetIndicators(lpIDArray,nIDCount))	// trouble with standard bar
    return FALSE;					// abort

  if(lpIDArray != NULL &&			      // have indicators and
     lpIDToolTipArray != NULL)			// want tooltiptexts
  {
    for(int i =0;i<nIDCount;i++)		// each (new) indicator
    {
      CRect oRect;
      GetItemRect(i,&oRect);
      UINT uID  = lpIDArray[i];
      UINT uIDText = lpIDToolTipArray[i];
      if (uID != ID_SEPARATOR &&			// have indicator and 
	        uIDText != 0)				        // want tooltiptext for it
        VERIFY(m_oToolTip.AddTool(this,uIDText,oRect,uID));
    }
  }
  if(lpIDArray != NULL &&
     lphIconArray != NULL)
  {
    for(int i =0;i<nIDCount;i++)		// each (new) indicator
    {
      UINT uID  = lpIDArray[i];
      if (uID != ID_SEPARATOR &&     // have indicator and
          lphIconArray[i] != NULL)   // want icon
      {
        GetStatusBarCtrl().SetIcon(i,lphIconArray[i]);
        int nWidth;
        UINT nID,nStyle;
	      GetPaneInfo(i,nID,nStyle,nWidth);
	      SetPaneInfo(i,nID,nStyle,nWidth + GetSystemMetrics(SM_CXSMICON));
      }
    }
  }
  return TRUE;
}

void MMStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	
	if(!IsWindow(m_oToolTip.GetSafeHwnd()))
		return ;

	Redraw();
}

void MMStatusBar::Redraw()
{
  CToolInfo oToolInfo;
	memset(&oToolInfo,0,sizeof(TOOLINFO));
	oToolInfo.cbSize = sizeof(TOOLINFO) ;

	CRect oRect;
	UINT uId;
	for(int i = 0 ; i < m_nCount ; i++)
	{
		uId = GetItemID(i);
		GetItemRect(i,&oRect);
		
		oToolInfo.hwnd = this->GetSafeHwnd();
		oToolInfo.uId = uId;
		oToolInfo.rect.left = oRect.left;
		oToolInfo.rect.right = oRect.right;
		oToolInfo.rect.top = oRect.top;
		oToolInfo.rect.bottom = oRect.bottom;

    m_oToolTip.SendMessage(TTM_NEWTOOLRECT,0,(LPARAM)(TOOLINFO*)&oToolInfo);
    MMStatusDrawStruct* pstDrawStruct = GetPaneStruct(i);

    if(pstDrawStruct)
      SendMessage(SB_SETTEXT,pstDrawStruct->m_nIndex | SBT_OWNERDRAW,(LPARAM)pstDrawStruct);
	}
}

void MMStatusBar::UpdateTipText(LPCTSTR lpszTxt,UINT nIDTool)
{
	//if (uIDTxt != 0)				    
	m_oToolTip.UpdateTipText(lpszTxt,this->GetParent(),nIDTool);
}

void MMStatusBar::SetDelayTime(UINT uDelayTime)
{
  m_oToolTip.SetDelayTime(uDelayTime);
}

void MMStatusCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CStatusBar* pStatusBar = (CStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pStatusBar->GetPaneStyle(m_nIndex) & ~SBPS_DISABLED;
	if (!bOn)
		nNewStyle |= SBPS_DISABLED;
	pStatusBar->SetPaneStyle(m_nIndex, nNewStyle);
}

void MMStatusCmdUI::SetCheck(int nCheck)
{
	CStatusBar* pStatusBar = (CStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pStatusBar->GetPaneStyle(m_nIndex) & ~SBPS_POPOUT;
	if (nCheck != 0)
		nNewStyle |= SBPS_POPOUT;
	pStatusBar->SetPaneStyle(m_nIndex, nNewStyle);
}

void MMStatusCmdUI::SetText(LPCTSTR lpszText)
{
	MMStatusBar* pStatusBar = (MMStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

  MMStatusDrawStruct* pstDrawStruct = pStatusBar->GetPaneStruct(m_nIndex);

  if(pstDrawStruct && pstDrawStruct->m_szText.Compare(lpszText))
  {
    pstDrawStruct->m_szText = lpszText;
    pStatusBar->GetStatusBarCtrl().SendMessage(SB_SETTEXT,m_nIndex | SBT_OWNERDRAW,(LPARAM)pstDrawStruct);
    pStatusBar->Invalidate();
    pStatusBar->UpdateWindow();
  }
  else
  {
	  pStatusBar->SetPaneText(m_nIndex, lpszText);
  }

  int cxExtra = 0;
  HICON hIcon = (HICON)pStatusBar->GetStatusBarCtrl().SendMessage(SB_GETICON,(WPARAM)m_nIndex,0);
  if(hIcon)
    cxExtra = GetSystemMetrics(SM_CXSMICON);//get icon width

  if(pstDrawStruct)
  {
    if(hIcon)
      pstDrawStruct->m_cxExtra = cxExtra;
    else
      pstDrawStruct->m_cxExtra = 0;
  }

  CClientDC dc(NULL);
  HFONT hFont,hOldFont;
	hFont = (HFONT)pStatusBar->SendMessage(WM_GETFONT);
	hOldFont = (HFONT)dc.SelectObject(hFont);
	SetWidth(dc.GetTextExtent(lpszText).cx + cxExtra);
  //cleanup
	dc.SelectObject(hOldFont);
}

void MMStatusCmdUI::SetToolTipText(LPCTSTR lpszText)
{
	MMStatusBar* pStatusBar = (MMStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	pStatusBar->m_oToolTip.UpdateTipText(lpszText,m_pOther,m_nID);
}

void MMStatusCmdUI::SetWidth(int cxWidth)
{
	MMStatusBar* pStatusBar = (MMStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);
	UINT nID;
	UINT nStyle;
	int nWidth;
	pStatusBar->GetPaneInfo(m_nIndex,nID,nStyle,nWidth);
	pStatusBar->SetPaneInfo(m_nIndex,m_nID,nStyle,cxWidth);

  pStatusBar->Redraw();
}

void MMStatusCmdUI::SetIcon(HICON hIcon)
{
	MMStatusBar* pStatusBar = (MMStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);
	pStatusBar->GetStatusBarCtrl().SetIcon(m_nIndex,hIcon);

  CString szPaneText;
  pStatusBar->GetPaneText(m_nIndex,szPaneText);
  int cxExtra = 0;
  if(hIcon)
    cxExtra = GetSystemMetrics(SM_CXSMICON);

  MMStatusDrawStruct* pstDrawStruct = pStatusBar->GetPaneStruct(m_nIndex);
  if(pstDrawStruct)
  {
    if(hIcon)
      pstDrawStruct->m_cxExtra = cxExtra;
    else
      pstDrawStruct->m_cxExtra = 0;
  }
  CClientDC dc(NULL);
  HFONT hFont,hOldFont;
  //get current font
	hFont = (HFONT)pStatusBar->SendMessage(WM_GETFONT);
	hOldFont = (HFONT)dc.SelectObject(hFont);
	SetWidth(dc.GetTextExtent(szPaneText).cx + cxExtra);
  //cleanup
	dc.SelectObject(hOldFont);
}

void MMStatusBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	MMStatusCmdUI state;
	state.m_pOther = this;
	state.m_pMenu = (CMenu*)(UINT_PTR)MM_STATUS_PANE_UPDATE;
	state.m_nIndexMax = (UINT)m_nCount;
	for (state.m_nIndex=0; state.m_nIndex<state.m_nIndexMax;state.m_nIndex++)
	{
		state.m_nID = GetItemID(state.m_nIndex);

		if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
			continue;

		state.DoUpdate(pTarget, FALSE);
	}

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

BOOL MMStatusBar::PreTranslateMessage(MSG *pMsg)
{
	switch(pMsg->message)
	{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		m_oToolTip.RelayEvent (pMsg);
		default :break;
	}
	return CStatusBar::PreTranslateMessage(pMsg);
}

BOOL MMStatusBar::OnChildNotify(UINT message,WPARAM wParam,LPARAM lParam,LRESULT* pResult)
{
	switch(message)
	{
		case WM_NOTIFY:
			{
				NMHDR* pNMHDR = (NMHDR*)lParam;
				if(NM_DBLCLK == pNMHDR->code)
				{
					NMMOUSE* pNMMouse = (NMMOUSE*)lParam;
					if(pNMMouse->dwItemSpec >= 0 && pNMMouse->dwItemSpec < (UINT)m_nCount)
					{
						UINT uCommandId = GetItemID(static_cast<UINT>(pNMMouse->dwItemSpec));
						this->GetParent()->SendMessage(WM_COMMAND,uCommandId,0);
					}
				}
			}
			break;
		default:break;
	}

	return CStatusBar::OnChildNotify(message,wParam,lParam,pResult);
}

void MMStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  MMStatusDrawStruct* pstDrawStruct = (MMStatusDrawStruct*)GetPaneStruct(lpDrawItemStruct->itemID);

  ASSERT(pstDrawStruct);

  CDC oDC;
  oDC.Attach(lpDrawItemStruct->hDC);
  oDC.SetTextColor(pstDrawStruct->m_crTextColor);
  if(DEFAULT_PANE_COLOR == pstDrawStruct->m_crBkColor)
    oDC.SetBkMode(TRANSPARENT);
  else
    oDC.SetBkColor(pstDrawStruct->m_crBkColor);
  lpDrawItemStruct->rcItem.left += 2;
  lpDrawItemStruct->rcItem.left += pstDrawStruct->m_cxExtra;
  oDC.DrawText(pstDrawStruct->m_szText,&lpDrawItemStruct->rcItem,DT_LEFT);
  oDC.Detach();
}

void MMStatusBar::SetPaneTextColor(int nIndex,COLORREF crTextColor)
{
  MMStatusDrawStruct* pstDrawStruct = GetPaneStruct(nIndex);
  if(pstDrawStruct && crTextColor != pstDrawStruct->m_crTextColor)
  {
    pstDrawStruct->m_crTextColor = crTextColor;
  }
  else if(NULL == pstDrawStruct)
  {
    pstDrawStruct = new MMStatusDrawStruct;
    pstDrawStruct->m_cxExtra = 0;
    pstDrawStruct->m_nIndex = nIndex;
    pstDrawStruct->m_crTextColor = crTextColor;
    pstDrawStruct->m_crBkColor = DEFAULT_PANE_COLOR;
    GetPaneText(nIndex,pstDrawStruct->m_szText);
    m_oDrawStructArray.Add(pstDrawStruct);
  }

  SendMessage(SB_SETTEXT,nIndex | SBT_OWNERDRAW,(LPARAM)pstDrawStruct);
} 

void MMStatusBar::SetPaneTextBkColor(int nIndex,COLORREF crTextBkColor)
{
  MMStatusDrawStruct* pstDrawStruct = GetPaneStruct(nIndex);
  if(pstDrawStruct && crTextBkColor != pstDrawStruct->m_crBkColor)
  {
    pstDrawStruct->m_crBkColor = crTextBkColor;
  }
  else if(NULL == pstDrawStruct)
  {
    pstDrawStruct = new MMStatusDrawStruct;
    pstDrawStruct->m_cxExtra = 0;
    pstDrawStruct->m_nIndex = nIndex;
    pstDrawStruct->m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
    pstDrawStruct->m_crBkColor = crTextBkColor;
    GetPaneText(nIndex,pstDrawStruct->m_szText);
    m_oDrawStructArray.Add(pstDrawStruct);
  }

  SendMessage(SB_SETTEXT,nIndex | SBT_OWNERDRAW,(LPARAM)pstDrawStruct);
}

MMStatusDrawStruct* MMStatusBar::GetPaneStruct(int nIndex)
{
  for(int j = 0 ; j < m_oDrawStructArray.GetSize(); j++)
  {
    MMStatusDrawStruct* pDrawStruct = (MMStatusDrawStruct*)m_oDrawStructArray.GetAt(j);
    if(pDrawStruct && pDrawStruct->m_nIndex == nIndex)
      return pDrawStruct;
  }
  return NULL;
}