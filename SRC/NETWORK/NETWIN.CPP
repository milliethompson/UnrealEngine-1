/*=============================================================================
	NetMain.cpp: Windows-specific network DLL code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */
#include <windows.h>
#include "Net.h"

/*-----------------------------------------------------------------------------
	Dll load
-----------------------------------------------------------------------------*/

BOOL APIENTRY DllMain
	(
	HANDLE	hModule, 
	DWORD	ulReasonForCall, 
	LPVOID	lpReserved
	)
	{
	switch(ulReasonForCall)
		{
		case DLL_PROCESS_ATTACH:
			NetManager.hInstance = (DWORD)hModule;
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		};
	return TRUE;
	};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
