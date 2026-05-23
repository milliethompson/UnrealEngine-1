# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) External Target" 0x0106
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Classes - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Classes - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Engine - Win32 Release" && "$(CFG)" != "Engine - Win32 Debug"\
 && "$(CFG)" != "Game - Win32 Release" && "$(CFG)" != "Game - Win32 Debug" &&\
 "$(CFG)" != "Windows - Win32 Release" && "$(CFG)" != "Windows - Win32 Debug" &&\
 "$(CFG)" != "Documentation - Win32 Release" && "$(CFG)" !=\
 "Network - Win32 Release" && "$(CFG)" != "Network - Win32 Debug" && "$(CFG)" !=\
 "Editor - Win32 Release" && "$(CFG)" != "Editor - Win32 Debug" && "$(CFG)" !=\
 "Render - Win32 Release" && "$(CFG)" != "Render - Win32 Debug" && "$(CFG)" !=\
 "Classes - Win32 Release" && "$(CFG)" != "Classes - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unreal.mak" CFG="Classes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Windows - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Windows - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Documentation - Win32 Release" (based on\
 "Win32 (x86) External Target")
!MESSAGE "Network - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Network - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Editor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Editor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Render - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Render - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Classes - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Classes - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Classes - Win32 Debug"

!IF  "$(CFG)" == "Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Engine\Release"
# PROP BASE Intermediate_Dir "Engine\Release"
# PROP BASE Target_Dir "Engine"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Engine\Release"
# PROP Target_Dir "Engine"
OUTDIR=.\Lib
INTDIR=.\Engine\Release

ALL : "$(OUTDIR)\UnEngine.dll"

CLEAN : 
	-@erase "..\System\UnEngine.dll"
	-@erase ".\Engine\Release\UnInput.obj"
	-@erase ".\Engine\Release\UnCamera.obj"
	-@erase ".\Engine\Release\UnParams.obj"
	-@erase ".\Engine\Release\UnFile.obj"
	-@erase ".\Engine\Release\UnClsPrp.obj"
	-@erase ".\Engine\Release\UnMem.obj"
	-@erase ".\Engine\Release\UnScript.obj"
	-@erase ".\Engine\Release\UnLevTic.obj"
	-@erase ".\Engine\Release\UnEngine.obj"
	-@erase ".\Engine\Release\UnOutDev.obj"
	-@erase ".\Engine\Release\UnTopics.obj"
	-@erase ".\Engine\Release\UnDynBSP.obj"
	-@erase ".\Engine\Release\UnActor.obj"
	-@erase ".\Engine\Release\UnServer.obj"
	-@erase ".\Engine\Release\UnCamMgr.obj"
	-@erase ".\Engine\Release\UnKeyVal.obj"
	-@erase ".\Engine\Release\UnAction.obj"
	-@erase ".\Engine\Release\UnFGAud.obj"
	-@erase ".\Engine\Release\UnGfx.obj"
	-@erase ".\Engine\Release\UnRes.obj"
	-@erase ".\Engine\Release\UnConfig.obj"
	-@erase ".\Engine\Release\UnActLst.obj"
	-@erase ".\Engine\Release\UnMath.obj"
	-@erase ".\Engine\Release\UnFPoly.obj"
	-@erase ".\Engine\Release\UnChecks.obj"
	-@erase ".\Engine\Release\UnPhys.obj"
	-@erase ".\Engine\Release\UnClass.obj"
	-@erase ".\Engine\Release\UnParse.obj"
	-@erase ".\Engine\Release\UnModel.obj"
	-@erase ".\Engine\Release\UnCache.obj"
	-@erase ".\Engine\Release\UnCheat.obj"
	-@erase ".\Engine\Release\UnPrefer.obj"
	-@erase ".\Engine\Release\UnTex.obj"
	-@erase ".\Engine\Release\UnDeflts.obj"
	-@erase ".\Engine\Release\UnMesh.obj"
	-@erase ".\Engine\Release\UnLevAct.obj"
	-@erase ".\Engine\Release\UnLevel.obj"
	-@erase ".\Engine\Release\UnSound.obj"
	-@erase ".\Engine\Release\UnScrRTL.obj"
	-@erase ".\Engine\Release\Engine.res"
	-@erase ".\Lib\UnEngine.lib"
	-@erase ".\Lib\UnEngine.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /c 
CPP_OBJS=.\Engine\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Engine.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Engine.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\UnEngine.dll"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/UnEngine.pdb" /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\UnEngine.dll" /implib:"$(OUTDIR)/UnEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnInput.obj" \
	"$(INTDIR)/UnCamera.obj" \
	"$(INTDIR)/UnParams.obj" \
	"$(INTDIR)/UnFile.obj" \
	"$(INTDIR)/UnClsPrp.obj" \
	"$(INTDIR)/UnMem.obj" \
	"$(INTDIR)/UnScript.obj" \
	"$(INTDIR)/UnLevTic.obj" \
	"$(INTDIR)/UnEngine.obj" \
	"$(INTDIR)/UnOutDev.obj" \
	"$(INTDIR)/UnTopics.obj" \
	"$(INTDIR)/UnDynBSP.obj" \
	"$(INTDIR)/UnActor.obj" \
	"$(INTDIR)/UnServer.obj" \
	"$(INTDIR)/UnCamMgr.obj" \
	"$(INTDIR)/UnKeyVal.obj" \
	"$(INTDIR)/UnAction.obj" \
	"$(INTDIR)/UnFGAud.obj" \
	"$(INTDIR)/UnGfx.obj" \
	"$(INTDIR)/UnRes.obj" \
	"$(INTDIR)/UnConfig.obj" \
	"$(INTDIR)/UnActLst.obj" \
	"$(INTDIR)/UnMath.obj" \
	"$(INTDIR)/UnFPoly.obj" \
	"$(INTDIR)/UnChecks.obj" \
	"$(INTDIR)/UnPhys.obj" \
	"$(INTDIR)/UnClass.obj" \
	"$(INTDIR)/UnParse.obj" \
	"$(INTDIR)/UnModel.obj" \
	"$(INTDIR)/UnCache.obj" \
	"$(INTDIR)/UnCheat.obj" \
	"$(INTDIR)/UnPrefer.obj" \
	"$(INTDIR)/UnTex.obj" \
	"$(INTDIR)/UnDeflts.obj" \
	"$(INTDIR)/UnMesh.obj" \
	"$(INTDIR)/UnLevAct.obj" \
	"$(INTDIR)/UnLevel.obj" \
	"$(INTDIR)/UnSound.obj" \
	"$(INTDIR)/UnScrRTL.obj" \
	"$(INTDIR)/Engine.res"

"$(OUTDIR)\UnEngine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Engine\Debug"
# PROP BASE Intermediate_Dir "Engine\Debug"
# PROP BASE Target_Dir "Engine"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Engine\Debug"
# PROP Target_Dir "Engine"
OUTDIR=.\Lib
INTDIR=.\Engine\Debug

ALL : "$(OUTDIR)\dEngine.dll"

CLEAN : 
	-@erase ".\Engine\Debug\vc40.pdb"
	-@erase "..\System\dEngine.dll"
	-@erase ".\Engine\Debug\UnDeflts.obj"
	-@erase ".\Engine\Debug\UnFPoly.obj"
	-@erase ".\Engine\Debug\UnClass.obj"
	-@erase ".\Engine\Debug\UnActLst.obj"
	-@erase ".\Engine\Debug\UnParse.obj"
	-@erase ".\Engine\Debug\UnModel.obj"
	-@erase ".\Engine\Debug\UnChecks.obj"
	-@erase ".\Engine\Debug\UnCache.obj"
	-@erase ".\Engine\Debug\UnCheat.obj"
	-@erase ".\Engine\Debug\UnParams.obj"
	-@erase ".\Engine\Debug\UnFile.obj"
	-@erase ".\Engine\Debug\UnFGAud.obj"
	-@erase ".\Engine\Debug\UnEngine.obj"
	-@erase ".\Engine\Debug\UnLevel.obj"
	-@erase ".\Engine\Debug\UnSound.obj"
	-@erase ".\Engine\Debug\UnAction.obj"
	-@erase ".\Engine\Debug\UnInput.obj"
	-@erase ".\Engine\Debug\UnPrefer.obj"
	-@erase ".\Engine\Debug\UnLevAct.obj"
	-@erase ".\Engine\Debug\UnMesh.obj"
	-@erase ".\Engine\Debug\UnScrRTL.obj"
	-@erase ".\Engine\Debug\UnCamera.obj"
	-@erase ".\Engine\Debug\UnConfig.obj"
	-@erase ".\Engine\Debug\UnTex.obj"
	-@erase ".\Engine\Debug\UnActor.obj"
	-@erase ".\Engine\Debug\UnClsPrp.obj"
	-@erase ".\Engine\Debug\UnScript.obj"
	-@erase ".\Engine\Debug\UnLevTic.obj"
	-@erase ".\Engine\Debug\UnGfx.obj"
	-@erase ".\Engine\Debug\UnOutDev.obj"
	-@erase ".\Engine\Debug\UnRes.obj"
	-@erase ".\Engine\Debug\UnTopics.obj"
	-@erase ".\Engine\Debug\UnDynBSP.obj"
	-@erase ".\Engine\Debug\UnMath.obj"
	-@erase ".\Engine\Debug\UnPhys.obj"
	-@erase ".\Engine\Debug\UnServer.obj"
	-@erase ".\Engine\Debug\UnCamMgr.obj"
	-@erase ".\Engine\Debug\UnKeyVal.obj"
	-@erase ".\Engine\Debug\UnMem.obj"
	-@erase ".\Engine\Debug\Engine.res"
	-@erase ".\Lib\dEngine.lib"
	-@erase ".\Lib\dEngine.exp"
	-@erase ".\Lib\dEngine.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_ENGINE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_ENGINE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /c 
CPP_OBJS=.\Engine\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Engine.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Engine.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\dEngine.dll"
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/dEngine.pdb" /debug /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\dEngine.dll" /implib:"$(OUTDIR)/dEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnDeflts.obj" \
	"$(INTDIR)/UnFPoly.obj" \
	"$(INTDIR)/UnClass.obj" \
	"$(INTDIR)/UnActLst.obj" \
	"$(INTDIR)/UnParse.obj" \
	"$(INTDIR)/UnModel.obj" \
	"$(INTDIR)/UnChecks.obj" \
	"$(INTDIR)/UnCache.obj" \
	"$(INTDIR)/UnCheat.obj" \
	"$(INTDIR)/UnParams.obj" \
	"$(INTDIR)/UnFile.obj" \
	"$(INTDIR)/UnFGAud.obj" \
	"$(INTDIR)/UnEngine.obj" \
	"$(INTDIR)/UnLevel.obj" \
	"$(INTDIR)/UnSound.obj" \
	"$(INTDIR)/UnAction.obj" \
	"$(INTDIR)/UnInput.obj" \
	"$(INTDIR)/UnPrefer.obj" \
	"$(INTDIR)/UnLevAct.obj" \
	"$(INTDIR)/UnMesh.obj" \
	"$(INTDIR)/UnScrRTL.obj" \
	"$(INTDIR)/UnCamera.obj" \
	"$(INTDIR)/UnConfig.obj" \
	"$(INTDIR)/UnTex.obj" \
	"$(INTDIR)/UnActor.obj" \
	"$(INTDIR)/UnClsPrp.obj" \
	"$(INTDIR)/UnScript.obj" \
	"$(INTDIR)/UnLevTic.obj" \
	"$(INTDIR)/UnGfx.obj" \
	"$(INTDIR)/UnOutDev.obj" \
	"$(INTDIR)/UnRes.obj" \
	"$(INTDIR)/UnTopics.obj" \
	"$(INTDIR)/UnDynBSP.obj" \
	"$(INTDIR)/UnMath.obj" \
	"$(INTDIR)/UnPhys.obj" \
	"$(INTDIR)/UnServer.obj" \
	"$(INTDIR)/UnCamMgr.obj" \
	"$(INTDIR)/UnKeyVal.obj" \
	"$(INTDIR)/UnMem.obj" \
	"$(INTDIR)/Engine.res"

"$(OUTDIR)\dEngine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game\Release"
# PROP BASE Intermediate_Dir "Game\Release"
# PROP BASE Target_Dir "Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Game\Release"
# PROP Target_Dir "Game"
OUTDIR=.\Lib
INTDIR=.\Game\Release

ALL : "$(OUTDIR)\UnGame.dll"

CLEAN : 
	-@erase "..\System\UnGame.dll"
	-@erase ".\Game\Release\UnTrig.obj"
	-@erase ".\Game\Release\UnDecora.obj"
	-@erase ".\Game\Release\UnPawn.obj"
	-@erase ".\Game\Release\UnGCheat.obj"
	-@erase ".\Game\Release\UnPawns.obj"
	-@erase ".\Game\Release\UnExplos.obj"
	-@erase ".\Game\Release\UnMover.obj"
	-@erase ".\Game\Release\AiAnimat.obj"
	-@erase ".\Game\Release\UnRandom.obj"
	-@erase ".\Game\Release\UnWeapon.obj"
	-@erase ".\Game\Release\UnFActor.obj"
	-@erase ".\Game\Release\UnKeyPts.obj"
	-@erase ".\Game\Release\UnGame.obj"
	-@erase ".\Game\Release\UnCon.obj"
	-@erase ".\Game\Release\UnRoot.obj"
	-@erase ".\Game\Release\UnPyro.obj"
	-@erase ".\Game\Release\UnPlayer.obj"
	-@erase ".\Game\Release\UnThings.obj"
	-@erase ".\Game\Release\UnActPro.obj"
	-@erase ".\Game\Release\UnInv.obj"
	-@erase ".\Game\Release\Game.res"
	-@erase ".\Lib\UnGame.lib"
	-@erase ".\Lib\UnGame.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D "COMPILING_GAME" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_GAME" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Game.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /c 
CPP_OBJS=.\Game\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Game.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Game.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /machine:I386 /out:"..\System\UnGame.dll"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/UnGame.pdb"\
 /machine:I386 /out:"..\System\UnGame.dll" /implib:"$(OUTDIR)/UnGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnTrig.obj" \
	"$(INTDIR)/UnDecora.obj" \
	"$(INTDIR)/UnPawn.obj" \
	"$(INTDIR)/UnGCheat.obj" \
	"$(INTDIR)/UnPawns.obj" \
	"$(INTDIR)/UnExplos.obj" \
	"$(INTDIR)/UnMover.obj" \
	"$(INTDIR)/AiAnimat.obj" \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnWeapon.obj" \
	"$(INTDIR)/UnFActor.obj" \
	"$(INTDIR)/UnKeyPts.obj" \
	"$(INTDIR)/UnGame.obj" \
	"$(INTDIR)/UnCon.obj" \
	"$(INTDIR)/UnRoot.obj" \
	"$(INTDIR)/UnPyro.obj" \
	"$(INTDIR)/UnPlayer.obj" \
	"$(INTDIR)/UnThings.obj" \
	"$(INTDIR)/UnActPro.obj" \
	"$(INTDIR)/UnInv.obj" \
	"$(INTDIR)/Game.res"

"$(OUTDIR)\UnGame.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Game\Debug"
# PROP BASE Intermediate_Dir "Game\Debug"
# PROP BASE Target_Dir "Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Game\Debug"
# PROP Target_Dir "Game"
OUTDIR=.\Lib
INTDIR=.\Game\Debug

ALL : "$(OUTDIR)\dGame.dll"

CLEAN : 
	-@erase ".\Game\Debug\vc40.pdb"
	-@erase "..\System\dGame.dll"
	-@erase ".\Game\Debug\UnRandom.obj"
	-@erase ".\Game\Debug\UnTrig.obj"
	-@erase ".\Game\Debug\UnThings.obj"
	-@erase ".\Game\Debug\UnPawn.obj"
	-@erase ".\Game\Debug\UnPyro.obj"
	-@erase ".\Game\Debug\UnGCheat.obj"
	-@erase ".\Game\Debug\UnExplos.obj"
	-@erase ".\Game\Debug\UnPawns.obj"
	-@erase ".\Game\Debug\UnWeapon.obj"
	-@erase ".\Game\Debug\UnGame.obj"
	-@erase ".\Game\Debug\UnFActor.obj"
	-@erase ".\Game\Debug\UnMover.obj"
	-@erase ".\Game\Debug\UnKeyPts.obj"
	-@erase ".\Game\Debug\UnPlayer.obj"
	-@erase ".\Game\Debug\UnRoot.obj"
	-@erase ".\Game\Debug\UnInv.obj"
	-@erase ".\Game\Debug\UnActPro.obj"
	-@erase ".\Game\Debug\UnCon.obj"
	-@erase ".\Game\Debug\UnDecora.obj"
	-@erase ".\Game\Debug\AiAnimat.obj"
	-@erase ".\Game\Debug\Game.res"
	-@erase ".\Lib\dGame.lib"
	-@erase ".\Lib\dGame.exp"
	-@erase ".\Lib\dGame.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_GAME" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_GAME" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Game.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv-\
 /c 
CPP_OBJS=.\Game\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Game.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Game.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\System\dGame.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/dGame.pdb" /debug\
 /machine:I386 /out:"..\System\dGame.dll" /implib:"$(OUTDIR)/dGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnTrig.obj" \
	"$(INTDIR)/UnThings.obj" \
	"$(INTDIR)/UnPawn.obj" \
	"$(INTDIR)/UnPyro.obj" \
	"$(INTDIR)/UnGCheat.obj" \
	"$(INTDIR)/UnExplos.obj" \
	"$(INTDIR)/UnPawns.obj" \
	"$(INTDIR)/UnWeapon.obj" \
	"$(INTDIR)/UnGame.obj" \
	"$(INTDIR)/UnFActor.obj" \
	"$(INTDIR)/UnMover.obj" \
	"$(INTDIR)/UnKeyPts.obj" \
	"$(INTDIR)/UnPlayer.obj" \
	"$(INTDIR)/UnRoot.obj" \
	"$(INTDIR)/UnInv.obj" \
	"$(INTDIR)/UnActPro.obj" \
	"$(INTDIR)/UnCon.obj" \
	"$(INTDIR)/UnDecora.obj" \
	"$(INTDIR)/AiAnimat.obj" \
	"$(INTDIR)/Game.res"

"$(OUTDIR)\dGame.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Windows - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Windows\Release"
# PROP BASE Intermediate_Dir "Windows\Release"
# PROP BASE Target_Dir "UnWn"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Windows\Release"
# PROP Target_Dir "UnWn"
OUTDIR=.\Lib
INTDIR=.\Windows\Release

ALL : "$(OUTDIR)\UnServer.exe" "$(OUTDIR)\Unreal.tlb"

CLEAN : 
	-@erase "..\System\Unreal.tlb"
	-@erase "..\System\UnServer.exe"
	-@erase ".\Windows\Release\UnWnDevD.obj"
	-@erase ".\Windows\Release\UnWn.pch"
	-@erase ".\Windows\Release\UnPlatfm.obj"
	-@erase ".\Windows\Release\UnWnSenD.obj"
	-@erase ".\Windows\Release\UnWnKeyD.obj"
	-@erase ".\Windows\Release\StdAfx.obj"
	-@erase ".\Windows\Release\UnWnProp.obj"
	-@erase ".\Windows\Release\UnWnTime.obj"
	-@erase ".\Windows\Release\UnWn.obj"
	-@erase ".\Windows\Release\UnWnPref.obj"
	-@erase ".\Windows\Release\UnWnDlg.obj"
	-@erase ".\Windows\Release\UnWnCam.obj"
	-@erase ".\Windows\Release\UnWnAcCD.obj"
	-@erase ".\Windows\Release\UnTask.obj"
	-@erase ".\Windows\Release\UnSrv.obj"
	-@erase ".\Windows\Release\UnInputP.obj"
	-@erase ".\Windows\Release\UnWnActD.obj"
	-@erase ".\Windows\Release\UnRenDev.obj"
	-@erase ".\Windows\Release\UnWnEdSv.obj"
	-@erase ".\Windows\Release\UnWnAudD.obj"
	-@erase ".\Windows\Release\UnRage.obj"
	-@erase ".\Windows\Release\UnPswd.obj"
	-@erase ".\Windows\Release\UnWnSerD.obj"
	-@erase ".\Windows\Release\UnWn.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"StdAfx.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)/UnWn.pch" /Yu"StdAfx.h" /Fo"$(INTDIR)/" /QIfdiv- /c 
CPP_OBJS=.\Windows\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/UnWn.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnWn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /machine:I386 /out:"..\System\UnServer.exe"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib\
 Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/UnServer.pdb" /machine:I386\
 /out:"..\System\UnServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)/UnWnDevD.obj" \
	"$(INTDIR)/UnPlatfm.obj" \
	"$(INTDIR)/UnWnSenD.obj" \
	"$(INTDIR)/UnWnKeyD.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/UnWnProp.obj" \
	"$(INTDIR)/UnWnTime.obj" \
	"$(INTDIR)/UnWn.obj" \
	"$(INTDIR)/UnWnPref.obj" \
	"$(INTDIR)/UnWnDlg.obj" \
	"$(INTDIR)/UnWnCam.obj" \
	"$(INTDIR)/UnWnAcCD.obj" \
	"$(INTDIR)/UnTask.obj" \
	"$(INTDIR)/UnSrv.obj" \
	"$(INTDIR)/UnInputP.obj" \
	"$(INTDIR)/UnWnActD.obj" \
	"$(INTDIR)/UnRenDev.obj" \
	"$(INTDIR)/UnWnEdSv.obj" \
	"$(INTDIR)/UnWnAudD.obj" \
	"$(INTDIR)/UnRage.obj" \
	"$(INTDIR)/UnPswd.obj" \
	"$(INTDIR)/UnWnSerD.obj" \
	"$(INTDIR)/UnWn.res"

"$(OUTDIR)\UnServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Windows\Debug"
# PROP BASE Intermediate_Dir "Windows\Debug"
# PROP BASE Target_Dir "UnWn"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Windows\Debug"
# PROP Target_Dir "UnWn"
OUTDIR=.\Lib
INTDIR=.\Windows\Debug

ALL : "$(OUTDIR)\dServer.exe" "$(OUTDIR)\Unreal.tlb"

CLEAN : 
	-@erase ".\Windows\Debug\vc40.pdb"
	-@erase ".\Windows\Debug\UnWn.pch"
	-@erase "..\System\Unreal.tlb"
	-@erase "..\System\dServer.exe"
	-@erase ".\Windows\Debug\UnWnEdSv.obj"
	-@erase ".\Windows\Debug\StdAfx.obj"
	-@erase ".\Windows\Debug\UnWnAudD.obj"
	-@erase ".\Windows\Debug\UnWnSerD.obj"
	-@erase ".\Windows\Debug\UnWnDlg.obj"
	-@erase ".\Windows\Debug\UnWnDevD.obj"
	-@erase ".\Windows\Debug\UnWnCam.obj"
	-@erase ".\Windows\Debug\UnPlatfm.obj"
	-@erase ".\Windows\Debug\UnWnSenD.obj"
	-@erase ".\Windows\Debug\UnWnKeyD.obj"
	-@erase ".\Windows\Debug\UnRage.obj"
	-@erase ".\Windows\Debug\UnWnProp.obj"
	-@erase ".\Windows\Debug\UnWnTime.obj"
	-@erase ".\Windows\Debug\UnSrv.obj"
	-@erase ".\Windows\Debug\UnWnPref.obj"
	-@erase ".\Windows\Debug\UnWnAcCD.obj"
	-@erase ".\Windows\Debug\UnInputP.obj"
	-@erase ".\Windows\Debug\UnWnActD.obj"
	-@erase ".\Windows\Debug\UnWn.obj"
	-@erase ".\Windows\Debug\UnTask.obj"
	-@erase ".\Windows\Debug\UnRenDev.obj"
	-@erase ".\Windows\Debug\UnPswd.obj"
	-@erase ".\Windows\Debug\UnWn.res"
	-@erase ".\Lib\dServer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG" /Yu"StdAfx.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG"\
 /Fp"$(INTDIR)/UnWn.pch" /Yu"StdAfx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv-\
 /c 
CPP_OBJS=.\Windows\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/UnWn.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnWn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib\
 Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/dServer.pdb" /debug /machine:I386\
 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)/UnWnEdSv.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/UnWnAudD.obj" \
	"$(INTDIR)/UnWnSerD.obj" \
	"$(INTDIR)/UnWnDlg.obj" \
	"$(INTDIR)/UnWnDevD.obj" \
	"$(INTDIR)/UnWnCam.obj" \
	"$(INTDIR)/UnPlatfm.obj" \
	"$(INTDIR)/UnWnSenD.obj" \
	"$(INTDIR)/UnWnKeyD.obj" \
	"$(INTDIR)/UnRage.obj" \
	"$(INTDIR)/UnWnProp.obj" \
	"$(INTDIR)/UnWnTime.obj" \
	"$(INTDIR)/UnSrv.obj" \
	"$(INTDIR)/UnWnPref.obj" \
	"$(INTDIR)/UnWnAcCD.obj" \
	"$(INTDIR)/UnInputP.obj" \
	"$(INTDIR)/UnWnActD.obj" \
	"$(INTDIR)/UnWn.obj" \
	"$(INTDIR)/UnTask.obj" \
	"$(INTDIR)/UnRenDev.obj" \
	"$(INTDIR)/UnPswd.obj" \
	"$(INTDIR)/UnWn.res"

"$(OUTDIR)\dServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Documentation - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Doc\Release"
# PROP BASE Intermediate_Dir "Doc\Release"
# PROP BASE Target_Dir "Doc"
# PROP BASE Cmd_Line "NMAKE /f Doc.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Doc\Doc.exe"
# PROP BASE Bsc_Name "Doc\Doc.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Doc\Release"
# PROP Intermediate_Dir "Doc\Release"
# PROP Target_Dir "Doc"
# PROP Cmd_Line "NMAKE /f Doc.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Doc\Doc.exe"
# PROP Bsc_Name "Doc\Doc.bsc"
OUTDIR=.\Doc\Release
INTDIR=.\Doc\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "Network - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Network\Release"
# PROP BASE Intermediate_Dir "Network\Release"
# PROP BASE Target_Dir "Network"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Network\Release"
# PROP Target_Dir "Network"
OUTDIR=.\Lib
INTDIR=.\Network\Release

ALL : "$(OUTDIR)\UnNet.dll"

CLEAN : 
	-@erase "..\System\UnNet.dll"
	-@erase ".\Network\Release\NetTeln.obj"
	-@erase ".\Network\Release\NetMain.obj"
	-@erase ".\Network\Release\NetINet.obj"
	-@erase ".\Network\Release\NetWin.obj"
	-@erase ".\Network\Release\NetDPlay.obj"
	-@erase ".\Network\Release\Network.res"
	-@erase ".\Lib\UnNet.lib"
	-@erase ".\Lib\UnNet.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D "COMPILING_NETWORK" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"windows.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_NETWORK" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Network.pch" /YX"windows.h" /Fo"$(INTDIR)/" /QIfdiv- /c 
CPP_OBJS=.\Network\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Network.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Network.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:0x14000000 /subsystem:windows /dll /machine:I386 /out:"..\System\UnNet.dll"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib\
 /nologo /base:0x14000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/UnNet.pdb" /machine:I386 /out:"..\System\UnNet.dll"\
 /implib:"$(OUTDIR)/UnNet.lib" 
LINK32_OBJS= \
	"$(INTDIR)/NetTeln.obj" \
	"$(INTDIR)/NetMain.obj" \
	"$(INTDIR)/NetINet.obj" \
	"$(INTDIR)/NetWin.obj" \
	"$(INTDIR)/NetDPlay.obj" \
	"$(INTDIR)/Network.res"

"$(OUTDIR)\UnNet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Network\Debug"
# PROP BASE Intermediate_Dir "Network\Debug"
# PROP BASE Target_Dir "Network"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Network\Debug"
# PROP Target_Dir "Network"
OUTDIR=.\Lib
INTDIR=.\Network\Debug

ALL : "$(OUTDIR)\dNet.dll"

CLEAN : 
	-@erase ".\Network\Debug\vc40.pdb"
	-@erase "..\System\dNet.dll"
	-@erase ".\Network\Debug\NetWin.obj"
	-@erase ".\Network\Debug\NetTeln.obj"
	-@erase ".\Network\Debug\NetINet.obj"
	-@erase ".\Network\Debug\NetDPlay.obj"
	-@erase ".\Network\Debug\NetMain.obj"
	-@erase ".\Network\Debug\Network.res"
	-@erase ".\Lib\dNet.lib"
	-@erase ".\Lib\dNet.exp"
	-@erase ".\Lib\dNet.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_NETWORK" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX"windows.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_NETWORK" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Network.pch" /YX"windows.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /c 
CPP_OBJS=.\Network\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Network.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Network.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:0x14000000 /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\System\dNet.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib\
 /nologo /base:0x14000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/dNet.pdb" /debug /machine:I386 /out:"..\System\dNet.dll"\
 /implib:"$(OUTDIR)/dNet.lib" 
LINK32_OBJS= \
	"$(INTDIR)/NetWin.obj" \
	"$(INTDIR)/NetTeln.obj" \
	"$(INTDIR)/NetINet.obj" \
	"$(INTDIR)/NetDPlay.obj" \
	"$(INTDIR)/NetMain.obj" \
	"$(INTDIR)/Network.res"

"$(OUTDIR)\dNet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Editor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Editor\Release"
# PROP BASE Intermediate_Dir "Editor\Release"
# PROP BASE Target_Dir "UnrealEd"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Editor\Release"
# PROP Target_Dir "UnrealEd"
OUTDIR=.\Lib
INTDIR=.\Editor\Release

ALL : "$(OUTDIR)\UnEditor.dll"

CLEAN : 
	-@erase "..\System\UnEditor.dll"
	-@erase ".\Editor\Release\UnShadow.obj"
	-@erase ".\Editor\Release\UnEdCsg.obj"
	-@erase ".\Editor\Release\UnEdScan.obj"
	-@erase ".\Editor\Release\UnTexEd.obj"
	-@erase ".\Editor\Release\UnMeshEd.obj"
	-@erase ".\Editor\Release\UnEditor.obj"
	-@erase ".\Editor\Release\UnBsp.obj"
	-@erase ".\Editor\Release\UnEdSrv.obj"
	-@erase ".\Editor\Release\UnEdCnst.obj"
	-@erase ".\Editor\Release\UnZone.obj"
	-@erase ".\Editor\Release\UnEdAct.obj"
	-@erase ".\Editor\Release\UnEdTran.obj"
	-@erase ".\Editor\Release\UnEdCam.obj"
	-@erase ".\Editor\Release\UnEditor.res"
	-@erase ".\Lib\UnEditor.lib"
	-@erase ".\Lib\UnEditor.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D "NDEBUG" /D "COMPILING_EDITOR" /D "WIN32" /D "_WINDOWS" /YX /QIfdiv- /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "NDEBUG" /D "COMPILING_EDITOR" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/UnrealEd.pch" /YX /Fo"$(INTDIR)/" /QIfdiv- /QIfdiv- /c 
CPP_OBJS=.\Editor\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/UnEditor.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnrealEd.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /machine:I386 /out:"..\System\UnEditor.dll"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/UnEditor.pdb"\
 /machine:I386 /out:"..\System\UnEditor.dll" /implib:"$(OUTDIR)/UnEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnShadow.obj" \
	"$(INTDIR)/UnEdCsg.obj" \
	"$(INTDIR)/UnEdScan.obj" \
	"$(INTDIR)/UnTexEd.obj" \
	"$(INTDIR)/UnMeshEd.obj" \
	"$(INTDIR)/UnEditor.obj" \
	"$(INTDIR)/UnBsp.obj" \
	"$(INTDIR)/UnEdSrv.obj" \
	"$(INTDIR)/UnEdCnst.obj" \
	"$(INTDIR)/UnZone.obj" \
	"$(INTDIR)/UnEdAct.obj" \
	"$(INTDIR)/UnEdTran.obj" \
	"$(INTDIR)/UnEdCam.obj" \
	"$(INTDIR)/UnEditor.res"

"$(OUTDIR)\UnEditor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Editor\Debug"
# PROP BASE Intermediate_Dir "Editor\Debug"
# PROP BASE Target_Dir "UnrealEd"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Editor\Debug"
# PROP Target_Dir "UnrealEd"
OUTDIR=.\Lib
INTDIR=.\Editor\Debug

ALL : "$(OUTDIR)\dEditor.dll"

CLEAN : 
	-@erase ".\Editor\Debug\vc40.pdb"
	-@erase "..\System\dEditor.dll"
	-@erase ".\Editor\Debug\UnZone.obj"
	-@erase ".\Editor\Debug\UnShadow.obj"
	-@erase ".\Editor\Debug\UnBsp.obj"
	-@erase ".\Editor\Debug\UnEdScan.obj"
	-@erase ".\Editor\Debug\UnEdCam.obj"
	-@erase ".\Editor\Debug\UnEdTran.obj"
	-@erase ".\Editor\Debug\UnMeshEd.obj"
	-@erase ".\Editor\Debug\UnEditor.obj"
	-@erase ".\Editor\Debug\UnEdCsg.obj"
	-@erase ".\Editor\Debug\UnTexEd.obj"
	-@erase ".\Editor\Debug\UnEdCnst.obj"
	-@erase ".\Editor\Debug\UnEdAct.obj"
	-@erase ".\Editor\Debug\UnEdSrv.obj"
	-@erase ".\Editor\Debug\UnEditor.res"
	-@erase ".\Lib\dEditor.lib"
	-@erase ".\Lib\dEditor.exp"
	-@erase ".\Lib\dEditor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_EDITOR" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_EDITOR" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/UnrealEd.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /c 
CPP_OBJS=.\Editor\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/UnEditor.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnrealEd.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\System\dEditor.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/dEditor.pdb" /debug\
 /machine:I386 /out:"..\System\dEditor.dll" /implib:"$(OUTDIR)/dEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnZone.obj" \
	"$(INTDIR)/UnShadow.obj" \
	"$(INTDIR)/UnBsp.obj" \
	"$(INTDIR)/UnEdScan.obj" \
	"$(INTDIR)/UnEdCam.obj" \
	"$(INTDIR)/UnEdTran.obj" \
	"$(INTDIR)/UnMeshEd.obj" \
	"$(INTDIR)/UnEditor.obj" \
	"$(INTDIR)/UnEdCsg.obj" \
	"$(INTDIR)/UnTexEd.obj" \
	"$(INTDIR)/UnEdCnst.obj" \
	"$(INTDIR)/UnEdAct.obj" \
	"$(INTDIR)/UnEdSrv.obj" \
	"$(INTDIR)/UnEditor.res"

"$(OUTDIR)\dEditor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Render - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Render\Render__"
# PROP BASE Intermediate_Dir "Render\Render__"
# PROP BASE Target_Dir "Render"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Render\Release"
# PROP Target_Dir "Render"
OUTDIR=.\Lib
INTDIR=.\Render\Release

ALL : "$(OUTDIR)\UnRender.dll"

CLEAN : 
	-@erase "..\System\UnRender.dll"
	-@erase ".\Render\Release\UnEdRend.obj"
	-@erase ".\Render\Release\UnRender.obj"
	-@erase ".\Render\Release\UnBlastC.obj"
	-@erase ".\Render\Release\UnTMapC.obj"
	-@erase ".\Render\Release\UnPalTbl.obj"
	-@erase ".\Render\Release\UnTest.obj"
	-@erase ".\Render\Release\UnSprite.obj"
	-@erase ".\Render\Release\UnRaster.obj"
	-@erase ".\Render\Release\UnEdge.obj"
	-@erase ".\Render\Release\UnRandom.obj"
	-@erase ".\Render\Release\UnDither.obj"
	-@erase ".\Render\Release\UnLight.obj"
	-@erase ".\Render\Release\UnSpan.obj"
	-@erase ".\Render\Release\Render.res"
	-@erase ".\Render\Release\UnBlast.obj"
	-@erase ".\Render\Release\UnSpanX.obj"
	-@erase ".\Lib\UnRender.lib"
	-@erase ".\Lib\UnRender.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /vd0 /GX /Ox /Ot /Oa /Og /Oi /Ob2 /I "$(InputPath)\..\Inc" /D "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /vd0 /GX /Ox /Ot /Oa /Og /Oi /Ob2 /I\
 "$(InputPath)\..\Inc" /D "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv-\
 /QIfdiv- /c 
CPP_OBJS=.\Render\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Render.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Render.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /machine:I386 /out:"..\System\UnRender.dll"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug
LINK32_FLAGS=Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/UnRender.pdb" /machine:I386 /out:"..\System\UnRender.dll"\
 /implib:"$(OUTDIR)/UnRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnEdRend.obj" \
	"$(INTDIR)/UnRender.obj" \
	"$(INTDIR)/UnBlastC.obj" \
	"$(INTDIR)/UnTMapC.obj" \
	"$(INTDIR)/UnPalTbl.obj" \
	"$(INTDIR)/UnTest.obj" \
	"$(INTDIR)/UnSprite.obj" \
	"$(INTDIR)/UnRaster.obj" \
	"$(INTDIR)/UnEdge.obj" \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnDither.obj" \
	"$(INTDIR)/UnLight.obj" \
	"$(INTDIR)/UnSpan.obj" \
	"$(INTDIR)/UnBlast.obj" \
	"$(INTDIR)/UnSpanX.obj" \
	"$(INTDIR)/Render.res"

"$(OUTDIR)\UnRender.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Render\Render__"
# PROP BASE Intermediate_Dir "Render\Render__"
# PROP BASE Target_Dir "Render"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Render\Debug"
# PROP Target_Dir "Render"
OUTDIR=.\Lib
INTDIR=.\Render\Debug

ALL : "$(OUTDIR)\dRender.dll"

CLEAN : 
	-@erase ".\Render\Debug\vc40.pdb"
	-@erase "..\System\dRender.dll"
	-@erase ".\Render\Debug\UnRaster.obj"
	-@erase ".\Render\Debug\UnTest.obj"
	-@erase ".\Render\Debug\UnLight.obj"
	-@erase ".\Render\Debug\UnEdge.obj"
	-@erase ".\Render\Debug\UnRender.obj"
	-@erase ".\Render\Debug\UnDither.obj"
	-@erase ".\Render\Debug\UnPalTbl.obj"
	-@erase ".\Render\Debug\UnSpan.obj"
	-@erase ".\Render\Debug\UnTMapC.obj"
	-@erase ".\Render\Debug\UnEdRend.obj"
	-@erase ".\Render\Debug\UnRandom.obj"
	-@erase ".\Render\Debug\UnBlastC.obj"
	-@erase ".\Render\Debug\UnSprite.obj"
	-@erase ".\Render\Debug\Render.res"
	-@erase ".\Render\Debug\UnBlast.obj"
	-@erase ".\Render\Debug\UnSpanX.obj"
	-@erase ".\Lib\dRender.lib"
	-@erase ".\Lib\dRender.exp"
	-@erase ".\Lib\dRender.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /c
CPP_PROJ=/nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /c 
CPP_OBJS=.\Render\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Render.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Render.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\System\dRender.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/dRender.pdb" /debug /machine:I386 /out:"..\System\dRender.dll"\
 /implib:"$(OUTDIR)/dRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnRaster.obj" \
	"$(INTDIR)/UnTest.obj" \
	"$(INTDIR)/UnLight.obj" \
	"$(INTDIR)/UnEdge.obj" \
	"$(INTDIR)/UnRender.obj" \
	"$(INTDIR)/UnDither.obj" \
	"$(INTDIR)/UnPalTbl.obj" \
	"$(INTDIR)/UnSpan.obj" \
	"$(INTDIR)/UnTMapC.obj" \
	"$(INTDIR)/UnEdRend.obj" \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnBlastC.obj" \
	"$(INTDIR)/UnSprite.obj" \
	"$(INTDIR)/UnBlast.obj" \
	"$(INTDIR)/UnSpanX.obj" \
	"$(INTDIR)/Render.res"

"$(OUTDIR)\dRender.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Classes - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Classes\Release"
# PROP BASE Intermediate_Dir "Classes\Release"
# PROP BASE Target_Dir "Classes"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Classes\Release"
# PROP Intermediate_Dir "Classes\Release"
# PROP Target_Dir "Classes"
OUTDIR=.\Classes\Release
INTDIR=.\Classes\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Classes.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Classes\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Classes.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Classes.pdb" /machine:I386 /out:"$(OUTDIR)/Classes.exe" 
LINK32_OBJS=

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Classes\Debug"
# PROP BASE Intermediate_Dir "Classes\Debug"
# PROP BASE Target_Dir "Classes"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Classes\Debug"
# PROP Intermediate_Dir "Classes\Debug"
# PROP Target_Dir "Classes"
OUTDIR=.\Classes\Debug
INTDIR=.\Classes\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Classes.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Classes\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Classes.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Classes.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Classes.exe" 
LINK32_OBJS=

!ENDIF 

################################################################################
# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"

!IF  "$(CFG)" == "Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Engine\UnClass.cpp
DEP_CPP_UNCLA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnClass.obj" : $(SOURCE) $(DEP_CPP_UNCLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnFile.cpp
DEP_CPP_UNFIL=\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	

"$(INTDIR)\UnFile.obj" : $(SOURCE) $(DEP_CPP_UNFIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnGfx.cpp
DEP_CPP_UNGFX=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnGfx.obj" : $(SOURCE) $(DEP_CPP_UNGFX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnLevel.cpp
DEP_CPP_UNLEV=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnLevel.obj" : $(SOURCE) $(DEP_CPP_UNLEV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnMath.cpp
DEP_CPP_UNMAT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMath.obj" : $(SOURCE) $(DEP_CPP_UNMAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnMem.cpp
DEP_CPP_UNMEM=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMem.obj" : $(SOURCE) $(DEP_CPP_UNMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnMesh.cpp
DEP_CPP_UNMES=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMesh.obj" : $(SOURCE) $(DEP_CPP_UNMES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnParams.cpp
DEP_CPP_UNPAR=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnParams.obj" : $(SOURCE) $(DEP_CPP_UNPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnPhys.cpp
DEP_CPP_UNPHY=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPhys.obj" : $(SOURCE) $(DEP_CPP_UNPHY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnRes.cpp
DEP_CPP_UNRES=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRes.obj" : $(SOURCE) $(DEP_CPP_UNRES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnServer.cpp
DEP_CPP_UNSER=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnServer.obj" : $(SOURCE) $(DEP_CPP_UNSER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnTex.cpp
DEP_CPP_UNTEX=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTex.obj" : $(SOURCE) $(DEP_CPP_UNTEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnTopics.cpp
DEP_CPP_UNTOP=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTopics.obj" : $(SOURCE) $(DEP_CPP_UNTOP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnModel.cpp
DEP_CPP_UNMOD=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnModel.obj" : $(SOURCE) $(DEP_CPP_UNMOD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnInput.cpp
DEP_CPP_UNINP=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnInput.obj" : $(SOURCE) $(DEP_CPP_UNINP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnFPoly.cpp
DEP_CPP_UNFPO=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnFPoly.obj" : $(SOURCE) $(DEP_CPP_UNFPO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnCache.cpp
DEP_CPP_UNCAC=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnCache.obj" : $(SOURCE) $(DEP_CPP_UNCAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnCamera.cpp
DEP_CPP_UNCAM=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnCamera.obj" : $(SOURCE) $(DEP_CPP_UNCAM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnClsPrp.cpp
DEP_CPP_UNCLS=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScrEng.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnClsPrp.obj" : $(SOURCE) $(DEP_CPP_UNCLS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnChecks.cpp
DEP_CPP_UNCHE=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnChecks.obj" : $(SOURCE) $(DEP_CPP_UNCHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnLevAct.cpp
DEP_CPP_UNLEVA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnLevAct.obj" : $(SOURCE) $(DEP_CPP_UNLEVA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnLevTic.cpp
DEP_CPP_UNLEVT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnLevTic.obj" : $(SOURCE) $(DEP_CPP_UNLEVT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnDeflts.cpp
DEP_CPP_UNDEF=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnDeflts.obj" : $(SOURCE) $(DEP_CPP_UNDEF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"

!IF  "$(CFG)" == "Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnScrRTL.cpp
DEP_CPP_UNSCR=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnScrRTL.obj" : $(SOURCE) $(DEP_CPP_UNSCR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnDynBSP.cpp
DEP_CPP_UNDYN=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnDynBSP.obj" : $(SOURCE) $(DEP_CPP_UNDYN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnCamMgr.cpp
DEP_CPP_UNCAMM=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnCamMgr.obj" : $(SOURCE) $(DEP_CPP_UNCAMM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\Engine.rc
DEP_RSC_ENGIN=\
	".\Render\..\Res\Unreal.ico"\
	

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\Engine.res" : $(SOURCE) $(DEP_RSC_ENGIN) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Engine.res" /i "Engine" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\Engine.res" : $(SOURCE) $(DEP_RSC_ENGIN) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Engine.res" /i "Engine" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnScript.cpp
DEP_CPP_UNSCRI=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScrEng.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

!IF  "$(CFG)" == "Engine - Win32 Release"

# ADD CPP /GX-

"$(INTDIR)\UnScript.obj" : $(SOURCE) $(DEP_CPP_UNSCRI) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W4 /WX /GX- /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\UnScript.obj" : $(SOURCE) $(DEP_CPP_UNSCRI) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_ENGINE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnActLst.cpp
DEP_CPP_UNACT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnActLst.obj" : $(SOURCE) $(DEP_CPP_UNACT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnEngine.cpp
DEP_CPP_UNENG=\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEngine.obj" : $(SOURCE) $(DEP_CPP_UNENG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnAction.cpp
DEP_CPP_UNACTI=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnAction.obj" : $(SOURCE) $(DEP_CPP_UNACTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnParse.cpp
DEP_CPP_UNPARS=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnParse.obj" : $(SOURCE) $(DEP_CPP_UNPARS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnKeyVal.cpp
DEP_CPP_UNKEY=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnKeyVal.obj" : $(SOURCE) $(DEP_CPP_UNKEY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnPrefer.cpp
DEP_CPP_UNPRE=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPrefer.obj" : $(SOURCE) $(DEP_CPP_UNPRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnConfig.cpp
DEP_CPP_UNCON=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnConfig.obj" : $(SOURCE) $(DEP_CPP_UNCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnCheat.cpp
DEP_CPP_UNCHEA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnCheat.obj" : $(SOURCE) $(DEP_CPP_UNCHEA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnOutDev.cpp
DEP_CPP_UNOUT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnOutDev.obj" : $(SOURCE) $(DEP_CPP_UNOUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnSound.cpp
DEP_CPP_UNSOU=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\SoundEng.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnSound.obj" : $(SOURCE) $(DEP_CPP_UNSOU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnFGAud.cpp
DEP_CPP_UNFGA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\SoundEng.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnFGAud.obj" : $(SOURCE) $(DEP_CPP_UNFGA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnActor.cpp
DEP_CPP_UNACTO=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnActor.obj" : $(SOURCE) $(DEP_CPP_UNACTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"

!IF  "$(CFG)" == "Game - Win32 Release"

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"

!IF  "$(CFG)" == "Game - Win32 Release"

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnWeapon.cpp
DEP_CPP_UNWEA=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnWeapon.obj" : $(SOURCE) $(DEP_CPP_UNWEA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\AiAnimat.cpp
DEP_CPP_AIANI=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\AiAnimat.obj" : $(SOURCE) $(DEP_CPP_AIANI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnActPro.cpp
DEP_CPP_UNACTP=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnActPro.obj" : $(SOURCE) $(DEP_CPP_UNACTP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnCon.cpp
DEP_CPP_UNCON_=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnCon.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnCon.obj" : $(SOURCE) $(DEP_CPP_UNCON_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnGame.cpp
DEP_CPP_UNGAM=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnCon.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnGame.obj" : $(SOURCE) $(DEP_CPP_UNGAM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnInv.cpp
DEP_CPP_UNINV=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnInv.obj" : $(SOURCE) $(DEP_CPP_UNINV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnKeyPts.cpp
DEP_CPP_UNKEYP=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnKeyPts.obj" : $(SOURCE) $(DEP_CPP_UNKEYP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnMover.cpp
DEP_CPP_UNMOV=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMover.obj" : $(SOURCE) $(DEP_CPP_UNMOV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnPawns.cpp
DEP_CPP_UNPAW=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPawns.obj" : $(SOURCE) $(DEP_CPP_UNPAW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnPlayer.cpp
DEP_CPP_UNPLA=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPlayer.obj" : $(SOURCE) $(DEP_CPP_UNPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnRoot.cpp
DEP_CPP_UNROO=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRoot.obj" : $(SOURCE) $(DEP_CPP_UNROO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnThings.cpp
DEP_CPP_UNTHI=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnThings.obj" : $(SOURCE) $(DEP_CPP_UNTHI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\Game.rc
DEP_RSC_GAME_=\
	".\Render\..\Res\Unreal.ico"\
	

!IF  "$(CFG)" == "Game - Win32 Release"


"$(INTDIR)\Game.res" : $(SOURCE) $(DEP_RSC_GAME_) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Game.res" /i "Game" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Game - Win32 Debug"


"$(INTDIR)\Game.res" : $(SOURCE) $(DEP_RSC_GAME_) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Game.res" /i "Game" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnRandom.cpp
DEP_CPP_UNRAN=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRandom.obj" : $(SOURCE) $(DEP_CPP_UNRAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnPawn.cpp
DEP_CPP_UNPAWN=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPawn.obj" : $(SOURCE) $(DEP_CPP_UNPAWN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnPyro.cpp
DEP_CPP_UNPYR=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPyro.obj" : $(SOURCE) $(DEP_CPP_UNPYR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnFActor.cpp
DEP_CPP_UNFAC=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRandom.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnFActor.obj" : $(SOURCE) $(DEP_CPP_UNFAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnTrig.cpp
DEP_CPP_UNTRI=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTrig.obj" : $(SOURCE) $(DEP_CPP_UNTRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnDecora.cpp
DEP_CPP_UNDEC=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnDecora.obj" : $(SOURCE) $(DEP_CPP_UNDEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnGCheat.cpp
DEP_CPP_UNGCH=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnCheat.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnGCheat.obj" : $(SOURCE) $(DEP_CPP_UNGCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnExplos.cpp
DEP_CPP_UNEXP=\
	".\$(InputPath)\..\Inc\AIAnimat.h"\
	".\$(InputPath)\..\Inc\AImotion.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Game\UnFActor.h"\
	".\Game\UnGame.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnExplos.obj" : $(SOURCE) $(DEP_CPP_UNEXP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Windows - Win32 Release"
# Name "Windows - Win32 Debug"

!IF  "$(CFG)" == "Windows - Win32 Release"

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"

!IF  "$(CFG)" == "Windows - Win32 Release"

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnTime.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNT=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnWnTime.obj" : $(SOURCE) $(DEP_CPP_UNWNT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnTime.obj" : $(SOURCE) $(DEP_CPP_UNWNT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnPlatfm.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNPLAT=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnPswd.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnPlatfm.obj" : $(SOURCE) $(DEP_CPP_UNPLAT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNPLAT=\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnPswd.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnPlatfm.obj" : $(SOURCE) $(DEP_CPP_UNPLAT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnPswd.cpp
DEP_CPP_UNPSW=\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnPswd.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnPswd.obj" : $(SOURCE) $(DEP_CPP_UNPSW) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnRage.cpp
DEP_CPP_UNRAG=\
	".\Windows\StdAfx.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\Windows\UnWnCam.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\Windows\Ati3dCif.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	
NODEP_CPP_UNRAG=\
	".\Windows\afxdos.h"\
	".\Windows\i3debug.h"\
	

"$(INTDIR)\UnRage.obj" : $(SOURCE) $(DEP_CPP_UNRAG) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnRenDev.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNREN=\
	".\Windows\StdAfx.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRenDev.obj" : $(SOURCE) $(DEP_CPP_UNREN) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNREN=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	

"$(INTDIR)\UnRenDev.obj" : $(SOURCE) $(DEP_CPP_UNREN) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnTask.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNTAS=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\Windows\UnWnCam.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	

"$(INTDIR)\UnTask.obj" : $(SOURCE) $(DEP_CPP_UNTAS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNTAS=\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnTask.obj" : $(SOURCE) $(DEP_CPP_UNTAS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWn.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWN_=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\Windows\UnWnCam.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\Windows\UnWnProp.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	

"$(INTDIR)\UnWn.obj" : $(SOURCE) $(DEP_CPP_UNWN_) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWN_=\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnProp.h"\
	

"$(INTDIR)\UnWn.obj" : $(SOURCE) $(DEP_CPP_UNWN_) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnCam.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNC=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\Windows\UnWnCam.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\DMouse.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnCam.obj" : $(SOURCE) $(DEP_CPP_UNWNC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNC=\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\DMouse.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnCam.obj" : $(SOURCE) $(DEP_CPP_UNWNC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnDlg.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWND=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\Windows\UnWnCam.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	

"$(INTDIR)\UnWnDlg.obj" : $(SOURCE) $(DEP_CPP_UNWND) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWND=\
	".\$(InputPath)\..\Inc\DDraw1.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnCam.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnDlg.obj" : $(SOURCE) $(DEP_CPP_UNWND) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnEdSv.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNE=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnWnEdSv.obj" : $(SOURCE) $(DEP_CPP_UNWNE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNE=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnEdSv.obj" : $(SOURCE) $(DEP_CPP_UNWNE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\StdAfx.cpp
DEP_CPP_STDAF=\
	".\Windows\StdAfx.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	

!IF  "$(CFG)" == "Windows - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /G5 /MD /W4 /WX /GX /O2 /Ob2 /I "$(InputPath)\..\Inc" /D\
 "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)/UnWn.pch" /Yc /Fo"$(INTDIR)/" /QIfdiv- /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UnWn.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc" /D\
 "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG"\
 /Fp"$(INTDIR)/UnWn.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv- /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UnWn.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWn.rc
DEP_RSC_UNWN_R=\
	".\Render\..\Res\Unreal.ico"\
	".\Windows\CURSORS\AddActor.cur"\
	".\Windows\CURSORS\BrushFr.cur"\
	".\Windows\CURSORS\BrushMov.cur"\
	".\Windows\CURSORS\BrushRot.cur"\
	".\Windows\CURSORS\BrushSca.cur"\
	".\Windows\CURSORS\BrushShr.cur"\
	".\Windows\CURSORS\BrushSna.cur"\
	".\Windows\CURSORS\BrushStr.cur"\
	".\Windows\CURSORS\BrushWrp.cur"\
	".\Windows\CURSORS\CamZoom.cur"\
	".\Windows\CURSORS\MoveAct.cur"\
	".\Windows\CURSORS\SelActor.cur"\
	".\Windows\CURSORS\SelectPo.cur"\
	".\Windows\CURSORS\TerraFor.cur"\
	".\Windows\CURSORS\TexGrab.cur"\
	".\Windows\CURSORS\TexPan.cur"\
	".\Windows\CURSORS\TexRot.cur"\
	".\Windows\CURSORS\TexScale.cur"\
	".\Windows\CURSORS\TexSet.cur"\
	".\Windows\RES\UnSplash.bmp"\
	".\Windows\RES\UnWn.rc2"\
	

!IF  "$(CFG)" == "Windows - Win32 Release"


"$(INTDIR)\UnWn.res" : $(SOURCE) $(DEP_RSC_UNWN_R) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/UnWn.res" /i "Windows" /i "Lib" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"


"$(INTDIR)\UnWn.res" : $(SOURCE) $(DEP_RSC_UNWN_R) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/UnWn.res" /i "Windows" /i "Lib" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWn.odl

!IF  "$(CFG)" == "Windows - Win32 Release"

# ADD MTL /tlb "../System/Unreal.tlb"

"$(OUTDIR)\Unreal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "../System/Unreal.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# ADD MTL /tlb "../System/Unreal.tlb"

"$(OUTDIR)\Unreal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "../System/Unreal.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnActD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNA=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnActD.h"\
	".\Windows\UnWnAccD.h"\
	".\Windows\UnWnKeyD.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnActD.obj" : $(SOURCE) $(DEP_CPP_UNWNA) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnAccD.h"\
	".\Windows\UnWnActD.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnKeyD.h"\
	

"$(INTDIR)\UnWnActD.obj" : $(SOURCE) $(DEP_CPP_UNWNA) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnDevD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNDE=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDevD.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnDevD.obj" : $(SOURCE) $(DEP_CPP_UNWNDE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNDE=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDevD.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnDevD.obj" : $(SOURCE) $(DEP_CPP_UNWNDE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnPref.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNP=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnPref.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnPref.obj" : $(SOURCE) $(DEP_CPP_UNWNP) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNP=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnPref.h"\
	

"$(INTDIR)\UnWnPref.obj" : $(SOURCE) $(DEP_CPP_UNWNP) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnSenD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNS=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnSend.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnSenD.obj" : $(SOURCE) $(DEP_CPP_UNWNS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNS=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnSend.h"\
	

"$(INTDIR)\UnWnSenD.obj" : $(SOURCE) $(DEP_CPP_UNWNS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnSerD.cpp
DEP_CPP_UNWNSE=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnSerD.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnSerD.obj" : $(SOURCE) $(DEP_CPP_UNWNSE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAudD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNAU=\
	".\Windows\StdAfx.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnAudD.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnAudD.obj" : $(SOURCE) $(DEP_CPP_UNWNAU) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNAU=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnAudD.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnAudD.obj" : $(SOURCE) $(DEP_CPP_UNWNAU) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnKeyD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNK=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnKeyD.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnWnKeyD.obj" : $(SOURCE) $(DEP_CPP_UNWNK) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNK=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnKeyD.h"\
	

"$(INTDIR)\UnWnKeyD.obj" : $(SOURCE) $(DEP_CPP_UNWNK) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnProp.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNPR=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnProp.h"\
	".\Windows\UnWnPref.h"\
	".\Windows\UnWnActD.h"\
	".\Windows\UnWnSend.h"\
	".\Windows\UnWnDevD.h"\
	".\Windows\UnWnAudD.h"\
	".\Windows\UnWnSerD.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	

"$(INTDIR)\UnWnProp.obj" : $(SOURCE) $(DEP_CPP_UNWNPR) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNPR=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnAction.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnConfig.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInput.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnKeyVal.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnParse.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPrefer.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnActD.h"\
	".\Windows\UnWnAudD.h"\
	".\Windows\UnWnDevD.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnPref.h"\
	".\Windows\UnWnProp.h"\
	".\Windows\UnWnSend.h"\
	".\Windows\UnWnSerD.h"\
	

"$(INTDIR)\UnWnProp.obj" : $(SOURCE) $(DEP_CPP_UNWNPR) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAcCD.cpp
DEP_CPP_UNWNAC=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnAccD.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnAcCD.obj" : $(SOURCE) $(DEP_CPP_UNWNAC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnInputP.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNINPU=\
	".\Windows\StdAfx.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnInputP.obj" : $(SOURCE) $(DEP_CPP_UNINPU) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNINPU=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnInputP.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	

"$(INTDIR)\UnInputP.obj" : $(SOURCE) $(DEP_CPP_UNINPU) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnSrv.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNSRV=\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnSrv.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnSrv.obj" : $(SOURCE) $(DEP_CPP_UNSRV) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNSRV=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnSrv.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnSrv.obj" : $(SOURCE) $(DEP_CPP_UNSRV) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Documentation - Win32 Release"

!IF  "$(CFG)" == "Documentation - Win32 Release"

"$(OUTDIR)\Doc.exe" : 
   CD U:\Unreal\Src\Doc
   NMAKE /f Doc.mak

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Doc\TimOld.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\Amff.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\Galaxy.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\ProjMan.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\Script.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\T3d.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\TimLog.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\TimNotes.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc\Console.txt
# End Source File
# End Target
################################################################################
# Begin Target

# Name "Network - Win32 Release"
# Name "Network - Win32 Debug"

!IF  "$(CFG)" == "Network - Win32 Release"

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Network\NetMain.cpp
DEP_CPP_NETMA=\
	".\Network\DPlay.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\Windows\..\Network\NetPrv.h"\
	".\Network\NetINet.h"\
	".\Network\NetDPlay.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	

"$(INTDIR)\NetMain.obj" : $(SOURCE) $(DEP_CPP_NETMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetINet.cpp
DEP_CPP_NETIN=\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\Network\NetINet.h"\
	".\Windows\..\Network\NetPrv.h"\
	

"$(INTDIR)\NetINet.obj" : $(SOURCE) $(DEP_CPP_NETIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetDPlay.cpp
DEP_CPP_NETDP=\
	".\Network\DPlay.h"\
	".\$(InputPath)\..\Inc\Net.h"\
	".\Windows\..\Network\NetPrv.h"\
	".\Network\NetDPlay.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	

"$(INTDIR)\NetDPlay.obj" : $(SOURCE) $(DEP_CPP_NETDP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\Network.rc
DEP_RSC_NETWO=\
	".\Network\Wizard.bmp"\
	".\Render\..\Res\Unreal.ico"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


"$(INTDIR)\Network.res" : $(SOURCE) $(DEP_RSC_NETWO) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Network.res" /i "Network" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\Network.res" : $(SOURCE) $(DEP_RSC_NETWO) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Network.res" /i "Network" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetWin.cpp
DEP_CPP_NETWI=\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	

"$(INTDIR)\NetWin.obj" : $(SOURCE) $(DEP_CPP_NETWI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"

!IF  "$(CFG)" == "Network - Win32 Release"

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetTeln.cpp
DEP_CPP_NETTE=\
	".\$(InputPath)\..\Inc\Net.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\Network\NetINet.h"\
	".\Windows\..\Network\NetPrv.h"\
	

"$(INTDIR)\NetTeln.obj" : $(SOURCE) $(DEP_CPP_NETTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Editor - Win32 Release"
# Name "Editor - Win32 Debug"

!IF  "$(CFG)" == "Editor - Win32 Release"

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Editor\UnEditor.rc
DEP_RSC_UNEDI=\
	".\Render\..\Res\Unreal.ico"\
	

!IF  "$(CFG)" == "Editor - Win32 Release"


"$(INTDIR)\UnEditor.res" : $(SOURCE) $(DEP_RSC_UNEDI) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/UnEditor.res" /i "Editor" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"


"$(INTDIR)\UnEditor.res" : $(SOURCE) $(DEP_RSC_UNEDI) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/UnEditor.res" /i "Editor" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnZone.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNZON=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnZone.obj" : $(SOURCE) $(DEP_CPP_UNZON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNZON=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnZone.obj" : $(SOURCE) $(DEP_CPP_UNZON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdAct.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDA=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEdAct.obj" : $(SOURCE) $(DEP_CPP_UNEDA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdAct.obj" : $(SOURCE) $(DEP_CPP_UNEDA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCsg.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDC=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEdCsg.obj" : $(SOURCE) $(DEP_CPP_UNEDC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDC=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCsg.obj" : $(SOURCE) $(DEP_CPP_UNEDC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnBsp.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNBSP=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnBsp.obj" : $(SOURCE) $(DEP_CPP_UNBSP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNBSP=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnBsp.obj" : $(SOURCE) $(DEP_CPP_UNBSP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCnst.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDCN=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEdCnst.obj" : $(SOURCE) $(DEP_CPP_UNEDCN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDCN=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCnst.obj" : $(SOURCE) $(DEP_CPP_UNEDCN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdSrv.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDS=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnEdSrv.obj" : $(SOURCE) $(DEP_CPP_UNEDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDS=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdSrv.obj" : $(SOURCE) $(DEP_CPP_UNEDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdTran.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDT=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEdTran.obj" : $(SOURCE) $(DEP_CPP_UNEDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdTran.obj" : $(SOURCE) $(DEP_CPP_UNEDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnShadow.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNSHA=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnShadow.obj" : $(SOURCE) $(DEP_CPP_UNSHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNSHA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnDynBsp.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnShadow.obj" : $(SOURCE) $(DEP_CPP_UNSHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEditor.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDIT=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEditor.obj" : $(SOURCE) $(DEP_CPP_UNEDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDIT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEditor.obj" : $(SOURCE) $(DEP_CPP_UNEDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdScan.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDSC=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnEdScan.obj" : $(SOURCE) $(DEP_CPP_UNEDSC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDSC=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdScan.obj" : $(SOURCE) $(DEP_CPP_UNEDSC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCam.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNEDCA=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnEdCam.obj" : $(SOURCE) $(DEP_CPP_UNEDCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNEDCA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCam.obj" : $(SOURCE) $(DEP_CPP_UNEDCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnMeshEd.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNMESH=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnMeshEd.obj" : $(SOURCE) $(DEP_CPP_UNMESH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNMESH=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMeshEd.obj" : $(SOURCE) $(DEP_CPP_UNMESH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnTexEd.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNTEXE=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	

"$(INTDIR)\UnTexEd.obj" : $(SOURCE) $(DEP_CPP_UNTEXE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNTEXE=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTexEd.obj" : $(SOURCE) $(DEP_CPP_UNTEXE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"

!IF  "$(CFG)" == "Editor - Win32 Release"

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Render - Win32 Release"
# Name "Render - Win32 Debug"

!IF  "$(CFG)" == "Render - Win32 Release"

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Render\UnTMapC.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNTMA=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnTMapC.obj" : $(SOURCE) $(DEP_CPP_UNTMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNTMA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTMapC.obj" : $(SOURCE) $(DEP_CPP_UNTMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnEdRend.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNEDR=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\Render\UnLine1.cpp"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	".\Render\UnLine.cpp"\
	

"$(INTDIR)\UnEdRend.obj" : $(SOURCE) $(DEP_CPP_UNEDR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNEDR=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Render\UnLine.cpp"\
	".\Render\UnLine1.cpp"\
	

"$(INTDIR)\UnEdRend.obj" : $(SOURCE) $(DEP_CPP_UNEDR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRaster.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNRAS=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRaster.obj" : $(SOURCE) $(DEP_CPP_UNRAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNRAS=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRaster.obj" : $(SOURCE) $(DEP_CPP_UNRAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRender.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNREND=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRender.obj" : $(SOURCE) $(DEP_CPP_UNREND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNREND=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRenDev.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRender.obj" : $(SOURCE) $(DEP_CPP_UNREND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSpan.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNSPA=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnSpan.obj" : $(SOURCE) $(DEP_CPP_UNSPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNSPA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnSpan.obj" : $(SOURCE) $(DEP_CPP_UNSPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSprite.cpp
DEP_CPP_UNSPR=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnSprite.obj" : $(SOURCE) $(DEP_CPP_UNSPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\Render.rc
DEP_RSC_RENDE=\
	".\Render\..\Res\Unreal.ico"\
	

!IF  "$(CFG)" == "Render - Win32 Release"


"$(INTDIR)\Render.res" : $(SOURCE) $(DEP_RSC_RENDE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Render.res" /i "Render" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"


"$(INTDIR)\Render.res" : $(SOURCE) $(DEP_RSC_RENDE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Render.res" /i "Render" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnBlast.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\Render\Release
InputPath=.\Render\UnBlast.asm

"$(IntDir)\UnBlast.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnBlast.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\Render\Debug
InputPath=.\Render\UnBlast.asm

"$(IntDir)\UnBlast.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnBlast.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnLight.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNLIG=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnLight.obj" : $(SOURCE) $(DEP_CPP_UNLIG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNLIG=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnLight.obj" : $(SOURCE) $(DEP_CPP_UNLIG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnBlastC.cpp
DEP_CPP_UNBLA=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnBlastC.obj" : $(SOURCE) $(DEP_CPP_UNBLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnTest.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

# ADD CPP /FAs /Fa"Listing\UnTest.asm"

"$(INTDIR)\UnTest.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W4 /WX /vd0 /GX /Ox /Ot /Oa /Og /Oi /Ob2 /I\
 "$(InputPath)\..\Inc" /D "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /FAs /Fa"Listing\UnTest.asm" /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h"\
 /Fo"$(INTDIR)/" /QIfdiv- /QIfdiv- /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# ADD CPP /FAs /Fa"Listing\UnTest.asm"

"$(INTDIR)\UnTest.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W4 /WX /GX /Zi /Od /Gf /Gy /I "$(InputPath)\..\Inc"\
 /D "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAs\
 /Fa"Listing\UnTest.asm" /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /QIfdiv- /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRandom.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNRAN=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRandom.obj" : $(SOURCE) $(DEP_CPP_UNRAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNRAN=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRandom.obj" : $(SOURCE) $(DEP_CPP_UNRAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnDither.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNDIT=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnDither.obj" : $(SOURCE) $(DEP_CPP_UNDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNDIT=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnDither.obj" : $(SOURCE) $(DEP_CPP_UNDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnEdge.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNEDG=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnEdge.obj" : $(SOURCE) $(DEP_CPP_UNEDG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNEDG=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnRaster.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdge.obj" : $(SOURCE) $(DEP_CPP_UNEDG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSpanX.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\Render\Release
InputPath=.\Render\UnSpanX.asm

"$(IntDir)\UnSpanX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnSpanX.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\Render\Debug
InputPath=.\Render\UnSpanX.asm

"$(IntDir)\UnSpanX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnSpanX.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnPalTbl.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNPAL=\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\$(InputPath)\..\Inc\Root.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnClass.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnPalTbl.obj" : $(SOURCE) $(DEP_CPP_UNPAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

DEP_CPP_UNPAL=\
	".\$(InputPath)\..\Inc\Root.h"\
	".\$(InputPath)\..\Inc\UnBuild.h"\
	".\$(InputPath)\..\Inc\UnCache.h"\
	".\$(InputPath)\..\Inc\UnChecks.h"\
	".\$(InputPath)\..\Inc\UnFGAud.h"\
	".\$(InputPath)\..\Inc\UnFile.h"\
	".\$(InputPath)\..\Inc\UnMath.h"\
	".\$(InputPath)\..\Inc\UnMsgs.h"\
	".\$(InputPath)\..\Inc\UnPlatfm.h"\
	".\$(InputPath)\..\Inc\UnPort.h"\
	".\$(InputPath)\..\Inc\Unreal.h"\
	".\$(InputPath)\..\Inc\UnRender.h"\
	".\$(InputPath)\..\Inc\UnScript.h"\
	".\Inc\UnActLst.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCID.h"\
	".\Inc\UnClass.h"\
	".\Inc\UnClsPrp.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgPar.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRoot.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnPalTbl.obj" : $(SOURCE) $(DEP_CPP_UNPAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Classes - Win32 Release"
# Name "Classes - Win32 Debug"

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Trigger.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Decorati.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Engine.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Explosio.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Inventor.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Keypoint.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Light.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Mover.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Pawn.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Projecti.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Pyrotech.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Actor.tcx

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Unreal\Classes\Classes.mac

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section UnWn : {32F3286C-1F43-11CF-98C0-0000C06958A7}
# 	2:20:UnrealTextureBrowser:Palette support added
# End Section
################################################################################
