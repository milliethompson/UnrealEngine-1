/*=============================================================================
	UnDeflts.cpp: Unreal global default class implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Architectural note:
		* This matrix code organization sucks!

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#pragma DISABLE_OPTIMIZATION /* Non performance critical code */

/*-----------------------------------------------------------------------------
	FGlobalDefaults implementation.
-----------------------------------------------------------------------------*/

//
// Set all global platform-dependent defaults.
//
// Should be expanded to only allocate tons of stuff if the
// editor is active.
//
// Not guarded: Called before engine startup.
//
void FGlobalDefaults::Init( char *ThisCmdLine )
{
	// Process command-line parameters.
	strcpy(CmdLine,ThisCmdLine);
	if (*CmdLine)
		debugf (LOG_Info,"CmdLine is '%s'",CmdLine);

	// Camera properties.
	CameraSXR 		= 320;
	CameraSYR 		= 200;

    CameraSXR = GApp->GetProfileInteger("Screen","CameraSXR",320);
    CameraSYR = GApp->GetProfileInteger("Screen","CameraSYR",200);

	// Audio properties.
	AudioActive	= 1; GetONOFF (CmdLine,"AUDIO=",&AudioActive);

	// Transaction tracking.
	MaxTrans     	= 80;
	MaxChanges		= 12000;
	MaxDataOffset	= 2048 * 1024; // 2 megs undo buffer.

	// Startup level (FILE= or first command-line parameter).
	if( !GetSTRING( CmdLine, "FILE=", AutoLevel, ARRAY_COUNT(AutoLevel) ) )
	{
		const char *Temp = CmdLine;
		char TestFilename[256];

		if( !GrabSTRING( Temp, AutoLevel, ARRAY_COUNT(AutoLevel) )
		||	!GApp->FindFile( AutoLevel, TestFilename ) )
			strcpy( AutoLevel, DEFAULT_STARTUP_FNAME );
	}

	// Startup URL.
	AutoURL[0]=0;
	if( GetSTRING( CmdLine, "URL=", AutoURL, 256 ) )
	{
		appError("Sorry, Unreal URLs are not yet supported!");
	}

	// Editor.
	LaunchEditor = GetParam (CmdLine,"EDITOR");
	if( LaunchEditor )
	{
		debug (LOG_Init,"UnrealServer spawned for editing");
	}
	else
	{
		debug (LOG_Init,"UnrealServer spawned for gameplay");
	}
	GetDWORD (CmdLine,"HWND=",&GApp->hWndParent);
}

//
// Shut down the defaults.
//
void FGlobalDefaults::Exit()
{
	// Nothing to do.
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
