/*=============================================================================
	UnBuild.h: Unreal build settings

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	This file contains all settings and options related to a particular build
	of Unreal.
=============================================================================*/

#ifndef _INC_UNBUILD /* Prevent multiple inclusion */
#define _INC_UNBUILD

/*-----------------------------------------------------------------------------
	Notes on other defines

Any of the following must be defined in each project's standard definitions,
rather than in this shared header:

RELEASE				If this is a release version
_DEBUG				If this is a debug version

COMPILING_GAME		If compiling UnGame.dll
COMPILING_ENGINE	If compiling UnEngine.dll
COMPILING_RENDER	If compiling UnRender.dll
COMPILING_WINDOWS	If compiling UnServer.exe
COMPILING_NETWORK	If compiling UnNet.dll
COMPILING_EDITOR	If compiling UnEditor.dll
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Major compile options
-----------------------------------------------------------------------------*/

//
// Runtime checks.
//
#ifndef PARANOID
	#define	PARANOID		0	/* Perform slow, expensive validity checks */
#endif

#ifndef STATS
	#define STATS			1	/* Support rendering statistics */
#endif

#ifndef NO_CLOCK
	#define NO_CLOCK		0	/* Don't perform simple timing of critical loops */
#endif

#ifndef NO_SLOW_CLOCK
	#define NO_SLOW_CLOCK	1	/* Don't perform CPU-intense timing of critical loops */
#endif

#ifndef NO_GUARD
	#define NO_GUARD		0	/* Don't trap errors */
#endif

#ifndef NO_SLOW_GUARD
	#define NO_SLOW_GUARD	0	/* Don't trap errors where that would slow down the code significantly */
#endif

//
// Modules to compile.
//
#define EDITOR				1	/* Include Unreal editor code */

//
// Code to generate.
//
#define ASM					1	/* Use Intel assembler code */

/*-----------------------------------------------------------------------------
	Defines for all platforms
-----------------------------------------------------------------------------*/

//
// Names and version numbers:
//
#define GAME_NAME		"Unreal"
#define ENGINE_NAME		"Unreal Engine"
#define CAMERA_NAME		"Unreal Camera"

#define GAME_VERSION	"0.84"
#define ENGINE_VERSION  "0.84"

#define CONSOLE_SPAWN_1	"UnrealServer " ENGINE_VERSION
#define CONSOLE_SPAWN_2	"Copyright 1996 Epic MegaGames, Inc."

//
// Gatekeeper information.
//
#define GK_VER		    "10"
#define GK_APP          "Unreal"
#define GK_APP_VER		"84"

//
// Resource file version number.  
// Prevents outdated resource files from being loaded.
//
#define RES_FILE_VERSION (0x000A)
#define RES_FILE_TAG     "Unreal Resource\x1A"

//
// Hardcoded filenames, paths, and extensions:
//
#define DEFAULT_STARTUP_FNAME	"..\\Maps\\Unreal.unr"
#define DEFAULT_CLASS_FNAME		"..\\Classes\\Unreal.ucx"
#define DEFAULT_PALETTE_FNAME	"..\\Graphics\\Palette.pcx"
#define WEB_LINK_FNAME			"http://www.epicgames.com/"
#define HELP_LINK_FNAME			"..\\Help\\Unreal.hlp"
#define EDITOR_FNAME			"..\\UnrealEd.exe"
#define PROFILE_RELATIVE_FNAME	"\\Unreal.ini"
#define FACTORY_PROFILE_RELATIVE_FNAME "\\Default.ini"
#define HELP_RELATIVE_FNAME		"\\..\\Help\\Unreal.hlp"
#define MAP_RELATIVE_PATH		"..\\Maps\\"				
#define CLASS_BOOTSTRAP_FNAME	"..\\Classes\\Classes.mac"
#define GFX_BOOTSTRAP_FNAME		"..\\Graphics\\Graphics.mac"
#define TYPELIB_PARTIAL			"Unreal.tlb"
#define LOG_PARTIAL				"Unreal.log"
#define LAUNCH_PARTIAL			"Unreal.exe"
#define EDITOR_PARTIAL			"UnrealEd.exe"
#define ENGINE_PARTIAL			"UnServer.exe"
#define SYSTEM_PARTIAL			"System"

#ifndef _DEBUG
	#define GAME_DLL			"UnGame.dll"
	#define MFC_HELP_PARTIAL	"UNSERVER.HLP" /* Must be all caps */
#else
	#define GAME_DLL			"dGame.dll"
	#define MFC_HELP_PARTIAL	"DSERVER.HLP" /* Must be all caps */
#endif

//
// URL's
//
#define URL_WEB					"http://www.epicgames.com/"
#define URL_GAME				"unreal://unreal.epicgames.com/"
#define URL_UNAVAILABLE			"..\\Help\\Unreal.htm"

/*-----------------------------------------------------------------------------
	Windows 95 settings
-----------------------------------------------------------------------------*/

//
// For shell, Ole, and file dialogs:
//
#define MAP_EXTENSION			".unr"
#define OLE_APP					"Unreal"
#define OLE_APP_DESCRIPTION		"Unreal Engine"
#define OLE_MAP_TYPE			"Unreal.Level"
#define OLE_MAP_DESCRIPTION		"Unreal Level"
#define MIME_MAP_TYPE			"application/unreal"
#define PLAY_COMMAND			"&Play this Unreal level"
#define EDIT_COMMAND			"&Edit with UnrealEd"
#define LOAD_MAP_MASK			"Unreal maps (*.unr) | *.unr| All Files (*.*) | *.* ||"
#define SAVE_MAP_MASK			"Unreal maps (*.unr) | *.unr| All Files (*.*) | *.* ||"

//
// Registry usage:
//
#define REGISTRY_KEY_BASE		"Software"
#define REGISTRY_KEY_COMPANY	"Epic MegaGames"
#define REGISTRY_KEY_PRODUCT	"Unreal"

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
#endif // _INC_UNBUILD

