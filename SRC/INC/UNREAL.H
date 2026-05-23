/*=============================================================================
	Unreal.h: Main header for the Unreal engine

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
		* include unpath.h SEP 3/97
=============================================================================*/

#ifndef _INC_UNREAL
#define _INC_UNREAL

/*----------------------------------------------------------------------------
	General includes.
----------------------------------------------------------------------------*/

#include "UnBuild.h"	// Version specific info.
#include "UnObjVer.h"	// Object version info.
#include "UnPort.h"		// Portability code.
#include "UnPlatfm.h"	// Platform dependent hooks.
#include "UnChecks.h"	// Debugging, checking, and guarding.

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

//
// Every major, Unreal subsystem has a global class associated with it,
// and a global variable named GSomthing.
//
UNENGINE_API extern class FGlobalUnrealEngine		GUnreal;
UNENGINE_API extern class FGlobalObjectManager		GObj;
UNENGINE_API extern class FGlobalUnrealServer		GServer;
UNENGINE_API extern class FGlobalDefaults			GDefaults;
UNENGINE_API extern class FGlobalPlatform*			GApp;
UNENGINE_API extern class FGlobalTopicTable			GTopics;
UNENGINE_API extern class FMemCache					GCache;
UNENGINE_API extern class FMemStack					GMem,GDynMem;
UNENGINE_API extern class FGlobalGfx				GGfx;
UNENGINE_API extern class FGlobalMath				GMath;
UNENGINE_API extern class FGlobalAudio				GAudio;
UNENGINE_API extern class FRenderBase*				GRend;
UNENGINE_API extern class FCameraManagerBase*		GCameraManager;
UNENGINE_API extern class UTransBuffer*				GTrans;
UNENGINE_API extern class FTransactionTracker*		GUndo;
UNENGINE_API extern class FGlobalEditor*			GEditor;
UNENGINE_API extern class FEditorRenderBase*		GEdRend;
UNENGINE_API extern class FGameBase*				GGameBase;
UNENGINE_API extern class FMovingBrushTrackerBase&	GBrushTracker;
UNENGINE_API extern class NManager*					GNetManager;

/*-----------------------------------------------------------------------------
	Unreal engine includes.
-----------------------------------------------------------------------------*/

#include "UnNames.h"	// Hardcoded names.
#include "UnArc.h"		// Archiver code.
#include "UnFile.h"		// Low-level, platform-independent file functions.
#include "UnMath.h"		// Vector math functions.
#include "UnCache.h"	// In-memory object caching.
#include "UnMem.h"		// Fast memory pool allocation.
#include "UnName.h"	    // Global name system.
#include "UnStack.h"    // UnrealScript stack definition.
#include "UnObjBas.h"	// Object base class.
#include "UnParams.h"	// Parameter parsing routines.
#include "UnCID.h"		// Cache identifiers.
#include "UnObj.h"		// Standard object definitions.
#include "UnPrim.h"		// Primitive class.
#include "UnModel.h"	// Model class.
#include "UnTopics.h"	// Topic handlers for editor/server communication.
#include "Root.h"		// All actor classes.
#include "UnActLst.h"	// Actor list object class definition.
#include "UnProp.h"		// Actor class property definitions.
#include "UnClass.h"	// Actor class object class definition.
#include "UnScript.h"	// Script object class.
#include "UnMsgPar.h"	// Actor message parameter classes.
#include "UnPath.h"		// path building and reachspec creation and management SEP 3/97
#include "UnLevel.h"	// Level object.
#include "UnIn.h"		// Input system.
#include "UnCamera.h"	// Camera subsystem.
#include "UnFGAud.h"	// Audio subsystem FGlobalAudio.
#include "UnSound.h"	// Sound subsystem main.
#include "UnMusic.h"	// Music.
#include "UnGfx.h"		// Graphics subsystem.
#include "UnServer.h"	// Unreal server.
#include "UnDeflts.h"	// Unreal defaults.
#include "UnEngine.h"	// Unreal engine.
#include "UnGamBas.h"	// Game base class.
#include "UnDynBsp.h"	// Dynamic Bsp objects.
#include "UnMesh.h"     // Mesh objects.
#include "UnActor.h"	// Actor inlines.

/*-----------------------------------------------------------------------------
	Unreal editor includes.
-----------------------------------------------------------------------------*/

#if EDITOR
#include "UnEditor.h"	// Unreal editor.
#include "UnEdTran.h"	// Transaction tracking system.
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNREAL
