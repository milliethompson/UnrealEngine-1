/*=============================================================================
	UnMsgPmp.cpp: Unreal message pump.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include "UnWn.h"
#include "Unreal.h"
#include "UnWnCam.h"

/*-----------------------------------------------------------------------------
	Windows Message Pump.
-----------------------------------------------------------------------------*/

//
// Unreal's main message pump.  All windows in Unreal receive messages
// somewhere below this function on the stack.
//
void CUnrealWnApp::MessagePump()
{
	guard(CUnrealWnApp::MessagePump);
	SQWORD OldTime = Platform.MicrosecondTime(), NewTime;
	static DWORD ThreadId = GetCurrentThreadId();

	for( ; ; )
	{
		// Update misc subsystems.
		GObj.Tick();				

		// Handle all incoming messages.
		MSG Msg;
		while( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( Msg.message == WM_QUIT )
				return;

			RouteMessage( &Msg );

			if( Msg.message==WM_MOUSEMOVE )
			{
				while( PeekMessage( &Msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
					RouteMessage( &Msg );
				break;
			}
		}

		// Update the world.
		NewTime = Platform.MicrosecondTime();
		GServer.Tick( (float)(NewTime - OldTime)/1000000.0 );
		OldTime = NewTime;

		// Update audio.
		clock(GServer.AudioTickTime);
		GAudio.Tick();
		unclock(GServer.AudioTickTime);

		// Render everything.
		GCameraManager->Tick();

		// If this thread doesn't have the focus, don't suck up too much CPU time.
		static BOOL HadFocus=1;
		BOOL HasFocus = (GetWindowThreadProcessId(GetForegroundWindow(),NULL) == ThreadId );
		if( HadFocus && !HasFocus )
		{
			// Drop our priority to speed up whatever is in the foreground.
			SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );
		}
		else if( HasFocus && !HadFocus )
		{
			// Boost our priority back to normal.
			SetThreadPriority( THREAD_PRIORITY_HIGHEST );
		}
		if( !HasFocus )
		{
			// Surrender the rest of this timeslice.
			Sleep(0);
		}
		HadFocus = HasFocus;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
