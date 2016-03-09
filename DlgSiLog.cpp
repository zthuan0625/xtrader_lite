#include "stdafx.h"
#include "xTrader.h"
#include "DlgSiLog.h"

extern HANDLE g_hEvent;
IMPLEMENT_DYNAMIC(DlgSiLog, CDialog)
DlgSiLog::DlgSiLog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SILOGIN, pParent)
{

}

DlgSiLog::~DlgSiLog()
{
}

void DlgSiLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRGSILOGIN, m_prg);
}

BEGIN_MESSAGE_MAP(DlgSiLog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void DlgSiLog::ProgressUpdate(LPCTSTR szMsg, const int nPercent)
{
    ASSERT (AfxIsValidString(szMsg));
    ASSERT ( nPercent >= 0  &&  nPercent <= 100 );
	
    SetDlgItemText(IDC_SLISTAMSG,szMsg);
    m_prg.SetPos(nPercent);
}

BOOL DlgSiLog::OnInitDialog()
{
	CDialog::OnInitDialog();

	AfxBeginThread((AFX_THREADPROC)LoginThread, this);

	return TRUE; 
}

void DlgSiLog::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}

void DlgSiLog::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}

void DlgSiLog::OnDestroy()
{
	CDialog::OnDestroy();
    delete this;
}

UINT DlgSiLog::LoginThread(LPVOID pParam)
{
	CtpTdSpi* td = gc_Td;
	DlgSiLog* pDlg = static_cast<DlgSiLog*>(pParam);

    td->ClrAllVecs();
	td->InitMgrFee();

	#ifdef _NEED_LOGIN_
	pDlg->ProgressUpdate(_T("确认结算单>>>"), 2);
	td->ReqSetInfConfirm();
	DWORD dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("确认结算单成功!"), 5);
		ResetEvent(g_hEvent);
	}
	else
	{
		pDlg->ProgressUpdate(_T("确认结算超时!"),0);
		goto THREAD_END;
	}
	/////////////////////////////////////////////////
	pDlg->ProgressUpdate(_T("查询行情信息>>"), 10);

	td->ReqQryDepthMD(NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取行情信息成功!"), 15);
		ResetEvent(g_hEvent);	
	}
	else
	{
		pDlg->ProgressUpdate(_T("查询行情信息超时!"),0);
		goto THREAD_END;
	}
	/////////////////////////////////////////////////
	pDlg->ProgressUpdate(_T("查询合约列表>>>"), 18);
	td->ReqQryInst(NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取合约列表成功!"), 23);
		ResetEvent(g_hEvent);
	}
	else
	{
		pDlg->ProgressUpdate(_T("查询合约超时!"),0);
		goto THREAD_END;
	}
	
	pDlg->ProgressUpdate(_T("查询当日成交信息>>"), 28);
	td->ReqQryTd(NULL,NULL,NULL,NULL,NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取成交信息成功!"), 33);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查询成交信息超时!"),0);
		goto THREAD_END;
	}
	
	pDlg->ProgressUpdate(_T("查询当日委托信息>>"), 38);
	td->ReqQryOrd(NULL,NULL,NULL,NULL,NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取委托信息成功!"), 43);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查询委托信息超时!"),0);
		goto THREAD_END;
	}

	pDlg->ProgressUpdate(_T("查询持仓信息>>"),48);
	td->ReqQryInvPos(NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取持仓信息成功!"), 53);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查持仓信息超时!"),0);
		goto THREAD_END;
	}
	
	pDlg->ProgressUpdate(_T("查询资金账户>>"), 58);
	
	td->ReqQryTdAcc();
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取资金信息成功!"), 63);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查账户超时!"),0);
		goto THREAD_END;
	}
	//////////////////////////////////////////
	pDlg->ProgressUpdate(_T("查询银期签约>>"), 65);
	
	td->ReqQryAccreg();
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取银期信息成功!"), 70);
		ResetEvent(g_hEvent);		
	}
	else
	{
		pDlg->ProgressUpdate(_T("查询银期信息超时!"),0);
		goto THREAD_END;
	}
	
	pDlg->ProgressUpdate(_T("查询交易编码>>"), 73);
		
	td->ReqQryTdCode();
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取交易编码成功!"), 78);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查询交易编码超时!"),0);
		goto THREAD_END;
	}
	
	pDlg->ProgressUpdate(_T("查询经纪参数>>"), 80);

	td->ReqQryBkrTdParams();
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取经纪参数成功!"), 85);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查询经纪参数超时!"),0);
		goto THREAD_END;
	}

	pDlg->ProgressUpdate(_T("查询预埋单>>"), 90);
	
	td->ReqQryParkedOrd(NULL,NULL);
	dwRet = WaitForSingleObject(g_hEvent,WAIT_MS);
	if (dwRet==WAIT_OBJECT_0)
	{
		pDlg->ProgressUpdate(_T("获取预埋单成功!"), 99);
		ResetEvent(g_hEvent);
	}	
	else
	{
		pDlg->ProgressUpdate(_T("查询预埋单超时!"),0);
		goto THREAD_END;
	}
	#endif

	g_Dlg->PostMessage(WM_RELOGINOK);
	
THREAD_END:
	pDlg->PostMessage(WM_CLOSE); 
	return 0;
}