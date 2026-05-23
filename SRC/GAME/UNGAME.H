/*=============================================================================
	UnGame.h: Game-specific handlers

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	This file includes UnActor.h with MAIN_UNREAL_DLL_FILE defined,
	causing unactor.h to create the function ActorDLLStartup, which
	creates and registers all of the actor classes defined in
	the actor class definition file, UnActDll.uac.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNGAME
#define _INC_UNGAME

/*-----------------------------------------------------------------------------
	FGame.
-----------------------------------------------------------------------------*/

//
// Game-specific init, exit, and global handlers.
// Overrides the generic FGameBase class.
//
class FGame : public FGameBase
{
public:
	/////////////////////////
	// FGameBase interface //
	/////////////////////////

	// Game-specific init and exit.
	void Init();
	void Exit();
	void CheckState();

	// Creating and destroying game-specific camera consoles.
	class FCameraConsoleBase *CreateCameraConsole(UCamera *Camera);
	void DestroyCameraConsole(class FCameraConsoleBase *Console);

	// Command line.
	int Exec(const char *Cmd,FOutputDevice *Out);

	/////////////////////
	// FGame interface //
	/////////////////////

	int	PlayerTick(ULevel &Level, INDEX iActor, void *Params);
};

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// The game
//
UNGAME_API extern class FGame GGame;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNGAME
