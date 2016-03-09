# Microsoft Developer Studio Project File - Name="xTrader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XTRADER - WIN32 REL64
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xTrader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xTrader.mak" CFG="XTRADER - WIN32 REL64"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xTrader - Win32 Rel64" (based on "Win32 (x86) Application")
!MESSAGE "xTrader - Win32 Rel32" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xTrader - Win32 Rel64"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin64"
# PROP BASE Intermediate_Dir "Rel64"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin64"
# PROP Intermediate_Dir "Rel64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /O2 /D "WIN64" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_AFXDLL" /Yu"stdafx.h" /FD /EHsc /EHsc /EHsc /EHsc /EHsc /EHsc /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /pdb:none /machine:IX86 /machine:AMD64 /opt:nowin98 /MANIFEST:no
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "xTrader - Win32 Rel32"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin"
# PROP BASE Intermediate_Dir "Rel32"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "Rel32"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "api_libs64" /I "XListCtrl" /D "WIN64" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /EHsc /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "api_libs" /I "XListCtrl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_AFXDLL" /Yu"stdafx.h" /FD /EHsc /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:IX86 /out:"bin64/xTrader.exe" /libpath:"api_libs" /machine:AMD64 /opt:nowin98
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /pdb:none /machine:IX86 /libpath:"api_libs" /opt:nowin98

!ENDIF 

# Begin Target

# Name "xTrader - Win32 Rel64"
# Name "xTrader - Win32 Rel32"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppCfgs.cpp
# End Source File
# Begin Source File

SOURCE=.\BfTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\BkrNtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\ColorStatic.cpp

!IF  "$(CFG)" == "xTrader - Win32 Rel64"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "xTrader - Win32 Rel32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CostMs.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBfLog.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgModPass.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNetSel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOrdEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQryHiSet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRecent.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSiLog.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgUnLock.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\FixHtmlView.cpp
# End Source File
# Begin Source File

SOURCE=.\GenMfDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\HtmlCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\mdspi.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\MMStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\MyCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\MyMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\NoticeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\NumSpinCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ParkOrd.cpp
# End Source File
# Begin Source File

SOURCE=.\pugixml.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\SliderEx.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\TimeEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\traderspi.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\XListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\XPGroupBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\XTList.cpp
# End Source File
# Begin Source File

SOURCE=.\xTrader.cpp
# End Source File
# Begin Source File

SOURCE=.\xTraderDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\BfTransfer.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\Color.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\ComboEx.h
# End Source File
# Begin Source File

SOURCE=.\DlgModPass.h
# End Source File
# Begin Source File

SOURCE=.\DlgNetSel.h
# End Source File
# Begin Source File

SOURCE=.\DlgOrdEx.h
# End Source File
# Begin Source File

SOURCE=.\DlgQryHiSet.h
# End Source File
# Begin Source File

SOURCE=.\DlgRecent.h
# End Source File
# Begin Source File

SOURCE=.\GenMfDlg.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\mdspi.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\MMStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\MyMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\NoticeDlg.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\NumSpinCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ParkOrd.h
# End Source File
# Begin Source File

SOURCE=.\PPageMisc.h
# End Source File
# Begin Source File

SOURCE=.\pugiconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\pugixml.hpp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\SliderEx.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# Begin Source File

SOURCE=.\traderspi.h
# End Source File
# Begin Source File

SOURCE=.\DIYCtrl\XPGroupBox.h
# End Source File
# Begin Source File

SOURCE=.\xTrader.h
# End Source File
# Begin Source File

SOURCE=.\xTraderDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\conf.bin
# End Source File
# Begin Source File

SOURCE=.\res\empty.htm
# End Source File
# Begin Source File

SOURCE=.\res\readme.htm
# End Source File
# Begin Source File

SOURCE=.\res\xTrader.ico
# End Source File
# Begin Source File

SOURCE=.\xTrader.rc
# End Source File
# Begin Source File

SOURCE=.\res\xTrader.rc2
# End Source File
# Begin Source File

SOURCE=.\res\xTrader.xml
# End Source File
# End Group
# End Target
# End Project
