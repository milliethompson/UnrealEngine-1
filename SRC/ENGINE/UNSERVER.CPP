/*=============================================================================
	UnServer.cpp: Player login/logout/information functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

enum {MAX_SERVER_PLAYERS	= 256};
enum {MAX_WORLD_LEVELS		= 64};

/*-----------------------------------------------------------------------------
	FGlobalUnrealServer init & exit.
-----------------------------------------------------------------------------*/

//
// Start up the server.
//
void FGlobalUnrealServer::Init()
{
	guard(FGlobalUnrealServer::Init);

	TimeSeconds	= 0.0;
	Pauseable	= 1;
	Paused		= 0;
	NoCollision = 0;

	// Allocate server array.
	ServerArray = new("Server",CREATE_Unique)UArray(0);

	// Allocate all objects the server needs.
	Players = new("PlayerList",CREATE_Unique)TArray<UPlayer*>(0);

	// Add all newly-allocated objects to the server array for tracking.
	ServerArray->AddItem(Players);
	GObj.AddToRoot(ServerArray);

	debug (LOG_Init,"Server initialized");
	unguard;
}

//
// Shut down the server.
//
void FGlobalUnrealServer::Exit()
{
	guard(FGlobalUnrealServer::Exit);

	// Remove all players.
	int PlayerCount = Players->Num;
	for( int i=PlayerCount-1; i>=0; i-- )
	{
		LogoutPlayer(Players->Element(i));
	}
	Players->Kill();

	// Close and kill all levels.
	if( Level )
	{
		GObj.RemoveFromRoot(Level);
		Level->Kill();
	}

	// Remove from root array.
	GObj.RemoveFromRoot(ServerArray); 
	ServerArray->Kill();

	debugf (LOG_Exit,"Server shut down, %i player(s) logged off",PlayerCount);
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalUnrealServer command line.
-----------------------------------------------------------------------------*/

int FGlobalUnrealServer::Exec( const char *Cmd,FOutputDevice *Out )
{
	guard(FGlobalUnrealServer::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"PLAYERSONLY") )
	{
		PlayersOnly ^= 1;
		Out->Log(PlayersOnly ? "Updating players only" : "Updating all actors");
		return 1;
	}
	else if( GetCMD(&Str,"COLLISION") )
	{
		NoCollision ^= 1;
		return 1;
	}
	else if( GetCMD(&Str,"DEBUG") )
	{
		if( GetCMD(&Str,"CRASH") )
		{
			appError ("Unreal crashed at your request");
			return 1;
		}
		else if( GetCMD(&Str,"GPF") )
		{
			Out->Log("Unreal crashing with voluntary GPF");
			*(int *)NULL = 123;
			return 1;
		}
		else if( GetCMD(&Str,"EATMEM") )
		{
			Out->Log("Eating up all available memory");
			while( 1 )
			{
				void *Eat = GApp->Malloc(65536,"EatMem");
				memset(Eat,0,65536);
			}
			return 1;
		}
		else return 0;
	}
	else return 0; // Not executed.
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalUnrealServer player login/logout.
-----------------------------------------------------------------------------*/

//
// Log a new player in and return his UPlayer object or NULL if not accepted.
//
UPlayer *FGlobalUnrealServer::Login( ULevel *Level, const char *Name, FSocket *Socket )
{
	guard(FGlobalUnrealServer::Login);
	UPlayer *NewPlayer;

	if( new(Name,FIND_Optional)UPlayer )
	{
		// Already logged in.
		return NULL;
	}

	// Add new player object.
	NewPlayer = new(Name,CREATE_Unique)UPlayer;

	// Set player's information.
	NewPlayer->Socket	= Socket;
	NewPlayer->Level	= Level;
	NewPlayer->iActor	= INDEX_NONE;

	// Add to player list.
	Players->AddItem(NewPlayer);

	debugf (LOG_ComeGo,"Player %s logged in",NewPlayer->GetName());
	return NewPlayer;

	unguard;
}

//
// Log a player out.
//
void FGlobalUnrealServer::LogoutPlayer (UPlayer *Player)
{
	guard(FGlobalUnrealServer::LogoutPlayer);

	//	Remove entry from player list.
	Players->RemoveItem(Player);

	// Kill player object.
	debugf (LOG_ComeGo,"Player %s logged out",Player->GetName());
	Player->Kill();

	unguard;
}

//
// Log a player on a particular socket out.
//
void FGlobalUnrealServer::LogoutSocket( FSocket *Socket )
{
}

/*-----------------------------------------------------------------------------
	Server timer tick function.
-----------------------------------------------------------------------------*/

//
// Calls ULevel::Tick for all levels that are up for playing.  Doesn't do anything
// with levels that are being edited, because the editor cameras take care
// of all updating (when editing a level with player controls, the editor
// cameras handle kTick calls).
//
void FGlobalUnrealServer::Tick( FLOAT DeltaSeconds )
{
	guard(FGlobalUnrealServer::Tick);

	UCamera	*Camera,*ActiveCamera;
	ULevel	*ActiveLevel;
	AActor	*ActiveActor;

	// Tick the cache.
	GCache.Tick();

	// Update time.
	TimeSeconds += DeltaSeconds;

	// Init states and info.
	LevelTickTime			= 0;
	ScriptExecTime			= 0;
	ActorTickTime			= 0;
	AudioTickTime			= 0;

	// Find active (input) camera:
	ActiveCamera 	= NULL;
	ActiveLevel 	= NULL;
	ActiveActor		= NULL;
	for (int i=0; i<GCameraManager->CameraArray->Num; i++)
	{
		Camera = GCameraManager->CameraArray->Element(i);
		if (Camera->Current)
		{
			ActiveCamera	= Camera;
			ActiveLevel		= Camera->Level;
			ActiveActor		= (AActor *)Camera->Actor;
			break;
		}
	}

	// Update the level:
	if( Level )
	{
		int IsPlay    = Level->GetState()==LEVEL_UpPlay;
		int IsEdit    = Level->GetState()==LEVEL_UpEdit;
		int NowPaused = 0;

		if( IsPlay || (IsEdit && ActiveCamera && ActiveCamera->IsRealtime()) )
		{
			// Gather input.
			for( int iActor=0; iActor<Level->Num; iActor++ )
			{
				AActor *Actor = Level->Element(iActor);
			}

			// Get actions.
            if( Level==ActiveLevel && ActiveActor && ActiveCamera && ActiveCamera->Current )
            {
                //!!ActiveCamera->Console->UpdateActionStatus();
				//Get pause status!!
				NowPaused = Pauseable && 0;
            }

			// Handle change of pause state.
			if( NowPaused && !Paused )
			{
                GAudio.Pause();
                Level->Log( LOG_Play, "Paused");
				Paused = 1;
			}
			else if( Paused && !NowPaused )
			{
				GAudio.UnPause();
				Paused = 0;
			}

			// Update the level.
            if( !Paused )
            {
			    Level->Tick
				(
					IsEdit || PlayersOnly,
					(Level==ActiveLevel) ? ActiveActor : NULL,
					DeltaSeconds
				);
            }

			// Restart.
			if( Level->GetLevelInfo()->bRestartLevel )
				GUnreal.EnterWorld( "restart", 0 );
		}
		else
		{
			// Update level time only.
			Level->Tick( 2, NULL, DeltaSeconds );
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	UPlayer object implementation.
-----------------------------------------------------------------------------*/

void UPlayer::InitHeader()
{
	guard(UPlayer::InitHeader);

	// Init UObject info.
	UObject::InitHeader();
	
	// Init UPlayer info.
	Socket	= NULL;
	Level   = NULL;
	iActor	= INDEX_NONE;
	unguard;
}
IMPLEMENT_CLASS(UPlayer);

/*-----------------------------------------------------------------------------
	Accessors.
-----------------------------------------------------------------------------*/

// Get the server's level.
ULevel *FGlobalUnrealServer::GetLevel()
{
	return Level;
}

// Set the server's level.
void FGlobalUnrealServer::SetLevel( ULevel *ThisLevel )
{
	if( Level ) GObj.RemoveFromRoot(Level);
	Level = ThisLevel;
	if( Level ) GObj.AddToRoot(Level);
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
