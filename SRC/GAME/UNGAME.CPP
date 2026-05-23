/*=============================================================================
	UnActors.cpp: Main actor DLL file

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnGame.h"
#include "UnCon.h"

/*-----------------------------------------------------------------------------
	Game init and exit.
-----------------------------------------------------------------------------*/

//
// Initialized all global game info.  Called once per run.
//
void FGame::Init()
{
	guard(FGame::Init);
	debug( LOG_Init, "Game DLL initialized" );
	unguard;
}

//
// Check state of game info.
// Called between Init() and Exit().
//
void FGame::CheckState()
{
	guard(FGame::CheckState);
	// Insert state checks here.
	unguard;
}

//
// Shut down and deallocate all game-specific stuff.
//
void FGame::Exit()
{
	guard(FGame::Exit);
	debug( LOG_Exit, "Game DLL shut down" );
	unguard;
}

/*-----------------------------------------------------------------------------
	Creating and destroying camera consoles.
-----------------------------------------------------------------------------*/

//
// Create a new game-specific console for the specific camera.  The console
// includes a status bar, text console, and whatever other stuff is desired.
//
class FCameraConsoleBase *FGame::CreateCameraConsole(UCamera *Camera)
{
	guard(FGame::CreateCameraConsole);

	FCameraConsole *Result = new FCameraConsole;
	Result->Old = new FCameraConsole;
	return Result;

	unguard;
}

//
// Destroy a camera console.
//
void FGame::DestroyCameraConsole(class FCameraConsoleBase *Console)
{
	guard(FGame::DestroyCameraConsole);

	delete ((FCameraConsole *)Console)->Old;
	delete Console;

	unguard;
}

/*-----------------------------------------------------------------------------
	Game command line.
-----------------------------------------------------------------------------*/

int FGame::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FGame::Exec);
	
	// Place any command line execution code here.
	return 0;
	unguard;
};

/*-----------------------------------------------------------------------------
	Instantiation.
-----------------------------------------------------------------------------*/

UNGAME_API FGame		GGame;
UNGAME_API FGameBase	*GGamePtr = &GGame;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
