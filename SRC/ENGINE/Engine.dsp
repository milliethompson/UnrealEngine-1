# Microsoft Developer Studio Project File - Name="Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Engine - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engine - Win32 Release"

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
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O2 /Ob2 /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Yu"Unreal.h" /FD /QIfdiv- /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\Lib\UnAudio.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:"0x1200000" /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib:"LIBC" /out:"..\..\System\UnEngine.dll"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

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
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /Zi /Od /Gf /Gy /I "..\Inc" /I "..\Inc\3dfx" /D "COMPILING_ENGINE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"Unreal.h" /FD /QIfdiv- /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib ..\Lib\UnAudio.lib /nologo /base:"0x1200000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBC" /out:"..\..\System\dEngine.dll"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\Inc\AActor.h
# End Source File
# Begin Source File

SOURCE=..\Inc\ABrush.h
# End Source File
# Begin Source File

SOURCE=..\Inc\AMover.h
# End Source File
# Begin Source File

SOURCE=..\Inc\APawn.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=..\Res\Unreal.ico
# End Source File
# End Group
# Begin Source File

SOURCE="..\Doc\!ReadMe.txt"
# End Source File
# Begin Source File

SOURCE=.\Engine.rc

!IF  "$(CFG)" == "Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnActCol.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnActLst.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnActor.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnCache.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnCamera.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnCamMgr.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnChecks.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnClass.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnConfig.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnDeflts.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnDynBSP.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Inc\UnEditor.h
# End Source File
# Begin Source File

SOURCE=.\UnEngine.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnFGAud.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2 /Yu

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnFont.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnFPoly.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnGfx.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnIn.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnKeyVal.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnLevAct.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnLevel.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnLevTic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnMath.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnMem.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnMesh.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnModel.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnMusic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnName.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnObj.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnOutDev.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnParams.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnParse.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnPath.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Game\UnPawn.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnPhysic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnPrefer.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2 /Yu

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnPrim.cpp
# End Source File
# Begin Source File

SOURCE=.\UnProp.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnReach.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Unreal.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2 /Yc"Unreal.h"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# ADD CPP /Yc"Unreal.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnRoute.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnScript.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /FA<none>

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnServer.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnSound.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnTex.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnTopics.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnTrace.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
