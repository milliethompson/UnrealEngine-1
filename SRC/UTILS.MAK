# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Install - Win32 Release
!MESSAGE No configuration specified.  Defaulting to Install - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Install - Win32 Release" && "$(CFG)" !=\
 "Launcher - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Utils.mak" CFG="Install - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Install - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Launcher - Win32 Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Install - Win32 Release"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Install - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Install\Install_"
# PROP BASE Intermediate_Dir "Install\Install_"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Install\Release"
# PROP Intermediate_Dir "Install\Release"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Release
INTDIR=.\Install\Release

ALL : "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "..\System\Install.exe"
	-@erase ".\Install\Release\InDX.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /WX /GX /O1 /I "Inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_OBJS=.\Install\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Install\DSetup.lib kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386 /out:"..\System\Install.exe"
LINK32_FLAGS=Install\DSetup.lib kernel32.lib user32.lib gdi32.lib advapi32.lib\
 /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/Install.pdb"\
 /machine:I386 /out:"..\System\Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)/InDX.obj"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Launcher - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Launcher\Launcher"
# PROP BASE Intermediate_Dir "Launcher\Launcher"
# PROP BASE Target_Dir "Launcher"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Launcher\Release"
# PROP Intermediate_Dir "Launcher\Release"
# PROP Target_Dir "Launcher"
OUTDIR=.\Launcher\Release
INTDIR=.\Launcher\Release

ALL : "$(OUTDIR)\Unreal.exe"

CLEAN : 
	-@erase "..\Unreal.exe"
	-@erase ".\Launcher\Release\UnLaunch.obj"
	-@erase ".\Launcher\Release\UnLaunch.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /WX /GX /O1 /I "Inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_OBJS=.\Launcher\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Launcher.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /machine:I386 /out:"..\Unreal.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Unreal.pdb" /machine:I386 /out:"..\Unreal.exe" 
LINK32_OBJS= \
	"$(INTDIR)/UnLaunch.obj" \
	"$(INTDIR)/UnLaunch.res"

"$(OUTDIR)\Unreal.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

CPP_PROJ=/nologo /MT /W3 /WX /GX /O1 /I "Inc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /c 

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

MTL_PROJ=/nologo /D "NDEBUG" /win32 
################################################################################
# Begin Target

# Name "Install - Win32 Release"
################################################################################
# Begin Source File

SOURCE=.\Install\InDX.cpp
DEP_CPP_INDX_=\
	".\Inc\DSetup.h"\
	

"$(INTDIR)\InDX.obj" : $(SOURCE) $(DEP_CPP_INDX_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"
# End Source File
# End Target
################################################################################
# Begin Target

# Name "Launcher - Win32 Release"
################################################################################
# Begin Source File

SOURCE=.\Launcher\UnLaunch.rc
DEP_RSC_UNLAU=\
	".\Launcher\..\Res\Unreal.ico"\
	

"$(INTDIR)\UnLaunch.res" : $(SOURCE) $(DEP_RSC_UNLAU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/UnLaunch.res" /i "Launcher" /d "NDEBUG"\
 $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Launcher\UnLaunch.cpp
DEP_CPP_UNLAUN=\
	".\Inc\UnBuild.h"\
	

"$(INTDIR)\UnLaunch.obj" : $(SOURCE) $(DEP_CPP_UNLAUN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Doc\!ReadMe.txt"
# End Source File
# End Target
# End Project
################################################################################
