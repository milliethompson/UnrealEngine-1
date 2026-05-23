/*=============================================================================
	UnBuild.h: Unreal build settings.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	This file contains all settings and options related to a particular build
	of Unreal.
=============================================================================*/

#ifndef _INC_UNBUILD /* Prevent multiple inclusion */
#define _INC_UNBUILD

/*-----------------------------------------------------------------------------
	Notes on other defines.

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
	Major compile options.
-----------------------------------------------------------------------------*/

// Runtime checks to perform.
#ifndef CHECK_ALL
#define	CHECK_ALL		0	/* Perform pretty slow validity checks? */
#endif

#ifndef CHECK_ALLOCS
#define CHECK_ALLOCS	0	/* Check for malloc memory leaks */
#endif

#ifndef STATS
#define STATS			1	/* Support rendering statistics? */
#endif

// Runtime timing.
#ifndef DO_CLOCK
#define DO_CLOCK		1	/* Perform simple timing of critical loops? */
#endif

#ifndef DO_SLOW_CLOCK
#define DO_SLOW_CLOCK	0	/* Perform CPU-intense timing of critical loops? */
#endif

// Runtime callstack display.
#ifndef DO_GUARD
#define DO_GUARD		1	/* Trap errors? */
#endif

#ifndef DO_SLOW_GUARD
#define DO_SLOW_GUARD	0	/* Trap errors where that would slow down the code significantly? */
#endif

// Modules to compile.
#define EDITOR			1	/* Include Unreal editor code? */

// Code to generate.
#define ASM				1	/* Use Intel assembler code? */

/*-----------------------------------------------------------------------------
	Conditional macros.
-----------------------------------------------------------------------------*/

// Conditional that only takes effect if editor support is enabled.
#if EDITOR
#define IFEDITOR(cmds) cmds
#else
#define IFEDITOR(cmds)
#endif

/*-----------------------------------------------------------------------------
	Defines for all platforms.
-----------------------------------------------------------------------------*/

// Names and version numbers.
#define GAME_NAME		"Unreal"
#define ENGINE_NAME		"Unreal Virtual Machine"
#define CAMERA_NAME		"Unreal Camera"

#define GAME_VERSION	"0.86"
#define ENGINE_VERSION  "0.86"

#define CONSOLE_SPAWN_1	"UnrealServer " ENGINE_VERSION
#define CONSOLE_SPAWN_2	"Copyright 1997 Epic MegaGames, Inc."

// Class name to apply top-level Make-All to.
#define TOP_CLASS_NAME "Object"

// Gatekeeper information.
#define GK_VER		    "10"
#define GK_APP          "Unreal"
#define GK_APP_VER		"86"

// Unrealfile information.
// Prevents outdated Unrealfiles from being loaded.
#define UNREALFILE_TAG "Unrealfile\x1A" /* 31 characters or less */

// Hardcoded filenames, paths, and extensions:
#define DEFAULT_STARTUP_FNAME	"Unreal.unr"
#define DEFAULT_CLASS_FNAME		"Unreal.ucx"
#define DEFAULT_BINDINGS_FNAME	"Bind.mac"
#define DEFAULT_PALETTE_FNAME	"..\\Graphics\\Palette.pcx"
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

// Paths to search for UnrealFiles.
#define FIND_PATHS              {"..\\Cache\\", "..\\System\\", "..\\Classes\\", "..\\Maps\\", "..\\Textures\\", "..\\Audio\\", "..\\Brushes\\"}

#ifndef _DEBUG
	#define MFC_HELP_PARTIAL	"UNSERVER.HLP" /* Must be all caps. */
#else
	#define MFC_HELP_PARTIAL	"DSERVER.HLP" /* Must be all caps. */
#endif

// URL's.
#define URL_WEB					"http://www.epicgames.com/"
#define URL_GAME				"unreal://unreal.epicgames.com/"
#define URL_UNAVAILABLE			"..\\Help\\Unreal.htm"

/*-----------------------------------------------------------------------------
	Windows 95 settings.
-----------------------------------------------------------------------------*/

// For shell, Ole, and file dialogs.
#define MAP_EXTENSION			".unr"
#define OLE_APP					"Unreal"
#define OLE_APP_DESCRIPTION		"Unreal Engine"
#define OLE_MAP_TYPE			"Unreal.Level"
#define OLE_MAP_DESCRIPTION		"Unreal Level"
#define MIME_MAP_TYPE			"application/unreal"
#define PLAY_COMMAND			"&Play this Unreal level"
#define EDIT_COMMAND			"&Edit with UnrealEd"
#define LOAD_MAP_MASK			"Unreal maps (*.unr)|*.unr|All Files (*.*)|*.*||"
#define LOAD_MAP_FILTER         "Unreal maps (*.unr)\x00*.unr\x00All Files (*.*)\x00*.*\x00\x00"
#define SAVE_MAP_MASK			"Unreal maps (*.unr)|*.unr|All Files (*.*)|*.*||"
#define SAVE_MAP_FILTER         "Unreal maps (*.unr)\x00*.unr\x00All Files (*.*)\x00*.*\x00\x00"

// Registry usage.
#define REGISTRY_KEY_BASE		"Software"
#define REGISTRY_KEY_COMPANY	"Epic MegaGames"
#define REGISTRY_KEY_PRODUCT	"Unreal"

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNBUILD
