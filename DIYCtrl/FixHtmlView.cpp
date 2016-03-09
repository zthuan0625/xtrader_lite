#include "stdafx.h"
#include "FixHtmlView.h"

CString CFixHtmlView::GetFullName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_FullName(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr);
    return retVal;
}

CString CFixHtmlView::GetType() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_Type(&bstr);

    CString retVal(bstr);

    SysFreeString(bstr);
    return retVal;
}

CString CFixHtmlView::GetLocationName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationName(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr);
    return retVal;
}
 
CString CFixHtmlView::GetLocationURL() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationURL(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr);
    return retVal;
}

void CFixHtmlView::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
    CString strURL(lpszURL);
    BSTR bstrURL = strURL.AllocSysString();

    COleSafeArray vPostData;
    if (lpvPostData != NULL)
    {
       if (dwPostDataLen == 0)
          dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

       vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
    }

    m_pBrowserApp->Navigate(bstrURL, COleVariant((long) dwFlags, VT_I4), COleVariant(lpszTargetFrameName, VT_BSTR), 
                            vPostData, COleVariant(lpszHeaders, VT_BSTR));

    SysFreeString(bstrURL);
}


BOOL CFixHtmlView::LoadFromResource(LPCTSTR lpszResource)
{
    HINSTANCE hInstance = AfxGetResourceHandle();
    ASSERT(hInstance != NULL);

    CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

    if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
       strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
       Navigate(strResourceURL, 0, 0, 0);
    }
    else
       bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}

BOOL CFixHtmlView::LoadFromResource(UINT nRes)
{
    HINSTANCE hInstance = AfxGetResourceHandle();
    ASSERT(hInstance != NULL);

    CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

    if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
       strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
       Navigate(strResourceURL, 0, 0, 0);
    }
    else
       bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}