#include "stdafx.h"
#include "Htmlctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMyIEWnd, CWnd)
BEGIN_MESSAGE_MAP(CMyIEWnd, CWnd)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CMyIEWnd::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	//::PostMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_CONTMENU_MSG,0,0);
}

IMPLEMENT_DYNAMIC(CMyHtmlCtrl, CHtmlCtrl)

void CMyHtmlCtrl::OnNavigateComplete2(LPCTSTR strURL)
{
	HWND hwnd = GetBrowserWindow();
	if (!m_myIEWnd.m_hWnd) { m_myIEWnd.SubclassWindow(hwnd); }
}

IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)
BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CHtmlCtrl::CHtmlCtrl()
{
	m_bMemoryMode=FALSE;
	m_lpBodyContent=NULL;
	m_lpstrCharset=NULL;
	m_lpstrScript=NULL;
	m_lpstrTitle=NULL;
	m_pHtmlDoc2=NULL;
}

CHtmlCtrl::~CHtmlCtrl()
{
	if( m_bMemoryMode)
	{
		while( !m_BodyAttributes.IsEmpty())
		{
			CBodyAttr* pAttr=m_BodyAttributes.RemoveHead();
			delete pAttr;
		}
	}
}

void CHtmlCtrl::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

void CHtmlCtrl::OnDestroy()
{
	if (m_pBrowserApp) 
	{
	#if _MSC_VER < 1600
		m_pBrowserApp->Release();
	#endif
		m_pBrowserApp = NULL;
	}
	
	CHtmlView::OnDestroy();
}

int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CHtmlView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
								  DWORD nFlags,
								  LPCTSTR lpszTargetFrameName,
								  CByteArray& baPostedData,
								  LPCTSTR lpszHeaders,
								  BOOL* pbCancel )
{
	//内存网页模式里有链接的话
	//先切换到普通模式方便正常访问
	if (m_bMemoryMode)
	{
		NavigateNormal();
	}
}

BOOL CHtmlCtrl::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassDlgItem(nID, pParent))
		return FALSE;

	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();

	return Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rc, pParent, nID, NULL);
}

HWND CHtmlCtrl::GetBrowserWindow()
{
	HWND hwnd = m_hWnd;
	while (TRUE)
	{
		HWND hwndChild = ::GetWindow(hwnd, GW_CHILD);
		if (hwndChild == NULL) { return hwnd; }
		hwnd = hwndChild;
	}
	return NULL;
}

void CHtmlCtrl::ShowMemoryHtml()
{
	SetTitle(_T(""));
	SetScript(_T(""));
	SetCharset(_T(""));
	PutBodyContent(_T(""));
	SetBodyAttributes(_T(""), COleVariant());
}

BOOL CHtmlCtrl::PutBodyContent(LPCTSTR lpszContent)
{
	if(_tcslen(lpszContent)) m_lpBodyContent=(LPTSTR)lpszContent;
	if( m_pHtmlDoc2)
	{
		HRESULT hr = S_OK;
		IHTMLElement *pBodyElement;
		hr=m_pHtmlDoc2->get_body( &pBodyElement);
		_bstr_t pbBody(m_lpBodyContent);
		hr=pBodyElement->put_innerHTML( pbBody);

		if( hr==S_FALSE) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

BOOL CHtmlCtrl::SetBodyAttributes(CString sName, COleVariant vValue)
{
	if(!sName.IsEmpty())
	{
		CBodyAttr* pAttr;
		pAttr=new CBodyAttr;

		pAttr->sName=sName;	pAttr->vValue=vValue;
		m_BodyAttributes.AddHead( pAttr);
	}
	if( m_pHtmlDoc2)
	{
		HRESULT hr = S_OK;
		IHTMLElement *pBodyElement;
		hr=m_pHtmlDoc2->get_body( &pBodyElement);
		
		POSITION pos;
		for( pos=m_BodyAttributes.GetHeadPosition(); pos!=NULL;)
		{
			CBodyAttr* pAttr;
			pAttr=m_BodyAttributes.GetNext( pos);
			_bstr_t bpAttributeName(pAttr->sName);
			hr=pBodyElement->setAttribute( bpAttributeName, pAttr->vValue, FALSE);
		}

		if( hr==S_FALSE) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

BOOL CHtmlCtrl::SetScript(LPCTSTR lpstrScript)
{
	if(_tcslen(lpstrScript)) m_lpstrScript=(LPTSTR)lpstrScript;
	if( m_pHtmlDoc2)
	{
		HRESULT hr = S_OK;
		IHTMLElementCollection *pElCol;
		hr=m_pHtmlDoc2->get_scripts( &pElCol);

		COleVariant vName( (long)0);
		COleVariant vIndex( (long)0);
		IDispatch *pDisp;	
		hr=pElCol->item( vName, vIndex, &pDisp);
	
		IHTMLScriptElement *pScriptEl;
		hr=pDisp->QueryInterface( IID_IHTMLScriptElement, (void**)&pScriptEl);
		_bstr_t bpTextNew( m_lpstrScript);
		hr=pScriptEl->put_text( bpTextNew);

		if( hr==S_FALSE) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

BOOL CHtmlCtrl::SetTitle(LPCTSTR lpstrTitle)
{
	if(_tcslen(lpstrTitle)) m_lpstrTitle=(LPTSTR)lpstrTitle;
	if( m_pHtmlDoc2)
	{
		HRESULT hr = S_OK;
		_bstr_t pbTitle( m_lpstrTitle);
		hr=m_pHtmlDoc2->put_title( pbTitle);
	
		if( hr==S_FALSE) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

BOOL CHtmlCtrl::SetCharset(LPCTSTR lpstrCharset)
{
	if(_tcslen(lpstrCharset)) m_lpstrCharset=(LPTSTR)lpstrCharset;
	if( m_pHtmlDoc2)
	{
		HRESULT hr = S_OK;
		//put charset
		_bstr_t bpCharset( m_lpstrCharset);
		hr=m_pHtmlDoc2->put_charset( bpCharset);

		if( hr==S_FALSE) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

void CHtmlCtrl::NavigateMemory()
{
	LoadFromResource(_T("empty.htm"));
	m_bMemoryMode = TRUE;
}

void CHtmlCtrl::NavigateNormal()
{
	m_bMemoryMode=FALSE;
}

void CHtmlCtrl::OnDocumentComplete(LPCTSTR lpszURL) 
{
	if(m_bMemoryMode)
	{
		LPDISPATCH lpDispatch;
		lpDispatch=GetHtmlDocument();
		ASSERT(lpDispatch);
		HRESULT hr=lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&m_pHtmlDoc2);

		if( SUCCEEDED(hr)) ShowMemoryHtml();
	}

	CHtmlView::OnDocumentComplete(lpszURL);
}

void CHtmlCtrl::OnDownloadComplete() 
{
	//show html on refresh
	if( m_bMemoryMode)
		if( m_pHtmlDoc2)
			ShowMemoryHtml();
	
	CHtmlView::OnDownloadComplete();
}
