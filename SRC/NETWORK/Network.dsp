# Microsoft Developer Studio Project File - Name="Network" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Network - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Network.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Network.mak" CFG="Network - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Network - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Network - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Network - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O1 /Ob2 /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_NETWORK" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"windows.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:"0x14000000" /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\..\System\UnNet.dll"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /Zi /Od /Gf /Gy /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_NETWORK" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX"windows.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:"0x14000000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\System\dNet.dll"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "Network - Win32 Release"
# Name "Network - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\DPlay.h
# End Source File
# Begin Source File

SOURCE=.\NetDPlay.h
# End Source File
# Begin Source File

SOURCE=.\NetINet.h
# End Source File
# Begin Source File

SOURCE=.\NetPrv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Res\Unreal.ico
# End Source File
# Begin Source File

SOURCE=.\Wizard.bmp
# End Source File
# End Group
# Begin Source File

SOURCE="..\Doc\!ReadMe.txt"
# End Source File
# Begin Source File

SOURCE=.\NetDPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\NetINet.cpp
# End Source File
# Begin Source File

SOURCE=.\NetMain.cpp
# End Source File
# Begin Source File

SOURCE=.\NetWin.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.rc

!IF  "$(CFG)" == "Network - Win32 Release"

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
