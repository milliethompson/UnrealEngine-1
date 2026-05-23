/*=============================================================================
	UnDeflts.h: Global Unreal defaults

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNDEFLTS
#define _INC_UNDEFLTS

/*-----------------------------------------------------------------------------
	Global Unreal defaults.
-----------------------------------------------------------------------------*/

class UNENGINE_API FGlobalDefaults
{
public:

	// Command line info.
	char CmdLine	[256]; 	// Uppercase command line.
	char AutoLevel	[256];	// Level to load automatically.
	char AutoURL	[256];	// URL to load automatically.

	// Subsystems to activate.
	int	LaunchEditor;

	// Camera properties.
	int	CameraSXR;
	int	CameraSYR;

	// Transaction tracking.
	int	MaxTrans;
	int	MaxChanges;
	int	MaxDataOffset;

	// Audio.
	int AudioActive;

	// Functions.
	void Init(char *CmdLine);
	void Exit();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNDEFLTS

