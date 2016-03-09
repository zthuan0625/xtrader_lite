#pragma once

#pragma warning(disable:4101)
#pragma warning(disable:4102)

#define _CRT_SECURE_NO_WARNINGS

#ifndef WINVER  
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0502
#endif

#ifndef _WIN32_WINDOWS 
#define _WIN32_WINDOWS 0x0500 
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600     
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define VC_EXTRALEAN

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxdtctl.h>
#include <afxtempl.h>
#include <afxdlgs.h>
#include <afxcmn.h>
#include <afxpriv.h>
#include <afxhtml.h>
#include <mshtml.h>
#include <comdef.h>
#include <Iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <afxsock.h>
#include <shlwapi.h>	
#include <mmsystem.h>

#include "global.h"
#include "tools.h"

#if _MSC_VER >= 1400
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


