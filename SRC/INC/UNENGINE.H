/*=============================================================================
	UnEngine.h: Unreal engine definition

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNENGINE
#define _INC_UNENGINE

/*-----------------------------------------------------------------------------
	Unreal engine.
-----------------------------------------------------------------------------*/

class UNREAL_API FUnrealEngine
{
public:

	int Init
	(
		class FGlobalPlatform	*Platform, 
		class FTaskManager		*TaskManager,
		class FCameraManager	*CameraManager, 
		class FGlobalRender		*Rend, 
		class FGameBase			*Game,
		class NManager			*NetManager,
		class FGlobalEditor		*Editor
	);
	void Exit();
	void ErrorExit();

	UCamera *OpenCamera();
	void Draw (UCamera *Camera, int Scan);

	void EnterWorld(const char *WorldURL);
	void GetWorldInfo(char *WorldURL, char *WorldTitle);

	void InitGame();
	void ExitGame();

	int Exec(const char *Cmd,FOutputDevice *Out=GApp);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNENGINE
