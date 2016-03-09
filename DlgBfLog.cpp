#include "stdafx.h"
#include "xTrader.h"
#include "DlgBfLog.h"

IMPLEMENT_DYNAMIC(DlgBfLog, CDialog)

DlgBfLog::DlgBfLog(CWnd* pParent /*=NULL*/)
	: CDialog(DlgBfLog::IDD, pParent)
{

}

DlgBfLog::~DlgBfLog()
{
}

void DlgBfLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_BFDETAILS, m_LstLog);
}

BEGIN_MESSAGE_MAP(DlgBfLog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL DlgBfLog::OnInitDialog()
{
	InitList();

	UpdateLog();
	return TRUE;
}

void DlgBfLog::InitList()
{
	m_LstLog.SubclassDlgItem(IDC_LST_BFDETAILS,this);

	LPCTSTR lpHdrs[6] = {_T("流水号"),_T("银行账号"),_T("交易类型"),_T("发生金额"),_T("交易时间"),_T("信息")};
	int iWidths[6] = {46,140,100,80,60,180};
	for (int i = 0;i<6 ; i++)
	{
		m_LstLog.InsertColumn(i, lpHdrs[i],LVCFMT_CENTER,iWidths[i]);
	}
}

void DlgBfLog::UpdateLog()
{
    CtpTdSpi* td = gc_Td;
	int uSize = static_cast<int>(td->m_BfTransVec.size());
	
	int iSubItem = 0;
	CString szItems[6];
	for (int i=0;i<uSize;i++)
	{
		szItems[0].Format(_T("%d"),td->m_BfTransVec[i].FutureSerial);
		
		ansi2uni(CP_ACP,td->m_BfTransVec[i].BankAccount,szItems[1].GetBuffer(MAX_PATH));
		szItems[1].ReleaseBuffer();
		
		szItems[2]=JgBfTdType(td->m_BfTransVec[i].TradeCode);
		szItems[3].Format(_T("%.2f"),td->m_BfTransVec[i].TradeAmount);
		szItems[4] = TransTime(td->m_BfTransVec[i].TradeTime);
		
		ansi2uni(CP_ACP,td->m_BfTransVec[i].ErrorMsg,szItems[5].GetBuffer(3*MAX_PATH));
		szItems[5].ReleaseBuffer();
		
		for (iSubItem=0;iSubItem<6;iSubItem++)
		{
			if (iSubItem==0) { m_LstLog.InsertItem(i,NULL);}
			m_LstLog.SetItemText(i,iSubItem,szItems[iSubItem]);
		}
	}
}
void DlgBfLog::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgBfLog::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}

void DlgBfLog::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}
