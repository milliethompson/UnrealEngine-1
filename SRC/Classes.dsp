# Microsoft Developer Studio Project File - Name="Classes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Classes - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Classes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Classes.mak" CFG="Classes - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Classes - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Classes\Release"
# PROP BASE Intermediate_Dir ".\Classes\Release"
# PROP BASE Target_Dir ".\Classes"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Classes\Release"
# PROP Intermediate_Dir ".\Classes\Release"
# PROP Target_Dir ".\Classes"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# Begin Target

# Name "Classes - Win32 Release"
# Begin Group "Unreal Macros"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Classes\Classes.mac
# End Source File
# End Group
# Begin Source File

SOURCE=..\Classes\Actor.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Decor.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Explos.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Info.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Inv.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Keypoint.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Light.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Model.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Mover.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Object.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Pawn.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Proj.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Pyrotech.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Texture.u
# End Source File
# Begin Source File

SOURCE=..\Classes\Trigger.u
# End Source File
# End Target
# End Project
