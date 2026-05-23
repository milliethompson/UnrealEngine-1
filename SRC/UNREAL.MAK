# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
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
!MESSAGE NMAKE /f "UNREAL.MAK" CFG="Render - Win32 Debug"
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

ALL : "$(OUTDIR)\UnEngine.dll"

CLEAN : 
	-@erase "..\System\UnEngine.dll"
	-@erase ".\Engine\Release\UnServer.obj"
	-@erase ".\Engine\Release\UnCamMgr.obj"
	-@erase ".\Engine\Release\UnKeyVal.obj"
	-@erase ".\Engine\Release\UnAction.obj"
	-@erase ".\Engine\Release\UnClass.obj"
	-@erase ".\Engine\Release\UnParse.obj"
	-@erase ".\Engine\Release\UnModel.obj"
	-@erase ".\Engine\Release\UnCache.obj"
	-@erase ".\Engine\Release\UnCheat.obj"
	-@erase ".\Engine\Release\UnConfig.obj"
	-@erase ".\Engine\Release\UnLevel.obj"
	-@erase ".\Engine\Release\UnActLst.obj"
	-@erase ".\Engine\Release\UnSound.obj"
	-@erase ".\Engine\Release\UnChecks.obj"
	-@erase ".\Engine\Release\UnInput.obj"
	-@erase ".\Engine\Release\UnParams.obj"
	-@erase ".\Engine\Release\UnEngine.obj"
	-@erase ".\Engine\Release\UnPrefer.obj"
	-@erase ".\Engine\Release\UnDeflts.obj"
	-@erase ".\Engine\Release\UnMesh.obj"
	-@erase ".\Engine\Release\UnLevAct.obj"
	-@erase ".\Engine\Release\UnInputP.obj"
	-@erase ".\Engine\Release\UnTex.obj"
	-@erase ".\Engine\Release\UnFGAud.obj"
	-@erase ".\Engine\Release\UnScrRTL.obj"
	-@erase ".\Engine\Release\UnGfx.obj"
	-@erase ".\Engine\Release\UnCamera.obj"
	-@erase ".\Engine\Release\UnRes.obj"
	-@erase ".\Engine\Release\UnFile.obj"
	-@erase ".\Engine\Release\UnClsPrp.obj"
	-@erase ".\Engine\Release\UnScript.obj"
	-@erase ".\Engine\Release\UnLevTic.obj"
	-@erase ".\Engine\Release\UnMath.obj"
	-@erase ".\Engine\Release\UnMem.obj"
	-@erase ".\Engine\Release\UnOutDev.obj"
	-@erase ".\Engine\Release\UnPhys.obj"
	-@erase ".\Engine\Release\UnTopics.obj"
	-@erase ".\Engine\Release\UnDynBSP.obj"
	-@erase ".\Engine\Release\UnFPoly.obj"
	-@erase ".\Engine\Release\Engine.res"
	-@erase "..\System\UnEngine.ilk"
	-@erase ".\Lib\UnEngine.lib"
	-@erase ".\Lib\UnEngine.exp"

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
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\UnEngine.dll"
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/UnEngine.pdb" /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\UnEngine.dll" /implib:"$(OUTDIR)/UnEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnServer.obj" \
	"$(INTDIR)/UnCamMgr.obj" \
	"$(INTDIR)/UnKeyVal.obj" \
	"$(INTDIR)/UnAction.obj" \
	"$(INTDIR)/UnClass.obj" \
	"$(INTDIR)/UnParse.obj" \
	"$(INTDIR)/UnModel.obj" \
	"$(INTDIR)/UnCache.obj" \
	"$(INTDIR)/UnCheat.obj" \
	"$(INTDIR)/UnConfig.obj" \
	"$(INTDIR)/UnLevel.obj" \
	"$(INTDIR)/UnActLst.obj" \
	"$(INTDIR)/UnSound.obj" \
	"$(INTDIR)/UnChecks.obj" \
	"$(INTDIR)/UnInput.obj" \
	"$(INTDIR)/UnParams.obj" \
	"$(INTDIR)/UnEngine.obj" \
	"$(INTDIR)/UnPrefer.obj" \
	"$(INTDIR)/UnDeflts.obj" \
	"$(INTDIR)/UnMesh.obj" \
	"$(INTDIR)/UnLevAct.obj" \
	"$(INTDIR)/UnInputP.obj" \
	"$(INTDIR)/UnTex.obj" \
	"$(INTDIR)/UnFGAud.obj" \
	"$(INTDIR)/UnScrRTL.obj" \
	"$(INTDIR)/UnGfx.obj" \
	"$(INTDIR)/UnCamera.obj" \
	"$(INTDIR)/UnRes.obj" \
	"$(INTDIR)/UnFile.obj" \
	"$(INTDIR)/UnClsPrp.obj" \
	"$(INTDIR)/UnScript.obj" \
	"$(INTDIR)/UnLevTic.obj" \
	"$(INTDIR)/UnMath.obj" \
	"$(INTDIR)/UnMem.obj" \
	"$(INTDIR)/UnOutDev.obj" \
	"$(INTDIR)/UnPhys.obj" \
	"$(INTDIR)/UnTopics.obj" \
	"$(INTDIR)/UnDynBSP.obj" \
	"$(INTDIR)/UnFPoly.obj" \
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
	-@erase ".\Engine\Debug\UnCache.obj"
	-@erase ".\Engine\Debug\UnCheat.obj"
	-@erase ".\Engine\Debug\UnMath.obj"
	-@erase ".\Engine\Debug\UnEngine.obj"
	-@erase ".\Engine\Debug\UnOutDev.obj"
	-@erase ".\Engine\Debug\UnTopics.obj"
	-@erase ".\Engine\Debug\UnPhys.obj"
	-@erase ".\Engine\Debug\UnCamMgr.obj"
	-@erase ".\Engine\Debug\UnAction.obj"
	-@erase ".\Engine\Debug\UnPrefer.obj"
	-@erase ".\Engine\Debug\UnLevAct.obj"
	-@erase ".\Engine\Debug\UnInputP.obj"
	-@erase ".\Engine\Debug\UnScrRTL.obj"
	-@erase ".\Engine\Debug\UnCamera.obj"
	-@erase ".\Engine\Debug\UnClass.obj"
	-@erase ".\Engine\Debug\UnParse.obj"
	-@erase ".\Engine\Debug\UnTex.obj"
	-@erase ".\Engine\Debug\UnConfig.obj"
	-@erase ".\Engine\Debug\UnModel.obj"
	-@erase ".\Engine\Debug\UnClsPrp.obj"
	-@erase ".\Engine\Debug\UnScript.obj"
	-@erase ".\Engine\Debug\UnLevTic.obj"
	-@erase ".\Engine\Debug\UnActLst.obj"
	-@erase ".\Engine\Debug\UnGfx.obj"
	-@erase ".\Engine\Debug\UnRes.obj"
	-@erase ".\Engine\Debug\UnDynBSP.obj"
	-@erase ".\Engine\Debug\UnFGAud.obj"
	-@erase ".\Engine\Debug\UnFile.obj"
	-@erase ".\Engine\Debug\UnServer.obj"
	-@erase ".\Engine\Debug\UnLevel.obj"
	-@erase ".\Engine\Debug\UnKeyVal.obj"
	-@erase ".\Engine\Debug\UnSound.obj"
	-@erase ".\Engine\Debug\UnMem.obj"
	-@erase ".\Engine\Debug\UnInput.obj"
	-@erase ".\Engine\Debug\UnDeflts.obj"
	-@erase ".\Engine\Debug\UnMesh.obj"
	-@erase ".\Engine\Debug\UnFPoly.obj"
	-@erase ".\Engine\Debug\UnChecks.obj"
	-@erase ".\Engine\Debug\UnParams.obj"
	-@erase ".\Engine\Debug\Engine.res"
	-@erase "..\System\dEngine.ilk"
	-@erase ".\Lib\dEngine.lib"
	-@erase ".\Lib\dEngine.exp"
	-@erase ".\Lib\dEngine.pdb"

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
# ADD LINK32 Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x1200000 /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBC" /out:"..\System\dEngine.dll"
LINK32_FLAGS=Lib\SoundEng.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 /nologo /base:0x1200000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dEngine.pdb" /debug /machine:I386 /nodefaultlib:"LIBC"\
 /out:"..\System\dEngine.dll" /implib:"$(OUTDIR)/dEngine.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnCache.obj" \
	"$(INTDIR)/UnCheat.obj" \
	"$(INTDIR)/UnMath.obj" \
	"$(INTDIR)/UnEngine.obj" \
	"$(INTDIR)/UnOutDev.obj" \
	"$(INTDIR)/UnTopics.obj" \
	"$(INTDIR)/UnPhys.obj" \
	"$(INTDIR)/UnCamMgr.obj" \
	"$(INTDIR)/UnAction.obj" \
	"$(INTDIR)/UnPrefer.obj" \
	"$(INTDIR)/UnLevAct.obj" \
	"$(INTDIR)/UnInputP.obj" \
	"$(INTDIR)/UnScrRTL.obj" \
	"$(INTDIR)/UnCamera.obj" \
	"$(INTDIR)/UnClass.obj" \
	"$(INTDIR)/UnParse.obj" \
	"$(INTDIR)/UnTex.obj" \
	"$(INTDIR)/UnConfig.obj" \
	"$(INTDIR)/UnModel.obj" \
	"$(INTDIR)/UnClsPrp.obj" \
	"$(INTDIR)/UnScript.obj" \
	"$(INTDIR)/UnLevTic.obj" \
	"$(INTDIR)/UnActLst.obj" \
	"$(INTDIR)/UnGfx.obj" \
	"$(INTDIR)/UnRes.obj" \
	"$(INTDIR)/UnDynBSP.obj" \
	"$(INTDIR)/UnFGAud.obj" \
	"$(INTDIR)/UnFile.obj" \
	"$(INTDIR)/UnServer.obj" \
	"$(INTDIR)/UnLevel.obj" \
	"$(INTDIR)/UnKeyVal.obj" \
	"$(INTDIR)/UnSound.obj" \
	"$(INTDIR)/UnMem.obj" \
	"$(INTDIR)/UnInput.obj" \
	"$(INTDIR)/UnDeflts.obj" \
	"$(INTDIR)/UnMesh.obj" \
	"$(INTDIR)/UnFPoly.obj" \
	"$(INTDIR)/UnChecks.obj" \
	"$(INTDIR)/UnParams.obj" \
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
	-@erase ".\Game\Release\UnRandom.obj"
	-@erase ".\Game\Release\UnWeapon.obj"
	-@erase ".\Game\Release\UnFActor.obj"
	-@erase ".\Game\Release\UnKeyPts.obj"
	-@erase ".\Game\Release\UnPlayer.obj"
	-@erase ".\Game\Release\UnTrig.obj"
	-@erase ".\Game\Release\UnPawn.obj"
	-@erase ".\Game\Release\UnPawns.obj"
	-@erase ".\Game\Release\UnMover.obj"
	-@erase ".\Game\Release\AiAnimat.obj"
	-@erase ".\Game\Release\UnThings.obj"
	-@erase ".\Game\Release\UnActPro.obj"
	-@erase ".\Game\Release\UnInv.obj"
	-@erase ".\Game\Release\UnCon.obj"
	-@erase ".\Game\Release\UnGame.obj"
	-@erase ".\Game\Release\UnDecora.obj"
	-@erase ".\Game\Release\UnRoot.obj"
	-@erase ".\Game\Release\UnGCheat.obj"
	-@erase ".\Game\Release\UnPyro.obj"
	-@erase ".\Game\Release\UnExplos.obj"
	-@erase ".\Game\Release\Game.res"
	-@erase "..\System\UnGame.ilk"
	-@erase ".\Lib\UnGame.lib"
	-@erase ".\Lib\UnGame.exp"

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
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnGame.dll"
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/UnGame.pdb"\
 /machine:I386 /out:"..\System\UnGame.dll" /implib:"$(OUTDIR)/UnGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnWeapon.obj" \
	"$(INTDIR)/UnFActor.obj" \
	"$(INTDIR)/UnKeyPts.obj" \
	"$(INTDIR)/UnPlayer.obj" \
	"$(INTDIR)/UnTrig.obj" \
	"$(INTDIR)/UnPawn.obj" \
	"$(INTDIR)/UnPawns.obj" \
	"$(INTDIR)/UnMover.obj" \
	"$(INTDIR)/AiAnimat.obj" \
	"$(INTDIR)/UnThings.obj" \
	"$(INTDIR)/UnActPro.obj" \
	"$(INTDIR)/UnInv.obj" \
	"$(INTDIR)/UnCon.obj" \
	"$(INTDIR)/UnGame.obj" \
	"$(INTDIR)/UnDecora.obj" \
	"$(INTDIR)/UnRoot.obj" \
	"$(INTDIR)/UnGCheat.obj" \
	"$(INTDIR)/UnPyro.obj" \
	"$(INTDIR)/UnExplos.obj" \
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
	-@erase ".\Game\Debug\UnRoot.obj"
	-@erase ".\Game\Debug\AiAnimat.obj"
	-@erase ".\Game\Debug\UnPawns.obj"
	-@erase ".\Game\Debug\UnRandom.obj"
	-@erase ".\Game\Debug\UnThings.obj"
	-@erase ".\Game\Debug\UnMover.obj"
	-@erase ".\Game\Debug\UnActPro.obj"
	-@erase ".\Game\Debug\UnDecora.obj"
	-@erase ".\Game\Debug\UnTrig.obj"
	-@erase ".\Game\Debug\UnCon.obj"
	-@erase ".\Game\Debug\UnGCheat.obj"
	-@erase ".\Game\Debug\UnExplos.obj"
	-@erase ".\Game\Debug\UnPawn.obj"
	-@erase ".\Game\Debug\UnPyro.obj"
	-@erase ".\Game\Debug\UnWeapon.obj"
	-@erase ".\Game\Debug\UnFActor.obj"
	-@erase ".\Game\Debug\UnKeyPts.obj"
	-@erase ".\Game\Debug\UnPlayer.obj"
	-@erase ".\Game\Debug\UnInv.obj"
	-@erase ".\Game\Debug\UnGame.obj"
	-@erase ".\Game\Debug\Game.res"
	-@erase "..\System\dGame.ilk"
	-@erase ".\Lib\dGame.lib"
	-@erase ".\Lib\dGame.exp"
	-@erase ".\Lib\dGame.pdb"

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
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dGame.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x13000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/dGame.pdb" /debug\
 /machine:I386 /out:"..\System\dGame.dll" /implib:"$(OUTDIR)/dGame.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnRoot.obj" \
	"$(INTDIR)/AiAnimat.obj" \
	"$(INTDIR)/UnPawns.obj" \
	"$(INTDIR)/UnRandom.obj" \
	"$(INTDIR)/UnThings.obj" \
	"$(INTDIR)/UnMover.obj" \
	"$(INTDIR)/UnActPro.obj" \
	"$(INTDIR)/UnDecora.obj" \
	"$(INTDIR)/UnTrig.obj" \
	"$(INTDIR)/UnCon.obj" \
	"$(INTDIR)/UnGCheat.obj" \
	"$(INTDIR)/UnExplos.obj" \
	"$(INTDIR)/UnPawn.obj" \
	"$(INTDIR)/UnPyro.obj" \
	"$(INTDIR)/UnWeapon.obj" \
	"$(INTDIR)/UnFActor.obj" \
	"$(INTDIR)/UnKeyPts.obj" \
	"$(INTDIR)/UnPlayer.obj" \
	"$(INTDIR)/UnInv.obj" \
	"$(INTDIR)/UnGame.obj" \
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

ALL : "$(OUTDIR)\UnServer.exe" ".\System\UnWn.tlb"

CLEAN : 
	-@erase ".\System\UnWn.tlb"
	-@erase "..\System\UnServer.exe"
	-@erase ".\Windows\Release\UnWnEdSv.obj"
	-@erase ".\Windows\Release\UnWn.pch"
	-@erase ".\Windows\Release\UnWnAudD.obj"
	-@erase ".\Windows\Release\UnWnSerD.obj"
	-@erase ".\Windows\Release\UnRage.obj"
	-@erase ".\Windows\Release\UnPswd.obj"
	-@erase ".\Windows\Release\UnWnDevD.obj"
	-@erase ".\Windows\Release\UnPlatfm.obj"
	-@erase ".\Windows\Release\UnWnSenD.obj"
	-@erase ".\Windows\Release\UnWnKeyD.obj"
	-@erase ".\Windows\Release\UnWn.obj"
	-@erase ".\Windows\Release\UnWnProp.obj"
	-@erase ".\Windows\Release\UnWnTime.obj"
	-@erase ".\Windows\Release\UnWnPref.obj"
	-@erase ".\Windows\Release\StdAfx.obj"
	-@erase ".\Windows\Release\UnWnAcCD.obj"
	-@erase ".\Windows\Release\UnTask.obj"
	-@erase ".\Windows\Release\UnWnActD.obj"
	-@erase ".\Windows\Release\UnRenDev.obj"
	-@erase ".\Windows\Release\UnWnDlg.obj"
	-@erase ".\Windows\Release\UnWnCam.obj"
	-@erase ".\Windows\Release\UnWn.res"
	-@erase "..\System\UnServer.ilk"

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
# ADD LINK32 Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /incremental:yes /machine:I386 /out:"..\System\UnServer.exe"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=Lib\UnRender.lib Lib\UnNet.lib Lib\UnEngine.lib Lib\UnEditor.lib\
 Lib\UnGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/UnServer.pdb" /machine:I386\
 /out:"..\System\UnServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)/UnWnEdSv.obj" \
	"$(INTDIR)/UnWnAudD.obj" \
	"$(INTDIR)/UnWnSerD.obj" \
	"$(INTDIR)/UnRage.obj" \
	"$(INTDIR)/UnPswd.obj" \
	"$(INTDIR)/UnWnDevD.obj" \
	"$(INTDIR)/UnPlatfm.obj" \
	"$(INTDIR)/UnWnSenD.obj" \
	"$(INTDIR)/UnWnKeyD.obj" \
	"$(INTDIR)/UnWn.obj" \
	"$(INTDIR)/UnWnProp.obj" \
	"$(INTDIR)/UnWnTime.obj" \
	"$(INTDIR)/UnWnPref.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/UnWnAcCD.obj" \
	"$(INTDIR)/UnTask.obj" \
	"$(INTDIR)/UnWnActD.obj" \
	"$(INTDIR)/UnRenDev.obj" \
	"$(INTDIR)/UnWnDlg.obj" \
	"$(INTDIR)/UnWnCam.obj" \
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

ALL : "$(OUTDIR)\dServer.exe" ".\System\UnWn.tlb"

CLEAN : 
	-@erase ".\Windows\Debug\vc40.pdb"
	-@erase ".\Windows\Debug\UnWn.pch"
	-@erase ".\System\UnWn.tlb"
	-@erase "..\System\dServer.exe"
	-@erase ".\Windows\Debug\UnWnDlg.obj"
	-@erase ".\Windows\Debug\UnWnCam.obj"
	-@erase ".\Windows\Debug\UnTask.obj"
	-@erase ".\Windows\Debug\UnWnActD.obj"
	-@erase ".\Windows\Debug\UnRenDev.obj"
	-@erase ".\Windows\Debug\UnWnEdSv.obj"
	-@erase ".\Windows\Debug\UnWnAudD.obj"
	-@erase ".\Windows\Debug\UnWnSerD.obj"
	-@erase ".\Windows\Debug\UnRage.obj"
	-@erase ".\Windows\Debug\UnPswd.obj"
	-@erase ".\Windows\Debug\UnWnDevD.obj"
	-@erase ".\Windows\Debug\UnPlatfm.obj"
	-@erase ".\Windows\Debug\UnWnSenD.obj"
	-@erase ".\Windows\Debug\UnWnKeyD.obj"
	-@erase ".\Windows\Debug\UnWnProp.obj"
	-@erase ".\Windows\Debug\UnWnTime.obj"
	-@erase ".\Windows\Debug\UnWnPref.obj"
	-@erase ".\Windows\Debug\StdAfx.obj"
	-@erase ".\Windows\Debug\UnWnAcCD.obj"
	-@erase ".\Windows\Debug\UnWn.obj"
	-@erase ".\Windows\Debug\UnWn.res"
	-@erase "..\System\dServer.ilk"
	-@erase ".\Lib\dServer.pdb"

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
# ADD LINK32 Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=Lib\dRender.lib Lib\dNet.lib Lib\dEngine.lib Lib\dEditor.lib\
 Lib\dGame.lib winmm.lib /nologo /base:0x410000 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/dServer.pdb" /debug /machine:I386\
 /nodefaultlib:"MSVCRT" /out:"..\System\dServer.exe" 
LINK32_OBJS= \
	"$(INTDIR)/UnWnDlg.obj" \
	"$(INTDIR)/UnWnCam.obj" \
	"$(INTDIR)/UnTask.obj" \
	"$(INTDIR)/UnWnActD.obj" \
	"$(INTDIR)/UnRenDev.obj" \
	"$(INTDIR)/UnWnEdSv.obj" \
	"$(INTDIR)/UnWnAudD.obj" \
	"$(INTDIR)/UnWnSerD.obj" \
	"$(INTDIR)/UnRage.obj" \
	"$(INTDIR)/UnPswd.obj" \
	"$(INTDIR)/UnWnDevD.obj" \
	"$(INTDIR)/UnPlatfm.obj" \
	"$(INTDIR)/UnWnSenD.obj" \
	"$(INTDIR)/UnWnKeyD.obj" \
	"$(INTDIR)/UnWnProp.obj" \
	"$(INTDIR)/UnWnTime.obj" \
	"$(INTDIR)/UnWnPref.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/UnWnAcCD.obj" \
	"$(INTDIR)/UnWn.obj" \
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

ALL : "$(OUTDIR)\UnNet.dll" ".\Lib\Network.bsc"

CLEAN : 
	-@erase ".\Lib\Network.bsc"
	-@erase ".\Network\Release\NetWin.sbr"
	-@erase ".\Network\Release\NetMain.sbr"
	-@erase ".\Network\Release\NetDPlay.sbr"
	-@erase ".\Network\Release\NetINet.sbr"
	-@erase ".\Network\Release\NetTeln.sbr"
	-@erase "..\System\UnNet.dll"
	-@erase ".\Network\Release\NetWin.obj"
	-@erase ".\Network\Release\NetMain.obj"
	-@erase ".\Network\Release\NetDPlay.obj"
	-@erase ".\Network\Release\NetINet.obj"
	-@erase ".\Network\Release\NetTeln.obj"
	-@erase ".\Network\Release\Network.res"
	-@erase "..\System\UnNet.ilk"
	-@erase ".\Lib\UnNet.lib"
	-@erase ".\Lib\UnNet.exp"

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
	"$(INTDIR)/NetWin.sbr" \
	"$(INTDIR)/NetMain.sbr" \
	"$(INTDIR)/NetDPlay.sbr" \
	"$(INTDIR)/NetINet.sbr" \
	"$(INTDIR)/NetTeln.sbr"

".\Lib\Network.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
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
	"$(INTDIR)/NetWin.obj" \
	"$(INTDIR)/NetMain.obj" \
	"$(INTDIR)/NetDPlay.obj" \
	"$(INTDIR)/NetINet.obj" \
	"$(INTDIR)/NetTeln.obj" \
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
	-@erase ".\Network\Debug\NetDPlay.obj"
	-@erase ".\Network\Debug\NetTeln.obj"
	-@erase ".\Network\Debug\NetMain.obj"
	-@erase ".\Network\Debug\NetINet.obj"
	-@erase ".\Network\Debug\NetWin.obj"
	-@erase ".\Network\Debug\Network.res"
	-@erase "..\System\dNet.ilk"
	-@erase ".\Lib\dNet.lib"
	-@erase ".\Lib\dNet.exp"
	-@erase ".\Lib\dNet.pdb"

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
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib /nologo /base:0x14000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dNet.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib wsock32.lib\
 /nologo /base:0x14000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dNet.pdb" /debug /machine:I386 /out:"..\System\dNet.dll"\
 /implib:"$(OUTDIR)/dNet.lib" 
LINK32_OBJS= \
	"$(INTDIR)/NetDPlay.obj" \
	"$(INTDIR)/NetTeln.obj" \
	"$(INTDIR)/NetMain.obj" \
	"$(INTDIR)/NetINet.obj" \
	"$(INTDIR)/NetWin.obj" \
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
	-@erase ".\Editor\Release\UnEdScan.obj"
	-@erase ".\Editor\Release\UnEdSrv.obj"
	-@erase ".\Editor\Release\UnEdAct.obj"
	-@erase ".\Editor\Release\UnEdTran.obj"
	-@erase ".\Editor\Release\UnMeshEd.obj"
	-@erase ".\Editor\Release\UnEditor.obj"
	-@erase ".\Editor\Release\UnEdCam.obj"
	-@erase ".\Editor\Release\UnEdCnst.obj"
	-@erase ".\Editor\Release\UnZone.obj"
	-@erase ".\Editor\Release\UnEdCsg.obj"
	-@erase ".\Editor\Release\UnTexEd.obj"
	-@erase ".\Editor\Release\UnBsp.obj"
	-@erase ".\Editor\Release\UnShadow.obj"
	-@erase ".\Editor\Release\UnEditor.res"
	-@erase "..\System\UnEditor.ilk"
	-@erase ".\Lib\UnEditor.lib"
	-@erase ".\Lib\UnEditor.exp"

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
# ADD LINK32 Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnEditor.dll"
LINK32_FLAGS=Lib\UnEngine.lib Lib\UnRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/UnEditor.pdb"\
 /machine:I386 /out:"..\System\UnEditor.dll" /implib:"$(OUTDIR)/UnEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnEdScan.obj" \
	"$(INTDIR)/UnEdSrv.obj" \
	"$(INTDIR)/UnEdAct.obj" \
	"$(INTDIR)/UnEdTran.obj" \
	"$(INTDIR)/UnMeshEd.obj" \
	"$(INTDIR)/UnEditor.obj" \
	"$(INTDIR)/UnEdCam.obj" \
	"$(INTDIR)/UnEdCnst.obj" \
	"$(INTDIR)/UnZone.obj" \
	"$(INTDIR)/UnEdCsg.obj" \
	"$(INTDIR)/UnTexEd.obj" \
	"$(INTDIR)/UnBsp.obj" \
	"$(INTDIR)/UnShadow.obj" \
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
	-@erase ".\Editor\Debug\UnEdCam.obj"
	-@erase ".\Editor\Debug\UnEdTran.obj"
	-@erase ".\Editor\Debug\UnZone.obj"
	-@erase ".\Editor\Debug\UnEdCsg.obj"
	-@erase ".\Editor\Debug\UnTexEd.obj"
	-@erase ".\Editor\Debug\UnEdCnst.obj"
	-@erase ".\Editor\Debug\UnEdScan.obj"
	-@erase ".\Editor\Debug\UnEdSrv.obj"
	-@erase ".\Editor\Debug\UnMeshEd.obj"
	-@erase ".\Editor\Debug\UnEditor.obj"
	-@erase ".\Editor\Debug\UnShadow.obj"
	-@erase ".\Editor\Debug\UnBsp.obj"
	-@erase ".\Editor\Debug\UnEdAct.obj"
	-@erase ".\Editor\Debug\UnEditor.res"
	-@erase "..\System\dEditor.ilk"
	-@erase ".\Lib\dEditor.lib"
	-@erase ".\Lib\dEditor.exp"
	-@erase ".\Lib\dEditor.pdb"

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
# ADD LINK32 Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dEditor.dll"
LINK32_FLAGS=Lib\dEngine.lib Lib\dRender.lib /nologo /base:0x11000000\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/dEditor.pdb" /debug\
 /machine:I386 /out:"..\System\dEditor.dll" /implib:"$(OUTDIR)/dEditor.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnEdCam.obj" \
	"$(INTDIR)/UnEdTran.obj" \
	"$(INTDIR)/UnZone.obj" \
	"$(INTDIR)/UnEdCsg.obj" \
	"$(INTDIR)/UnTexEd.obj" \
	"$(INTDIR)/UnEdCnst.obj" \
	"$(INTDIR)/UnEdScan.obj" \
	"$(INTDIR)/UnEdSrv.obj" \
	"$(INTDIR)/UnMeshEd.obj" \
	"$(INTDIR)/UnEditor.obj" \
	"$(INTDIR)/UnShadow.obj" \
	"$(INTDIR)/UnBsp.obj" \
	"$(INTDIR)/UnEdAct.obj" \
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
	-@erase ".\Render\Release\UnSprite.obj"
	-@erase ".\Render\Release\UnSpan.obj"
	-@erase ".\Render\Release\UnLight.obj"
	-@erase ".\Render\Release\UnRaster.obj"
	-@erase ".\Render\Release\UnTMapC.obj"
	-@erase ".\Render\Release\UnTest.obj"
	-@erase ".\Render\Release\UnEdRend.obj"
	-@erase ".\Render\Release\UnRender.obj"
	-@erase ".\Render\Release\UnBlastC.obj"
	-@erase ".\Render\Release\UnMiscC.obj"
	-@erase ".\Render\Release\Render.res"
	-@erase ".\Render\Release\UnTmap.obj"
	-@erase ".\Render\Release\UnBlast.obj"
	-@erase "..\System\UnRender.ilk"
	-@erase ".\Lib\UnRender.lib"
	-@erase ".\Lib\UnRender.exp"

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
# ADD LINK32 Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /incremental:yes /machine:I386 /out:"..\System\UnRender.dll"
LINK32_FLAGS=Lib\UnEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/UnRender.pdb" /machine:I386 /out:"..\System\UnRender.dll"\
 /implib:"$(OUTDIR)/UnRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnSprite.obj" \
	"$(INTDIR)/UnSpan.obj" \
	"$(INTDIR)/UnLight.obj" \
	"$(INTDIR)/UnRaster.obj" \
	"$(INTDIR)/UnTMapC.obj" \
	"$(INTDIR)/UnTest.obj" \
	"$(INTDIR)/UnEdRend.obj" \
	"$(INTDIR)/UnRender.obj" \
	"$(INTDIR)/UnBlastC.obj" \
	"$(INTDIR)/UnMiscC.obj" \
	"$(INTDIR)/UnTmap.obj" \
	"$(INTDIR)/UnBlast.obj" \
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
	-@erase ".\Render\Debug\UnBlastC.obj"
	-@erase ".\Render\Debug\UnMiscC.obj"
	-@erase ".\Render\Debug\UnTest.obj"
	-@erase ".\Render\Debug\UnRaster.obj"
	-@erase ".\Render\Debug\UnLight.obj"
	-@erase ".\Render\Debug\UnEdRend.obj"
	-@erase ".\Render\Debug\UnTMapC.obj"
	-@erase ".\Render\Debug\UnSprite.obj"
	-@erase ".\Render\Debug\UnSpan.obj"
	-@erase ".\Render\Debug\UnRender.obj"
	-@erase ".\Render\Debug\Render.res"
	-@erase ".\Render\Debug\UnTmapA.obj"
	-@erase ".\Render\Debug\UnBlast.obj"
	-@erase "..\System\dRender.ilk"
	-@erase ".\Lib\dRender.lib"
	-@erase ".\Lib\dRender.exp"
	-@erase ".\Lib\dRender.pdb"

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
# ADD LINK32 Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x15000000 /subsystem:windows /dll /debug /machine:I386 /out:"..\System\dRender.dll"
LINK32_FLAGS=Lib\dEngine.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /base:0x15000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/dRender.pdb" /debug /machine:I386 /out:"..\System\dRender.dll"\
 /implib:"$(OUTDIR)/dRender.lib" 
LINK32_OBJS= \
	"$(INTDIR)/UnBlastC.obj" \
	"$(INTDIR)/UnMiscC.obj" \
	"$(INTDIR)/UnTest.obj" \
	"$(INTDIR)/UnRaster.obj" \
	"$(INTDIR)/UnLight.obj" \
	"$(INTDIR)/UnEdRend.obj" \
	"$(INTDIR)/UnTMapC.obj" \
	"$(INTDIR)/UnSprite.obj" \
	"$(INTDIR)/UnSpan.obj" \
	"$(INTDIR)/UnRender.obj" \
	"$(INTDIR)/UnTmapA.obj" \
	"$(INTDIR)/UnBlast.obj" \
	"$(INTDIR)/Render.res"

"$(OUTDIR)\dRender.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
	".\GAME\UNCON.H"\
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
	".\GAME\UNCON.H"\
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

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNT=\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	
NODEP_CPP_UNWNT=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnTime.obj" : $(SOURCE) $(DEP_CPP_UNWNT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNPSWD.H"\
	".\Inc\Net.h"\
	".\Inc\UnInput.h"\
	".\Inc\unaction.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\DDraw1.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnParse.h"\
	
NODEP_CPP_UNPLAT=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnPlatfm.obj" : $(SOURCE) $(DEP_CPP_UNPLAT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNPSWD.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnPlatfm.obj" : $(SOURCE) $(DEP_CPP_UNPLAT) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnPswd.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNPSW=\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNPSWD.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnPswd.obj" : $(SOURCE) $(DEP_CPP_UNPSW) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNPSW=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNPSWD.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnPswd.obj" : $(SOURCE) $(DEP_CPP_UNPSW) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnRage.cpp
DEP_CPP_UNRAG=\
	".\WINDOWS\STDAFX.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\Inc\UnRenDev.h"\
	".\WINDOWS\ATI3DCIF.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	".\Inc\DDraw1.h"\
	
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
	".\WINDOWS\STDAFX.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	
NODEP_CPP_UNREN=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnRenDev.obj" : $(SOURCE) $(DEP_CPP_UNREN) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\DDraw1.h"\
	
NODEP_CPP_UNTAS=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnTask.obj" : $(SOURCE) $(DEP_CPP_UNTAS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\Inc\Net.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\Inc\UnConfig.h"\
	".\WINDOWS\UNWNPROP.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\DDraw1.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnParse.h"\
	

"$(INTDIR)\UnWn.obj" : $(SOURCE) $(DEP_CPP_UNWN_) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNPROP.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\Inc\UnRenDev.h"\
	".\Inc\DMouse.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnConfig.h"\
	".\Inc\unaction.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\DDraw1.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnCam.obj" : $(SOURCE) $(DEP_CPP_UNWNC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\WINDOWS\UNWNCAM.H"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\DDraw1.h"\
	
NODEP_CPP_UNWND=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnDlg.obj" : $(SOURCE) $(DEP_CPP_UNWND) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNCAM.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	
NODEP_CPP_UNWNE=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnEdSv.obj" : $(SOURCE) $(DEP_CPP_UNWNE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnWnEdSv.obj" : $(SOURCE) $(DEP_CPP_UNWNE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\StdAfx.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_STDAF=\
	".\WINDOWS\STDAFX.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	
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

DEP_CPP_STDAF=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\STDAFX.H"\
	
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

"$(OUTDIR)\UnWn.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "System/UnWn.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

# ADD MTL /tlb "System/UnWn.tlb"

"$(OUTDIR)\UnWn.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "System/UnWn.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnActD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNA=\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNACTD.H"\
	".\WINDOWS\UNWNACCD.H"\
	".\WINDOWS\UNWNKEYD.H"\
	".\Inc\Unreal.h"\
	".\Inc\unaction.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnActD.obj" : $(SOURCE) $(DEP_CPP_UNWNA) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNACCD.H"\
	".\WINDOWS\UNWNACTD.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNKEYD.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDEVD.H"\
	".\Inc\UnInput.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnParse.h"\
	
NODEP_CPP_UNWNDE=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnDevD.obj" : $(SOURCE) $(DEP_CPP_UNWNDE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDEVD.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNPREF.H"\
	".\Inc\UnPrefer.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnPref.obj" : $(SOURCE) $(DEP_CPP_UNWNP) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNPREF.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNSEND.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnConfig.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnInputP.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnParse.h"\
	

"$(INTDIR)\UnWnSenD.obj" : $(SOURCE) $(DEP_CPP_UNWNS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNSEND.H"\
	

"$(INTDIR)\UnWnSenD.obj" : $(SOURCE) $(DEP_CPP_UNWNS) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnSerD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNSE=\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNSERD.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnWnSerD.obj" : $(SOURCE) $(DEP_CPP_UNWNSE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNSE=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNSERD.H"\
	

"$(INTDIR)\UnWnSerD.obj" : $(SOURCE) $(DEP_CPP_UNWNSE) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAudD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNAU=\
	".\WINDOWS\STDAFX.H"\
	".\Inc\Unreal.h"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNAUDD.H"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
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
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	
NODEP_CPP_UNWNAU=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnAudD.obj" : $(SOURCE) $(DEP_CPP_UNWNAU) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNAUDD.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNKEYD.H"\
	".\Inc\Unreal.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	
NODEP_CPP_UNWNK=\
	".\Windows\UnPort.h"\
	

"$(INTDIR)\UnWnKeyD.obj" : $(SOURCE) $(DEP_CPP_UNWNK) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNKEYD.H"\
	

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNPROP.H"\
	".\WINDOWS\UNWNPREF.H"\
	".\WINDOWS\UNWNACTD.H"\
	".\WINDOWS\UNWNSEND.H"\
	".\WINDOWS\UNWNDEVD.H"\
	".\WINDOWS\UNWNAUDD.H"\
	".\WINDOWS\UNWNSERD.H"\
	".\Inc\UnConfig.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\Inc\UnPrefer.h"\
	".\Inc\Unreal.h"\
	".\Inc\UnKeyVal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnParse.h"\
	".\Inc\unaction.h"\
	".\Inc\UnInput.h"\
	".\Inc\UnInputP.h"\
	

"$(INTDIR)\UnWnProp.obj" : $(SOURCE) $(DEP_CPP_UNWNPR) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

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
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNACTD.H"\
	".\WINDOWS\UNWNAUDD.H"\
	".\WINDOWS\UNWNDEVD.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	".\WINDOWS\UNWNPREF.H"\
	".\WINDOWS\UNWNPROP.H"\
	".\WINDOWS\UNWNSEND.H"\
	".\WINDOWS\UNWNSERD.H"\
	

"$(INTDIR)\UnWnProp.obj" : $(SOURCE) $(DEP_CPP_UNWNPR) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Windows\UnWnAcCD.cpp

!IF  "$(CFG)" == "Windows - Win32 Release"

DEP_CPP_UNWNAC=\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNACCD.H"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnWnAcCD.obj" : $(SOURCE) $(DEP_CPP_UNWNAC) "$(INTDIR)"\
 "$(INTDIR)\UnWn.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Windows - Win32 Debug"

DEP_CPP_UNWNAC=\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	".\WINDOWS\STDAFX.H"\
	".\WINDOWS\UNWN.H"\
	".\WINDOWS\UNWNACCD.H"\
	".\WINDOWS\UNWNDLG.H"\
	".\WINDOWS\UNWNEDSV.H"\
	

"$(INTDIR)\UnWnAcCD.obj" : $(SOURCE) $(DEP_CPP_UNWNAC) "$(INTDIR)"\
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
   CD F:\Projects\EMISSARY\PROG\SRC\Doc
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
	".\Network\DPlay.h"\
	".\Inc\Net.h"\
	".\Windows\..\Network\NetPrv.h"\
	".\Network\NetINet.h"\
	".\Network\NetDPlay.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	

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
	".\Network\DPlay.h"\
	".\Inc\Net.h"\
	".\Windows\..\Network\NetPrv.h"\
	".\Network\NetDPlay.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnPort.h"\
	

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

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNZON=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnZone.obj" : $(SOURCE) $(DEP_CPP_UNZON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNZON=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnZone.obj" : $(SOURCE) $(DEP_CPP_UNZON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdAct.cpp
DEP_CPP_UNEDA=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEdAct.obj" : $(SOURCE) $(DEP_CPP_UNEDA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCsg.cpp
DEP_CPP_UNEDC=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEdCsg.obj" : $(SOURCE) $(DEP_CPP_UNEDC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnBsp.cpp
DEP_CPP_UNBSP=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnBsp.obj" : $(SOURCE) $(DEP_CPP_UNBSP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCnst.cpp
DEP_CPP_UNEDCN=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEdCnst.obj" : $(SOURCE) $(DEP_CPP_UNEDCN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdSrv.cpp
DEP_CPP_UNEDS=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnEdSrv.obj" : $(SOURCE) $(DEP_CPP_UNEDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdTran.cpp
DEP_CPP_UNEDT=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEdTran.obj" : $(SOURCE) $(DEP_CPP_UNEDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnShadow.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNSHA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnShadow.obj" : $(SOURCE) $(DEP_CPP_UNSHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNSHA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnDynBsp.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnShadow.obj" : $(SOURCE) $(DEP_CPP_UNSHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEditor.cpp
DEP_CPP_UNEDIT=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEditor.obj" : $(SOURCE) $(DEP_CPP_UNEDIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdScan.cpp
DEP_CPP_UNEDSC=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

"$(INTDIR)\UnEdScan.obj" : $(SOURCE) $(DEP_CPP_UNEDSC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnEdCam.cpp
DEP_CPP_UNEDCA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnEdCam.obj" : $(SOURCE) $(DEP_CPP_UNEDCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnMeshEd.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

DEP_CPP_UNMESH=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnMeshEd.obj" : $(SOURCE) $(DEP_CPP_UNMESH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

DEP_CPP_UNMESH=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnMeshEd.obj" : $(SOURCE) $(DEP_CPP_UNMESH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor\UnTexEd.cpp
DEP_CPP_UNTEXE=\
	".\Inc\Unreal.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnLevel.h"\
	".\Inc\UnCamera.h"\
	".\Inc\UnFGAud.h"\
	".\Inc\UnSound.h"\
	".\Inc\UnGfx.h"\
	".\Inc\UnServer.h"\
	".\Inc\UnDeflts.h"\
	".\Inc\UnEngine.h"\
	".\Inc\UnVGame.h"\
	".\Inc\UnEditor.h"\
	".\Inc\UnEdTran.h"\
	".\Inc\UnMsgs.h"\
	".\Inc\UnArray.h"\
	

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

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNTMA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnTMapC.obj" : $(SOURCE) $(DEP_CPP_UNTMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnEdRend.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNEDR=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\RENDER\UNLINE1.CPP"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	".\RENDER\UNLINE.CPP"\
	

"$(INTDIR)\UnEdRend.obj" : $(SOURCE) $(DEP_CPP_UNEDR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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
	".\RENDER\UNLINE.CPP"\
	".\RENDER\UNLINE1.CPP"\
	

"$(INTDIR)\UnEdRend.obj" : $(SOURCE) $(DEP_CPP_UNEDR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRaster.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNRAS=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRaster.obj" : $(SOURCE) $(DEP_CPP_UNRAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnRender.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNREND=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\Inc\UnRenDev.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnRender.obj" : $(SOURCE) $(DEP_CPP_UNREND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSpan.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNSPA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnSpan.obj" : $(SOURCE) $(DEP_CPP_UNSPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnSprite.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNSPR=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnRaster.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnSprite.obj" : $(SOURCE) $(DEP_CPP_UNSPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

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

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNLIG=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnLight.obj" : $(SOURCE) $(DEP_CPP_UNLIG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnBlastC.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNBLA=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\Root.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnBlastC.obj" : $(SOURCE) $(DEP_CPP_UNBLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Render\UnTest.cpp

!IF  "$(CFG)" == "Render - Win32 Release"

DEP_CPP_UNTES=\
	".\Inc\Unreal.h"\
	".\Inc\UnRender.h"\
	".\Inc\UnBuild.h"\
	".\Inc\UnPort.h"\
	".\Inc\UnPlatfm.h"\
	".\Inc\UnFile.h"\
	".\Inc\UnMath.h"\
	".\Inc\UnParams.h"\
	".\Inc\UnCache.h"\
	".\Inc\UnMem.h"\
	".\Inc\UnResTyp.h"\
	".\Inc\UnRes.h"\
	".\Inc\UnTopics.h"\
	".\Inc\UnActor.h"\
	".\Inc\UnSpan.h"\
	

"$(INTDIR)\UnTest.obj" : $(SOURCE) $(DEP_CPP_UNTES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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
