# Microsoft Developer Studio Project File - Name="Render" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Render - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Render.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Render.mak" CFG="Render - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Render - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Render - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Render - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Render__"
# PROP BASE Intermediate_Dir ".\Render__"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /GB /Zp4 /MD /W4 /WX /vd0 /GX /O2 /Ob2 /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Unreal.h" /FD /QIfdiv- /QIfdiv- /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:"0x15000000" /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\..\System\UnRender.dll"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Render__"
# PROP BASE Intermediate_Dir ".\Render__"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /Zi /Od /Gf /Gy /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Unreal.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:"0x15000000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\System\dRender.dll"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "Render - Win32 Release"
# Name "Render - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AaFLLib.h
# End Source File
# Begin Source File

SOURCE=.\AaLineDo.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Res\Unreal.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\Render.rc

!IF  "$(CFG)" == "Render - Win32 Release"

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnBlast.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\.\Release
InputPath=.\UnBlast.asm

"$(IntDir)\UnBlast.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnBlast.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\.\Debug
InputPath=.\UnBlast.asm

"$(IntDir)\UnBlast.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnBlast.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnBlastC.cpp
# End Source File
# Begin Source File

SOURCE=.\UnDither.cpp
# End Source File
# Begin Source File

SOURCE=.\UnEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\UnEdRend.cpp
# End Source File
# Begin Source File

SOURCE=.\UnFire.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\.\Release
InputPath=.\UnFire.asm

"$(IntDir)\UnFire.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnFire.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\.\Debug
InputPath=.\UnFire.asm

"$(IntDir)\UnFire.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnFire.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnFireEn.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\UnFireEn.h
# End Source File
# Begin Source File

SOURCE=.\UnLight.cpp
# End Source File
# Begin Source File

SOURCE=.\UnLine.cpp
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UnLine1.cpp
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UnMeshRn.cpp
# End Source File
# Begin Source File

SOURCE=.\UnPalTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\UnRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\UnRaster.cpp
# End Source File
# Begin Source File

SOURCE=..\Engine\Unreal.cpp
# ADD CPP /Yc"Unreal.h"
# End Source File
# Begin Source File

SOURCE=.\UnRender.cpp
# End Source File
# Begin Source File

SOURCE=.\UnRender.inc
# End Source File
# Begin Source File

SOURCE=.\UnSpan.cpp
# End Source File
# Begin Source File

SOURCE=.\UnSpanX.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\.\Release
InputPath=.\UnSpanX.asm

"$(IntDir)\UnSpanX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnSpanX.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\.\Debug
InputPath=.\UnSpanX.asm

"$(IntDir)\UnSpanX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnSpanX.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\UnTest.cpp
# ADD CPP /FAs /Fa"..\Listing\UnTest.asm"
# End Source File
# Begin Source File

SOURCE=.\UnTMapC.cpp
# End Source File
# Begin Source File

SOURCE=.\UnWater.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\.\Release
InputPath=.\UnWater.asm

"$(IntDir)\UnWater.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnWater.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\.\Debug
InputPath=.\UnWater.asm

"$(IntDir)\UnWater.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Fo$(IntDir)\UnWater.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
