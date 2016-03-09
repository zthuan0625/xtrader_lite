#include "stdafx.h"
#include "xTrader.h"
#include "DlgRecent.h"
#include "LoginDlg.h"

IMPLEMENT_DYNAMIC(DlgRecent, CDialog)

DlgRecent::DlgRecent(CWnd* pParent /*=NULL*/)
	: CDialog(DlgRecent::IDD, pParent)
{
	m_bSave = g_s.m_bSaveHis;
}

DlgRecent::~DlgRecent()
{

}

void DlgRecent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LBRECENT, m_List);
	DDX_Check(pDX, IDC_CHK_RECENT, m_bSave);
}

BEGIN_MESSAGE_MAP(DlgRecent, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTLBDEL, OnClkLbDel)
	ON_BN_CLICKED(IDC_CHK_RECENT, OnBnClkChkRecent)
	ON_LBN_DBLCLK(IDC_LBRECENT, OnLbnDblclkLbRecent)
	ON_LBN_SELCHANGE(IDC_LBRECENT, OnLbnSelchange)
END_MESSAGE_MAP()

void DlgRecent::OnBnClkChkRecent()
{
	LoginDlg* pLogin = (LoginDlg*)GetParent();
	m_bSave = !m_bSave;
	g_s.m_bSaveHis = !!m_bSave;
}

void DlgRecent::OnLbnDblclkLbRecent()
{
	LoginDlg* pLogin = (LoginDlg*)GetParent();
		
	int iSel = m_List.GetCurSel();
	if (iSel>=0) { pLogin->OnLogin(); }
}

void DlgRecent::OnDestroy()
{
	CDialog::OnDestroy();

	LoginDlg* pLogin = (LoginDlg*)GetParent();
	pLogin->GetDlgItem(IDC_EXTRA)->EnableWindow(TRUE);

	delete this;
}

void DlgRecent::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

void DlgRecent::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgRecent::InitLBox()
{
	LoginDlg* pLogin = (LoginDlg*)GetParent();
	m_List.SubclassDlgItem(IDC_LBRECENT, this);

	int i = 0;
	CAppCfgs& s = g_s;
	vector<LOGINPARA>& pAcc = s.m_pInfVec;
	vector<BKRPARA>& pBkr = s.m_BkrParaVec;
	int iSize = static_cast<int>(pAcc.size());
	CString szAccInf,szUid,szPass;

	int iBkrGroup, iSvrGroup;
	CString szSvrGN;
	int iH=0;
	if (gv_Bkr.size())
	{
		for (i = 0; i < iSize; i++)
		{
			ansi2uni(CP_UTF8, pAcc[i].szUid, szUid.GetBuffer(MAX_PATH));
			szUid.ReleaseBuffer();
				
			ansi2uni(CP_UTF8, pAcc[i].szPass, szPass.GetBuffer(MAX_PATH));
			szPass.ReleaseBuffer();
			
			iBkrGroup = pAcc[i].iBkrGroup;
			iSvrGroup = pAcc[i].iSvrGroup;
			GetSvrGNByIdx(szSvrGN, iBkrGroup, iSvrGroup);
				
			szAccInf.Format(_T("%s,%s,账户:%s,密码:%s"), pBkr[iBkrGroup].BkrName,
				(LPCTSTR)szSvrGN, (LPCTSTR)szUid, (LPCTSTR)szPass);
				
			m_List.AddString(szAccInf);
			iH = m_List.GetItemHeight(i)+1;
			m_List.SetItemHeight(i, iH);
			SetHScroll();
		}
	}
}

void DlgRecent::GetSvrGNByIdx(CString &szOut, int iBkrG, int iSvrG)
{
	xml_document doc;
	xml_parse_result result;

	szOut.Empty();
	result = doc.load_file(gv_Bkr[iBkrG].XmlPath);
	if (result.status == status_ok)
	{
		///////////读出服务器//////////////////
		xpath_node_set sVrs = doc.select_nodes(_NDPATHSVR);
		if (sVrs.empty()) return;
		int iCount = 0;
		char strName[64];
		for (xpath_node_set::const_iterator it = sVrs.begin(); it != sVrs.end(); ++it)
		{
			xpath_node nd = *it;
			iCount++;
			//读出服务器组名称
			if (iCount == (iSvrG + 1))
			{
				strcpy(strName, nd.node().child_value(_NDNAME));
				ansi2uni(CP_ACP, strName, szOut.GetBuffer(MAX_PATH));
				szOut.ReleaseBuffer();

				break;
			}
		}
	}
}

BOOL DlgRecent::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitLBox();
	return TRUE;
}

void DlgRecent::OnLbnSelchange()
{
	LoginDlg* pLogin = (LoginDlg*)GetParent();
	CString szUid,szPass;
	TThostFtdcPasswordType szDecPass;
		
	int iSel = m_List.GetCurSel();
	if (iSel>=0)
	{
		vector<LOGINPARA>& pAcc = g_s.m_pInfVec;
		int iBkrGroup = pAcc[iSel].iBkrGroup;
		int iSvrGroup = pAcc[iSel].iSvrGroup;
			
		ansi2uni(CP_UTF8, pAcc[iSel].szUid, szUid.GetBuffer(MAX_PATH));
		szUid.ReleaseBuffer();
			
		Base64Decode(szDecPass, (const char*)pAcc[iSel].szPass, 0);
		ansi2uni(CP_UTF8, szDecPass, szPass.GetBuffer(MAX_PATH));
		szPass.ReleaseBuffer();
			
		pLogin->m_ComboBkr.ResetContent();
		int iIdx = pLogin->m_ComboBkr.AddString(gv_Bkr[iBkrGroup].BkrName);
		pLogin->m_ComboBkr.SetItemData(iIdx, iBkrGroup);
		pLogin->m_ComboBkr.SetCurSel(0);
		pLogin->SendMessage(WM_COMMAND,MAKEWPARAM(IDC_BKR_LIST,CBN_SELCHANGE),(LPARAM)(pLogin->m_ComboBkr.GetSafeHwnd()));
			
		pLogin->m_ComboIsp.SetCurSel(iSvrGroup);
		pLogin->SendMessage(WM_COMMAND,MAKEWPARAM(IDC_ISPLIST,CBN_SELCHANGE),(LPARAM)(pLogin->m_ComboIsp.GetSafeHwnd()));
			
		HWND hEdUser = ::GetDlgItem(pLogin->m_hWnd,IDC_USERNAME);
		::SendMessage(hEdUser,WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szUid);
			
		HWND hEdPass = ::GetDlgItem(pLogin->m_hWnd,IDC_PASSWORD);
		::SendMessage(hEdPass,WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szPass);
	}
}

void DlgRecent::OnClkLbDel()
{
	int iSel = m_List.GetCurSel();
	if (iSel>=0)
	{
		m_List.DeleteString(iSel);
		gv_Acc.erase(gv_Acc.begin()+iSel);	
	}
}

void DlgRecent::SetHScroll()
{
	CDC* dc = GetDC();
	SIZE s;
	int index;
	CString str;
	long temp;
	for(index= 0; index< m_List.GetCount(); index++)
	{
		m_List.GetText(index,str);
		s = dc->GetTextExtent(str,str.GetLength()+1);  
		temp = (long)SendDlgItemMessage(IDC_LBRECENT, LB_GETHORIZONTALEXTENT, 0, 0);
		if (s.cx > temp)  
		{
			SendDlgItemMessage(IDC_LBRECENT, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
		}
	}
	ReleaseDC(dc);
}
