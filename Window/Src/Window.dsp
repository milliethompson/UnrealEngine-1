# Microsoft Developer Studio Project File - Name="Window" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Window - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Window.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Window.mak" CFG="Window - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Window - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Window - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Unreal/Window", FAAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Window - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O2 /I "." /I "..\Inc" /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D WINDOW_API=__declspec(dllexport) /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\..\Core\Lib\Core.lib ..\..\Engine\Lib\Engine.lib user32.lib kernel32.lib gdi32.lib advapi32.lib comctl32.lib comdlg32.lib /nologo /base:"0x10B00000" /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\..\System\Window.dll"

!ELSEIF  "$(CFG)" == "Window - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MDd /W4 /WX /Gm /vd0 /GX /Zi /Od /I "." /I "..\Inc" /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D WINDOW_API=__declspec(dllexport) /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\Core\Lib\Core.lib ..\..\Engine\Lib\Engine.lib user32.lib kernel32.lib gdi32.lib advapi32.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\System\Window.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Window - Win32 Release"
# Name "Window - Win32 Debug"
# Begin Group "Src"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\Res\WindowRes.h
# End Source File
# End Group
# Begin Group "Res"

# PROP Default_Filter "*.res"
# Begin Source File

SOURCE=.\Res\Unreal.ico
# End Source File
# Begin Source File

SOURCE=.\Res\WindowRes.rc

!IF  "$(CFG)" == "Window - Win32 Release"

!ELSEIF  "$(CFG)" == "Window - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Inc"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\Inc\Window.h
# End Source File
# End Group
# End Target
# End Project
