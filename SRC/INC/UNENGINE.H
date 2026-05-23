/*=============================================================================
	UnEngine.h: Unreal engine definition

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNENGINE
#define _INC_UNENGINE

/*-----------------------------------------------------------------------------
	Unreal engine.
-----------------------------------------------------------------------------*/

class UNENGINE_API FGlobalUnrealEngine
{
public:
	// FGlobalUnrealEngine Startup/shutdown.
	int Init
	(
		class FGlobalPlatform		*Platform, 
		class FCameraManagerBase	*CameraManager, 
		class FRenderBase			*Rend, 
		class FGameBase				*Game,
		class NManager				*NetManager,
		class FGlobalEditor			*Editor,
		class FEditorRenderBase		*EdRend
	);
	void Exit();
	void ErrorExit();

	// Camera functions.
	UCamera *OpenCamera();
	void Draw (UCamera *Camera, int Scan);

	// World functions.
	void EnterWorld(const char *WorldURL, BOOL LoadSaveGame);

	// Game functions.
	void InitGame();
	void ExitGame();

	// Command line.
	int Exec(const char *Cmd,FOutputDevice *Out=GApp);

	// Accessors.
	int IsClient() {return EngineIsClient;}
	int IsServer() {return EngineIsServer;}
	int IsEditor() {return EngineIsEditor;}

private:
	// Implementation variables.
	int EngineIsClient;
	int EngineIsServer;
	int EngineIsEditor;
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNENGINE
