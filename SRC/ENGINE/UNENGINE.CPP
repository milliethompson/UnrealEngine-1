/*=============================================================================
	UnEngine.cpp: Unreal engine main

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnDynBsp.h"
#include "Net.h"
#include "UnConfig.h"
#include "UnCheat.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// All engine-wide global variables.
//
UNREAL_API FUnrealEngine			GUnreal;
UNREAL_API FGlobalResourceManager	GRes;
UNREAL_API FUnrealServer			GServer;
UNREAL_API FGlobalDefaults			GDefaults;
UNREAL_API FGlobalPlatform			*GApp;
UNREAL_API FGlobalTopicTable		GTopics;
UNREAL_API FGlobalClasses			GClasses;
UNREAL_API FMemoryCache				GCache;
UNREAL_API FMemPool					GMem,GDynMem;
UNREAL_API FGlobalMath				GMath;
UNREAL_API FGlobalGfx				GGfx;

UNREAL_API FGlobalRender			*GRend				= NULL;
UNREAL_API FCameraManager			*GCameraManager		= NULL;
UNREAL_API FTaskManager				*GTaskManager		= NULL;
UNREAL_API FGlobalEditor			*GEditor			= NULL;
UNREAL_API NManager					*GNetManager		= NULL;
UNREAL_API UTransBuffer				*GTrans				= NULL;
UNREAL_API FGameBase				*GGameBase			= NULL;

int UNREAL_API GEngineExec(const char *Cmd,FOutputDevice *Out)
{
	return GUnreal.Exec(Cmd,Out);
}

/*-----------------------------------------------------------------------------
	Unreal Init.
-----------------------------------------------------------------------------*/

int FUnrealEngine::Init
(
	class FGlobalPlatform	*Platform,
	class FTaskManager		*TaskManager,
	class FCameraManager	*CameraManager, 
	class FGlobalRender		*Rend,
	class FGameBase			*Game,
	class NManager			*NetManager,
	class FGlobalEditor		*Editor
)
{
	guard(FUnrealEngine::Init);

	// Init defaults based on command-line.
	GDefaults.Init(Platform->CmdLine);

	GApp				= Platform;
	GTaskManager		= TaskManager;
	GCameraManager		= CameraManager;
	GEditor				= Editor;
	GNetManager			= NetManager;
	GGameBase			= Game;
	GRend				= Rend;

	GMem.AllocatePool			(GDefaults.GlobalsMemSize,"Globals");
	GDynMem.AllocatePool		(GDefaults.DynamicsMemSize,"Dynamics");
	GCache.Init					(1024*1024*(GEditor ? 8 : 4),2000); // Allocate a 4 or 8 meg object cache

	GRes.Init					();         // Start resource manager.
	GTaskManager->Init			();			// Start task manager.
	GTopics.Init				();			// Start link topic handler.
	GMath.Init					(); 		// Init math tables.
	GClasses.Init				();			// Init actor classes.
	GServer.Init				(); 		// Init server.
	GCameraManager->Init		();			// Init camera manager.
	if (GEditor) GEditor->Init	();			// Init editor.
	if (GNetManager) GNetManager->Init();	// Initialize networking.
	GGfx.Init					(); 		// Init graphics subsystem.
	GRend->Init					(); 		// Init rendering subsystem.
	GAudio.Init					(GDefaults.AudioActive); // Init music and sound.
	GGameBase->Init				();			// Initialize game-specific info and actor messages.
	GGameBase->CheckState		();			// Verify that game state is valid.
    GConfiguration.Initialize	();			// Initialize configuration.

	if (GEditor)
	{	
		// Load classes for UnrealEd.
		if (GRes.AddFile(DEFAULT_CLASS_FNAME)==FILE_NONE)
		{
			// Import the classes from their source.
			guard(Importing classes);
			GApp->BeginSlowTask	("Building Unreal.ucx",1,0);
			GEditor->Exec		("MACRO PLAY NAME=Classes FILE=" CLASS_BOOTSTRAP_FNAME);
			GApp->EndSlowTask	();
			unguard;
		}
		GClasses.Associate();
		GApp->ServerAlive = 1;
		GServer.Levels->Element(0)->Empty();
		GEditor->Exec ("SERVER OPEN");
		debug (LOG_Init,"UnrealServer " ENGINE_VERSION " launched for editing!");
	}
	else
	{
		InitGame();
		GApp->ServerAlive = 1;
		OpenCamera();
		debug (LOG_Init,"UnrealServer " ENGINE_VERSION " launched for gameplay!");
	}
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Unreal Exit.
-----------------------------------------------------------------------------*/

void FUnrealEngine::Exit()
{
	guard(FUnrealEngine::Exit);

	GApp->ServerAlive = 0;
    GConfiguration.Exit();

	if (GEditor)	GEditor->Exit();
	else			ExitGame();

	GGameBase->Exit			();		// Shut down game-specific code.
	GAudio.Exit				();		// Shut down music and sound.
	GRend->Exit				();		// Shut down rendering subsystem.
	GGfx.Exit				();		// Shut down graphics subsystem.
	if (GNetManager) GNetManager->Exit();	// Initialize networking.
	if (GEditor) GEditor->Exit();			// Init editor.
	GCameraManager->Exit	();		// Exit camera manager.
	GServer.Exit			();		// Shut down the server.
	GClasses.Exit			();		// Shut down actor classes.
	GMath.Exit				();		// Shut down math routines.
	GTopics.Exit			();		// Shut down link topic handler.
	GTaskManager->Exit		();		// Shut down trask manager.
	GRes.Exit				();		// Shut down resource manager.
	GCache.Exit				();		// Shut down the memory cache.
	GMem.FreePool			();		// Free memory pool.
	GDynMem.FreePool		();		// Free memory pool.
	GDefaults.Exit			();		// Shut down global parameters.

	debug (LOG_Exit,"Unreal engine shut down");

	unguard;
}

/*-----------------------------------------------------------------------------
	Game init/exit functions.
-----------------------------------------------------------------------------*/

void FUnrealEngine::InitGame()
{
	guard(FUnrealEngine::InitGame);

	// Init defaults.
	GGfx.DefaultCameraFlags  = SHOW_Backdrop | SHOW_Actors | SHOW_Menu | SHOW_PlayerCtrl | SHOW_RealTime;
	GGfx.DefaultRendMap      = REN_DynLight;

	// Open default game world.
	if (GRes.AddFile(GDefaults.AutoLevel)==FILE_NONE)
		appError( "resAddFile failed" );
	ULevel *Level = (ULevel *)GServer.Levels->Element(0);
	if (!Level)
		appError( "Can't find level" );

	// Associate the intrinsic classes.
	GClasses.Associate();

	// Make sure all scripts are up to date.
	if (!MakeScripts(GClasses.Actor,0))
		appError("Script compilation failed");

	// Associate cameras and players.
	Level->ReconcileActors(0);

	// Init audio for this level.
	GAudio.InitLevel(Level->ActorList->Max);

	// Begin gameplay.
	PBeginPlay PlayInfo;
	PlayInfo.bNetCooperative	= 0;
	PlayInfo.bNetDeathMatch		= 0;
	PlayInfo.bNetPersistent		= 1;
	PlayInfo.bNetNoMonsters		= 0;
	PlayInfo.Difficulty         = 1;
	Level->SetState(LEVEL_UpPlay,&PlayInfo);

	// Init moving brushes.
	sporeInit(Level);

	// Purge any unused resources.
	GRes.Purge(GApp);

	unguard;
}

void FUnrealEngine::ExitGame()
{
	guard(FUnrealEngine::ExitGame);

	sporeExit();
	GAudio.ExitLevel();
	
	unguard;
}

/*-----------------------------------------------------------------------------
	Camera functions.
-----------------------------------------------------------------------------*/

//
// Open a normal camera for gameplay or editing.
//
UCamera *FUnrealEngine::OpenCamera()
{
	guard(FUnrealEngine::OpenCamera);

	UCamera *Camera = new(NULL,CREATE_Unique)UCamera(GServer.Levels->Element(0));
	Camera->OpenWindow(NULL,0);
	return Camera;

	unguard;
}

//
// Draw a camera view.
//
void FUnrealEngine::Draw(UCamera *Camera, int Scan)
{
	guard(FUnrealEngine::Draw);

	ICamera 			CameraInfo;
	FVector				OriginalLocation;
	FRotation			OriginalRotation;
	PCalcView			ViewInfo;
	DWORD				ShowFlags;

	if( Camera->Level->Model->ModelFlags & MF_InvalidBsp )
	{
		debug (LOG_Ed,"Can't draw game view - invalid Bsp");
		return;
	}
	if( !Camera->Lock(&CameraInfo) )
	{
		debug (LOG_Ed,"Couldn't lock camera for drawing");
		return;
	}
	GRend->PreRender(&CameraInfo);

	// Do game-specific prerendering; handles adjusting the view location
	// according to the actor's status, rendering the status bar, etc.
	CameraInfo.Camera->Console->PreRender(&CameraInfo);
	if( (Camera->SXR>0) && (Camera->SYR>0) )
	{
		// Handle graphics-mode prerendering; handles special preprocessing
		// and postprocessing special effects and stretching.
		GGfx.PreRender(&CameraInfo);
		if( (Camera->SXR>0) && (Camera->SYR>0) )
		{
			// Adjust viewing location based on the player's response to ACTOR_PlayerCalcView.

			APawn *Actor			= CameraInfo.Actor;
			ShowFlags				= Actor->ShowFlags;

			ViewInfo.Coords			= &CameraInfo.Coords;
			ViewInfo.Uncoords		= &CameraInfo.Uncoords;
			ViewInfo.ViewLocation	= Actor->Location;
			ViewInfo.ViewRotation	= Actor->ViewRot;
			CameraInfo.Level.SendMessage(CameraInfo.iActor,ACTOR_PlayerCalcView,&ViewInfo);
			OriginalLocation		= Actor->Location;
			OriginalRotation		= Actor->ViewRot;
			Actor->Location			= ViewInfo.ViewLocation;
			Actor->ViewRot			= ViewInfo.ViewRotation;

			CameraInfo.BuildCoords();

			Actor->Location = OriginalLocation;
			Actor->ViewRot  = OriginalRotation;

			// Draw the level.
			GRend->DrawWorld(&CameraInfo);

			// Draw the player's weapon.
			if( (CameraInfo.Actor->iWeapon !=INDEX_NONE) && 
				(!GCheat->InvisibleWeapons) && (!CameraInfo.Actor->bBehindView) && 
				(CameraInfo.ShowFlags & SHOW_Actors) )
			{
				INDEX iWeapon = CameraInfo.Actor->iWeapon;
				AActor *Weapon = &CameraInfo.Level.Actors->Element(iWeapon);

				CameraInfo.Level.SendMessage(iWeapon,ACTOR_InventoryCalcView,NULL);

				FRotation Temp  = Weapon->DrawRot;
				Weapon->DrawRot = Weapon->ViewRot;
				GRend->DrawActor (&CameraInfo,iWeapon);
				Weapon->DrawRot = Temp;
			}
		}
		GGfx.PostRender(&CameraInfo);
	}
	CameraInfo.Camera->Console->PostRender(&CameraInfo,0);
	GRend->PostRender(&CameraInfo);

	Camera->Unlock(&CameraInfo,!Scan);

	unguard;
}

/*-----------------------------------------------------------------------------
	World functions
-----------------------------------------------------------------------------*/

void FUnrealEngine::EnterWorld(const char *WorldURL)
{
	guard(FUnrealEngine::EnterWorld);
	// Abort if bsp is invalid.
	unguard;
}

void FUnrealEngine::GetWorldInfo(char *WorldURL, char *WorldTitle)
{
	guard(FUnrealEngine::GetWorldInfo);
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line executor
-----------------------------------------------------------------------------*/

//
// This always going to be the last handler in the chain. It
// handles passing the command to all other global handlers.
// There may be console-specific handlers before this in the
// chain, such as a camera console handler or a Telnet console
// handler.
//
int FUnrealEngine::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FUnrealEngine::Exec);
	const char *Str = Cmd;

	// See if any other subsystems claim the command.
	if (GApp && GApp->Exec						(Cmd,Out)) return 1;
	if (GTaskManager && GTaskManager->Exec		(Cmd,Out)) return 1;
	if (GRes.Exec								(Cmd,Out)) return 1;
	if (GCameraManager && GCameraManager->Exec	(Cmd,Out)) return 1;
	if (GServer.Exec							(Cmd,Out)) return 1;
	if (GNetManager && GNetManager->Exec		(Cmd,Out)) return 1;
	if (GEditor && GEditor->Exec				(Cmd,Out)) return 1;
	if (GRend->Exec								(Cmd,Out)) return 1;
	if (GAudio.Exec								(Cmd,Out)) return 1;

	// Handle engine command line.
	if (GetCMD(&Str,"STATUS"))
	{
		if( GetCMD(&Str,"ENGINE") || !Str[0] )
		{
			Out->Logf("   ENGINE - Launched for %s",GEditor ? "editing" : "play");
			return 1;
		}
		else return 1;
	}
	else if( GetCMD(&Str,"HELP") )
	{
		Out->Log("   STATUS - Major subsystem status");
		Out->Log("   FLUSH - Flush memory caches");
		Out->Log("   (See the docs for a complete command list)");
		return 1;
	}
	else if( GetCMD(&Str,"FLUSH") )
	{
		GCache.Flush();
		Out->Log("Flushed memory caches");
		return 1;
	}
	else if( GetCMD(&Str,"_HELP") )
	{
		return 1;
	}
	else
	{
		Out->Log(LOG_ExecError,"Unrecognized command");
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
