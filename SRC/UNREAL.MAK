# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) External Target" 0x0106
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Render - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Render - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Engine - Win32 Release" && "$(CFG)" != "Engine - Win32 Debug"\
 && "$(CFG)" != "Game - Win32 Release" && "$(CFG)" != "Game - Win32 Debug" &&\
 "$(CFG)" != "Windows - Win32 Release" && "$(CFG)" != "Windows - Win32 Debug" &&\
 "$(CFG)" != "Documentation - Win32 Release" && "$(CFG)" !=\
 "Network - Win32 Release" && "$(CFG)" != "Network - Win32 Debug" && "$(CFG)" !=\
 "Editor - Win32 Release" && "$(CFG)" != "Editor - Win32 Debug" && "$(CFG)" !=\
 "Render - Win32 Release" && "$(CFG)" != "Render - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unreal.mak" CFG="Render - Win32 Debug"
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
# PROP Target_Last_Scanned "Render - Win32 Debug"

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

ALL : "..\System\UnEngine.dll"

CLEAN : 
	-@erase "$(INTDIR)\Engine.res"
	-@erase "$(INTDIR)\UnAction.obj"
	-@erase "$(INTDIR)\UnActLst.obj"
	-@erase "$(INTDIR)\UnCache.obj"
	-@erase "$(INTDIR)\UnCamera.obj"
	-@erase "$(INTDIR)\UnCamMgr.obj"
	-@erase "$(INTDIR)\UnCheat.obj"
	-@erase "$(INTDIR)\UnChecks.obj"
	-@erase "$(INTDIR)\UnClass.obj"
	-@erase "$(INTDIR)\UnClsPrp.obj"
	-@erase "$(INTDIR)\UnConfig.obj"
	-@erase "$(INTDIR)\UnDeflts.obj"
	-@erase "$(INTDIR)\UnDynBSP.obj"
	-@erase "$(INTDIR)\UnEngine.obj"
	-@erase "$(INTDIR)\UnFGAud.obj"
	-@erase "$(INTDIR)\UnFile.obj"
	-@erase "$(INTDIR)\UnFPoly.obj"
	-@erase "$(INTDIR)\UnGfx.obj"
	-@erase "$(INTDIR)\UnInput.obj"
	-@erase "$(INTDIR)\UnInputP.obj"
	-@erase "$(INTDIR)\UnKeyVal.obj"
	-@erase "$(INTDIR)\UnLevAct.obj"
	-@erase "$(INTDIR)\UnLevel.obj"
	-@erase "$(INTDIR)\UnLevTic.obj"
	-@erase "$(INTDIR)\UnMath.obj"
	-@erase "$(INTDIR)\UnMem.obj"
	-@erase "$(INTDIR)\UnMesh.obj"
	-@erase "$(INTDIR)\UnModel.obj"
	-@erase "$(INTDIR)\UnOutDev.obj"
	-@erase "$(INTDIR)\UnParams.obj"
	-@erase "$(INTDIR)\UnParse.obj"
	-@erase "$(INTDIR)\UnPhys.obj"
	-@erase "$(INTDIR)\UnPrefer.obj"
	-@erase "$(INTDIR)\UnRes.obj"
	-@erase "$(INTDIR)\UnScript.obj"
	-@erase "$(INTDIR)\UnScrRTL.obj"
	-@erase "$(INTDIR)\UnServer.obj"
	-@erase "$(INTDIR)\UnSound.obj"
	-@erase "$(INTDIR)\UnTex.obj"
	-@erase "$(INTDIR)\UnTopics.obj"
	-@erase "$(OUTDIR)\UnEngine.exp"
	-@erase "$(OUTDIR)\UnEngine.lib"
	-@erase "..\System\UnEngine.dll"
	-@erase "..\System\UnEngine.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_ENGINE" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /GM /c 
CPP_OBJS=.\Engine\Release/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\UnEngine.dll"
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/UnEngine.pdb" /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\UnEngine.dll" /implib:"$(OUTDIR)/UnEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Engine.res" \
	"$(INTDIR)\UnAction.obj" \
	"$(INTDIR)\UnActLst.obj" \
	"$(INTDIR)\UnCache.obj" \
	"$(INTDIR)\UnCamera.obj" \
	"$(INTDIR)\UnCamMgr.obj" \
	"$(INTDIR)\UnCheat.obj" \
	"$(INTDIR)\UnChecks.obj" \
	"$(INTDIR)\UnClass.obj" \
	"$(INTDIR)\UnClsPrp.obj" \
	"$(INTDIR)\UnConfig.obj" \
	"$(INTDIR)\UnDeflts.obj" \
	"$(INTDIR)\UnDynBSP.obj" \
	"$(INTDIR)\UnEngine.obj" \
	"$(INTDIR)\UnFGAud.obj" \
	"$(INTDIR)\UnFile.obj" \
	"$(INTDIR)\UnFPoly.obj" \
	"$(INTDIR)\UnGfx.obj" \
	"$(INTDIR)\UnInput.obj" \
	"$(INTDIR)\UnInputP.obj" \
	"$(INTDIR)\UnKeyVal.obj" \
	"$(INTDIR)\UnLevAct.obj" \
	"$(INTDIR)\UnLevel.obj" \
	"$(INTDIR)\UnLevTic.obj" \
	"$(INTDIR)\UnMath.obj" \
	"$(INTDIR)\UnMem.obj" \
	"$(INTDIR)\UnMesh.obj" \
	"$(INTDIR)\UnModel.obj" \
	"$(INTDIR)\UnOutDev.obj" \
	"$(INTDIR)\UnParams.obj" \
	"$(INTDIR)\UnParse.obj" \
	"$(INTDIR)\UnPhys.obj" \
	"$(INTDIR)\UnPrefer.obj" \
	"$(INTDIR)\UnRes.obj" \
	"$(INTDIR)\UnScript.obj" \
	"$(INTDIR)\UnScrRTL.obj" \
	"$(INTDIR)\UnServer.obj" \
	"$(INTDIR)\UnSound.obj" \
	"$(INTDIR)\UnTex.obj" \
	"$(INTDIR)\UnTopics.obj"

"..\System\UnEngine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dEngine.dll"

CLEAN : 
	-@erase "$(INTDIR)\Engine.res"
	-@erase "$(INTDIR)\UnAction.obj"
	-@erase "$(INTDIR)\UnActLst.obj"
	-@erase "$(INTDIR)\UnCache.obj"
	-@erase "$(INTDIR)\UnCamera.obj"
	-@erase "$(INTDIR)\UnCamMgr.obj"
	-@erase "$(INTDIR)\UnCheat.obj"
	-@erase "$(INTDIR)\UnChecks.obj"
	-@erase "$(INTDIR)\UnClass.obj"
	-@erase "$(INTDIR)\UnClsPrp.obj"
	-@erase "$(INTDIR)\UnConfig.obj"
	-@erase "$(INTDIR)\UnDeflts.obj"
	-@erase "$(INTDIR)\UnDynBSP.obj"
	-@erase "$(INTDIR)\UnEngine.obj"
	-@erase "$(INTDIR)\UnFGAud.obj"
	-@erase "$(INTDIR)\UnFile.obj"
	-@erase "$(INTDIR)\UnFPoly.obj"
	-@erase "$(INTDIR)\UnGfx.obj"
	-@erase "$(INTDIR)\UnInput.obj"
	-@erase "$(INTDIR)\UnInputP.obj"
	-@erase "$(INTDIR)\UnKeyVal.obj"
	-@erase "$(INTDIR)\UnLevAct.obj"
	-@erase "$(INTDIR)\UnLevel.obj"
	-@erase "$(INTDIR)\UnLevTic.obj"
	-@erase "$(INTDIR)\UnMath.obj"
	-@erase "$(INTDIR)\UnMem.obj"
	-@erase "$(INTDIR)\UnMesh.obj"
	-@erase "$(INTDIR)\UnModel.obj"
	-@erase "$(INTDIR)\UnOutDev.obj"
	-@erase "$(INTDIR)\UnParams.obj"
	-@erase "$(INTDIR)\UnParse.obj"
	-@erase "$(INTDIR)\UnPhys.obj"
	-@erase "$(INTDIR)\UnPrefer.obj"
	-@erase "$(INTDIR)\UnRes.obj"
	-@erase "$(INTDIR)\UnScript.obj"
	-@erase "$(INTDIR)\UnScrRTL.obj"
	-@erase "$(INTDIR)\UnServer.obj"
	-@erase "$(INTDIR)\UnSound.obj"
	-@erase "$(INTDIR)\UnTex.obj"
	-@erase "$(INTDIR)\UnTopics.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dEngine.exp"
	-@erase "$(OUTDIR)\dEngine.lib"
	-@erase "$(OUTDIR)\dEngine.pdb"
	-@erase "..\System\dEngine.dll"
	-@erase "..\System\dEngine.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "COMPILING_ENGINE" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_ENGINE" /D "WIN32" /D "_WINDOWS" /D "_DEBUG"\
 /Fp"$(INTDIR)/Engine.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /GM /c 
CPP_OBJS=.\Engine\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\dEngine.dll"
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dEngine.pdb" /debug /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\dEngine.dll" /implib:"$(OUTDIR)/dEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Engine.res" \
	"$(INTDIR)\UnAction.obj" \
	"$(INTDIR)\UnActLst.obj" \
	"$(INTDIR)\UnCache.obj" \
	"$(INTDIR)\UnCamera.obj" \
	"$(INTDIR)\UnCamMgr.obj" \
	"$(INTDIR)\UnCheat.obj" \
	"$(INTDIR)\UnChecks.obj" \
	"$(INTDIR)\UnClass.obj" \
	"$(INTDIR)\UnClsPrp.obj" \
	"$(INTDIR)\UnConfig.obj" \
	"$(INTDIR)\UnDeflts.obj" \
	"$(INTDIR)\UnDynBSP.obj" \
	"$(INTDIR)\UnEngine.obj" \
	"$(INTDIR)\UnFGAud.obj" \
	"$(INTDIR)\UnFile.obj" \
	"$(INTDIR)\UnFPoly.obj" \
	"$(INTDIR)\UnGfx.obj" \
	"$(INTDIR)\UnInput.obj" \
	"$(INTDIR)\UnInputP.obj" \
	"$(INTDIR)\UnKeyVal.obj" \
	"$(INTDIR)\UnLevAct.obj" \
	"$(INTDIR)\UnLevel.obj" \
	"$(INTDIR)\UnLevTic.obj" \
	"$(INTDIR)\UnMath.obj" \
	"$(INTDIR)\UnMem.obj" \
	"$(INTDIR)\UnMesh.obj" \
	"$(INTDIR)\UnModel.obj" \
	"$(INTDIR)\UnOutDev.obj" \
	"$(INTDIR)\UnParams.obj" \
	"$(INTDIR)\UnParse.obj" \
	"$(INTDIR)\UnPhys.obj" \
	"$(INTDIR)\UnPrefer.obj" \
	"$(INTDIR)\UnRes.obj" \
	"$(INTDIR)\UnScript.obj" \
	"$(INTDIR)\UnScrRTL.obj" \
	"$(INTDIR)\UnServer.obj" \
	"$(INTDIR)\UnSound.obj" \
	"$(INTDIR)\UnTex.obj" \
	"$(INTDIR)\UnTopics.obj"

"..\System\dEngine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\UnGame.dll"

CLEAN : 
	-@erase "$(INTDIR)\AiAnimat.obj"
	-@erase "$(INTDIR)\Game.res"
	-@erase "$(INTDIR)\UnActPro.obj"
	-@erase "$(INTDIR)\UnCon.obj"
	-@erase "$(INTDIR)\UnDecora.obj"
	-@erase "$(INTDIR)\UnExplos.obj"
	-@erase "$(INTDIR)\UnFActor.obj"
	-@erase "$(INTDIR)\UnGame.obj"
	-@erase "$(INTDIR)\UnGCheat.obj"
	-@erase "$(INTDIR)\UnInv.obj"
	-@erase "$(INTDIR)\UnKeyPts.obj"
	-@erase "$(INTDIR)\UnMover.obj"
	-@erase "$(INTDIR)\UnPawn.obj"
	-@erase "$(INTDIR)\UnPawns.obj"
	-@erase "$(INTDIR)\UnPlayer.obj"
	-@erase "$(INTDIR)\UnPyro.obj"
	-@erase "$(INTDIR)\UnRandom.obj"
	-@erase "$(INTDIR)\UnRoot.obj"
	-@erase "$(INTDIR)\UnThings.obj"
	-@erase "$(INTDIR)\UnTrig.obj"
	-@erase "$(INTDIR)\UnWeapon.obj"
	-@erase "$(OUTDIR)\UnGame.exp"
	-@erase "$(OUTDIR)\UnGame.lib"
	-@erase "..\System\UnGame.dll"
	-@erase "..\System\UnGame.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "COMPILING_GAME" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_GAME" /D "RELEASE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Game.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /GM /c 
CPP_OBJS=.\Game\Release/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnGame.dll"
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/UnGame.pdb"\
 /machine:I386 /out:"..\System\UnGame.dll" /implib:"$(OUTDIR)/UnGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AiAnimat.obj" \
	"$(INTDIR)\Game.res" \
	"$(INTDIR)\UnActPro.obj" \
	"$(INTDIR)\UnCon.obj" \
	"$(INTDIR)\UnDecora.obj" \
	"$(INTDIR)\UnExplos.obj" \
	"$(INTDIR)\UnFActor.obj" \
	"$(INTDIR)\UnGame.obj" \
	"$(INTDIR)\UnGCheat.obj" \
	"$(INTDIR)\UnInv.obj" \
	"$(INTDIR)\UnKeyPts.obj" \
	"$(INTDIR)\UnMover.obj" \
	"$(INTDIR)\UnPawn.obj" \
	"$(INTDIR)\UnPawns.obj" \
	"$(INTDIR)\UnPlayer.obj" \
	"$(INTDIR)\UnPyro.obj" \
	"$(INTDIR)\UnRandom.obj" \
	"$(INTDIR)\UnRoot.obj" \
	"$(INTDIR)\UnThings.obj" \
	"$(INTDIR)\UnTrig.obj" \
	"$(INTDIR)\UnWeapon.obj"

"..\System\UnGame.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dGame.dll"

CLEAN : 
	-@erase "$(INTDIR)\AiAnimat.obj"
	-@erase "$(INTDIR)\Game.res"
	-@erase "$(INTDIR)\UnActPro.obj"
	-@erase "$(INTDIR)\UnCon.obj"
	-@erase "$(INTDIR)\UnDecora.obj"
	-@erase "$(INTDIR)\UnExplos.obj"
	-@erase "$(INTDIR)\UnFActor.obj"
	-@erase "$(INTDIR)\UnGame.obj"
	-@erase "$(INTDIR)\UnGCheat.obj"
	-@erase "$(INTDIR)\UnInv.obj"
	-@erase "$(INTDIR)\UnKeyPts.obj"
	-@erase "$(INTDIR)\UnMover.obj"
	-@erase "$(INTDIR)\UnPawn.obj"
	-@erase "$(INTDIR)\UnPawns.obj"
	-@erase "$(INTDIR)\UnPlayer.obj"
	-@erase "$(INTDIR)\UnPyro.obj"
	-@erase "$(INTDIR)\UnRandom.obj"
	-@erase "$(INTDIR)\UnRoot.obj"
	-@erase "$(INTDIR)\UnThings.obj"
	-@erase "$(INTDIR)\UnTrig.obj"
	-@erase "$(INTDIR)\UnWeapon.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dGame.exp"
	-@erase "$(OUTDIR)\dGame.lib"
	-@erase "$(OUTDIR)\dGame.pdb"
	-@erase "..\System\dGame.dll"
	-@erase "..\System\dGame.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "COMPILING_GAME" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_GAME" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"$(INTDIR)/Game.pch"\
 /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv- /GM /c 
CPP_OBJS=.\Game\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dGame.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/dGame.pdb" /debug\
 /machine:I386 /out:"..\System\dGame.dll" /implib:"$(OUTDIR)/dGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AiAnimat.obj" \
	"$(INTDIR)\Game.res" \
	"$(INTDIR)\UnActPro.obj" \
	"$(INTDIR)\UnCon.obj" \
	"$(INTDIR)\UnDecora.obj" \
	"$(INTDIR)\UnExplos.obj" \
	"$(INTDIR)\UnFActor.obj" \
	"$(INTDIR)\UnGame.obj" \
	"$(INTDIR)\UnGCheat.obj" \
	"$(INTDIR)\UnInv.obj" \
	"$(INTDIR)\UnKeyPts.obj" \
	"$(INTDIR)\UnMover.obj" \
	"$(INTDIR)\UnPawn.obj" \
	"$(INTDIR)\UnPawns.obj" \
	"$(INTDIR)\UnPlayer.obj" \
	"$(INTDIR)\UnPyro.obj" \
	"$(INTDIR)\UnRandom.obj" \
	"$(INTDIR)\UnRoot.obj" \
	"$(INTDIR)\UnThings.obj" \
	"$(INTDIR)\UnTrig.obj" \
	"$(INTDIR)\UnWeapon.obj"

"..\System\dGame.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\UnServer.exe" ".\System\UnWn.tlb"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UnPlatfm.obj"
	-@erase "$(INTDIR)\UnPswd.obj"
	-@erase "$(INTDIR)\UnRage.obj"
	-@erase "$(INTDIR)\UnRenDev.obj"
	-@erase "$(INTDIR)\UnTask.obj"
	-@erase "$(INTDIR)\UnWn.obj"
	-@erase "$(INTDIR)\UnWn.pch"
	-@erase "$(INTDIR)\UnWn.res"
	-@erase "$(INTDIR)\UnWnAcCD.obj"
	-@erase "$(INTDIR)\UnWnActD.obj"
	-@erase "$(INTDIR)\UnWnAudD.obj"
	-@erase "$(INTDIR)\UnWnCam.obj"
	-@erase "$(INTDIR)\UnWnDevD.obj"
	-@erase "$(INTDIR)\UnWnDlg.obj"
	-@erase "$(INTDIR)\UnWnEdSv.obj"
	-@erase "$(INTDIR)\UnWnKeyD.obj"
	-@erase "$(INTDIR)\UnWnPref.obj"
	-@erase "$(INTDIR)\UnWnProp.obj"
	-@erase "$(INTDIR)\UnWnSenD.obj"
	-@erase "$(INTDIR)\UnWnSerD.obj"
	-@erase "$(INTDIR)\UnWnTime.obj"
	-@erase "..\System\UnServer.exe"
	-@erase "..\System\UnServer.ilk"
	-@erase ".\System\UnWn.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"StdAfx.h" /QIfdiv- /GM /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)/UnWn.pch" /Yu"StdAfx.h" /Fo"$(INTDIR)/" /QIfdiv- /GM\
 /GM /c 
CPP_OBJS=.\Windows\Release/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /incremental:yes /machine:I386 /out:"..\System\UnServer.exe"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib\
 Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/UnServer.pdb" /machine:I386\
 /out:"..\System\UnServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\UnPlatfm.obj" \
	"$(INTDIR)\UnPswd.obj" \
	"$(INTDIR)\UnRage.obj" \
	"$(INTDIR)\UnRenDev.obj" \
	"$(INTDIR)\UnTask.obj" \
	"$(INTDIR)\UnWn.obj" \
	"$(INTDIR)\UnWn.res" \
	"$(INTDIR)\UnWnAcCD.obj" \
	"$(INTDIR)\UnWnActD.obj" \
	"$(INTDIR)\UnWnAudD.obj" \
	"$(INTDIR)\UnWnCam.obj" \
	"$(INTDIR)\UnWnDevD.obj" \
	"$(INTDIR)\UnWnDlg.obj" \
	"$(INTDIR)\UnWnEdSv.obj" \
	"$(INTDIR)\UnWnKeyD.obj" \
	"$(INTDIR)\UnWnPref.obj" \
	"$(INTDIR)\UnWnProp.obj" \
	"$(INTDIR)\UnWnSenD.obj" \
	"$(INTDIR)\UnWnSerD.obj" \
	"$(INTDIR)\UnWnTime.obj"

"..\System\UnServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dServer.exe" ".\System\UnWn.tlb"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UnPlatfm.obj"
	-@erase "$(INTDIR)\UnPswd.obj"
	-@erase "$(INTDIR)\UnRage.obj"
	-@erase "$(INTDIR)\UnRenDev.obj"
	-@erase "$(INTDIR)\UnTask.obj"
	-@erase "$(INTDIR)\UnWn.obj"
	-@erase "$(INTDIR)\UnWn.pch"
	-@erase "$(INTDIR)\UnWn.res"
	-@erase "$(INTDIR)\UnWnAcCD.obj"
	-@erase "$(INTDIR)\UnWnActD.obj"
	-@erase "$(INTDIR)\UnWnAudD.obj"
	-@erase "$(INTDIR)\UnWnCam.obj"
	-@erase "$(INTDIR)\UnWnDevD.obj"
	-@erase "$(INTDIR)\UnWnDlg.obj"
	-@erase "$(INTDIR)\UnWnEdSv.obj"
	-@erase "$(INTDIR)\UnWnKeyD.obj"
	-@erase "$(INTDIR)\UnWnPref.obj"
	-@erase "$(INTDIR)\UnWnProp.obj"
	-@erase "$(INTDIR)\UnWnSenD.obj"
	-@erase "$(INTDIR)\UnWnSerD.obj"
	-@erase "$(INTDIR)\UnWnTime.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dServer.pdb"
	-@erase "..\System\dServer.exe"
	-@erase "..\System\dServer.ilk"
	-@erase ".\System\UnWn.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG" /Yu"StdAfx.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG"\
 /Fp"$(INTDIR)/UnWn.pch" /Yu"StdAfx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv-\
 /GM /c 
CPP_OBJS=.\Windows\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib\
 Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/dServer.pdb" /debug /machine:I386\
 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\UnPlatfm.obj" \
	"$(INTDIR)\UnPswd.obj" \
	"$(INTDIR)\UnRage.obj" \
	"$(INTDIR)\UnRenDev.obj" \
	"$(INTDIR)\UnTask.obj" \
	"$(INTDIR)\UnWn.obj" \
	"$(INTDIR)\UnWn.res" \
	"$(INTDIR)\UnWnAcCD.obj" \
	"$(INTDIR)\UnWnActD.obj" \
	"$(INTDIR)\UnWnAudD.obj" \
	"$(INTDIR)\UnWnCam.obj" \
	"$(INTDIR)\UnWnDevD.obj" \
	"$(INTDIR)\UnWnDlg.obj" \
	"$(INTDIR)\UnWnEdSv.obj" \
	"$(INTDIR)\UnWnKeyD.obj" \
	"$(INTDIR)\UnWnPref.obj" \
	"$(INTDIR)\UnWnProp.obj" \
	"$(INTDIR)\UnWnSenD.obj" \
	"$(INTDIR)\UnWnSerD.obj" \
	"$(INTDIR)\UnWnTime.obj"

"..\System\dServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\UnNet.dll" "$(OUTDIR)\Network.bsc"

CLEAN : 
	-@erase "$(INTDIR)\NetDPlay.obj"
	-@erase "$(INTDIR)\NetDPlay.sbr"
	-@erase "$(INTDIR)\NetINet.obj"
	-@erase "$(INTDIR)\NetINet.sbr"
	-@erase "$(INTDIR)\NetMain.obj"
	-@erase "$(INTDIR)\NetMain.sbr"
	-@erase "$(INTDIR)\NetTeln.obj"
	-@erase "$(INTDIR)\NetTeln.sbr"
	-@erase "$(INTDIR)\NetWin.obj"
	-@erase "$(INTDIR)\NetWin.sbr"
	-@erase "$(INTDIR)\Network.res"
	-@erase "$(OUTDIR)\Network.bsc"
	-@erase "$(OUTDIR)\UnNet.exp"
	-@erase "$(OUTDIR)\UnNet.lib"
	-@erase "..\System\UnNet.dll"
	-@erase "..\System\UnNet.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "COMPILING_NETWORK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX"windows.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_NETWORK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/Network.pch" /YX"windows.h" /Fo"$(INTDIR)/" /QIfdiv- /GM /c 
CPP_OBJS=.\Network\Release/
CPP_SBRS=.\Network\Release/

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
BSC32_SBRS= \
	"$(INTDIR)\NetDPlay.sbr" \
	"$(INTDIR)\NetINet.sbr" \
	"$(INTDIR)\NetMain.sbr" \
	"$(INTDIR)\NetTeln.sbr" \
	"$(INTDIR)\NetWin.sbr"

"$(OUTDIR)\Network.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:0x14000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnNet.dll"
LINK32_FLAGS=Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib\
 /nologo /base:0x14000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/UnNet.pdb" /machine:I386 /out:"..\System\UnNet.dll"\
 /implib:"$(OUTDIR)/UnNet.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NetDPlay.obj" \
	"$(INTDIR)\NetINet.obj" \
	"$(INTDIR)\NetMain.obj" \
	"$(INTDIR)\NetTeln.obj" \
	"$(INTDIR)\NetWin.obj" \
	"$(INTDIR)\Network.res"

"..\System\UnNet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dNet.dll"

CLEAN : 
	-@erase "$(INTDIR)\NetDPlay.obj"
	-@erase "$(INTDIR)\NetINet.obj"
	-@erase "$(INTDIR)\NetMain.obj"
	-@erase "$(INTDIR)\NetTeln.obj"
	-@erase "$(INTDIR)\NetWin.obj"
	-@erase "$(INTDIR)\Network.res"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dNet.exp"
	-@erase "$(OUTDIR)\dNet.lib"
	-@erase "$(OUTDIR)\dNet.pdb"
	-@erase "..\System\dNet.dll"
	-@erase "..\System\dNet.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "COMPILING_NETWORK" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX"windows.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_NETWORK" /D "WIN32" /D "_WINDOWS" /D "_DEBUG"\
 /Fp"$(INTDIR)/Network.pch" /YX"windows.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /GM /c 
CPP_OBJS=.\Network\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:0x14000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dNet.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib\
 /nologo /base:0x14000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dNet.pdb" /debug /machine:I386 /out:"..\System\dNet.dll"\
 /implib:"$(OUTDIR)/dNet.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NetDPlay.obj" \
	"$(INTDIR)\NetINet.obj" \
	"$(INTDIR)\NetMain.obj" \
	"$(INTDIR)\NetTeln.obj" \
	"$(INTDIR)\NetWin.obj" \
	"$(INTDIR)\Network.res"

"..\System\dNet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\UnEditor.dll"

CLEAN : 
	-@erase "$(INTDIR)\UnBsp.obj"
	-@erase "$(INTDIR)\UnEdAct.obj"
	-@erase "$(INTDIR)\UnEdCam.obj"
	-@erase "$(INTDIR)\UnEdCnst.obj"
	-@erase "$(INTDIR)\UnEdCsg.obj"
	-@erase "$(INTDIR)\UnEditor.obj"
	-@erase "$(INTDIR)\UnEditor.res"
	-@erase "$(INTDIR)\UnEdScan.obj"
	-@erase "$(INTDIR)\UnEdSrv.obj"
	-@erase "$(INTDIR)\UnEdTran.obj"
	-@erase "$(INTDIR)\UnMeshEd.obj"
	-@erase "$(INTDIR)\UnShadow.obj"
	-@erase "$(INTDIR)\UnTexEd.obj"
	-@erase "$(INTDIR)\UnZone.obj"
	-@erase "$(OUTDIR)\UnEditor.exp"
	-@erase "$(OUTDIR)\UnEditor.lib"
	-@erase "..\System\UnEditor.dll"
	-@erase "..\System\UnEditor.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILING_EDITOR" /YX /QIfdiv- /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "COMPILING_EDITOR" /Fp"$(INTDIR)/UnrealEd.pch" /YX\
 /Fo"$(INTDIR)/" /QIfdiv- /QIfdiv- /GM /c 
CPP_OBJS=.\Editor\Release/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnEditor.dll"
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/UnEditor.pdb"\
 /machine:I386 /out:"..\System\UnEditor.dll" /implib:"$(OUTDIR)/UnEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)\UnBsp.obj" \
	"$(INTDIR)\UnEdAct.obj" \
	"$(INTDIR)\UnEdCam.obj" \
	"$(INTDIR)\UnEdCnst.obj" \
	"$(INTDIR)\UnEdCsg.obj" \
	"$(INTDIR)\UnEditor.obj" \
	"$(INTDIR)\UnEditor.res" \
	"$(INTDIR)\UnEdScan.obj" \
	"$(INTDIR)\UnEdSrv.obj" \
	"$(INTDIR)\UnEdTran.obj" \
	"$(INTDIR)\UnMeshEd.obj" \
	"$(INTDIR)\UnShadow.obj" \
	"$(INTDIR)\UnTexEd.obj" \
	"$(INTDIR)\UnZone.obj"

"..\System\UnEditor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dEditor.dll"

CLEAN : 
	-@erase "$(INTDIR)\UnBsp.obj"
	-@erase "$(INTDIR)\UnEdAct.obj"
	-@erase "$(INTDIR)\UnEdCam.obj"
	-@erase "$(INTDIR)\UnEdCnst.obj"
	-@erase "$(INTDIR)\UnEdCsg.obj"
	-@erase "$(INTDIR)\UnEditor.obj"
	-@erase "$(INTDIR)\UnEditor.res"
	-@erase "$(INTDIR)\UnEdScan.obj"
	-@erase "$(INTDIR)\UnEdSrv.obj"
	-@erase "$(INTDIR)\UnEdTran.obj"
	-@erase "$(INTDIR)\UnMeshEd.obj"
	-@erase "$(INTDIR)\UnShadow.obj"
	-@erase "$(INTDIR)\UnTexEd.obj"
	-@erase "$(INTDIR)\UnZone.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dEditor.exp"
	-@erase "$(OUTDIR)\dEditor.lib"
	-@erase "$(OUTDIR)\dEditor.pdb"
	-@erase "..\System\dEditor.dll"
	-@erase "..\System\dEditor.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "WIN32" /D "_WINDOWS" /D "COMPILING_EDITOR" /D "_DEBUG" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "WIN32" /D "_WINDOWS" /D "COMPILING_EDITOR" /D "_DEBUG"\
 /Fp"$(INTDIR)/UnrealEd.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /GM /c 
CPP_OBJS=.\Editor\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dEditor.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/dEditor.pdb" /debug\
 /machine:I386 /out:"..\System\dEditor.dll" /implib:"$(OUTDIR)/dEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)\UnBsp.obj" \
	"$(INTDIR)\UnEdAct.obj" \
	"$(INTDIR)\UnEdCam.obj" \
	"$(INTDIR)\UnEdCnst.obj" \
	"$(INTDIR)\UnEdCsg.obj" \
	"$(INTDIR)\UnEditor.obj" \
	"$(INTDIR)\UnEditor.res" \
	"$(INTDIR)\UnEdScan.obj" \
	"$(INTDIR)\UnEdSrv.obj" \
	"$(INTDIR)\UnEdTran.obj" \
	"$(INTDIR)\UnMeshEd.obj" \
	"$(INTDIR)\UnShadow.obj" \
	"$(INTDIR)\UnTexEd.obj" \
	"$(INTDIR)\UnZone.obj"

"..\System\dEditor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\UnRender.dll"

CLEAN : 
	-@erase "$(INTDIR)\Render.res"
	-@erase "$(INTDIR)\UnBlast.obj"
	-@erase "$(INTDIR)\UnBlastC.obj"
	-@erase "$(INTDIR)\UnEdRend.obj"
	-@erase "$(INTDIR)\UnLight.obj"
	-@erase "$(INTDIR)\UnMiscC.obj"
	-@erase "$(INTDIR)\UnRaster.obj"
	-@erase "$(INTDIR)\UnRender.obj"
	-@erase "$(INTDIR)\UnSpan.obj"
	-@erase "$(INTDIR)\UnSprite.obj"
	-@erase "$(INTDIR)\UnTest.obj"
	-@erase "$(INTDIR)\UnTmap.obj"
	-@erase "$(INTDIR)\UnTMapC.obj"
	-@erase "$(OUTDIR)\UnRender.exp"
	-@erase "$(OUTDIR)\UnRender.lib"
	-@erase "..\System\UnRender.dll"
	-@erase "..\System\UnRender.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /QIfdiv- /GM /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /QIfdiv- /GM\
 /GM /c 
CPP_OBJS=.\Render\Release/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnRender.dll"
LINK32_FLAGS=Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/UnRender.pdb" /machine:I386 /out:"..\System\UnRender.dll"\
 /implib:"$(OUTDIR)/UnRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Render.res" \
	"$(INTDIR)\UnBlast.obj" \
	"$(INTDIR)\UnBlastC.obj" \
	"$(INTDIR)\UnEdRend.obj" \
	"$(INTDIR)\UnLight.obj" \
	"$(INTDIR)\UnMiscC.obj" \
	"$(INTDIR)\UnRaster.obj" \
	"$(INTDIR)\UnRender.obj" \
	"$(INTDIR)\UnSpan.obj" \
	"$(INTDIR)\UnSprite.obj" \
	"$(INTDIR)\UnTest.obj" \
	"$(INTDIR)\UnTmap.obj" \
	"$(INTDIR)\UnTMapC.obj"

"..\System\UnRender.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\System\dRender.dll"

CLEAN : 
	-@erase "$(INTDIR)\Render.res"
	-@erase "$(INTDIR)\UnBlast.obj"
	-@erase "$(INTDIR)\UnBlastC.obj"
	-@erase "$(INTDIR)\UnEdRend.obj"
	-@erase "$(INTDIR)\UnLight.obj"
	-@erase "$(INTDIR)\UnMiscC.obj"
	-@erase "$(INTDIR)\UnRaster.obj"
	-@erase "$(INTDIR)\UnRender.obj"
	-@erase "$(INTDIR)\UnSpan.obj"
	-@erase "$(INTDIR)\UnSprite.obj"
	-@erase "$(INTDIR)\UnTest.obj"
	-@erase "$(INTDIR)\UnTmapA.obj"
	-@erase "$(INTDIR)\UnTMapC.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dRender.exp"
	-@erase "$(OUTDIR)\dRender.lib"
	-@erase "$(OUTDIR)\dRender.pdb"
	-@erase "..\System\dRender.dll"
	-@erase "..\System\dRender.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"Unreal.h" /QIfdiv- /GM /c
CPP_PROJ=/nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /GM /c 
CPP_OBJS=.\Render\Debug/
CPP_SBRS=.\.

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
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dRender.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dRender.pdb" /debug /machine:I386 /out:"..\System\dRender.dll"\
 /implib:"$(OUTDIR)/dRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Render.res" \
	"$(INTDIR)\UnBlast.obj" \
	"$(INTDIR)\UnBlastC.obj" \
	"$(INTDIR)\UnEdRend.obj" \
	"$(INTDIR)\UnLight.obj" \
	"$(INTDIR)\UnMiscC.obj" \
	"$(INTDIR)\UnRaster.obj" \
	"$(INTDIR)\UnRender.obj" \
	"$(INTDIR)\UnSpan.obj" \
	"$(INTDIR)\UnSprite.obj" \
	"$(INTDIR)\UnTest.obj" \
	"$(INTDIR)\UnTmapA.obj" \
	"$(INTDIR)\UnTMapC.obj"

"..\System\dRender.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnFile.h"\
	".\Inc\UnPort.h"\
	

"$(INTDIR)\UnFile.obj" : $(SOURCE) $(DEP_CPP_UNFIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnGfx.cpp
DEP_CPP_UNGFX=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnScript.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnScript.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnScript.obj" : $(SOURCE) $(DEP_CPP_UNSCRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnActLst.cpp
DEP_CPP_UNACT=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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

!IF  "$(CFG)" == "Engine - Win32 Release"

DEP_CPP_UNENG=\
	".\Inc\Net.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEngine.obj" : $(SOURCE) $(DEP_CPP_UNENG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

DEP_CPP_UNENG=\
	".\Inc\Net.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	
NODEP_CPP_UNENG=\
	".\Engine\REN_DynLight"\
	

"$(INTDIR)\UnEngine.obj" : $(SOURCE) $(DEP_CPP_UNENG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnInputP.cpp
DEP_CPP_UNINPU=\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnInputP.obj" : $(SOURCE) $(DEP_CPP_UNINPU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine\UnAction.cpp
DEP_CPP_UNACTI=\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\SoundEng.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Inc\SoundEng.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnFGAud.obj" : $(SOURCE) $(DEP_CPP_UNFGA) "$(INTDIR)"
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnCon.h"\
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnCon.h"\
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnGame.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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

SOURCE=\Unreal\Classes\Root.tcx

!IF  "$(CFG)" == "Game - Win32 Release"

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Game\UnRandom.cpp
DEP_CPP_UNRAN=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRandom.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnCheat.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
	".\Game\UnFactor.h"\
	".\Game\UnGame.h"\
	".\Inc\AIAnimat.h"\
	".\Inc\AImotion.h"\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnChecks.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
DEP_CPP_UNWNT=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnPlatfm.cpp
DEP_CPP_UNPLAT=\
	".\Inc\DDraw1.h"\
	".\Inc\Net.h"\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnPswd.cpp
DEP_CPP_UNPSW=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
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
	".\Inc\DDraw1.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\Ati3dCif.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWnCam.h"\
	
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
DEP_CPP_UNREN=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Windows\StdAfx.h"\
	

"$(INTDIR)\UnRenDev.obj" : $(SOURCE) $(DEP_CPP_UNREN) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnTask.cpp
DEP_CPP_UNTAS=\
	".\Inc\DDraw1.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWn.cpp
DEP_CPP_UNWN_=\
	".\Inc\DDraw1.h"\
	".\Inc\Net.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnCam.cpp
DEP_CPP_UNWNC=\
	".\Inc\DDraw1.h"\
	".\Inc\DMouse.h"\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnDlg.cpp
DEP_CPP_UNWND=\
	".\Inc\DDraw1.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnEdSv.cpp
DEP_CPP_UNWNE=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\StdAfx.cpp
DEP_CPP_STDAF=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Windows\StdAfx.h"\
	

!IF  "$(CFG)" == "Windows - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_WINDOWS" /D "RELEASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)/UnWn.pch" /Yc /Fo"$(INTDIR)/" /QIfdiv- /GM /GM /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UnWn.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_WINDOWS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG"\
 /Fp"$(INTDIR)/UnWn.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QIfdiv- /GM /c\
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
	".\Windows\Cursors\AddActor.cur"\
	".\Windows\Cursors\BrushFr.cur"\
	".\Windows\Cursors\BrushMov.cur"\
	".\Windows\Cursors\BrushRot.cur"\
	".\Windows\Cursors\BrushSca.cur"\
	".\Windows\Cursors\BrushShr.cur"\
	".\Windows\Cursors\BrushSna.cur"\
	".\Windows\Cursors\BrushStr.cur"\
	".\Windows\cursors\BrushWrp.cur"\
	".\Windows\Cursors\CamZoom.cur"\
	".\Windows\Cursors\MoveAct.cur"\
	".\Windows\Cursors\SelActor.cur"\
	".\Windows\Cursors\SelectPo.cur"\
	".\Windows\Cursors\Terrafor.cur"\
	".\Windows\Cursors\TexGrab.cur"\
	".\Windows\Cursors\TexPan.cur"\
	".\Windows\Cursors\TexRot.cur"\
	".\Windows\Cursors\TexScale.cur"\
	".\Windows\Cursors\TexSet.cur"\
	".\Windows\Res\UnSplash.bmp"\
	".\Windows\Res\UnWn.rc2"\
	

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

# ADD MTL /tlb "System/UnWn.tlb"

".\System\UnWn.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "System/UnWn.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# ADD MTL /tlb "System/UnWn.tlb"

".\System\UnWn.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "System/UnWn.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnActD.cpp
DEP_CPP_UNWNA=\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnDevD.cpp
DEP_CPP_UNWNDE=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnPref.cpp
DEP_CPP_UNWNP=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnSenD.cpp
DEP_CPP_UNWNS=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnSerD.cpp
DEP_CPP_UNWNSE=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	".\Windows\UnWnSerD.h"\
	

"$(INTDIR)\UnWnSerD.obj" : $(SOURCE) $(DEP_CPP_UNWNSE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAudD.cpp
DEP_CPP_UNWNAU=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnKeyD.cpp
DEP_CPP_UNWNK=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnProp.cpp
DEP_CPP_UNWNPR=\
	".\Inc\unaction.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnParse.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAcCD.cpp
DEP_CPP_UNWNAC=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Windows\StdAfx.h"\
	".\Windows\UnWn.h"\
	".\Windows\UnWnAccD.h"\
	".\Windows\UnWnDlg.h"\
	".\Windows\UnWnEdSv.h"\
	

"$(INTDIR)\UnWnAcCD.obj" : $(SOURCE) $(DEP_CPP_UNWNAC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Documentation - Win32 Release"

!IF  "$(CFG)" == "Documentation - Win32 Release"

".\Doc\Doc.exe" : 
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

SOURCE=.\Doc\Actions.txt
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
	".\Inc\Net.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Network\NetDPlay.h"\
	".\Network\NetINet.h"\
	".\Windows\..\Network\NetPrv.h"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\NetMain.obj" : $(SOURCE) $(DEP_CPP_NETMA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetMain.sbr" : $(SOURCE) $(DEP_CPP_NETMA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\NetMain.obj" : $(SOURCE) $(DEP_CPP_NETMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetINet.cpp
DEP_CPP_NETIN=\
	".\Inc\Net.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Network\NetINet.h"\
	".\Windows\..\Network\NetPrv.h"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\NetINet.obj" : $(SOURCE) $(DEP_CPP_NETIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetINet.sbr" : $(SOURCE) $(DEP_CPP_NETIN) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\NetINet.obj" : $(SOURCE) $(DEP_CPP_NETIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\NetDPlay.cpp
DEP_CPP_NETDP=\
	".\Inc\Net.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Network\NetDPlay.h"\
	".\Windows\..\Network\NetPrv.h"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\NetDPlay.obj" : $(SOURCE) $(DEP_CPP_NETDP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetDPlay.sbr" : $(SOURCE) $(DEP_CPP_NETDP) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\NetDPlay.obj" : $(SOURCE) $(DEP_CPP_NETDP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Network\Network.rc
DEP_RSC_NETWO=\
	".\Network\Res\X_wix1.bmp"\
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
	".\Inc\Net.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\NetWin.obj" : $(SOURCE) $(DEP_CPP_NETWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetWin.sbr" : $(SOURCE) $(DEP_CPP_NETWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\NetWin.obj" : $(SOURCE) $(DEP_CPP_NETWI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

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
	".\Inc\Net.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Network\NetINet.h"\
	".\Windows\..\Network\NetPrv.h"\
	

!IF  "$(CFG)" == "Network - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\NetTeln.obj" : $(SOURCE) $(DEP_CPP_NETTE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetTeln.sbr" : $(SOURCE) $(DEP_CPP_NETTE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"


"$(INTDIR)\NetTeln.obj" : $(SOURCE) $(DEP_CPP_NETTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

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
DEP_CPP_UNZON=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnZone.obj" : $(SOURCE) $(DEP_CPP_UNZON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdAct.cpp
DEP_CPP_UNEDA=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdAct.obj" : $(SOURCE) $(DEP_CPP_UNEDA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCsg.cpp
DEP_CPP_UNEDC=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCsg.obj" : $(SOURCE) $(DEP_CPP_UNEDC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnBsp.cpp
DEP_CPP_UNBSP=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnBsp.obj" : $(SOURCE) $(DEP_CPP_UNBSP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCnst.cpp
DEP_CPP_UNEDCN=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCnst.obj" : $(SOURCE) $(DEP_CPP_UNEDCN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdSrv.cpp
DEP_CPP_UNEDS=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdSrv.obj" : $(SOURCE) $(DEP_CPP_UNEDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdTran.cpp
DEP_CPP_UNEDT=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdTran.obj" : $(SOURCE) $(DEP_CPP_UNEDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnShadow.cpp
DEP_CPP_UNSHA=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnShadow.obj" : $(SOURCE) $(DEP_CPP_UNSHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEditor.cpp
DEP_CPP_UNEDIT=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEditor.obj" : $(SOURCE) $(DEP_CPP_UNEDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdScan.cpp
DEP_CPP_UNEDSC=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdScan.obj" : $(SOURCE) $(DEP_CPP_UNEDSC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCam.cpp
DEP_CPP_UNEDCA=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnEdCam.obj" : $(SOURCE) $(DEP_CPP_UNEDCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnMeshEd.cpp
DEP_CPP_UNMESH=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnMeshEd.obj" : $(SOURCE) $(DEP_CPP_UNMESH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnTexEd.cpp
DEP_CPP_UNTEXE=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTexEd.obj" : $(SOURCE) $(DEP_CPP_UNTEXE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
DEP_CPP_UNTMA=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTMapC.obj" : $(SOURCE) $(DEP_CPP_UNTMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnEdRend.cpp
DEP_CPP_UNEDR=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	".\Render\UnLine.cpp"\
	".\Render\UnLine1.cpp"\
	

"$(INTDIR)\UnEdRend.obj" : $(SOURCE) $(DEP_CPP_UNEDR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRaster.cpp
DEP_CPP_UNRAS=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRaster.obj" : $(SOURCE) $(DEP_CPP_UNRAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRender.cpp
DEP_CPP_UNREND=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnRender.obj" : $(SOURCE) $(DEP_CPP_UNREND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSpan.cpp
DEP_CPP_UNSPA=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnSpan.obj" : $(SOURCE) $(DEP_CPP_UNSPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSprite.cpp
DEP_CPP_UNSPR=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnRaster.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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

SOURCE=.\Render\UnTMap.asm

!IF  "$(CFG)" == "Render - Win32 Release"

# Begin Custom Build
IntDir=.\Render\Release
InputPath=.\Render\UnTMap.asm

"$(IntDir)\UnTmap.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnTmap.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

# Begin Custom Build
IntDir=.\Render\Debug
InputPath=.\Render\UnTMap.asm

"$(IntDir)\UnTmapA.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /Fo$(IntDir)\UnTmapA.obj /c /coff /Cp /Cx $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnMiscC.c

!IF  "$(CFG)" == "Render - Win32 Release"

# ADD CPP /MD

"$(INTDIR)\UnMiscC.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W3 /WX /GX /O2 /Ob2 /I "Inc" /D "ASM" /D\
 "COMPILING_RENDER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /QIfdiv- /QIfdiv- /GM\
 /GM /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"


"$(INTDIR)\UnMiscC.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /G5 /MD /W3 /WX /GX /Zi /Od /Gf /Gy /I "Inc" /D "ASM" /D\
 "COMPILING_RENDER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Render.pch" /YX"Unreal.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QIfdiv- /GM /c $(SOURCE)


!ENDIF 

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
DEP_CPP_UNLIG=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnLight.obj" : $(SOURCE) $(DEP_CPP_UNLIG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnBlastC.cpp
DEP_CPP_UNBLA=\
	".\Inc\Root.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
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
DEP_CPP_UNTES=\
	".\Inc\UnActor.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnSpan.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnVGame.h"\
	

"$(INTDIR)\UnTest.obj" : $(SOURCE) $(DEP_CPP_UNTES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section UnWn : {32F3286C-1F43-11CF-98C0-0000C06958A7}
# 	2:20:UnrealTextureBrowser:Palette support added
# End Section
################################################################################
