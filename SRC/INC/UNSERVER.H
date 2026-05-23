/*=============================================================================
	UnServer.h: UnrealServer

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSERVER
#define _INC_UNSERVER

/*-----------------------------------------------------------------------------
	UPlayer.
-----------------------------------------------------------------------------*/

//
// A player object is a client owned by the server in a client/server game.
// This is mainly a management structure and isn't intended to be involved
// in gameplay at all.  All pure game-related information should be
// stored in the actor referenced by the player.
//
class UNENGINE_API UPlayer : public UObject
{
	DECLARE_CLASS(UPlayer,UObject,NAME_Player,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Transient};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	FSocket		*Socket;		// Network communication socket.
	ULevel		*Level;			// Level that player is in.
	INDEX		iActor;			// Index of actor he's controlling.

	// Also needs:
	// Time limit info
	// Chat info
	// Description, specified by player
	// Stats for tracking and debugging
	void InitHeader();
};

/*-----------------------------------------------------------------------------
	FGlobalUnrealServer.
-----------------------------------------------------------------------------*/

//
// All globals used by the server.
//
class UNENGINE_API FGlobalUnrealServer
{
public:
	// Server variables.
private:
	ULevel			*Level;					// The level.
public:
	TArray<UPlayer*>::Ptr Players;			// Player array.
	UArray			*ServerArray;			// Misc server objects.
	FLOAT			TimeSeconds;			// Time in seconds.
	int				PlayersOnly;			// Update players only.
	int				NoCollision;			// Don't perform actor/actor collision checking.
	int				LevelTickTime;			// Time consumed by ULevel::Tick.
	int				ActorTickTime;			// Time consumed by ticking all actors.
	int				AudioTickTime;			// Time consumed by FGlobalAudio::Tick.
	int				ActorCollisionFrags;	// Number of actor collision fragments.
	int				Paused,Pauseable;		// Pausing.
	int				ScriptExecTime;			// Script execution time.
	int				Pad[9];					// Space available.

	// Main.
	void	Init			();
	void	Exit			();
	int		Exec			(const char *Cmd,FOutputDevice *Out=GApp);
	ULevel	*GetLevel		();
	void	SetLevel		(ULevel *Level);
	void   Tick             (FLOAT DeltaSeconds);

	// Player-related.
	UPlayer *Login 			(ULevel *Level, const char *Name, FSocket *Socket);
	void LogoutPlayer		(UPlayer *Player);
	void LogoutSocket 		(FSocket *Socket);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSERVER
