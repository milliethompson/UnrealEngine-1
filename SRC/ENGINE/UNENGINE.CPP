/*=============================================================================
	UnEngine.cpp: Unreal engine main

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "Net.h"
#include "UnConfig.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Global subsystems in the engine.
UNENGINE_API FGlobalUnrealEngine	GUnreal;
UNENGINE_API FGlobalObjectManager   GObj;
UNENGINE_API FGlobalUnrealServer	GServer;
UNENGINE_API FGlobalDefaults		GDefaults;
UNENGINE_API FGlobalTopicTable		GTopics;
UNENGINE_API FMemCache				GCache;
UNENGINE_API FMemStack				GMem;
UNENGINE_API FMemStack				GDynMem;
UNENGINE_API FGlobalMath			GMath;
UNENGINE_API FGlobalGfx				GGfx;

// Global subsystems outside the engine.
UNENGINE_API FGlobalPlatform*       GApp			= NULL;
UNENGINE_API FRenderBase*           GRend			= NULL;
UNENGINE_API FCameraManagerBase*    GCameraManager	= NULL;
UNENGINE_API FGlobalEditor*         GEditor			= NULL;
UNENGINE_API FEditorRenderBase*     GEdRend			= NULL;
UNENGINE_API NManager*              GNetManager		= NULL;
UNENGINE_API UTransBuffer*          GTrans			= NULL;
UNENGINE_API FTransactionTracker*   GUndo			= NULL;
UNENGINE_API FGameBase*             GGameBase		= NULL;

// Engine exec function so subsystems can exec without including the headers.
int UNENGINE_API GEngineExec(const char *Cmd,FOutputDevice *Out)
	{return GUnreal.Exec(Cmd,Out);}

/*-----------------------------------------------------------------------------
	Register all global names.
-----------------------------------------------------------------------------*/

#define REGISTER_NAME(num,msg) AUTOREGISTER_NAME(num,msg,RF_HardcodedName);
#define REG_NAME_HIGH(num,msg) AUTOREGISTER_NAME(num,msg,RF_HardcodedName|RF_HighlightedName);
#include "UnNames.h"

/*-----------------------------------------------------------------------------
	Unreal Init.
-----------------------------------------------------------------------------*/

//
// Initialize the engine.
//
int FGlobalUnrealEngine::Init
(
	class FGlobalPlatform		*Platform,
	class FCameraManagerBase	*CameraManager, 
	class FRenderBase			*Rend,
	class FGameBase				*Game,
	class NManager				*NetManager,
	class FGlobalEditor			*Editor,
	class FEditorRenderBase		*EdRend
)
{
	guard(FGlobalUnrealEngine::Init);

	// Set global subsystem pointers.
	GApp					= Platform;
	GCameraManager			= CameraManager;
	GEditor					= Editor;
	GEdRend					= EdRend;
	GNetManager				= NetManager;
	GGameBase				= Game;
	GRend					= Rend;

	// Set informational flags.
	if( GEditor )
	{
		EngineIsClient = EngineIsServer = EngineIsEditor = 1;
	}
	else
	{
		EngineIsClient = EngineIsServer = 1;
		EngineIsEditor = 0;
	}

	// Init memory subsystem.
	GCache.Init					(1024*1024*(GEditor ? 10 : 6),2048);
	GMem.Init					(GCache,16384,65536);
	GDynMem.Init				(GCache,16384,65536);

	// Init major subsystems.
	GObj.Init					();         // Start object manager.
	GTopics.Init				();			// Start link topic handler.
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

	if( GEditor )
	{
		// Init editor.
		GEditor->LoadClasses();
		GApp->ServerAlive = 1;
		ULevel *Level = new( "TestLev", CREATE_Unique )ULevel(3000,1,0);
		GServer.SetLevel(Level);
		GEditor->Exec ("SERVER OPEN");
		debug( LOG_Init, "UnrealServer " ENGINE_VERSION " launched for editing!" );
	}
	else
	{
		// Init game.
		InitGame();
		EnterWorld( GDefaults.AutoLevel, 0 );
		GApp->ServerAlive = 1;
		OpenCamera();
		debug( LOG_Init, "UnrealServer " ENGINE_VERSION " launched for gameplay!" );
	}
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Unreal Exit.
-----------------------------------------------------------------------------*/

//
// Exit the engine.
//
void FGlobalUnrealEngine::Exit()
{
	guard(FGlobalUnrealEngine::Exit);

	GApp->ServerAlive = 0;
    GConfiguration.Exit();

	if( GEditor ) GEditor->Exit();
	else ExitGame();

	GGameBase->Exit();
	GAudio.Exit();
	GRend->Exit();
	GGfx.Exit();
	if( GNetManager ) GNetManager->Exit();
	if( GEditor ) GEditor->Exit();
	GCameraManager->Exit();
	GServer.Exit();
	GTopics.Exit();
	GObj.Exit();
	GCache.Exit(1);
	GMem.Exit();
	GDynMem.Exit();
	GDefaults.Exit();

	debug (LOG_Exit,"Unreal engine shut down");
	unguard;
}

/*-----------------------------------------------------------------------------
	Game init/exit functions.
-----------------------------------------------------------------------------*/

//
// Initialize the game code.
//
void FGlobalUnrealEngine::InitGame()
{
	guard(FGlobalUnrealEngine::InitGame);

	// Init defaults.
	GGfx.DefaultCameraFlags  = SHOW_Backdrop | SHOW_Actors | SHOW_Menu | SHOW_PlayerCtrl | SHOW_RealTime;
	GGfx.DefaultRendMap      = REN_DynLight;

	unguard;
}

//
// Exit the game code.
//
void FGlobalUnrealEngine::ExitGame()
{
	guard(FGlobalUnrealEngine::ExitGame);

	GBrushTracker.Exit();
	GAudio.ExitLevel();
	
	unguard;
}

/*-----------------------------------------------------------------------------
	Camera functions.
-----------------------------------------------------------------------------*/

//
// Open a normal camera for gameplay or editing.
//
UCamera *FGlobalUnrealEngine::OpenCamera()
{
	guard(FGlobalUnrealEngine::OpenCamera);

	UCamera *Camera = new( NULL, CREATE_Unique )UCamera( GServer.GetLevel() );
	Camera->OpenWindow( NULL, 0 );
	return Camera;

	unguard;
}

//
// Draw a camera view.
//
void FGlobalUnrealEngine::Draw(UCamera *Camera, int Scan)
{
	guard(FGlobalUnrealEngine::Draw);

	FVector				OriginalLocation;
	FRotation			OriginalRotation;

	if( Camera->Level->Model->ModelFlags & MF_InvalidBsp )
	{
		debug (LOG_Problem,"Can't draw game view - invalid Bsp");
		return;
	}
	if( !Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
	{
		debug (LOG_Problem,"Couldn't lock camera for drawing");
		return;
	}
	GRend->PreRender(Camera);

	// Do game-specific prerendering; handles adjusting the view location
	// according to the actor's status, rendering the status bar, etc.
	Camera->Console->PreRender(Camera);
	if( Camera->X>0 && Camera->Y>0 )
	{
		// Handle graphics-mode prerendering; handles special preprocessing
		// and postprocessing special effects and stretching.
		GGfx.PreRender(Camera);
		if( Camera->X>0 && Camera->Y>0 )
		{
			// Adjust viewing location based on the player's response to NAME_PlayerCalcView.
			APawn *Actor			= Camera->Actor;
			DWORD ShowFlags			= Actor->ShowFlags;

			OriginalLocation		= Actor->Location;
			OriginalRotation		= Actor->ViewRotation;
			PCalcView ViewInfo( Actor->Location, Actor->ViewRotation );
			Camera->Actor->Process( NAME_PlayerCalcView, &ViewInfo );
			Actor->Location			= ViewInfo.CameraLocation;
			Actor->ViewRotation		= ViewInfo.CameraRotation;

			Camera->PrecomputeRenderInfo(Camera->X,Camera->Y);

			Actor->Location     = OriginalLocation;
			Actor->ViewRotation = OriginalRotation;

			// Draw the level.
			GRend->DrawWorld(Camera);

			// Draw the player's weapon.
			if( ( Actor->Weapon != NULL)
			&&  (!Actor->bBehindView)
			&&	( Actor->ShowFlags & SHOW_Actors) )
			{
				Actor->Weapon->Process
				(
					Actor->bBehindView ? NAME_InvCalcView3 : NAME_InvCalcView1,
					&PInvCalcView( ViewInfo.CameraLocation, ViewInfo.CameraRotation )
				);
				FRotation Temp          = Actor->Weapon->Rotation;
				Actor->Weapon->Rotation = Actor->Weapon->Rotation;
				Actor->Weapon->bHidden  = 0;
				GRend->DrawActor( Camera, Actor->Weapon );
				Actor->Weapon->bHidden  = 1;
				Actor->Weapon->Rotation = Temp;
			}
		}
		GGfx.PostRender(Camera);
	}
	Camera->Console->PostRender(Camera,0);
	GRend->PostRender(Camera);
	Camera->Unlock(LOCK_ReadWrite,1);

	unguard;
}

/*-----------------------------------------------------------------------------
	World functions.
-----------------------------------------------------------------------------*/

#define CHECK_OFFSET(class,cclass,member) \
{ \
	UClass    *Class = new(#class,FIND_Existing)UClass; \
	FProperty *Property = Class->FindProperty(FName(#member,FNAME_Add),PROPBIN_PerObject); \
	if( Property==NULL ) appError("Can't find '" #member "' in '" #class "'"); \
	if( Property->Offset != STRUCT_OFFSET(cclass,member) ) \
		appErrorf("Offset of '" #member "' in '" #class "' mismatch: C=%i Script=%i",STRUCT_OFFSET(cclass,member),Property->Offset); \
}

//
// Enter a level by URL.
//
void FGlobalUnrealEngine::EnterWorld( const char *WorldURL, BOOL LoadSaveGame )
{
	guard(FGlobalUnrealEngine::EnterWorld);

	// Exit any existing game.
	if( GServer.GetLevel() )
	{
		GServer.GetLevel()->RememberActors();
		GServer.SetLevel(NULL);
		ExitGame();
	}

	// Open default game world.
	if( !GObj.AddFile(WorldURL,NULL) )
		appErrorf( "Couldn't load level %s",GDefaults.AutoLevel );

	// Verify classes.
	CHECK_OFFSET( Actor, AActor, Owner        );
	CHECK_OFFSET( Actor, AActor, TimerCounter );
	CHECK_OFFSET( Pawn,  APawn,  Camera       );
	CHECK_OFFSET( Pawn,  APawn,  MaxStepHeight);

	ULevel *Level = new("TestLev",FIND_Existing)ULevel;
	GServer.SetLevel(Level);

	// Make sure all scripts are up to date.
	if( GEditor && !GEditor->CheckScripts(new("Object",FIND_Existing)UClass, *GApp) )
		appError("Scripts are not up to date");

	// Set LevelInfo info.
	ALevelInfo *Info  = Level->GetLevelInfo();
	Info->Difficulty  = 0;
	Info->NetMode     = 0;

	// Init audio for this level.
	//if( Info.Song ) debugf("Song is %s",Info.Song->GetName());
	GAudio.SpecifySong( Info->Song );
	GAudio.InitLevel( GObj.GetMaxRes() );

	// Bring level up for play.
	Level->SetState(LEVEL_UpPlay);

	// Associate cameras and players.
	Level->ReconcileActors(0);

	// Init moving brushes.
	GBrushTracker.Init(Level);

	// Purge any unused object.
	GObj.CollectGarbage(GApp);

	unguard;
}

/*-----------------------------------------------------------------------------
	Command line executor.
-----------------------------------------------------------------------------*/

//
// This always going to be the last exec handler in the chain. It
// handles passing the command to all other global handlers.
//
int FGlobalUnrealEngine::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FGlobalUnrealEngine::Exec);
	const char *Str = Cmd;

	// See if any other subsystems claim the command.
	if (GApp && GApp->Exec						(Cmd,Out)) return 1;
	if (GObj.Exec								(Cmd,Out)) return 1;
	if (GCameraManager && GCameraManager->Exec	(Cmd,Out)) return 1;
	if (GServer.GetLevel() && GServer.GetLevel()->Exec(Cmd,Out)) return 1;
	if (GServer.Exec							(Cmd,Out)) return 1;
	if (GNetManager && GNetManager->Exec		(Cmd,Out)) return 1;
	if (GEditor && GEditor->Exec				(Cmd,Out)) return 1;
	if (GRend->Exec								(Cmd,Out)) return 1;
	if (GAudio.Exec								(Cmd,Out)) return 1;

	// Handle engine command line.
	if( GetCMD(&Str,"FLUSH") )
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
		if( Str[0] ) Out->Log(LOG_ExecError,"Unrecognized command");
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
