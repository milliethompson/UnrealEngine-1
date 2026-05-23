/*=============================================================================
	UnServer.h: UnrealServer

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSERVER
#define _INC_UNSERVER

/*-----------------------------------------------------------------------------
	FUnrealServer.
-----------------------------------------------------------------------------*/

//
// All globals used by the server.
//
class UNREAL_API FUnrealServer : public FTask
{
public:

	// FTask interface.
	void TaskTick();
	void TaskExit();
	char *TaskStatus(char *Name,char *Desc);

	// Server variables.
	TArray<ULevel>	*Levels;				// World level array.
	TArray<UPlayer>	*Players;				// User list array.
	UArray			*ServerArray;			// Misc server resource.
	int				Ticks;					// Server's actual tick count.
	int				Rate;					// Server's tick rate.
	int				PlayersOnly;			// Update players only.
	int				NoCollision;			// Don't perform actor/actor collision checking.
	int				LevelTickTime;			// Time consumed by ILevel::Tick.
	int				ActorTickTime;			// Time consumed by ticking all actors.
	int				AudioTickTime;			// Time consumed by FGlobalAudio::Tick.
	int				ActorCollisionFrags;	// Number of actor collision fragments.
	int				Pad[10];				// Space available.
	int				IsPaused;				// Is the game paused?

	// Main.
	void Init				();
	void Exit				();
	int  Exec				(const char *Cmd,FOutputDevice *Out=GApp);
	//
	// Player-related:
	//
	UPlayer *Login 			(ULevel *Level, const char *Name, FSocket *Socket);
	void LogoutPlayer		(UPlayer *Player);
	void LogoutSocket 		(FSocket *Socket);
};

/*-----------------------------------------------------------------------------
	UPlayer.
-----------------------------------------------------------------------------*/

//
// A player resource is a client owned by the server in a client/server game.
// This is mainly a management structure and isn't intended to be involved
// in gameplay at all.  All pure game-related information should be
// stored in the actor referenced by the player.
//
class UNREAL_API UPlayer : public UResource
{
	RESOURCE_CLASS(UPlayer,BYTE,RES_Player)

	FSocket		*Socket;		// Network communication socket.
	ULevel		*Level;			// Level that player is in.
	INDEX		iActor;			// Index of actor he's controlling.

	// Also needs:
	// Time limit info
	// Chat info
	// Description, specified by player
	// Stats for tracking and debugging
	void Register				(FResourceType *Type);
	void InitHeader				();
	void Flip					();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSERVER
