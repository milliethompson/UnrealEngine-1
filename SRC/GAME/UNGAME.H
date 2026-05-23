/*=============================================================================
	UnGame.h: Game-specific handlers

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
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
	Includes.
-----------------------------------------------------------------------------*/

//
// Main Unreal includes. Make sure that Unreal.h hasn't already been included.
// We must include Unreal.h with the special option, _DO_NOT_DEFINE_CLASS_ENUMS,
// to prevent Unreal from defining the enums that we will later define in 
// UnRoot.h.
//
#ifdef _INC_UNREAL
#error You must include UnGame.h in all game-specific code, rather than Unreal.h.
#endif

//
// If compiling the primary game-specific file, UnGame.cpp, _COMPILING_UNGAME
// is defined.  This tells us to autoregister all of the actor classes with 
// the Unreal engine.
//
// The AUTOREGISTER_CLASS macro causes inclusion of UnRoot.h to create a
// DLL-exported global variable of type void * for each actor class and
// assign in the address of that class's Process function.  This is how
// the Actor DLL communicates its handler function pointers to Unreal so that
// Unreal can call the appropriate actor functions when needed.
//
#ifdef _COMPILING_UNGAME
#define AUTOREGISTER_CLASS(classname)\
	int (classname::*classname##Temp)(ILevel *Level, FName Message, void *Params) = classname::Process;\
	DLL_EXPORT void *classname##Process = *(void **)&classname##Temp;
#endif

#define IMPLEMENTATION_API UNGAME_API /* DLL exporting of AActorBase-derived classes */

//
// Now, the main includes.
//
#include "Unreal.h"

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

	// Other functions.
	int Exec(const char *Cmd,FOutputDevice *Out);

	// Variables.
	UEnum *ActorMessages;			// List of all actor message names.

	//todo: These should be moved to class FActor.
	//!!
    INDEX SpawnProjectile(INDEX iParentActor, UClass *Class, const FVector & Velocity, FLOAT StartDist);
    INDEX SpawnForwardProjectile(INDEX iParentActor, UClass * Class, FLOAT StartDist,int YawDeviation = 0, int PitchDeviation = 0);
	int	PlayerTick(INDEX iActor, void *Params);
    void PlayerYaw(APawn & Pawn, FLOAT Analog, FLOAT Differential );
    void PlayerPitch(APawn & Pawn, FLOAT Analog, FLOAT Differential );
    void PlayerRoll(APawn & Pawn, FLOAT Analog, FLOAT Differential );
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
