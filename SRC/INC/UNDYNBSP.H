/*=============================================================================
	UnDynBsp.h: Unreal dynamic Bsp object support

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNDYNBSP
#define _INC_UNDYNBSP

/*---------------------------------------------------------------------------------------
	FMovingBrushTrackerBase virtual base class.
---------------------------------------------------------------------------------------*/

//
// Moving brush tracker.
//
class FMovingBrushTrackerBase
{
public:
	// Init/exit.
	virtual void Init( ULevel *ThisLevel ) = 0;
	virtual void Exit() = 0;
	
	// Lock/unlock.
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

	// Public operations:
	virtual void Update( AActor *Actor ) = 0;
	virtual void Flush( AActor *Actor ) = 0;
	virtual int  SurfIsDynamic( INDEX iSurf ) = 0;
};

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
#endif // _INC_UNDYNBSP
