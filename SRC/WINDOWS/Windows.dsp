# Microsoft Developer Studio Project File - Name="Windows" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Windows - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Windows.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Windows.mak" CFG="Windows - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Windows - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Windows - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Windows - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "..\UnWn"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\UnWn"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O1 /Ob2 /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"StdAfx.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\Lib\UnRender.lib ..\Lib\UnNet.lib ..\Lib\UnEngine.lib ..\Lib\UnEditor.lib ..\Lib\UnGame.lib ..\lib\dxguid.lib winmm.lib /nologo /base:"0x16000000" /subsystem:windows /incremental:yes /machine:I386 /out:"..\..\System\UnServer.exe"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "..\UnWn"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\UnWn"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /Zi /Od /Gf /Gy /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG" /Yu"StdAfx.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\Lib\dRender.lib ..\Lib\dNet.lib ..\Lib\dEngine.lib ..\Lib\dEditor.lib ..\Lib\dGame.lib ..\lib\dxguid.lib winmm.lib /nologo /base:"0x16000000" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\..\System\dServer.exe"
# SUBTRACT LINK32 /profile /incremental:no

!ENDIF 

# Begin Target

# Name "Windows - Win32 Release"
# Name "Windows - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Ati3dCif.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UnPswd.h
# End Source File
# Begin Source File

SOURCE=.\UnSrv.h
# End Source File
# Begin Source File

SOURCE=.\UnWn.h
# End Source File
# Begin Source File

SOURCE=.\UnWnAudD.h
# End Source File
# Begin Source File

SOURCE=.\UnWnCam.h
# End Source File
# Begin Source File

SOURCE=.\UnWnDlg.h
# End Source File
# Begin Source File

SOURCE=.\UnWnEdSv.h
# End Source File
# Begin Source File

SOURCE=.\UnWnPref.h
# End Source File
# Begin Source File

SOURCE=.\UnWnProp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Cursors\AddActor.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushFr.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushMov.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushRot.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushSca.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushShr.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushSna.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushStr.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\BrushWrp.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\CamZoom.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\MoveAct.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\SelActor.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\SelectPo.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TerraFor.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TexGrab.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TexPan.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TexRot.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TexScale.cur
# End Source File
# Begin Source File

SOURCE=.\Cursors\TexSet.cur
# End Source File
# Begin Source File

SOURCE=..\Res\Unreal.ico
# End Source File
# Begin Source File

SOURCE=.\Res\UnSplash.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\UnSplash2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\UnWn.rc2
# End Source File
# End Group
# Begin Source File

SOURCE="..\Doc\!ReadMe.txt"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\UnD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\UnGlide.cpp
# End Source File
# Begin Source File

SOURCE=.\UnMsgPmp.cpp
# End Source File
# Begin Source File

SOURCE=.\UnPlatfm.cpp
# End Source File
# Begin Source File

SOURCE=.\UnPswd.cpp
# End Source File
# Begin Source File

SOURCE=.\UnRage.cpp
# End Source File
# Begin Source File

SOURCE=.\UnRenDev.cpp
# End Source File
# Begin Source File

SOURCE=.\UnSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWn.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWn.odl
# ADD MTL /tlb "..\..\System\Unreal.tlb"
# End Source File
# Begin Source File

SOURCE=.\UnWn.rc

!IF  "$(CFG)" == "Windows - Win32 Release"

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnWnAudD.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWnCam.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWnEdSv.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWnPref.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWnProp.cpp
# End Source File
# End Target
# End Project
