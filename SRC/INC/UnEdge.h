/*=============================================================================
	UnEdge.h: Unreal edge code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNEDGE
#define _INC_UNEDGE

/*-----------------------------------------------------------------------------
	Edges.
-----------------------------------------------------------------------------*/

// A Bsp leaf.
struct FBspLeaf
{
	// Logical leaf number, if leaves have been merged.
	INT iLogicalLeaf;

	// Zone this leaf belongs in.
	INT iZone;
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNEDGE
