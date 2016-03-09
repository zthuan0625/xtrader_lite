#include "stdafx.h"
#include "MyCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MyCombo::MyCombo()
{
    //m_bAutoComplete = TRUE;
}

MyCombo::~MyCombo()
{
}


BEGIN_MESSAGE_MAP(MyCombo, CComboBox)
	//{{AFX_MSG_MAP(MyCombo)
	ON_WM_CTLCOLOR()
	//ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
void MyCombo::OnEditUpdate() 
{
	if (!m_bAutoComplete) { return; }

	CString str;
	GetWindowText(str);
	int nLength = str.GetLength();
  
	DWORD dwCurSel = GetEditSel();
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);

	if (SelectString(-1, str) == CB_ERR)
	{
		SetWindowText(str);
		if (dwCurSel != CB_ERR) { SetEditSel(dStart, dEnd); }
	}

	if (dEnd < nLength && dwCurSel != CB_ERR)
		SetEditSel(dStart, dEnd);
	else
		SetEditSel(nLength, -1);
}
*/

HBRUSH MyCombo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:
		break;
	case CTLCOLOR_LISTBOX:
		int iItemNum=GetCount();
		int iWidth=0;
		CString strItem;
		CClientDC dc(this);
		int iSaveDC=dc.SaveDC();
		dc.SelectObject(GetFont());
		int iVSWidth=::GetSystemMetrics(SM_CXVSCROLL);
		for(int i=0;i<iItemNum;i++)
		{
			GetLBText(i,strItem);
			int iWholeWidth=dc.GetTextExtent(strItem).cx+iVSWidth;
			iWidth=max(iWidth,iWholeWidth);
		}
		iWidth+=dc.GetTextExtent(_T("a")).cx;
		dc.RestoreDC(iSaveDC);
		if(iWidth>0)
		{
			CRect rc;
			pWnd->GetWindowRect(&rc);
			if(rc.Width()!=iWidth)
			{
				rc.right=rc.left+iWidth;
				pWnd->MoveWindow(&rc);
			}
		}
		break;
	}
	return hbr;
}
